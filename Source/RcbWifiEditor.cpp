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
//	desiredWidth = 412;
    desiredWidth = 480;
  
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
    versionLabel->setBounds(428, 6, 60, 15);
    versionLabel->setFont(Font("CP Mono", "Plain", 15));
    versionLabel->setColour(Label::textColourId, Colours::white);
    addAndMakeVisible(versionLabel);
    LOGC("[dspw] RCB Wi-Fi version = ", PLUGIN_VERSION);

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
    rcbIpNumLabel->setEditable(true,false,true);
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
    hostIpNumLabel->setEditable(true,false,true);
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
    portNumLabel->setEditable(true,false,true);
    portNumLabel->addListener(this);
    addAndMakeVisible(portNumLabel);

    // Init Button Label
   // initLabel = new Label("Init", "Configure");
    initLabel = new Label("Init", "Initialize");
    initLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    initLabel->setBounds(52, 96, 68, 12);
    initLabel->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(initLabel);

    // Init button
    initButton = new UtilityButton("Init Me!", Font("Small Text", 13, Font::bold));
    initButton->setTooltip("Press to Initialize RCB device with UI values.");

    initButton->setBounds(57, 108, 55, 18);
    initButton->addListener(this);
    addAndMakeVisible(initButton.get());
  
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
    dspCutNumLabel->setEditable(true,false,true);
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
    chStartNumLabel->setEditable(true,false,true);
    chStartNumLabel->addListener(this);
    addAndMakeVisible(chStartNumLabel);

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
    
    // Aux Enable
    auxEnableLabel = new Label("sampleEventLabel", "AUX CH");
    auxEnableLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    auxEnableLabel->setBounds(258, 28, 85, 12);
    auxEnableLabel->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(auxEnableLabel);
    
    // AUX Enable
    auxEnableButton = new UtilityButton("Enable", Font("Small Text", 13, Font::plain));
    auxEnableButton->setRadius(3.0f);
    auxEnableButton->setBounds(260, 40, 52, 18);
    auxEnableButton->addListener(this);
    auxEnableButton->setClickingTogglesState(true);
    auxEnableButton->setTooltip("Enable RHD AUX Inputs");
    addAndMakeVisible(auxEnableButton);
    auxEnableButton->setToggleState(false, dontSendNotification);
    
    // Sample Event Enable
    sampleEventButton = new UtilityButton("Set", Font("Small Text", 13, Font::plain));
    //sampleEventButton = new UtilityButton("Enable", Font("Small Text", 13, Font::plain));
    sampleEventButton->setRadius(3.0f);
    sampleEventButton->setBounds(260, 74, 27, 18);
   // sampleEventButton->setBounds(260, 74, 50, 18);
    sampleEventButton->addListener(this);
    sampleEventButton->setClickingTogglesState(true);
    sampleEventButton->setTooltip("Enable Sample Event # or Enable Broadcast Sync. ");
    addAndMakeVisible(sampleEventButton);
    sampleEventButton->setToggleState(false, dontSendNotification);
    
    sampleEventNumLabel = new Label("sampleEventNumLabel", "1");
    sampleEventNumLabel->setBounds(292, 75, 18, 16);
    sampleEventNumLabel->setTooltip("Enter Event Number.   Default is 1.\nEnter 0 for Broadcast Sync on Event 1.");
    sampleEventNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    sampleEventNumLabel->setColour(Label::textColourId, Colours::black);
    sampleEventNumLabel->setColour(Label::backgroundColourId, Colours::lightgrey);
    sampleEventNumLabel->setEditable(true,false,true);
    sampleEventNumLabel->addListener(this);
    addAndMakeVisible(sampleEventNumLabel);
    
    // Sample Event Settings
    sampleEventLabel = new Label("sampleEventLabel", "Event #");
    sampleEventLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    sampleEventLabel->setBounds(257, 62, 85, 12);
    sampleEventLabel->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(sampleEventLabel);

    numSamplesEventLabel = new Label("numSamplesLabel", "Samples");
    numSamplesEventLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    numSamplesEventLabel->setBounds(256, 95, 85, 12);
    numSamplesEventLabel->setColour(Label::textColourId, Colours::black);
    addAndMakeVisible(numSamplesEventLabel);
    
    samplesNumLabel = new Label("samplesNumLabel", "1000");
    samplesNumLabel->setBounds(260, 108, 52, 17);
    samplesNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    samplesNumLabel->setColour(Label::textColourId, Colours::black);
    samplesNumLabel->setColour(Label::backgroundColourId, Colours::white);
    samplesNumLabel->setTooltip("Enter Number of Samples in each Event.\nor\nEnter Broadcast Sync Timer Pulse (msec).");
    
    samplesNumLabel->setEditable(true,false,true);
    
    samplesNumLabel->addListener(this);
    addAndMakeVisible(samplesNumLabel);
    
