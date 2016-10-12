# AreaDetector Driver for Cameralink PCO Cameras



##PCO Driver

This driver is for PCO cameras. Though PCO cameras have generally the same software interface,
this driver is intended for Cameralink-based Dimax and Edge cameras. It should work for
any PCO Cameralink camera however.

The PCO driver depends on ADCameralink for support of the camera link frame grabbers. To get ADPCO to 
build, the ADCameralink driver must be successfully built first. Please see the Readme.md in ADCameralink.
Because ADCameralink can support multiple commercial grabbers, and depends on commercial software
libraries, there are several steps to building ADCameralink. ADCameralink hides most grabber-specific
issues from the ADPCO driver.

To build ADPCO
1. Configure and build ADCameralink.
2. Build ADPCO.

ADPCO must build an application library in pcoApp, and also the IOC in iocs.
In building the iocs, the proper commercial library from the grabber must be linked.
See the Makefile in iocs/pcoIOC/pcoApp/src. 


Though ADCameralink supports multiple frame grabbers, PCO requires silocon Software because there
are special firmwares in the grabber the PCO Edge needs. Dimax can run with either
Silocon software or Dalsa. 











##Plugin for Edge camera

The PCO Edge camera sends scrambled images that need to be descrambled. This plugin 
handles this. If the camera runs faster than the plugin, then images will be stored 
in the NDArray queues.


To configure the driver for your system:
1. In the iocBoot/iocPCO directory, find envPaths64.
2. Edit these lines at the top of the file.

epicsEnvSet("AREA_DETECTOR","C:/EPICS/GitHub/areaDetector-R2-4")
epicsEnvSet("EPICS_BASE","C:/EPICS/ADepics/baseR12.1")
epicsEnvSet("SUPPORT","C:/EPICS/ADepics/SynApps_5_5/support")
epicsEnvSet("REQPATH","C:/EPICS/ADEpics/iocs")

3. In the iocBoot/iocPCO directory, edit edgeGlobShutter.mcf. There is a line that has path to
   PCO_scmos_gl_x64.hap. Edit that line so the FULL path is there, starting from the drive name.
   The PCO silicon Software driver cannot deal with relative paths.
   
4. Edit STARTEDGE to set the PATHs correctly.  






