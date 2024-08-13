/*
 ------------------------------------------------------------------

 This file is part of the Open Ephys GUI
 Copyright (C) 2024 Open Ephys
 Copyright (C) 2024 DSP Wireless, Inc.

 ------------------------------------------------------------------

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */


#ifdef _WIN32
#include <Windows.h>
#endif

#include "RcbWifi.h"
#include "RcbWifiEditor.h"

using namespace RcbWifiNode;

DataThread* RcbWifi::createDataThread(SourceNode* sn)
{
	return new RcbWifi(sn);
}

RcbWifi::RcbWifi(SourceNode* sn) : DataThread(sn),
port(DEFAULT_PORT),
num_channels(DEFAULT_NUM_CHANNELS),
num_samp(DEFAULT_NUM_SAMPLES),
data_offset(DEFAULT_DATA_OFFSET),
data_scale(DEFAULT_DATA_SCALE),
sample_rate(DEFAULT_SAMPLE_RATE)
{
    // in RCB case auxbuffer size depends on num channels
    // these settings are recalculated in resizeBuffers() below
	sourceBuffers.add(new DataBuffer(num_channels, 10000));
	recvBufSize = 40 + (((num_channels + 2) * num_samp) * 2);
	recvbuf = (uint16_t*)malloc(recvBufSize);

	convBufSize = 0 + (((num_channels)*num_samp) * 4);
	//convBufSize = 0 + (((num_channels + 3)*num_samp) * 4); // with aux
	convbuf = (float*)malloc(convBufSize);

	auxbuf = (uint16_t*)malloc(8*num_samp);
}

std::unique_ptr<GenericEditor> RcbWifi::createEditor(SourceNode* sn)
{
	std::unique_ptr<RcbWifiEditor> editor = std::make_unique<RcbWifiEditor>(sn, this);

	return editor;
}

RcbWifi::~RcbWifi()
{
	free(recvbuf);
	free(convbuf);
	free(auxbuf);
	if (connected == true)
	{
		socket->shutdown();  // check if this is needed.
        connected = false;
	}
    
    if (initPassed == true)
    {
        // send OFF message to RCB if GUI crashes or if user exits without stopping record 
        sendRCBTriggerPost(ipNumStr, "__SL_P_ULD=OFF");
    }
}


// this might be useful
//void RcbWifi::connectionLost()
//{
    
//}

void RcbWifi::handleBroadcastMessage(String msg)
{

}

String RcbWifi::handleConfigMessage(String msg)
{
	return "";
}

void RcbWifi::resizeBuffers()
{
    LOGD( "[dspw] In Resize Buffers()");
    if (auxEnableState == true)
    {
        sourceBuffers[0]->resize(num_channels + 3, 10000);  // with aux
        convBufSize = 0 + (((num_channels + 3)*num_samp) * 4);  // with aux
        
    } else
    {
        sourceBuffers[0]->resize(num_channels, 10000);  // no aux
        convBufSize = 0 + (((num_channels)*num_samp) * 4); //no aux
    }
    
	recvBufSize = 40 + (((num_channels + 2) * num_samp) * 2);
    
	recvbuf = (uint16_t*)realloc(recvbuf, recvBufSize);
    convbuf = (float*)realloc(convbuf, convBufSize);
	auxbuf = (uint16_t*)realloc(auxbuf,  num_samp * 8);
    
    LOGD("[dspw] num_channels = ",String(num_channels));
    LOGD( "[dspw] num_samp = ",String(num_samp));
    LOGD("[dspw] resize recBufSize = ",String(recvBufSize));
    LOGD("[dspw] resize convBufSize = ",String(convBufSize));

	sampleNumbers.resize(num_samp);
	timestamps.clear();
	timestamps.insertMultiple(0, 0.0, num_samp);
	ttlEventWords.resize(num_samp);
}

