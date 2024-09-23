@echo off
::echo %WORKSPACE%
echo "Usage <Realative Lovcation> (:%WORKSPACE%\Build\ZhangYueX64\DEBUG_VS2019\X64\<Realative Location>)"
SET /P FileName="Input Relative Location!"
SET DestLocation=%WORKSPACE%\Build\EmulatorX64\DEBUG_VS2019\X64
echo %DestLocation%
::默认在ZhangYPkg/Application/<Realative Location>下
SET SorLocation=%WORKSPACE%\Build\ZhangYueX64\DEBUG_VS2019\X64\%FileName%
echo %SorLocation%
copy %SorLocation% %DestLocation%
echo DONE!