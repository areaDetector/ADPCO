D:
set PATH=D:\Madden\GitHub\areaDetector-R2-4\ADPCO\iocs\pcoIOC\bin\windows-x64-debug;%PATH%

set EPICS_CA_MAX_ARRAY_BYTES=40000000
set EPICS_DISPLAY_PATH=D:\EPICS\ADEpics\synApps_5_5\support\all_adl
 
  cd D:\Madden\GitHub\areaDetector-R2-4\ADPCO\iocs\pcoIOC\iocBoot\iocXRD22
  
  ..\..\bin\windows-x64-debug\PCOApp.exe PCO_st64_xrd22DBG.cmd
  pause
  
  
  