void RcbWifi::updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,
	OwnedArray<EventChannel>* eventChannels,
	OwnedArray<SpikeChannel>* spikeChannels,
	OwnedArray<DataStream>* sourceStreams,
	OwnedArray<DeviceInfo>* devices,
	OwnedArray<ConfigurationObject>* configurationObjects)
{
    //LOGC("[dspw] In updateSettings()");
    //LOGD("[dspw] In updateSettings()");
   
	continuousChannels->clear();
	eventChannels->clear();
	spikeChannels->clear();
	sourceStreams->clear();
	devices->clear();
	configurationObjects->clear();
	
	//channelNames.clear();  ??

	DataStream::Settings dataStreamSettings
	{
		"RCBWifiStream",
		"Data acquired via RCB UDP network stream",  // "description"
		"rcbwifi.data",  // "identifier"

		sample_rate

	};

	LOGD("[dspw] updateSettings num_channels = ",String(num_channels));
    
	DataStream* stream = new DataStream(dataStreamSettings);
	sourceStreams->add(stream);

	for (int ch = 0; ch < num_channels; ch++)
	{
		ContinuousChannel::Settings channelSettings{
			ContinuousChannel::Type::ELECTRODE,
			"CH" + String(ch + 1),
			"Channel acquired via RCB UDP network stream",  // "description"
			"rcbwifi.continuous",  // "identifier"

			data_scale, //0.195

			stream
			
		};

		continuousChannels->add(new ContinuousChannel(channelSettings));
		continuousChannels->getLast()->setUnits("uV");
	}

    if (auxEnableState == true)
    {
        for (int ch = 0; ch < 3; ch++)
        {
            ContinuousChannel::Settings channelSettings{
                ContinuousChannel::AUX,
                "_AUX" + String(ch + 1),
                "Aux input channel RCB UDP network stream",
                "rcbwifi.continuous.aux",
                
                0.0000374,
                
                stream
            };
            continuousChannels->add(new ContinuousChannel(channelSettings));
            continuousChannels->getLast()->setUnits("mV");
        }
    }

	EventChannel::Settings eventSettings{
		   EventChannel::Type::TTL,
		   "Events",
		   "description",
		   "identifier",
		   sourceStreams->getFirst(),//  getFirst(),
		   8  // max number of events want to use
	};

	eventChannels->add(new EventChannel(eventSettings));
}

bool RcbWifi::foundInputSource()
{
	if (initPassed == true)
	{
		if (connected == 0)
		{
			tryToConnect();
		}

		return connected;  // return true if connected (bound to datagramsocket port)
	}
    return false;  // if false then signal chain will be grey
}

void  RcbWifi::tryToConnect()
{
    // if socket is open, shut it down and start fresh
    if (socket != nullptr)
	{
        LOGD("[dspw] socket is not null");
		socket->shutdown();
        socket.reset();
	}

    socket = std::make_unique<DatagramSocket>();
	socket->setEnablePortReuse(true);
	bool bound = socket->bindToPort(port);
	connected = bound;  // this needs more cleanup and thought

	if (bound)
	{
        LOGC("[dspw] Socket bound to port ",port);
	}
	else {
        LOGC("[dspw] Could not bind socket to port ",port);
	}
}

bool RcbWifi::startAcquisition()
{
	// consider resending init messages ?  could call setRCBTokens() ?
    String myTime = Time::getCurrentTime().toString(false,true);
    LOGC("[dspw] Start Time = ",myTime);
    
	LOGC("[dspw] StartAcq batteryInit =  ",batteryInit);
	if (initPassed == true && (batteryInit > BATT_INIT_THRESH)) // and batt poll is > ?
	{
		sourceBuffers[0]->clear();  //macos
		firstPacket = 1;
		hit = 0;
		miss = 0;
		delayed = 0;
  
		total_samples = 0;  // reset sampleNumbers used in updateBuffer()
		eventState = 0;  // reset TTL event state
	
		startThread();

		//should already be connected but in case needed
		if (connected == 0)
		{
			tryToConnect();
		}

		// add check for connected before start UDP data stream
		if (connected == true)
		{
			// send HTTP Post message to RCB - RUN
			LOGC("[dspw] Start Wifi UDP Stream.  ",ipNumStr);
			sendRCBTriggerPost(ipNumStr, "__SL_P_ULD=ON");

			return true;
		}
	}
	else {
		initPassed = false;
		
		AlertWindow::showMessageBox(AlertWindow::NoIcon,
			"RCB WiFi Module not Initialized. \n\n"
			"Please check your IP and Host Address and Host Port settings. \n\nChanging any RHD configuration settings requires INIT. \r\n",
			"Press Init Button to retry INIT.",
			"OK", 0);

		CoreServices::setAcquisitionStatus(false);
		
	}
	return false;
}

