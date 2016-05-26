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
3. You need the siliconSoftware driver installed in your computer before the software will build or work.
Download  that from Silicon Software at https://silicon.software/




##Plugin for Edge camera

Edge Camera Plugin
The PCO Edge camera sends scrambled images that need to be descrambled. This plugin 
handles this. If the camera runs faster than the plugin, then images will be stored 
in the NDArray queues.


To configure the driver for your system:
1) In the iocBoot/iocPCO directory, find envPaths64.
2) Edit these lines at the top of the file.
epicsEnvSet("AREA_DETECTOR","C:/EPICS/GitHub/areaDetector-R2-4")
epicsEnvSet("EPICS_BASE","C:/EPICS/ADepics/baseR12.1")
epicsEnvSet("SUPPORT","C:/EPICS/ADepics/SynApps_5_5/support")
epicsEnvSet("REQPATH","C:/EPICS/ADEpics/iocs")

3) IN the iocBoot/iocPCO directory, edit edgeGlobShutter.mcf. There is a line that has path to
   PCO_scmos_gl_x64.hap. Edit that line so the FULL path is there, starting from the drive name.
   The PCO silicon Software driver cannot deal with relative paths.
   
 4) Edit STARTEDGE to set the PATHs correctly.  