// ****************
    
    // Battery Poll timer rate
    pollRateCbox = new ComboBox();
    pollRateCbox->setBounds(418, 109, 44, 17);

    pollRateCbox->addListener(this);
    for (int i = 0; i < 10; i++)
        pollRateCbox->addItem(pollRate[i], i + 1 ); // start numbering at one for
    pollRateCbox->setTooltip("Battery Poll Rate Timer(m)");
    pollRateCbox->setSelectedId(1, sendNotification);
    addAndMakeVisible(pollRateCbox);
    
    pollRateLabel = new Label("pollRateLabel", "Bat Poll\nRate (m)");
    pollRateLabel->setBounds(410, 82, 65, 26);

    pollRateLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    pollRateLabel->setColour(Label::textColourId, Colours::black);
    pollRateLabel->addListener(this);
    addAndMakeVisible(pollRateLabel);

	// UDP Packet Hit Miss
    seqNumLabel = new Label("seqNum", "UDP Rx Packet PDR:PDR%\nSQ N: 0\nGood: 0\nMiss: 0");
	seqNumLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    seqNumLabel->setBounds(318, 27, 150, 52);
    seqNumLabel->setBounds(318, 27, 160, 52);
	seqNumLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(seqNumLabel);

	// battery Label
    batteryLabel = new Label("batteryVolts", "Bat 0.00V ---");
	batteryLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    batteryLabel->setBounds(318, 112, 100, 13);
	batteryLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(batteryLabel);

	// RHD read only regs label
    rhdRegsLabel = new Label("rhdRegs", "Intan\nStatus ");
	rhdRegsLabel->setFont(Font(Font::getDefaultSerifFontName(), 13, Font::plain));
    rhdRegsLabel->setBounds(318, 82, 70, 26);
	rhdRegsLabel->setColour(Label::textColourId, Colours::black);
	addAndMakeVisible(rhdRegsLabel);

	// try to get host ip before init
    // this should be expanded to allow selection if multiple network interfaces are present.
	myHost = getCurrentIpAddress();
    LOGD("[dspw] Host IP = ",myHost.toString());
	hostIpNumLabel->setText(myHost.toString(), dontSendNotification);
    node->ipNumStr = rcbIpNumLabel->getText();

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
        
    // These 2 are ok to have enabled during aquisition
    //    sampleEventButton->setEnabled(false);
    //    samplesNumLabel->setEnabled(false);

		timeInt = 0;
        stopTimer(2); // stop battery voltage update timer
		startTimer(1, 2000); //packet update timer
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
        sampleEventButton->setEnabled(true);
        samplesNumLabel->setEnabled(true);
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
			node->getBatteryInfo();
			batteryLabel->setText(node->batteryInfo, dontSendNotification);
            
            if (node->isGoodRCB == false)
                initButton->setLabel("Init");

			// to slow battery display update and keep Packet update more speedy. Or have two timers? Or use MultiTimer?
			timeInt = 5;
		}
		timeInt--;
       
    // timer 2 is used when not streaming data.  checks that RCB is still alive on network and updates battery voltage display.
	}
    else if (timerID == 2)
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
                    "Press Initialize button to try again.",
                    "OK", 0);
            }
            rcbIsLost++;
		}
	}
    else if (timerID == 3)
    {
        // timer 3 used only to send Broadcast Sync messages
        if (sampleEventNumLabel->getText() == "0")
        {
            numSamplesEventLabel->setText("Time ms",sendNotification);
            if (sampleEventButton->getToggleState() == true)
            {
                if (bCastSync == 1)
                {
                    bCastSync=0;
                    String timerTime = samplesNumLabel->getText();
                    //  sendRCBTriggerPutEd( "DSPW RCB TIMER 1 700"); // Timer version
                    sendRCBTriggerPutEd("message", "DSPW RCB TRIGGER 1 1");  // Trigger version
                   
                }
                else
                {
                    bCastSync = 1;
                    sendRCBTriggerPutEd("message", "DSPW RCB TRIGGER 1 0"); // Trigger version
                }
            }
            else
            {   // stopTimer(3) ; // Timer version
                sendRCBTriggerPutEd("message", "DSPW RCB TRIGGER 1 0"); // Trigger version
            }
        }
       
    }
}

