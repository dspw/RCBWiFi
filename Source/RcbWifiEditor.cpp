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

#include "RcbWifiEditor.h"
#include "RcbWifi.h"

#include <string>
#include <iostream>

using namespace RcbWifiNode;

RcbWifiEditor::RcbWifiEditor(GenericProcessor* parentNode, RcbWifi* socket) : GenericEditor(parentNode)
{
	node = socket;
	desiredWidth = 412;
  
	String desiredFs[17] = { "30000", "25000", "20000", "15000", "12500", "10000", "8000", "6250", "5000", "4000", "3330", "3000", "2500", "2000", "1500", "1250", "1000" };
    
	String lowerBw[25] = { "500 Hz", "300 Hz", "250 Hz", "200 Hz", "150 Hz",
		"100 Hz", "75 Hz", "50 Hz", "30 Hz", "25 Hz", "20 Hz", "15 Hz", "10 Hz",
		"7.5 Hz", "5.0 Hz", "3.0 Hz", "2.5 Hz", "2.0 Hz", "1.5 Hz", "1.0 Hz",
		"0.75 Hz", "0.50 Hz", "0.30 Hz", "0.25 Hz", "0.10 Hz" };

	String upperBw[17] = { "20 kHz", "15 kHz", "10 kHz", "7.5 kHz", "5.0 kHz",
		"3.0 kHz", "2.5 kHz", "2.0 kHz", "1.5 kHz", "1.0 kHz", "750 Hz", "500 Hz",
		"300 Hz", "250 Hz", "200 Hz", "150 Hz", "100 Hz" };

    String pollRate[10] = {"OFF", "1", "2", "3", "4", "5", "10", "15", "20", "30" };
 
    // version label
    versionLabel = new Label("Version", PLUGIN_VERSION);
    versionLabel->setBounds(360, 6, 60, 15);
    versionLabel->setFont(Font("CP Mono", "Plain", 15));
    versionLabel->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(versionLabel);
    LOGC("[dspw] RCB Wi-Fi version = ", PLUGIN_VERSION);

	// Num chans
	chanLabel = new Label("NumCh", "Channels");
	chanLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	chanLabel->setBounds(188, 62, 75, 12);
	chanLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(chanLabel);

	chanCbox = new ComboBox();
    chanCbox->setBounds(190, 74, 36, 17);
    chanCbox->setTooltip("Choose number of RHD2000 channels to stream over WiFi.");

	chanCbox->addListener(this);
	for (int i = 8; i > 0; i--)
		chanCbox->addItem(String(i * 4), i);
	chanCbox->setSelectedId(8, dontSendNotification);
	addAndMakeVisible(chanCbox);
    
    chStartNumLabel = new Label("chStartNumLabel", "1");
    chStartNumLabel->setBounds(228, 74, 21, 17);
    chStartNumLabel->setTooltip("Enter Starting Channel. Default is 1.");

    chStartNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    chStartNumLabel->setColour(Label::textColourId, Colours::black);
    chStartNumLabel->setColour(Label::backgroundColourId, Colours::lightgrey);
    chStartNumLabel->setEditable(true);
    chStartNumLabel->addListener(this);
    addAndMakeVisible(chStartNumLabel);

	//  Desired Sample Rate  Fs
	fsLabel = new Label("Fs(Hz)", "Fs (Hz)");
	fsLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    fsLabel->setBounds(188, 28, 65, 12);
	fsLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(fsLabel);

	fsCbox = new ComboBox();
    fsCbox->setBounds(190, 40, 58, 17);
    fsCbox->setTooltip("Choose desired nominal Fs.");

	fsCbox->addListener(this);
	for (int i = 0; i < 17; i++)
		fsCbox->addItem(desiredFs[i], i + 1);
	fsCbox->setSelectedId(3, dontSendNotification);
	addAndMakeVisible(fsCbox);

	// RHD Upper BW
	upBwLabel = new Label("upBW", "Upper");
	upBwLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	upBwLabel->setBounds(120, 62, 85, 10);
	upBwLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(upBwLabel);

	upBwCbox = new ComboBox();
	upBwCbox->setBounds(122, 74, 59, 17);
    upBwCbox->setTooltip("Choose RHD2000 Filter Upper Freq.");

	upBwCbox->addListener(this);
	for (int i = 0; i < 17; i++)
		upBwCbox->addItem(upperBw[i], i + 1); // start numbering at one for
	upBwCbox->setSelectedId(4, dontSendNotification);
	addAndMakeVisible(upBwCbox);

	// RHD Lower BW
	lowBwLabel = new Label("lowBW", "Lower");
	lowBwLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	lowBwLabel->setBounds(120, 97, 85, 12);
	lowBwLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(lowBwLabel);

	lowBwCbox = new ComboBox();
	lowBwCbox->setBounds(122, 109, 59, 17);
    lowBwCbox->setTooltip("Choose RHD2000 Filter Lower Freq.");
	lowBwCbox->addListener(this);
	for (int i = 0; i < 25; i++)
		lowBwCbox->addItem(lowerBw[i], i + 1); // start numbering at one for
	lowBwCbox->setSelectedId(20, dontSendNotification);
	addAndMakeVisible(lowBwCbox);

	// RHD DSP Cutoff
	dspCutLabel = new Label("dspCutLabel", "DSP  HPF");
	dspCutLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	dspCutLabel->setBounds(120, 28, 85, 12);
	dspCutLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(dspCutLabel);

	dspCutNumLabel = new Label("dspCutNumLabel", "1");
	dspCutNumLabel->setBounds(153, 40, 28, 17);
	dspCutNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	dspCutNumLabel->setColour(Label::textColourId, Colours::black);
    dspCutNumLabel->setColour(Label::backgroundColourId, Colours::lightgrey);
	dspCutNumLabel->setEditable(true);
	dspCutNumLabel->addListener(this);
	addAndMakeVisible(dspCutNumLabel);

	// add DSP Offset Button
	dspOffsetButton = new UtilityButton("DSP:", Font("Small Text", 13, Font::plain));
	dspOffsetButton->setRadius(3.0f);
	dspOffsetButton->setBounds(122, 40, 29, 17);
	dspOffsetButton->addListener(this);
	dspOffsetButton->setClickingTogglesState(true);
	dspOffsetButton->setTooltip("Toggle DSP offset removal");
	addAndMakeVisible(dspOffsetButton);
	dspOffsetButton->setToggleState(true, dontSendNotification);

    // Battery Poll timer rate
    pollRateCbox = new ComboBox();
    pollRateCbox->setBounds(359, 109, 44, 17);
    pollRateCbox->addListener(this);
    for (int i = 0; i < 10; i++)
        pollRateCbox->addItem(pollRate[i], i + 1 ); // start numbering at one for
    pollRateCbox->setTooltip("Battery Poll Rate Timer(m)");
    pollRateCbox->setSelectedId(1, sendNotification);
    addAndMakeVisible(pollRateCbox);
    
    pollRateLabel = new Label("pollRateLabel", "Bat Poll\nRate(m)");
    pollRateLabel->setBounds(354, 82, 65, 26);
    pollRateLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    pollRateLabel->setColour(Label::textColourId, Colours::black);
    pollRateLabel->addListener(this);
    addAndMakeVisible(pollRateLabel);
    
    auxEnableButton = new UtilityButton("AUX", Font("Small Text", 13, Font::plain));
    auxEnableButton->setRadius(3.0f);
    auxEnableButton->setBounds(320, 81, 29, 24);
    auxEnableButton->addListener(this);
    auxEnableButton->setClickingTogglesState(true);
    auxEnableButton->setTooltip("Enable RHD AUX Inputs");
    addAndMakeVisible(auxEnableButton);
    auxEnableButton->setToggleState(false, dontSendNotification);

	// UDP Packet Hit Miss
    seqNumLabel = new Label("seqNum", "Rx Packet PDR:PDR%\nSQ N: 0\nGood: 0\nMiss: 0");
	seqNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    seqNumLabel->setBounds(254, 27, 150, 52);
	seqNumLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(seqNumLabel);

	// battery Label
    batteryLabel = new Label("batteryVolts", "Bat 0.00V ---");
	batteryLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    batteryLabel->setBounds(254, 112, 100, 13);
	batteryLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(batteryLabel);

	// RHD read only regs label
    rhdRegsLabel = new Label("rhdRegs", "Intan\nStatus ");
	rhdRegsLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    rhdRegsLabel->setBounds(254, 82, 70, 26);
	rhdRegsLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(rhdRegsLabel);

	// RCB IP address label
	destIpLabel = new Label("RCBIP", "RCB IP Addr:");
	destIpLabel->setBounds(8, 28, 130, 12);
	destIpLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	addAndMakeVisible(destIpLabel);

	rcbIpNumLabel = new Label("ipNumLabel", "192.168.0.93"); //default RCB-LVDS IP number is 192.168.0.93
	rcbIpNumLabel->setTooltip("Default RCB-LVDS IP number is 192.168.0.93");
	rcbIpNumLabel->setBounds(10, 41, 100, 15);
	rcbIpNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	rcbIpNumLabel->setColour(Label::textColourId, Colours::black);
	rcbIpNumLabel->setColour(Label::backgroundColourId, Colours::white);
	rcbIpNumLabel->setEditable(true);
	rcbIpNumLabel->addListener(this);
	addAndMakeVisible(rcbIpNumLabel);

	// Host IP address
	hostIpLabel = new Label("DestIP", "Host IP Addr:");
	hostIpLabel->setBounds(8, 61, 130, 12);
	hostIpLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	addAndMakeVisible(hostIpLabel);

	hostIpNumLabel = new Label("HostIP", "Press Init!");
    hostIpNumLabel->setTooltip("Enter IP address of computer that is running the GUI.");
	hostIpNumLabel->setBounds(10, 74, 100, 15);
	hostIpNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	hostIpNumLabel->setColour(Label::textColourId, Colours::black);
	hostIpNumLabel->setColour(Label::backgroundColourId, Colours::white);
	hostIpNumLabel->setEditable(true);
	hostIpNumLabel->addListener(this);
	addAndMakeVisible(hostIpNumLabel);
	//hostIpIsValid = false;  

	// Host Port
	portLabel = new Label("Port", "Port");
	portLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	portLabel->setBounds(8, 96, 65, 12);
	portLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(portLabel);

	// Host Port number
	portNumLabel = new Label("PortNum", String(DEFAULT_PORT));
    portNumLabel->setTooltip("Enter UDP port number between 49152 to 65535.");

	portNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	//   portNumLabel->setText(std::to_string(node->port), dontSendNotification);
	portNumLabel->setBounds(10, 109, 40, 15); //42
	portNumLabel->setColour(Label::textColourId, Colours::black);
	portNumLabel->setColour(Label::backgroundColourId, Colours::white);
	portNumLabel->setEditable(true);
	portNumLabel->addListener(this);
	addAndMakeVisible(portNumLabel);

	// Init Button Label
    initLabel = new Label("Init", "Configure"); 
	initLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	initLabel->setBounds(52, 96, 65, 12);
	initLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(initLabel);

	// Init button
    initButton = new UtilityButton("Init Me!", Font("Small Text", 13, Font::bold));
    initButton->setTooltip("Press to Initialize RCB device with UI values.");

	initButton->setBounds(57, 108, 55, 18);
	initButton->addListener(this);
	addAndMakeVisible(initButton.get());
	
	// RCB PA Power
	paPwrLabel = new Label("PAPWR", "PA Attn");
	paPwrLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
	paPwrLabel->setBounds(188, 96, 65, 12);
	paPwrLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(paPwrLabel);

	paPwrCbox = new ComboBox();
	paPwrCbox->setBounds(190, 108, 58, 17);
	paPwrCbox->addListener(this);
	for (int i = 0; i < 16; i++)
		paPwrCbox->addItem(String(i), i + 1); // start numbering at one for
	paPwrCbox->setSelectedId(5, dontSendNotification);
	paPwrCbox->setTooltip("RCB WiFi PA Attenuation. Best when set at 4.");
	addAndMakeVisible(paPwrCbox);

	// try to get host ip before init
    // this should be expanded to allow selection if multiple network interfaces are present.
	myHost = getCurrentIpAddress();
    LOGD("[dspw] Host IP = ",myHost.toString());
	hostIpNumLabel->setText(myHost.toString(), dontSendNotification);

	//AlertWindow::showMessageBox(AlertWindow::NoIcon,
	//	"OE GUI Host IP address is " + hostIpNumLabel->getText() + "",
	//	"Please check your Host IP address setting is correct. \r\n"
	//	"",
	//	"OK", 0);
}