bool RcbWifi::stopAcquisition()
{
	seqNum = 0;
	firstPacket = 1;
    String myTime = Time::getCurrentTime().toString(false,true);
    LOGC("[dspw] Stop Time = ",myTime);
    
	if (isThreadRunning())
	{
		signalThreadShouldExit();
		notify();
        LOGD( "[dspw] thread should exit");
	}

    if (initPassed == true)
    {
        sendRCBTriggerPost(ipNumStr, "__SL_P_ULD=OFF");
    }
    
	if (waitForThreadToExit(1000))
	{
		LOGD("[dspw] RCB WiFi data thread exited.");
	}
	else
	{
		LOGD("[dspw] RCB WiFi data thread failed to exit, continuing anyway...");
	}

	if (connected == true)
	{
        socket->shutdown(); // important to be after stopping thread
        connected = false;
	}
	
	sourceBuffers[0]->clear();

	return true;
}

bool RcbWifi::updateBuffer()
{
	int rc = socket->read(recvbuf, recvBufSize, true); //1444 1468

	if (rc == -1)
    {
		LOGD("[dspw] RCB WiFi : Data shape mismatch ");
		LOGD("[dspw] updateBuffer recBufSize = ",String(recvBufSize));
		LOGD("[dspw] updateBuffer convBufSize = ",String(convBufSize));
		return false;
	}

    magicNum = (uint8_t)(recvbuf[0] & 0x00ff);
    // LOGD("[dspw] mNum = ",(String::toHexString(magicNum)));
	//uint16_t sod = (recvbuf[0]);// &0x00ff);
    sod = (recvbuf[0]);// &0x00ff);
	
	if (magicNum == 0xc5) // is a good packet
	{
		seqNum = ((uint32_t)recvbuf[5] << 16) + recvbuf[4];
		auxMask = (uint8_t)(recvbuf[16] & 0x00ff);
		auxPhase = (uint8_t)(recvbuf[16] >> 8);
		batteryVolts = recvbuf[18]; // battery voltage
		//uint16_t digInputs = recvbuf[19];  // state of digital inputs.  connect to OE TTL Events
        digInputs = recvbuf[19];  // state of digital inputs.  connect to OE TTL Events

		int auxStart = auxPhase;

		if (firstPacket == 1)
		{
            // might need to indicate to user and stop acq if packet containing seqnum = 1 is lost
            // however does not happen in testing.
			if (seqNum == 1)
			{
				LOGC("[dspw] UDP Port - ",String(port),"  First seqNum = ",(String::toHexString(seqNum)));
				firstPacket = 0;
			}

			nextsn = seqNum;
			hit = 0; //macos
			miss = 0;
			delayed = 0;
            
            LOGD("[dspw] mNum = ",(String::toHexString(magicNum)));
			LOGD("[dspw] sod = ",(String::toHexString(sod)));
			LOGD("[dspw] port-",String(port),"  seqNum = ",(String::toHexString(seqNum)));
		}

		if (nextsn == 0 || seqNum == nextsn) {
			nextsn = seqNum + 1;
			hit++;
		}
		else if (seqNum < nextsn) {
			delayed++;
			nextsn = seqNum + 1; // macos
			LOGD("[dspw] port-",String(port),"  delayed seqNum = ",(String::toHexString(seqNum)));
		}
		else {
			miss += seqNum - nextsn;
			nextsn = seqNum + 1;
			hit++;
            //should mark data samples as missed?
		}

		// using the transpose version from EphysSocket
		int k = 0;
		for (int i = 0; i < num_samp; i++)
		{
			for (int j = 0; j < num_channels; j++)
			{
				convbuf[k++] = 0.195 * (float)(recvbuf[(j + 22) + (i * (num_channels + 2))] - 32768);
			}

            if (auxEnableState == true)
            {
                //collect aux into buffer
                int auxIndex = auxStart % 4;
               
                // in RCB packet aux samples are located before electrode samples.
                auxbuf[auxIndex] = recvbuf[(20) + (i * (num_channels + 2))] - 32768;
                auxStart = auxStart + 1;
                
                for (int j = 0; j < 3; j++)
                {
                    convbuf[k++] = 0.0000374 * (float)auxbuf[j + 1];
                }
            }

			sampleNumbers.set(i, total_samples + i);
			ttlEventWords.set(i, eventState);

			eventState = digInputs;

			/* OE Josh test code to toggle TTL events
			if ((total_samples + i) % 15000 == 0)
			{
				if (eventState == 0)
					eventState = 0xf;// each eventstate bit corresponds to an event 0x1 = event 1, 0xf = event 1,2,3,4
				else
					eventState = 0;
			}
			*/
		}
		sourceBuffers[0]->addToBuffer(convbuf,
			sampleNumbers.getRawDataPointer(),
			timestamps.getRawDataPointer(),
			ttlEventWords.getRawDataPointer(),
			num_samp,
			1);

		// mult by seqNum. if seqnum increment in packet is more than one then packets have been lost
		total_samples = (int64)num_samp * seqNum;

		return true;
	}
	LOGC("[dspw] RCB WiFi : Fail Packet MagicNum test. ");
	return false;
}