void RcbWifiEditor::stopAcquisition()
{
	stopTimer(1); // stop UDP packet update Timer
    stopTimer(3); // stop Broadcast Sync Timer
    sampleEventButton->setToggleState(false,sendNotification);
   // sendRCBTriggerPutEd( "DSPW RCB TRIGGER 1 0");
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
    sampleEventButton->setEnabled(true);
    samplesNumLabel->setEnabled(true);

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
                batteryLabel->setText(node->batteryStatusInfo, dontSendNotification);
                if (node->isGoodRCB == true)
                {
                    //batteryLabel->setText(node->batteryStatusInfo, dontSendNotification);
                    
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
                        
                        // get Sample Event enable state.
                        node->sampleEventEnableState = sampleEventButton->getToggleState();
                        node->samplesForEvent = samplesNumLabel->getText().getIntValue();
                        
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
                        std::string strSt = stream.str();
                        dspCutNumLabel->setText(strSt, dontSendNotification);
                        
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
                                                "Press Initialize button to try again.",
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
                                            "Press Initialize button to try again.",
                                            "OK", 0);
            }
        }
        else if (chStartIsValid == false)
        {
            LOGD("[dspw] UI Channel values failed ");
            
            AlertWindow::showMessageBox(AlertWindow::NoIcon,
                                        "Number of channels + channel Start cannot be greater than 33.",
                                        "Check your RCB Plugin UI Channel settings. \r\n\r\n"
                                        "Press Initialize button to try again.",
                                        "OK", 0);
        }
        else if (sampleEventIsValid == false)
        {
            LOGD("[dspw] Sample Event Number values failed ");
            
            AlertWindow::showMessageBox(AlertWindow::NoIcon,
                                        "Sample Event Number must be between 1 and 200000.",
                                        "Check your RCB Plugin Samples settings. \r\n\r\n"
                                        "Press Initialize button to try again.",
                                        "OK", 0);
        }
    }
    
	else if (button == dspOffsetButton)
	{
		node->initPassed = false;
		initButton->setLabel("Init");
	}
    else if (button == auxEnableButton)
    {
        node->initPassed = false;
        initButton->setLabel("Init");
    }
    else if (button == sampleEventButton)
    {
        // not needed as only affects the event generator.  No changes sent to the RCB device.
        //node->initPassed = false;
        //initButton->setLabel("Init");
        
      //  node->sampleEventEnableState = sampleEventButton->getToggleState();
        
        //use as Sw Sync Trigger on TTL Event 1
        if (sampleEventNumLabel->getText() == "0")
        {
            //first check to see if HTTP Server is enabled
            URL urlInit("http://localhost:37497/api/processors");
            auto result = getResultTextEd(urlInit);
            //LOGC("[dspw] result is - ",result);
            // do something to tell user
            if (result.length() < 200)
            {
                LOGC("[dspw] msg is  ",result);
                sampleEventButton->setToggleState(false, dontSendNotification);
                //return;
            }
            else
            {
                numSamplesEventLabel->setText("Time ms",sendNotification);
                if (sampleEventButton->getToggleState() == true)
                {
                    //sendRCBTriggerPutEd("processors", ""); // Trigger version
                    String timerTime = samplesNumLabel->getText();
                    //  sendRCBTriggerPutEd( "DSPW RCB TIMER 1 700"); // Timer version
                    //  sendRCBTriggerPutEd( "DSPW RCB TIMER 1 " + timerTime);
                    sendRCBTriggerPutEd("message", "DSPW RCB TRIGGER 1 1");
                    Value val = samplesNumLabel->getTextValue();
                    int requestedValue = int(val.getValue());
                    startTimer(3,requestedValue);
                }
                else
                {   // stop timer
                    // sendRCBTriggerPutEd( "DSPW RCB TIMER 1 0");
                    sendRCBTriggerPutEd("message", "DSPW RCB TRIGGER 1 0");
                    stopTimer(3);
                }
            }
        }
        else
        {   //use as Sw Event Trigger on selected TTL Event
            numSamplesEventLabel->setText("Samples",sendNotification);
            node->sampleEventEnableState = sampleEventButton->getToggleState();
        }
    }
}