void RcbWifiEditor::startAcquisition()
{
	// Check if initPassed is true.  If not the must press Init Button.
	if (node->initPassed == true)
	{
		// Disable the whole gui until stopAcquisition()
		rcbIpNumLabel->setEnabled(false);
		hostIpNumLabel->setEnabled(false);
		portNumLabel->setEnabled(false);
		chanCbox->setEnabled(false);
        chStartNumLabel->setEnabled(false);
		fsCbox->setEnabled(false);
		upBwCbox->setEnabled(false);
		lowBwCbox->setEnabled(false);
		paPwrCbox->setEnabled(false);
		dspCutNumLabel->setEnabled(false);
		dspOffsetButton->setEnabled(false);
        auxEnableButton->setEnabled(false);
        pollRateCbox->setEnabled(false);
        initButton->setEnabled(false);

		timeInt = 0;
        stopTimer(2); // stop battery voltage update timer
		startTimer(1, 1000); //packet update timer
	}
	else {
		//re enable GUI
		rcbIpNumLabel->setEnabled(true);
		hostIpNumLabel->setEnabled(true);
		portNumLabel->setEnabled(true);
		chanCbox->setEnabled(true);
        chStartNumLabel->setEnabled(true);
		fsCbox->setEnabled(true);
		upBwCbox->setEnabled(true);
		lowBwCbox->setEnabled(true);
		paPwrCbox->setEnabled(true);
		dspCutNumLabel->setEnabled(true);
		dspOffsetButton->setEnabled(true);
        auxEnableButton->setEnabled(true);
        pollRateCbox->setEnabled(true);
        initButton->setEnabled(true);

	}
}