void RcbWifi::sendRCBTriggerPost(String ipNumStr, String msgStr)
{
    initPassed = false;
	this->ipNumStr = ipNumStr;
	//URL urlPost("http://192.168.0.93");
	//URL urlPost("http://" + ipNumStr);  // only works with macOs and windows
    //urlPost = urlPost.withPOSTData(msgStr); // only works with macOs and windows
    URL urlPost = URL("http://" + ipNumStr).withPOSTData(msgStr);  //this approach needed for linux
    
	int statusCode = 0;
	
    //LOGD("POST ipNumStr - ", ipNumStr);
    //LOGD("POST msgStr - ", msgStr);
    LOGD("[dspw] POST str URL - ", urlPost.toString(true));
    LOGD("[dspw] POST str data - ", urlPost.getPostData());
	std::unique_ptr<InputStream> postStream(urlPost.createInputStream(true, nullptr, nullptr, String(), 2000, &responseHeaders, &statusCode, 5, "POST"));
    
	if (postStream != nullptr)
    {
        initPassed = true;
		String postStr = postStream->readEntireStreamAsString();
        LOGD("[dspw] PostStream StatusCode = ",statusCode);
        if (statusCode != 204)
        LOGD("[dspw] Post Stream = ",postStr);
	}
	else
	{
		initPassed = false;
		AlertWindow::showMessageBox(AlertWindow::NoIcon,
			"RCB-LVDS Module not found at IP address " + ipNumStr,
			"Please check your IP address setting. \r\n\r\n"
			"Press Init button to try again.",
			"OK", 0);
	}
}

