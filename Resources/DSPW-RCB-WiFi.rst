.. _dspwrcbwifi:
.. role:: raw-html-m2r(raw)
   :format: html

#################
DSPW RCB WiFi
#################

.. image:: ../../_static/images/plugins/xxxxxxx/xxxxxxxxx.png
  :alt: Annotated settings interface for the DSPW RCB WiFi plugin

.. csv-table:: Streams data from a `DSPW RCB WiFi Interface Module <https://intantech.com/RHD_USB_interface_board.html>`__.
   :widths: 18, 80

   "*Plugin Type*", "Source"
   "*Platforms*", "Windows, Linux, macOS"
   "*Built in?*", "No; install via *Rhythm Plugins*"
   "*Key Developers*", "Robert Paugh"
   "*Source Code*", "https://github.com/open-ephys-plugins/DSPW xxxxxx"


Installing and upgrading
==========================

The DSPW RCB WiFi plugin is not included by default in the Open Ephys GUI. To install, use **ctrl-P** or **⌘P** to open the Plugin Installer, browse to the "Rhythm Plugins", and click the "Install" button. After installation, *DSPW RCB WiFi* will appear in the processor list on the left side of the GUI's main window.

The Plugin Installer also allows you to upgrade to the latest version of this plugin, if it's already installed. The plugin must be removed from the signal chain prior to upgrading.


Plugin configuration
====================

RCB IP Addr:
#######################

RCB Module IP address. Default is 192.168.0.93.  Must be on the same subnet as your Host Computer.

HOST IP Addr:
#######################

HOST Computer IP address. Ex. 192.168.0.xx.  Must be on the same subnet as your RCB Module.

Host Port Number:
#######################

Host Computer Port Number. Default is 51234.  Port number must be between 49152 and 65535.  
Each instance of RCB WiFi plugin must have a different PORT number.

Configure Button
#######################

Press to Initialize the RCB Module with the selected plugin settings.  
Host Computer will attempt to connect to the RCB Module associated with the RCB IP Addr. If successful the plugin will turn Orange.

If you add or remove headstage after the RCB module has been Initialized, you need to press the "Configure" button. 


DSP Button
###########

In addition to the analog filters, the Intan chips also have an on-chip DSP high-pass filter for removing the DC offset on each channel. This can be toggled on and off by clicking the "DSP" button (yellow = on), and the cutoff frequency can be changed by typing a value into the associated HPF text box. Again, there are only certain values for the cutoff frequency; the chip will choose the one closest to what you selected and return this value to the GUI.

Bandwidth Upper/Lower Selection
###############################

Used to determine the settings for the analog high and low cut filters on the Intan chip. Because only certain values are possible for each, the number that appears may be different from the number you typed in; the chip will automatically select the nearest value, and that will be indicated in the GUI. 

Sample Rate selection (fs)
###########################

Sample rates between 1 and 30 kHz can be selected with this drop-down menu. This selection will determine the requested sample rate for the headstage connected to the RCB Module.
Because only certain values are possible, the actual sample rate may be different from the number you selected; the actual sample rate is dependent on the number of channels selected and the requested sample rate. The plugin will automatically select the nearest value, and that will be indicated in the GUI. 

If select 32 Channels then Sample Rate is restricted to 20ksps or less.  If select 28 channels or fewer Sample Rate can be 30ksps or less.

Headstage Number of RHD Channels Selection
################################

RCB Module will work with the Intan RHD2132 or RHD2216 ICs.  Does not work with the RHD2164 IC.  The RCB module will only acquire and transmit the channels that are selected.
Select the number of desired channels and the number of the first channel. Actual sample rate will depend on the number of channels.  If number of channels is changed then actual sample rate will change. 
Ex. For RHD2132 with 32 active channels, select 32 and 1.  

Using 16-channel headstages
----------------------------

This is necessary because the difference between 16-channel and 32-channel headstages cannot be detected in software and has to be selected manually.
For 16 channel RHD2132 headstage with active channels 8 thru 24 select 16 and 8. 
For 16 channel RHD2216 headstage with differential inputs select 16 and 1. 


RCB PA Attn Selection
###########################

The RCB WiFi Power Amplifier (PA) can be selected to reduce output power (attenuated) and save battery life. Reducing output power will reduce the WiFi transmit range.  The default setting of 4 is a good compromise between output power and battery savings.  Consult factory if wish to change.


Turning on AUX channels
#######################

Pressing the "AUX" button toggles the headstage AUX inputs on and off. Each Intan chip can communicate with up to 3 "auxiliary" inputs, which are connected to a 3-axis accelerometer on some headstages. If the button is off (gray), the AUX channels on each headstage will be ignored. If the button is on (yellow), they will be sent as parallel data channels, with the same sampling rate as the neural data.


UDP Rx Packet Status
#######################

UDP Packet delivery Ratio % is displayed.

Sequence number - restarts at each acquire and increments as each UDP packet arrives.  Each UDP Packet contains multiple data samples for each channel.
Good - Number of correctly received UDP packets.
Miss - Number of missed (dropped) UDP packets.

Intan Status
#######################

During Initialization the RCB module will read the Intan RHD read only registers to check that an RHD is connected and ready to be used for streaming data.

Battery Status
#######################

Displays the Battery voltage.

Battery Poll Rate Selection
###########################

Controls if the RCB Module Battery voltage is polled when in Standby, not Acquiring data.  The Polling interval can be selected 1 to 30 minutes.  Polling will use a small amount of battery capacity.  Requires that there has already been a successful Initialization.


Headstages
############

On the left-hand side of the module, there are slots for each of 8 possible headstages (A1, A2, B1, B2, etc.). Each row corresponds to one 12-channel Omnetics connector on the input board (from left to right, there are A, B, C, and D), and each column corresponds to one headstage on that input. Up to two headstages can be connected to each input using a dual headstage adapter. The module will automatically detect headstages that are connected, as well as whether they contain a 64- or 32-channel Intan chip. However, if you add or remove headstages after the module has been loaded, you need to press the "RESCAN" button. 




|