void RcbWifiEditor::timerCallback(int timerID)
{
    // timer 1 is used while streaming data. updates packet info and battery voltage display in plugin
	if (timerID == 1)
	{
		seqNumLabel->setText(node->getPacketInfo(), dontSendNotification);

		if (timeInt == 0)
		{
			// either of these approaches witll work, not sure if advantage to either
			//this
			node->getBatteryInfo();
			batteryLabel->setText(node->batteryInfo, dontSendNotification);

			// or this
			//batteryLabel->setText(node->getBatteryInfo(), dontSendNotification);

			// to slow battery display update and keep Packet update more speedy. Or have two timers? Or use MultiTimer?
			timeInt = 5;
		}
		timeInt--;
       
    // timer 2 is used when not streaming data.  checks that RCB is still alive on network and updates battery voltage display.
	}else if (timerID == 2)
    {
        //first check that RCB init happens ok
		String htmlStatus =	node->getIntanStatusInfo();

		if (node->isGoodRCB == true)
		{
            rcbIsLost = 0;
			rhdRegsLabel->setText(node->rhdStatusInfo, dontSendNotification);
			batteryLabel->setText(node->batteryStatusInfo, dontSendNotification);
			return;
		}
		else
		{
            // Give RCB more than one network status poll to respond it is alive
            if (rcbIsLost > 1)
            {
                stopTimer(2);
                CoreServices::setAcquisitionStatus(false);
                initButton->setLabel("Init");
                rcbIsLost = 0;
                node->initPassed = false;
                
                AlertWindow::showMessageBox(AlertWindow::NoIcon,
                    "RCB-LVDS Module not found at IP address " + node->ipNumStr,
                    "Please check RCB IP Address setting,\nWiFi router configuration,\nand RCB battery power.\r\n\r\n"
                    "Press Init button to try again.",
                    "OK", 0);
            }
            rcbIsLost++;
		}
	}
}