void RcbWifi::setRCBTokens()
{
	// only called from Init Button
	// now that we have good RCB WiFi and good Intan, send multiple initialization http post messages to RCB WiFi Module
	// some values are sent from editor, ex. rhdNumTsItems

	// send HTTP Post message to RCB - 
	//Init host ip and port 192.168.0.102:4416
    // sendRCBTriggerPost(ipNumStr, "__SL_P_UUU=192.168.0.102:4416");
	rcbMsgStr = "__SL_P_UUU=" + myHostStr;
	LOGC("[dspw] Host is  ",myHostStr);
    LOGD("[dspw] Msg is  ",rcbMsgStr);
    sendRCBTriggerPost(ipNumStr, rcbMsgStr);

	// send HTTP Post message to RCB - 
	//set RCB WiFi Power Amp value
	rcbMsgStr = "__SL_P_UPA=" + rcbPaStr;
    LOGD("[dspw] RCB PA =  ",rcbPaStr);
	sendRCBTriggerPost(ipNumStr, rcbMsgStr);

	// get number of channels from global
    // set rhd channel mask
	//String rhdChMaskStr = (chMask[num_channels - 1]) + " 6"; //the "6" is needed in all masks for correct aux sequence
    //LOGC("[dspw] rhdChMaskStr -  ",rhdChMaskStr);
    
    String rhdChMaskShftStr = String::toHexString(chShftMask[num_channels - 1] << (chShift - 1)) + " 6" ; //the "6" is needed in all masks for correct aux sequence
   
    //LOGD("[dspw] ChMaskShft -  ",chShift);
    LOGC("[dspw] rhdChMaskShftStr -  ",rhdChMaskShftStr.toUpperCase());

    rcbMsgStr = "__SL_P_U00=" + rhdChMaskShftStr.toUpperCase();
    LOGD("[dspw] rcbMsgStr with Shift  -  ",rcbMsgStr);
	sendRCBTriggerPost(ipNumStr, rcbMsgStr);

	// send SPI Bit Rate command to RCB
	//uint32_t bitrate = 4e7 / divider;   // actual spi clk rate that is sent to RCB
	//LOGD("[dspw] SPI bitrate -  ",bitrate);
	String bitRateStr = String(bitrate);
	rcbMsgStr = "__SL_P_URB=" + bitRateStr;
	LOGD("[dspw] SPI bitRateStr -  ",bitRateStr);
	sendRCBTriggerPost(ipNumStr, rcbMsgStr);

	// Set RHD filter Regs rhdReg08 thru rhdReg13
	// get up/low BW
    // first check aux enable state
    int d7 = 0x00;
    if (auxEnableState == true)
    {
        d7 = 0x80;  // sets RHD reg aux enable bit
    }
    LOGD("[dspw] auxEnableState = ",auxEnableState);
	rhdReg08 = upBwRh1Dac1[rhdUpBwInt];
    rhdReg09 = d7 + upBwRh1Dac2[rhdUpBwInt];
	rhdReg10 = upBwRh2Dac1[rhdUpBwInt];
    rhdReg11 = d7 + upBwRh2Dac2[rhdUpBwInt];
	rhdReg12 = lowBwDac1[rhdLowBwInt];
    rhdReg13 = d7 + lowBwDac2[rhdLowBwInt];
  
	LOGD("[dspw] rhdUpBwInt -  ",rhdUpBwInt);
	LOGD("[dspw] rhdLowBwInt -  ",rhdLowBwInt);

	// set RHD Amp power to agree with channel mask.  include channel start shift
    int rhdPaMask = (chShftMask[num_channels - 1] << (chShift - 1));
    LOGD("[dspw] rhdPaMask -  ", String::toHexString(rhdPaMask));
    
	rhdReg14 = (rhdPaMask & 0x000000ff);
	rhdReg15 = (rhdPaMask & 0x0000ff00) >> 8;
	rhdReg16 = (rhdPaMask & 0x00ff0000) >> 16;
	rhdReg17 = (rhdPaMask & 0xff000000) >> 24;

	// create RHD Register command string
	char buffer[100] = "";

	sprintf(buffer, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x00000000",
		rhdReg00, rhdReg01, rhdReg02, rhdReg03, rhdReg04, rhdReg05, rhdReg06, rhdReg07,
		rhdReg08, rhdReg09, rhdReg10, rhdReg11, rhdReg12, rhdReg13, rhdReg14, rhdReg15,
		rhdReg16, rhdReg17);

	String rhdRegAll = buffer;
    LOGD("[dspw] RHD Reg Init Values - ",rhdRegAll);
	rcbMsgStr = "__SL_P_UII=" + rhdRegAll;
	sendRCBTriggerPost(ipNumStr, rcbMsgStr);
    
    // if we get this far then try to connect to host UDP socket port
    // possible that user has changed port number so must re-connect
        tryToConnect();
        LOGD("[dspw] RCB Tokens Connected = ",connected);
}