void RcbWifiEditor::labelTextChanged(juce::Label* label)
{
    // these now done inside each Label Text Changed
//	bool ipIsValid = false;
//	bool hostIpIsValid = false;
//	bool portIsValid = false;
//	bool dspHpfIsValid = false;
//  bool chStartIsValid = false;
//  bool sampleEventIsValid = false;

	//node->initPassed = false;
	//initButton->setLabel("Init");
    
	if (label == rcbIpNumLabel)
	{
        node->initPassed = false;
        initButton->setLabel("Init");
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

			// reset ip address to RCB default
			String ipStr = "192.168.0.93";
			rcbIpNumLabel->setText(ipStr, sendNotification);
		}
		LOGD("[dspw] RCB IP valid = ",ipIsValid);
	}
	else if (label == hostIpNumLabel)
	{
        node->initPassed = false;
        initButton->setLabel("Init");
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
        node->initPassed = false;
        initButton->setLabel("Init");
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
        node->initPassed = false;
        initButton->setLabel("Init");
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
        node->initPassed = false;
        initButton->setLabel("Init");
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
    else if (label == sampleEventNumLabel)
    {
        // not needed as only affects the event generator.  No changes sent to the RCB device.
        //node->initPassed = false;
        //initButton->setLabel("Init");
        
        Value val = label->getTextValue();
        int requestedValue = int(val.getValue());
        LOGD("[dspw] sampleEventNum requested value = ",String(int(requestedValue)));
        
        int smplEventNum = chanCbox->getText().getIntValue();
        if (requestedValue < 0 || requestedValue -1 > 8)
        {
            smplEventNumIsValid = false;
            uiIsOk =false;
            label->setText("1", sendNotification);
            smplEventNumIsValid = true;
            
            AlertWindow::showMessageBox(AlertWindow::NoIcon,
                "Sample Event Number value " + String(int(requestedValue)) + " is not valid \r\n"
                "Event Number must be between 1 and 8.",
                "Please check your Event Number setting. \r\n"
                "",
                "OK", 0);
        }
        else
        {
            smplEventNumIsValid = true;
            if (requestedValue == 0)
            numSamplesEventLabel->setText("Time ms",sendNotification);
            else
                numSamplesEventLabel->setText("Samples",sendNotification);
            
            // each eventstate bit corresponds to an event 0x1 = event 1, 0xf = event 1,2,3,4
            int eventAdjust[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
            node->smplEventNum = eventAdjust[requestedValue - 1];
            uiIsOk = true;
        }
    }
    else if (label == samplesNumLabel)
    {
        Value val = label->getTextValue();
        int requestedValue = int(val.getValue());
        LOGD("[dspw] sampleEventNum requested value = ",String(int(requestedValue)));
        
        if (requestedValue < 1 || requestedValue > 200000)
        {
            sampleEventIsValid = false;
            uiIsOk =false;
            
            AlertWindow::showMessageBox(AlertWindow::NoIcon,
                "Sample Event Number value " + String(int(requestedValue)) + " is not valid \r\n"
                "Value must be between 1 and 200000.",
                "Please check your Sample Event Number setting. \r\n"
                "",
                "OK", 0);
        }
        else
        {
            node->samplesForEvent = requestedValue;
            sampleEventIsValid = true;
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
        // get number of channels from comboBox
        int num_channels = chanCbox->getText().getIntValue();
        Value chStartVal = chStartNumLabel->getTextValue();
        int requestedValue = int(chStartVal.getValue());
        
        if ( requestedValue + num_channels - 1 > 32)
        {
        chStartIsValid = false;
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
        
        chStartIsValid = true;
        uiIsOk = true;
        
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

// added to send Put message to Broadcast Handler via OE HTTP Server at localhost (127.0.0.1)
void RcbWifiEditor::sendRCBTriggerPutEd(String command, String msgStr)
{
    //initPassed = false;
   String ipNumStr = "127.0.0.1";
    
    DynamicObject* obj = new DynamicObject();
    obj->setProperty("text",msgStr);
    var json (obj);
    String s = JSON::toString(json);
 //   URL urlPut = URL("http://:127.0.0.137497/api/message").withPOSTData(s);
    URL urlPut = URL("http://localhost:37497/api/" + command).withPOSTData(s);

    int statusCode = 0;
    
    //LOGD("POST ipNumStr - ", ipNumStr);
    //LOGD("POST msgStr - ", msgStr);
    //LOGC("[dspw] PUT str URL - ", urlPut.toString(true));
    //LOGC("[dspw] PUT str data - ", urlPut.getPostData());
    
    std::unique_ptr<InputStream> putStream(urlPut.createInputStream(true, nullptr, nullptr, {"Content-Type: application/json"}, 1000, &responseHeaders, &statusCode, 5, "PUT"));
        
    if (putStream != nullptr)
    {
        //initPassed = true;
        String putStr = putStream->readEntireStreamAsString();
        //LOGC("[dspw] PUT Stream StatusCode = ",statusCode);
        if (statusCode != 200)
        {
            LOGC("[dspw] PUT Stream = ",putStr);
            LOGC("[dspw] PUT Stream Status Code = ",statusCode);
        }
    }
    else
    {
        //initPassed = false;
        stopTimer(3);
        CoreServices::setAcquisitionStatus(false);
        sampleEventButton->setToggleState(false,sendNotification);
        stopTimer(3);
        
        AlertWindow::showMessageBox(AlertWindow::NoIcon,
            "OE HTTP Server not found at IP address " + ipNumStr,
            "Please check that HTTP Server is Enabled. \r\n\r\n"
            "Press Initialize button to try again.",
            "OK", 0);
    }
}

// taken from Juce network demo
String RcbWifiEditor::getResultTextEd(const URL& url)
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
    {
        AlertWindow::showMessageBox(AlertWindow::NoIcon,
                                    "OE HTTP Server not found. ",
                                    "Please check that Open Ephys HTTP Server is Enabled. \r\n\r\n"
                                    "Restart Acquire.",
                                    "OK", 0);
        return "Failed to connect, status code = " + String(statusCode);
    }
    AlertWindow::showMessageBox(AlertWindow::NoIcon,
                                "OE HTTP Server not found. ",
                                "Please check that Open Ephys HTTP Server is Enabled. \r\n\r\n"
                                "Then Restart Acquire.",
                                "OK", 0);
    return "Failed to connect! Status code = " + String(statusCode);
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
//    parameters->setAttribute("sampleEnBut", sampleEventButton->getToggleState());
    parameters->setAttribute("samplesNum", samplesNumLabel->getText());
    parameters->setAttribute("samplesEventNum", sampleEventNumLabel->getText());
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
   //         sampleEventButton->setToggleState(subNode->getBoolAttribute("sampleEnBut", false), dontSendNotification);
            samplesNumLabel->setText(subNode->getStringAttribute("samplesNum", ""), dontSendNotification);
            sampleEventNumLabel->setText(subNode->getStringAttribute("samplesEventNum", ""), sendNotification);
		}
	}
    // this is needed due to possible old hostAddr saved in Paremeters
    myHost = getCurrentIpAddress();
    hostIpNumLabel->setText(myHost.toString(), dontSendNotification);
    
    node->ipNumStr = rcbIpNumLabel->getText();

}