void RcbWifiEditor::stopAcquisition()
{
	stopTimer(1); // stop UDP packet update Timer
   
    if (timer2Disable == false)
    {
        if (node->initPassed == 1)
        {
            // get timer polling rate from comboBox
            int pollRate = pollRateCbox->getText().getIntValue();
            timer2Rate = pollRate;
            
            //Start poll rate timer.  pollRate comboBox value x 1 min
            startTimer(2, timer2Rate * 60000);  // start battery voltage update timer
        }
    }
	// Reenable the whole gui
	rcbIpNumLabel->setEnabled(true);
	hostIpNumLabel->setEnabled(true);
	portNumLabel->setEnabled(true);
	chanCbox->setEnabled(true);
    chStartNumLabel->setEnabled(true);
	fsCbox->setEnabled(true);
	upBwCbox->setEnabled(true);
	lowBwCbox->setEnabled(true);
	paPwrCbox->setEnabled(true);
	dspCutNumLabel->setEnabled(true);
	dspOffsetButton->setEnabled(true);
    auxEnableButton->setEnabled(true);
    pollRateCbox->setEnabled(true);
    initButton->setEnabled(true);

}

IPAddress RcbWifiEditor::getCurrentIpAddress()
{
	Array<IPAddress> ipAddresses;
	IPAddress::findAllAddresses(ipAddresses);
	// return first non local addr
	for (int i = 0; i < ipAddresses.size(); ++i)
	{
		if (ipAddresses[i] != IPAddress::local())
        {
            //LOGD("[dspw] Host IP = ",ipAddresses[i].toString());
            return ipAddresses[i];
        }
	}
    
	return IPAddress();
}