float RcbWifi::updateSampleRate()
{
	// called from init
	// calc the actual sample rate using desisired sample rate and num channels

	//set RCB SPI bit rate
	// Calc the SPI bit rate to use during streaming
	// convert actual sample rate to SPI bit rate
	// SPI word period Tw = 200 ns + 16.5Tb
	// Sample period = numChannelsEnabled Tw
	// We always also enable two additional slots per frame for aux sequences

	// get number of channels from dropdown box
	// now done in init before updateSampleRate() is called
	numChannelsEnabled = num_channels;

	// get desired sample rate from dropdown box
	// now done in init before updateSampleRate() is called
	numChannelsEnabled += 2;
	float bitrequest = 16.5 * numChannelsEnabled * desiredSampleRate / (1.0 - 2e-7 * numChannelsEnabled * desiredSampleRate);

	// compute actual bit rate and return
	// replicate computations are done on rcblvds module
	uint32_t divider = roundf(4e7 / bitrequest);
	if (divider < 2) divider = 2;
    LOGD("[dspw] divider -  ",divider);

	bitrate = 4e7 / divider;         // actual spi clk rate that is sent to RCB
	LOGD("[dspw] SPI bitrate -  ",bitrate);

	double Ts;
	if (0 == (divider & 1))
	{   // clock divider is even, use 200ns delay
		Ts = numChannelsEnabled * (200e-9 + 16.5 / bitrate);   // actual sample period
		LOGD("[dspw] Ts even -  ",Ts);
	}
	else
	{// clock divider is odd, use 187.5ns delay
		Ts = numChannelsEnabled * (187.5e-9 + 16.5 / bitrate);   // actual sample period
        LOGD("[dspw] Ts odd -  ",Ts);
	}

	LOGC("[dspw] Actual Sample Rate = ",(1.0/Ts));
	return 1.0 / Ts;      // actual sample rate

}

// Set the DSP offset removal filter cutoff frequency as closely to the requested
// newDspCutoffFreq (in Hz) as possible; returns the actual cutoff frequency (in Hz).
double RcbWifi::setDspCutoffFreq(double newDspCutoffFreq,float sampleRate)
{
    using std::abs;
    
    int dspCutoffFreq = 0;
    int n = 0;
    double x, fCutoff[16], logNewDspCutoffFreq, logFCutoff[16], minLogDiff;
    const double Pi = 2*acos(0.0);

    fCutoff[0] = 0.0;   // We will not be using fCutoff[0], but we initialize it to be safe

    logNewDspCutoffFreq = log10(newDspCutoffFreq);

    // Generate table of all possible DSP cutoff frequencies
    for (n = 1; n < 16; ++n) {
        x = pow(2.0, (double) n);
        fCutoff[n] = sampleRate * log(x / (x - 1.0)) / (2*Pi);
        logFCutoff[n] = log10(fCutoff[n]);
     
        //std::cout << "  fCutoff[" << n << "] = " << fCutoff[n] << " Hz" << std::endl;
        //std::cout << "  logFCutoff[" << n << "] = " << logFCutoff[n] << " Hz" << std::endl;
    }

    // Now find the closest value to the requested cutoff frequency (on a logarithmic scale)
    if (newDspCutoffFreq > fCutoff[1]) {
        dspCutoffFreq = 1;
    } else if (newDspCutoffFreq < fCutoff[15]) {
        dspCutoffFreq = 15;
    } else {
        minLogDiff = 10000000.0;
        for (n = 1; n < 16; ++n) {
            if (abs(logNewDspCutoffFreq - logFCutoff[n]) < minLogDiff) {
                minLogDiff = abs(logNewDspCutoffFreq - logFCutoff[n]);
                dspCutoffFreq = n;
            }
        }
    }
    LOGD("[dspw] DSP HPF Cutoff bit value = ",dspCutoffFreq);
    if (dspHpfState == true)
    {
        rhdReg04 = 0x90 + dspCutoffFreq;
    }
    else
    {
        rhdReg04 = 0x80 + dspCutoffFreq;
    }
    LOGD("[dspw] rhdReg04 = ",std::hex,rhdReg04,std::dec);
    LOGD("[dspw] dspCutoffFreq = ",fCutoff[dspCutoffFreq]);
    
    return fCutoff[dspCutoffFreq];
}

