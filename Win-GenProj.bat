@echo off
pushd %~dp0
call Libraries\Tools\premake5\premake5.exe vs2022 --verbose
popd
PAUSE