void RcbWifiEditor::buttonClicked(Button* button)
{
    String hostStr = "";
    int rhdNumTsItems = 0;

	if (button == initButton.get())
    {
        if (uiIsOk == true)
        {
            rcbIsLost = 0;
            initButton->setLabel("Init");
            node->initPassed = false;
            //node->isGoodRCB == false; // is done in node init button pressed
            //node->isGoodIntan == false; // is done in node init button pressed
            
            LOGD("[dspw] Init Button Pressed");
            
            node->port = portNumLabel->getText().getIntValue();
            node->ipNumStr = rcbIpNumLabel->getText();
            
            hostStr = hostIpNumLabel->getText();
            LOGC("[dspw] RCB IP = ",node->ipNumStr);
            LOGC("[dspw] Host IP = ",myHost.toString());
            
            //check if host and RCB are on same network and subnet.  if not then they cannot communicate
            if (node->ipNumStr.substring(0, 8) == hostStr.substring(0, 8))
            {
                LOGD("[dspw] IP compare OK ");
                
                // set up host string with ip and port
                node->myHostStr = hostStr + ":" + portNumLabel->getText();
                
                //first check that RCB exists on network, get battery voltage
                String htmlStatus = node->getIntanStatusInfo();
                LOGD("[dspw] htmlStatus -  ", htmlStatus);
                
                if (node->isGoodRCB == true)
                {
                    batteryLabel->setText(node->batteryStatusInfo, dontSendNotification);
                    
                    // send stop stream command in case it is alreadry started ?
                    
                    //then report if intan rhd is working ok
                    rhdRegsLabel->setText(node->rhdStatusInfo, dontSendNotification);
                    
                    // if Intan RHD is good then continue setup.  but why do we care?
                    if (node->isGoodIntan == true || FACTORY_TEST_MODE == 1)
                    {
                        // set up rf pa attn
                        node->rcbPaStr = paPwrCbox->getText();
                        
                        // get number of channels from dropdown box
                        int num_channels = chanCbox->getText().getIntValue();
                        node->num_channels = num_channels;
                        
                        // get channel start number from label
                        int chShift = chStartNumLabel->getText().getIntValue();
                        node->chShift = chShift;
                        
                        if (chShift + (num_channels - 1) > 32)
                        {
                            AlertWindow::showMessageBox(AlertWindow::NoIcon,
                                "Channel Start Number value " + String(chShift) + " is not valid \r\n"
                                "when Number of Channels is " + String(num_channels) + ". \r\n"
                                "Combination must be between 1 and 33.",
                                "Please check your Channel settings. \r\n"
                                "",
                                "OK", 0);
                        
                            return;
                        }
                        
                        // get number of samples in each packet
                        // used to compute size of recbuf and convbuff
                        int rhdNumTsItems = chanCbox->getSelectedItemIndex();
                        node->num_samp = numTsItems[rhdNumTsItems];
                        
                        // get desired sample rate from combo box
                        node->desiredSampleRate = fsCbox->getText().getFloatValue();
                        node->sample_rate = node->updateSampleRate();
                        
                        // get RHD AUX enable state.  will affect resize buffers
                        node->auxEnableState = auxEnableButton->getToggleState();
                        
                        // update signal chain with new actual sample rate and number of channels
                        CoreServices::updateSignalChain(this);
                        
                        // get mux and ADC bias.  depends on actual sample rate.
                        node->getMuxAdcBias(node->sample_rate);
                        
                        // get DSP HPF value and enable state
                        node->dspHpfValue = dspCutNumLabel->getText().getFloatValue();
                        node->dspHpfState = dspOffsetButton->getToggleState();
                        float dspHpfCut = node->setDspCutoffFreq(dspCutNumLabel->getText().getFloatValue(), node->sample_rate);
                        //LOGD("[dspw] MY dspCut = ",dspHpfCut);
                        std::stringstream stream;
                        stream << std::fixed << std::setprecision(1) << dspHpfCut;
                        std::string s = stream.str();
                        dspCutNumLabel->setText(s, dontSendNotification);
                        
                        // get upper BW filter from combo box
                        node->rhdUpBwInt = upBwCbox->getSelectedItemIndex();
                        
                        // get lower BW filter from combo box
                        node->rhdLowBwInt = lowBwCbox->getSelectedItemIndex();
                        
                        // develop correct RHD Register values and create string
                        node->setRCBTokens();
                        
                        // if tokens are sent ok then RCB WiFi should be initialized!
                        node->initPassed = true;
                        initButton->setLabel("Ready");
                        
                        //Start poll rate timer.  pollRate comboBox value x 1 min
                        // poll rate timer is only active when NOT streaming data
                        // check pollrate comboBox value first
                        
                        if (timer2Disable == false)
                        {
                            // get timer polling rate from comboBox
                            int pollRate = pollRateCbox->getText().getIntValue();
                           
                            timer2Rate = pollRate;
                            
                            startTimer(2, timer2Rate * 60000);
                            LOGD("[dspw] timer poll rate = ",String(int(timer2Rate))," sec");
                        }else
                        {
                            //stopTimer(2);
                            LOGD("[dspw] timer poll rate = OFF");
                        }
                    }
                }
                else {
                    LOGD("[dspw] isGoodRCB = false");
                    AlertWindow::showMessageBox(AlertWindow::NoIcon,
                                                "RCB-LVDS Module not found at IP address " + node->ipNumStr,
                                                "Please check RCB IP Address setting,\nWiFi router configuration,\nand RCB battery power.\r\n\r\n"
                                                "Press Init button to try again.",
                                                "OK", 0);
                    return;
                }
            }
            else
            {
                LOGD("[dspw] IP compare failed ");
                
                AlertWindow::showMessageBox(AlertWindow::NoIcon,
                                            "RCB and Host IP network mismatch.",
                                            "Check your Wifi network connection. \r\n\r\n"
                                            "Press Init button to try again.",
                                            "OK", 0);
            }
        }
        else
        {
            LOGD("[dspw] UI Channel values failed ");
            
            AlertWindow::showMessageBox(AlertWindow::NoIcon,
                                        "Number of channels + channel Start canot be greater than 33.",
                                        "Check your RCB Plugin UI Channel settings. \r\n\r\n"
                                        "Press Init button to try again.",
                                        "OK", 0);
        }
    }
    
	else if (button == dspOffsetButton)
	{
		// get toggle state
		node->initPassed = false;
		initButton->setLabel("Init");
	}
    else if (button == auxEnableButton)
    {
        // get toggle state
        node->initPassed = false;
        initButton->setLabel("Init");
    }
}