void RcbWifi::getMuxAdcBias(float sampleRate)  //use actual sample rate
{
int muxBias = 4;
int adcBufferBias = 2;  

	if (sampleRate < 3334.0) {
		muxBias = 40;
		adcBufferBias = 32;
	}
	else if (sampleRate < 4001.0) {
		muxBias = 40;
		adcBufferBias = 16;
	}
	else if (sampleRate < 5001.0) {
		muxBias = 40;
		adcBufferBias = 8;
	}
	else if (sampleRate < 6251.0) {
		muxBias = 32;
		adcBufferBias = 8;
	}
	else if (sampleRate < 8001.0) {
		muxBias = 26;
		adcBufferBias = 8;
	}
	else if (sampleRate < 10001.0) {
		muxBias = 18;
		adcBufferBias = 4;
	}
	else if (sampleRate < 12501.0) {
		muxBias = 16;
		adcBufferBias = 3;
	}
	else if (sampleRate < 15001.0) {
		muxBias = 7;
		adcBufferBias = 3;
	}
	else {
		muxBias = 4;
		adcBufferBias = 2;
	}

	rhdReg02 = muxBias;
	rhdReg01 = 64 + adcBufferBias; // 64(0x40) is added to turn on VDD sense enable bit

    LOGD("[dspw] muxBias =  ",muxBias);
    LOGD("[dspw] adcBufferBias =  ",adcBufferBias);
}

String RcbWifi::getIntanStatusInfo()
{
	isGoodIntan = false;
	isGoodRCB = false;

    String myTime = Time::getCurrentTime().toString(false,true);
    LOGD("[dspw] Time = ",myTime);
    
	// URL urlInit("http://192.168.0.93/intan_status.html");
	URL urlInit("http://" + ipNumStr + "/intan_status.html");  

	// access RCB module embedded webpage. 
	auto result = getResultText(urlInit);
	//LOGD("[dspw] msg is  ",result);
	if (result.length() > 20)
	{
		//check that status code = 200, and line[6] is = to known value.  indicates RCB is available
		StringArray lines = StringArray::fromLines(result);
		//LOGD("[dspw] line 0 is  ",lines[0]);
		//LOGD("[dspw] line 6 is  ",lines[6]);
		if (lines[0] == "Status code: 200" && lines[6] == "Unknown Token")
		{
			//RCB is available at specified IP Addr
			isGoodRCB = true;

			// get RCB battery voltage
			auto voltStr = lines[8].substring(11, 15);
            // battery voltage calc might be different for different RCB versions.
            //batteryInit = 1.026 * voltStr.getFloatValue();  //correct for 210k,100k,100k
            batteryInit = 0.995 * voltStr.getFloatValue(); //correct for 200k,100k,100k
            LOGD("[dspw] RCB Battery =  ", batteryInit, "V");
            
            //LOGD("[dspw] voltStr = ",voltStr, "V");
            //LOGD("[dspw] batteryInit =  ",batteryInit);
            
            if (batteryInit > BATT_INIT_THRESH)
            {
                batteryStatusInfo = ("Bat " + String(batteryInit, 2) + "V OK");
            }
            else if (batteryInit > (BATT_INIT_THRESH - 0.1))
            {
                batteryStatusInfo = ("Bat " + String(batteryInit, 2) + "V Low");
            }
            else
            {
                batteryStatusInfo = ("Bat " + String(batteryInit, 2) + "V Fail");
            }
			
			// format and display RHD registers 40-44,and 60-63.  See Intan RHD data
			// sheet page 23 for description of register values.

			// check that Intan RHD is connected and we can read regs over SPI
			auto intanStr = lines[12].substring(8, 28);
			//LOGD("[dspw] IntanStr =",intanStr);
			 
			if (intanStr == "0049004e00540041004e") // Spells INTAN
			{
				isGoodIntan = true;
				LOGD("[dspw] isGoodIntan = ",String(int(isGoodIntan)));

				String dieRev = lines[12].substring(30, 32);
				auto uniBi = lines[12].substring(34, 36);
				numAmps = (lines[12].substring(38, 40).getHexValue32());
				auto chipId = lines[12].substring(42, 44);
				
				if (chipId == "01")
				{
					// is RHD2132
					chipId = "RHD2132";
					int maxNumCh = 32;  
				}
				else if (chipId == "02")
				{
					// is RHD2216
					chipId = "RHD2216";
					int maxNumCh = 16;
					// need to expand on this to limit and set up the channels cbox 
					if (maxNumCh > num_channels)
					{
						isGoodIntan = false;
						AlertWindow::showMessageBox(AlertWindow::NoIcon,
							"Number of channels mismatch.",
							"Please check that Channel setting is not greater than Headstage Max channels. \r\n\r\n"
							"Press Init button to try again.",
							"OK", 0);
					}
				}

				if (uniBi == "00")
				{
					// is Bi polar
                    uniBi = "Bi";
				}
				else if (uniBi == "01")
				{
					// is Uni polar
                    uniBi = "Uni";
				}
                
                rhdStatusInfo = (chipId + "\n" + String(numAmps) + "Ch " + uniBi);
                intanAlertNum = 0;  //track how many intan alerts
				return "Intan init passed.";
			}
			else
			{
                isGoodIntan = false;
                rhdStatusInfo = ("Intan\nError!");
			//	if (intanAlertNum < 1)
                if (FACTORY_TEST_MODE == 0)
                {
                    initPassed = 0;
                    isGoodIntan = false;
                    
                    AlertWindow::showMessageBoxAsync(AlertWindow::NoIcon,
                                                     "Intan Headstage not found.",
                                                     "Please check SPI cable connections. \r\n\r\n"
                                                     "Press Init button to try again.",
                                                     "OK", 0);
                }
                intanAlertNum = intanAlertNum + 1;

				return "Intan init failed.";
				LOGD("[dspw] isGoodIntan = ",isGoodIntan);
			}
		}
	}
	else
	{
		isGoodRCB = false;
		/*AlertWindow::showMessageBox(AlertWindow::NoIcon,
			"RCB-LVDS Module not found at IP address " + ipNumStr,
			"Please check RCB IP Address setting,\nWiFi router configuration,\nand RCB battery power.\r\n\r\n"
			"Press Init button to try again.",
			"OK", 0);
			*/
		return "RCB init failed.";
	}
    return "Runt packet.";
}

