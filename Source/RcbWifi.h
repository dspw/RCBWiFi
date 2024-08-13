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

#ifndef __RCBWIFIH__
#define __RCBWIFIH__

#include <DataThreadHeaders.h>

#include <list>
#include <vector>
#include <string>
#include <iostream>

// from ephysSocket
const int DEFAULT_PORT = 51234; //4416;
const float DEFAULT_SAMPLE_RATE = 20639.834; //for 16 at 20k 20768.433f;
const float DEFAULT_DATA_SCALE = 0.195f;
const uint16_t DEFAULT_DATA_OFFSET = 32768;
const int DEFAULT_NUM_SAMPLES = 21; // this is num samples in udp rxbuffer.  
const int DEFAULT_NUM_CHANNELS = 32;

// Battery thresholds needed to start data streams and keep them going
const int BATT_INIT_THRESH = 3.6;
const int BATT_STREAM_THRESH = 3.6;

// Factory Test mode.  Allows RCB streaming without Intan RHD.
// Or is this the prefered behavior. Always allow streaming regardless of Intan connected.
const bool FACTORY_TEST_MODE = 1;

// Plugin Version
const String PLUGIN_VERSION = "v0.1.1";

//#define CHIP_ID_RHD2132  1
//#define CHIP_ID_RHD2216  2
//const int DEFAULT_UPPERBW = 7500;
//const int DEFAULT_LOWERBW = 1;

namespace RcbWifiNode
{
    class RcbWifi : public DataThread   //, public Timer
    {

    public:
        /** Constructor */
        RcbWifi(SourceNode* sn);

        /** Destructor */
        ~RcbWifi();

        /** Create the DataThread custom editor */
        std::unique_ptr<GenericEditor> createEditor(SourceNode* sn) override;

        /** Create the DataThread object*/
        static DataThread* createDataThread(SourceNode* sn); 

        /** Returns true if the data source is connected.*/
        bool foundInputSource() override; 

        /** Sets info about available channels */
        void updateSettings(OwnedArray<ContinuousChannel>* continuousChannels,  
            OwnedArray<EventChannel>* eventChannels,
            OwnedArray<SpikeChannel>* spikeChannels,
            OwnedArray<DataStream>* sourceStreams,
            OwnedArray<DeviceInfo>* devices,
            OwnedArray<ConfigurationObject>* configurationObjects) override;
  
        /** Resizes buffers when input parameters are changed*/
        void resizeBuffers() override;

        /** Attempts to reconnect to the socket */
        void tryToConnect();

        /** Network stream parameters (must match features of incoming data) */
        int port = 0;
        float sample_rate = 0;
        float data_scale = 0;
        uint16_t data_offset = 0;
        int num_samp = 0;
        int num_channels = 0;

        // ** Allows the DataThread plugin to respond to messages sent by other processors */
        void handleBroadcastMessage(String msg) override;

        // ** Allows the DataThread plugin to handle a config message while acquisition is not active. */
        String handleConfigMessage(String msg) override;
        
        // RCB Specific
        int desiredSampleRate = 0;
        float dspHpfValue = 0;
        bool dspHpfState = 0;
        int rhdUpBwInt = 0;
        int rhdLowBwInt = 0;
        //int upBwRh1Dac1;  // not used
        int chShift = 0x00;
        bool auxEnableState = false;

        String ipNumStr = "";
        String myHostStr = "";
        String rcbMsgStr = "";
        String rcbPaStr = "";
        String rhdNumChStr = "";

        bool isGoodIntan = false;
        bool isGoodRCB = false;
        bool initPassed = false;

        String getIntanStatusInfo();
        String batteryStatusInfo;
        String rhdStatusInfo;
        String getPacketInfo();
        String packetInfo;
        String getBatteryInfo();
        String batteryInfo;

        void setRCBTokens();
     
        float updateSampleRate();
        void getMuxAdcBias(float sampleRate);
        double setDspCutoffFreq(double newDspCutoffFreq, float sampleRate);
        
    private:

        /** Receives data from network and pushes it to the DataBuffer */
        bool updateBuffer() override; //oe

        /** Resets variables and starts thread*/
        bool startAcquisition() override;

        /** Stops thread */
        bool stopAcquisition()  override; 

        /** Sample index counter */
        int64 total_samples = 0;

        /** Local event state variable */
        uint64 eventState = 0;

        /** True if socket is connected */
        bool connected = false;

        /** UPD socket object */
        std::unique_ptr<DatagramSocket> socket;

        /** Internal buffers */
        uint16_t* recvbuf;
        float* convbuf;
        uint16_t* auxbuf;
        
        // Intan RHD stuff
        int numAmps = 0;
        String chipId = "";
        
        void sendRCBTriggerPost(String ipNumStr, String msgStr);
        String getResultText(const URL& url);

        Array<int64> sampleNumbers;
        Array<double> timestamps; 
        Array<uint64> ttlEventWords; 

        // used by URL Post
        StringPairArray responseHeaders;
        
        // UDP Packet
        uint8_t magicNum = 0;
        uint32_t seqNum = 0;
        uint32_t nextsn = 0;
        uint32_t hit = 0;
        uint32_t miss = 0;
        uint32_t delayed = 0;
        uint16_t digInputs = 0;
        uint16_t sod = 0;
        bool firstPacket = 1;
        
        // battery status
        float batteryInit = 0;
        uint16_t batteryVolts = 0;
        
        uint8_t auxPhase = 0;
        uint8_t auxMask = 0;
        