void RcbWifiEditor::labelTextChanged(juce::Label* label)
{
	bool ipIsValid = false;
	bool hostIpIsValid = false;
	bool portIsValid = false;
	bool dspHpfIsValid = false;
    bool chStartIsValid = false;

	node->initPassed = false;
	initButton->setLabel("Init");
	if (label == rcbIpNumLabel)
	{
		IPAddress ip = IPAddress(rcbIpNumLabel->getText());
		if (ip.toString() == rcbIpNumLabel->getText())
		{
			ipIsValid = true;
		}
		else
		{
			ipIsValid = false;
			AlertWindow::showMessageBox(AlertWindow::NoIcon,
				"RCB-LVDS Module IP address " + rcbIpNumLabel->getText() + "is not valid.",
				"Please check your IP address setting. \r\n"
				"",
				"OK", 0);

			// reset ip address to default
			String ipStr = "192.168.0.93";
			rcbIpNumLabel->setText(ipStr, sendNotification);
		}
		LOGD("[dspw] RCB IP valid = ",ipIsValid);
	}
	else if (label == hostIpNumLabel)
	{
		IPAddress host = IPAddress(hostIpNumLabel->getText());
		if (host.toString() == hostIpNumLabel->getText())
		{
			hostIpIsValid = true;
		}
		else
		{
			hostIpIsValid = false;

			AlertWindow::showMessageBox(AlertWindow::NoIcon,
				"OE GUI Host IP address " + hostIpNumLabel->getText() + " is not valid.",
				"Please check your Host IP address setting. \r\n"
				"",
				"OK", 0);
			
			hostIpNumLabel->setText(myHost.toString(), sendNotification);
		}
		LOGD("[dspw] Host IP valid = ",hostIpIsValid);

	}
	else if (label == portNumLabel)
	{
		Value val = label->getTextValue();
		int requestedValue = int(val.getValue());

        if (requestedValue < 49151 || requestedValue > 65535)
		{
			portIsValid = false;
			
			AlertWindow::showMessageBox(AlertWindow::NoIcon,
				"OE GUI Host Port " + portNumLabel->getText() + " is not valid.",
				"Please check your Host Port setting is between 49152-65535. \r\n"
				"",
				"OK", 0);
		}
		else
		{
			portIsValid = true;

		}
	}
	else if (label == dspCutNumLabel)
	{
		Value val = label->getTextValue();
        float requestedValue = float(val.getValue());
        LOGD("[dspw] dspCut requested value = ",String(float(requestedValue)));
        
		if (requestedValue < 0.1 || requestedValue > 1000)  // not sure if these limits are correct
		{
			dspHpfIsValid = false;
			AlertWindow::showMessageBox(AlertWindow::NoIcon,
				"OE GUI DSP HPF value " + dspCutNumLabel->getText() + " is not valid.",
				"Please check your DSP HPF setting. \r\n"
				"",
				"OK", 0);
		}
		else
		{
			dspHpfIsValid = true;
		}
	}
    else if (label == chStartNumLabel)
    {
        Value val = label->getTextValue();
        int requestedValue = int(val.getValue());
        LOGD("[dspw] chStartNum requested value = ",String(int(requestedValue)));
        
        int numChannels = chanCbox->getText().getIntValue();
        if (requestedValue < 1 || requestedValue + numChannels-1 > 32)
        {
            chStartIsValid = false;
            uiIsOk =false;
            
            AlertWindow::showMessageBox(AlertWindow::NoIcon,
                "Channel Start Number value " + String(int(requestedValue)) + " is not valid \r\n"
                "when Number of Channels is " + chanCbox->getText() +". \r\n"
                "Combination must be between 1 and 33.",
                "Please check your Channel Start Number setting. \r\n"
                "",
                "OK", 0);
        }
        else
        {
            chStartIsValid = true;
            uiIsOk = true;
        }
    }
}