String RcbWifi::getPacketInfo()
{
    float pdr = (float(hit)/float(seqNum)) * 100;
    //LOGD("[dspw] PDR = ",String((pdr), 2));
    packetInfo = ("Packet PDR: " + String(pdr, 3) + "%");
    packetInfo.append(("\nSQ N-" + String(seqNum)), 100);
	packetInfo.append(("\nGood-" + String(hit)), 100);
	packetInfo.append(("\nMiss-" + String(miss)), 100);
	return packetInfo;
}

String RcbWifi::getBatteryInfo()
{
	// convert battery voltage from fixed point to float, accounting for resistor divider
    // original for RCB-W24A-LVDS v1
    //float b = (0xfff & (batteryVolts >> 2)) * 1.467 / 4096 * 62.0 / 15.0; //correct for 320k,150k,150k
    
    // for RCB-W24B-LVDS v1, RCB-W24C v1, RCB-W24A v2
    float battV = (0xfff & (batteryVolts >> 2)) * 1.467 / 4096 * 40.0 / 9.75;  //correct for 200k,100k,100k
    batteryInit = battV;
    
    if (battV > BATT_STREAM_THRESH)
    {
        batteryInfo = ("Bat " + String(battV, 2) + "V OK");
    }
    else if (battV > (BATT_STREAM_THRESH - 0.1))
    {
        batteryInfo = ("Bat " + String(battV, 2) + "V Low");
    }
    else
    {
        batteryInfo = ("Bat " + String(battV, 2) + "V Fail");
        // should we stop acquisition if battery is this low ?
    }
	//Reset Battery Voltage
	batteryVolts = 0;
	return batteryInfo;
}

// taken from Juce network demo 
String RcbWifi::getResultText(const URL& url)
{
	StringPairArray responseHeaders;
	int statusCode = 0;

	std::unique_ptr<InputStream> urlStream = url.createInputStream(false, nullptr, nullptr, String(), 3000, &responseHeaders, &statusCode);

	if (urlStream != nullptr)
	{
		return (statusCode != 0 ? "Status code: " + String(statusCode) + "\n" : String())
			+ "Response headers: " + "\n"
			+ responseHeaders.getDescription() + "\n"
			+ "----------------------------------------------------" + "\n"
			+ urlStream->readEntireStreamAsString();
	}

	if (statusCode != 0)
		return "Failed to connect, status code = " + String(statusCode);

	return "Failed to connect!";
}


