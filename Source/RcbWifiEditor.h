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

#ifndef __RCBWIFIEDITORH__
#define __RCBWIFIEDITORH__

#ifdef _WIN32
#include <Windows.h>
#endif

#include <VisualizerEditorHeaders.h>
#include <EditorHeaders.h>

namespace RcbWifiNode
{
    class RcbWifi;

    class RcbWifiEditor : public GenericEditor,
        public Label::Listener,
        public ComboBox::Listener,
        public Button::Listener,
        public MultiTimer //Timer
    {

    public:

        /** Constructor */
        RcbWifiEditor(GenericProcessor* parentNode, RcbWifi* node);

        /** Button listener callback, called by button when pressed. */
        void buttonClicked(Button* button) override;

        /** Called by processor graph in beginning of the acqusition, disables editor completly. */
        void startAcquisition() override;

        /** Called by processor graph at the end of the acqusition, reenables editor completly. */
        void stopAcquisition() override;

        /** Called when configuration is saved. Adds editors config to xml. */
        void saveCustomParametersToXml(XmlElement* xml) override;

        /** Called when configuration is loaded. Reads editors config from xml. */
        void loadCustomParametersFromXml(XmlElement* xml) override;

        /** Called when label is changed */
        void labelTextChanged(juce::Label* label) override;

        /** Called when comboBox is changed */
        void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override;
 
    private:
        
        // Version
        ScopedPointer<Label> versionLabel;
       
        // Channels
        ScopedPointer<Label> chanLabel;
        ScopedPointer<ComboBox> chanCbox;
        ScopedPointer<Label> chStartNumLabel;

        // Samples
        ScopedPointer<Label> sampLabel;
        ScopedPointer<TextEditor> sampText;

        // Desired Sample Rate
        ScopedPointer<Label> fsLabel;
        ScopedPointer<ComboBox> fsCbox;  

        // RHD Up Bw
        ScopedPointer<Label> upBwLabel;
        ScopedPointer<ComboBox> upBwCbox;

        // RHD Low Bw
        ScopedPointer<Label> lowBwLabel;
        ScopedPointer<ComboBox> lowBwCbox;

        // RHD DSP Cutoff
        ScopedPointer<Label> dspCutLabel;
        ScopedPointer<Label> dspCutNumLabel;
        ScopedPointer<UtilityButton> dspOffsetButton;

        // UDP Packet Info - Hit Miss
        ScopedPointer<Label> seqNumLabel;
        
        // UDP Packet Delivery Ratio
        ScopedPointer<Label> pdrNumLabel;

        //Battery
        ScopedPointer<Label> batteryLabel;

        // Intan RDH read only regs
        ScopedPointer<Label> rhdRegsLabel;

        // RCB PA Power
        ScopedPointer<Label> paPwrLabel;
        ScopedPointer<ComboBox> paPwrCbox;

        // RCB IP Addr
        ScopedPointer<Label> destIpLabel;
        ScopedPointer<Label> rcbIpNumLabel;

        // Host IP Addr
        ScopedPointer<Label> hostIpLabel;
        ScopedPointer<Label> hostIpNumLabel;

        // Port
        ScopedPointer<Label> portLabel;
        ScopedPointer<Label> portNumLabel;

        // Init Button
        ScopedPointer<Label> initLabel;
        ScopedPointer<UtilityButton> initButton;
        
        // Timer Poll rep rate
        ScopedPointer<Label> pollRateLabel;
        ScopedPointer<ComboBox> pollRateCbox;
        
        // RHD Aux Inputs enable
        ScopedPointer<UtilityButton> auxEnableButton;

        // Parent node
        RcbWifi* node;

        // timer.  timer1 is RCB info update to UI rate
        void timerCallback(int timerID) override;
        int timeInt;
        // timer2 is RCB poll rate to check battery when not streaming data
        bool timer2Disable = true;
        int timer2Rate = 1;
        
        // indicates values selected and entered on UI are ok to send to RCB
        bool uiIsOk = true;

        // IP stuff
        IPAddress getCurrentIpAddress();
        IPAddress myHost;

        // total number of 16-bit samples in UDP packet is numTs * (num chan + aux)
        int numTsItems[8] = { 21, 23, 27, 32, 39, 51, 71, 119 }; // numCh[32, 28, 24,20, 16, 12, 8, 4];
        
        //count how many RCB Timer2 polling events are lost
        int rcbIsLost = 0;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RcbWifiEditor);
    };
}

#endif