        uint32_t bitrate = 0;
        int numChannelsEnabled = 0;
        int intanAlertNum = 0;  //might not need this
        
        int recvBufSize;// = 40 + (((num_channels + 2) * num_samp) * 2);
        int convBufSize;// = 0 + (((num_channels) * num_samp) * 4);
        
        // for RHD register definitions please see Intan RHD2000 data sheet.
        // default values below, many will not change, Bandwidth and Bias will change

        int rhdReg00 = 0xDE;// ADC Configuration and Amplifier Fast Settle
        int rhdReg01 = 0x02;// Supply Sensor and ADC Buffer Bias Current
        int rhdReg02 = 0x04;// MUX Bias Current
        int rhdReg03 = 0x02;// MUX Load, Temperature Sensor, Auxiliary Digital Output
        int rhdReg04 = 0x9C;// ADC Output Format and DSP Offset Removal
        int rhdReg05 = 0x00;// Impedance Check Control
        int rhdReg06 = 0x00;// Impedance Check DAC
        int rhdReg07 = 0x00;// Impedance Check Amplifier Select
        int rhdReg08 = 0x16;// On - Chip Amplifier Bandwidth Select - RH1 DAC1
        int rhdReg09 = 0x00;// On - Chip Amplifier Bandwidth Select - RH1 DAC2
        int rhdReg10 = 0x17;// On - Chip Amplifier Bandwidth Select - RH2 DAC1
        int rhdReg11 = 0x00;// On - Chip Amplifier Bandwidth Select - RH2 DAC2
        int rhdReg12 = 0x10;// On - Chip Amplifier Bandwidth Select - RL1 DAC1
        int rhdReg13 = 0x7C;// On - Chip Amplifier Bandwidth Select - RL1 DAC2, 3
        int rhdReg14 = 0xFF;// Individual Amplifier Power - apwr(7 - 0)
        int rhdReg15 = 0xFF;// Individual Amplifier Power - apwr(15 - 8)
        int rhdReg16 = 0xFF;// Individual Amplifier Power - apwr(23 - 16)
        int rhdReg17 = 0xFF;// Individual Amplifier Power - apwr(31 - 24)
        // rhdReg18 = 0x00; // not used
        // rhdReg19 = 0x00; // not used
        // rhdReg20 = 0x00; // not used
        // rhdReg21 = 0x00; // not used

        // Compute DSP Cutoff - See RHD data sheet page 33
        float kFreq[16] = { 0.0, 0.1103, 0.04579, 0.02125, 0.01027, 0.005053, 0.002506, 0.001248, 0.0006229, 0.0003112,
            0.0001555, 0.00007773, 0.00003886, 0.00001943, 0.000009714, 0.000004857 };
        
        //  channel mask is sent to RCB during init so it knows which channels to send in UDP packet.
        // channel mask is also used to power down unused amplifier channels in RHD regs.
        String chMask[32] = { "1", "3", "7", "F",
        "1F", "3F", "7F", "FF",
        "1FF", "3FF", "7FF", "FFF",
        "1FFF", "3FFF", "7FFF", "FFFF",
        "1FFFF", "3FFFF", "7FFFF", "FFFFF",
        "1FFFFF", "3FFFFF", "7FFFFF", "FFFFFF",
        "1FFFFFF", "3FFFFFF", "7FFFFFF", "FFFFFFF",
        "1FFFFFFF", "3FFFFFFF", "7FFFFFFF", "FFFFFFFF" };
        
        uint32 chShftMask[32] = { 0x1, 0x3, 0x7, 0xF,
        0x1F, 0x3F, 0x7F, 0xFF,
        0x1FF,0x3FF, 0x7FF, 0xFFF,
        0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF,
        0x1FFFF, 0x3FFFF, 0x7FFFF, 0xFFFFF,
        0x1FFFFF, 0x3FFFFF, 0x7FFFFF, 0xFFFFFF,
        0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF, 0xFFFFFFF,
        0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF };
     
        //RHD lower bandwidth DAC register settings
        int lowBwDac1[25] = { 0x0D, 0x0F, 0x11, 0x12, 0x15, 0x19, 0x1C, 0x22, 0x2C, 0x30, 0x36,
        0x3E, 0x05, 0x12, 0x28, 0x14, 0x2A, 0x08, 0x09, 0x2C, 0x31, 0x23, 0x01, 0x38, 0x10 };

        int lowBwDac2[25] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04, 0x06, 0x09, 0x11, 0x28, 0x36, 0x7C };

        // not used
        // lowBwDac3 = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, ...
        //    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01];

        //RHD upper bandwidth DAC register settings
        int upBwRh1Dac1[17] = { 0x08, 0x0B, 0x11, 0x16, 0x21, 0x03, 0x0D, 0x1B, 0x01, 0x2E, 0x29,
        0x1E, 0x06, 0x2A, 0x18, 0x2C, 0x26 };

        int upBwRh1Dac2[17] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03,
        0x05, 0x09, 0x0A, 0x0D, 0x11, 0x1A };

        int upBwRh2Dac1[17] = { 0x04, 0x08, 0x10, 0x17, 0x25, 0x0D, 0x19, 0x2c, 0x17, 0x1E, 0x24,
        0x2B,0x02,0x05,0x07,0x08,0x05 };

        int upBwRh2Dac2[17] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x02, 0x03, 0x04,
        0x06, 0x0B, 0x0D, 0x10, 0x15, 0x1F };
        

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RcbWifi);
    };
}
#endif
