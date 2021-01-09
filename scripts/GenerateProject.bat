@echo off
pushd %~dp0\..\
call scripts\premake5.exe vs2019
popd
PAUSE
