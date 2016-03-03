# AreaDetector Driver for Cameralink PCO Cameras



##PCO Driver

The PCO Driver has three parts:

1. Camera Link driver for Area Detector (cameraLinkSrc2)
2. Camera Link serial port (camLinkSerialSrc)
3. PCO camera source (PCO2src)

Pars 1 and 2 are stored in the ADCameralink repository and inherited by the PCO driver.
To build:

1. Make ADCameralink
2. Make ADPCO





##Plugin for Edge camera

Edge Camera Plugin
The PCO Edge camera sends scrambled images that need to be descrambled. This plugin 
handles this. If the camera runs faster than the plugin, then images will be stored 
in the NDArray queues.