void RcbWifiEditor::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	node->initPassed = false;
	initButton->setLabel("Init");

	// currently at Init Button all comboBoxes are queried for latest values
	// can also make updates here.

	if (comboBoxThatHasChanged == chanCbox)
	{
      //  fsCbox->clear();
        // get number of channels from comboBox
        int num_channels = chanCbox->getText().getIntValue();
        Value chStartVal = chStartNumLabel->getTextValue();
        int requestedValue = int(chStartVal.getValue());
        
        if ( requestedValue + num_channels - 1 > 32)
        {
        //chStartIsValid = false;
        uiIsOk = false;
        
        AlertWindow::showMessageBox(AlertWindow::NoIcon,
            "Channel Start Number value " + String(int(requestedValue)) + " is not valid \r\n"
            "when Number of Channels is " + chanCbox->getText() +". \r\n"
            "Combination must be between 1 and 33.",
            "Please check your Channel Start Number setting. \r\n"
            "",
            "OK", 0);
            
            return;
        }
        
        // if channels = 32 then sample rte must be 20ksps or less
        if(chanCbox->getText() == "32")
        {
            if (fsCbox->getSelectedItemIndex() < 2)
            {
                fsCbox->setSelectedItemIndex(2);
                AlertWindow::showMessageBox(AlertWindow::NoIcon,
                    "Sample Rate value " + fsCbox->getText() + " is not valid \r\n"
                    "when Number of Channels is " + chanCbox->getText() +". \r\n"
                    "If channels = 32 Sample Rate must be 20,000 or lower.\r\n",
                    "Please check your Channel Numbers setting. \r\n"
                    "",
                    "OK", 0);
            }
        }
            //chStartIsValid = true;
        uiIsOk = true;
        
        // get number of channels from comboBox
        // int num_channels = chanCbox->getText().getIntValue();
        node->num_channels = num_channels;
        
        // get number of samples in each packet
        // used to compute size of recbuf and convbuff
        int rhdNumTsItems = chanCbox->getSelectedItemIndex();
        node->num_samp = numTsItems[rhdNumTsItems];
        node->sample_rate = node->updateSampleRate();
        CoreServices::updateSignalChain(this);
            
	}
	else if (comboBoxThatHasChanged == fsCbox)
	{
        // if channels = 32 then sample rte must be 20ksps or less
        if(chanCbox->getText() == "32")
        {
            if (fsCbox->getSelectedItemIndex() < 2)
            {
                fsCbox->setSelectedItemIndex(2);
                AlertWindow::showMessageBox(AlertWindow::NoIcon,
                    "Sample Rate value " + fsCbox->getText() + " is not valid \r\n"
                    "when Number of Channels is " + chanCbox->getText() +". \r\n"
                    "If channels = 32 Sample Rate must be 20,000 or lower.",
                    "Please check your Channel Number setting. \r\n"
                    "",
                    "OK", 0);
                
                //fsCbox->setSelectedItemIndex(2);
            }
        }
		// get desired sample rate from combo box
		node->desiredSampleRate = fsCbox->getText().getFloatValue();
		node->sample_rate = node->updateSampleRate();
		CoreServices::updateSignalChain(this);
	}
    else if (comboBoxThatHasChanged == pollRateCbox)
    {
        //first check if timer poll rate should be on/off
        String pollRateStr = pollRateCbox->getText();
        if (pollRateStr == "OFF")
        {
            timer2Disable = true;
            stopTimer(2);
        }else
        {
            // get timer polling rate from comboBox
            int pollRate = pollRateCbox->getText().getIntValue();
            timer2Disable = false;
            timer2Rate = pollRate;
        }
        
    }
	else if (comboBoxThatHasChanged == upBwCbox)
	{

	}
	else if (comboBoxThatHasChanged == lowBwCbox)
	{

	}
	else if (comboBoxThatHasChanged == paPwrCbox)
	{

	}
}

