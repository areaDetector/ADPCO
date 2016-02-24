C:
set PATH=C:\EPICS\GitHub\areaDetector-R2-4\ADPCO\iocs\pcoIOC\bin\windows-x64-debug;%PATH%

set EPICS_CA_MAX_ARRAY_BYTES=40000000
set EPICS_DISPLAY_PATH=C:\EPICS\ADEpics\synApps_5_5\support\all_adl
 
cd C:\EPICS\GitHub\areaDetector-R2-4\ADPCO\iocs\pcoIOC\iocBoot\iocAkitaDimax
  
  
  ..\..\bin\windows-x64-debug\pcoApp.exe PCO_st64Akita.cmd
rem  ..\..\bin\windows-x64-debug\pcoApp.exe 
pause
  
  
  