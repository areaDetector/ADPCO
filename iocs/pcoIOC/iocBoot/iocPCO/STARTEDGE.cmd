D:
set PATH=C:\EPICS\GitHub\areaDetector-R2-4\ADPCO\iocs\pcoIOC\bin\windows-x64-debug;%PATH%

set EPICS_CA_MAX_ARRAY_BYTES=40000000
rem set EPICS_DISPLAY_PATH=D:\EPICS\ADEpics\synApps_5_5\support\all_adl
 
cd C:\EPICS\GitHub\areaDetector-R2-4\ADPCO\iocs\pcoIOC\iocBoot\iocPCO
  
  
  ..\..\bin\windows-x64-debug\pcoApp.exe PCO_st64_Edge.cmd
rem  ..\..\bin\windows-x64-debug\pcoApp.exe 