void RcbWifiEditor::saveCustomParametersToXml(XmlElement* xmlNode)
{
	XmlElement* parameters = xmlNode->createNewChildElement("PARAMETERS");

	parameters->setAttribute("rcbIpAddr", rcbIpNumLabel->getText());
	parameters->setAttribute("hostIpAddr", hostIpNumLabel->getText());
	parameters->setAttribute("portNumber", portNumLabel->getText());
	parameters->setAttribute("dspHpfCut", dspCutNumLabel->getText());
	parameters->setAttribute("dspOffBut", dspOffsetButton->getToggleState());
	parameters->setAttribute("numChan", chanCbox->getSelectedItemIndex());
    parameters->setAttribute("startChan", chStartNumLabel->getText());
	parameters->setAttribute("fs", fsCbox->getSelectedItemIndex());
	parameters->setAttribute("upBw", upBwCbox->getSelectedItemIndex());
	parameters->setAttribute("lowBw", lowBwCbox->getSelectedItemIndex());
	parameters->setAttribute("paPwr", paPwrCbox->getSelectedItemIndex());
    parameters->setAttribute("pollRate", pollRateCbox->getSelectedItemIndex());
    parameters->setAttribute("auxEnBut", auxEnableButton->getToggleState());

}

void RcbWifiEditor::loadCustomParametersFromXml(XmlElement* xmlNode)
{
	forEachXmlChildElement(*xmlNode, subNode)
	{
		if (subNode->hasTagName("PARAMETERS"))
		{
			rcbIpNumLabel->setText(subNode->getStringAttribute("rcbIpAddr",""), dontSendNotification);
			hostIpNumLabel->setText(subNode->getStringAttribute("hostIpAddr", ""), dontSendNotification);
			portNumLabel->setText(subNode->getStringAttribute("portNumber", ""), dontSendNotification);
			dspCutNumLabel->setText(subNode->getStringAttribute("dspHpfCut", ""), dontSendNotification);
			dspOffsetButton->setToggleState(subNode->getBoolAttribute("dspOffBut", false), dontSendNotification);
			chanCbox->setSelectedItemIndex(subNode->getIntAttribute("numChan",0),dontSendNotification);
            chStartNumLabel->setText(subNode->getStringAttribute("startChan", ""),dontSendNotification);
			fsCbox->setSelectedItemIndex(subNode->getIntAttribute("fs", 2), dontSendNotification);
			upBwCbox->setSelectedItemIndex(subNode->getIntAttribute("upBw", 4), dontSendNotification);
			lowBwCbox->setSelectedItemIndex(subNode->getIntAttribute("lowBw", 19), dontSendNotification);
			paPwrCbox->setSelectedItemIndex(subNode->getIntAttribute("paPwr", 5), dontSendNotification);
            pollRateCbox->setSelectedItemIndex(subNode->getIntAttribute("pollRate", 0), dontSendNotification);
            auxEnableButton->setToggleState(subNode->getBoolAttribute("auxEnBut", false), dontSendNotification);

		}
	}
    // this is needed due to possible old hostAddr saved in Paremeters
    myHost = getCurrentIpAddress();
    hostIpNumLabel->setText(myHost.toString(), dontSendNotification);

}
