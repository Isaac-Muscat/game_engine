@echo off
call premake-bin\premake5.exe vs2022 
call premake-bin\premake5.exe export-compile-commands
move .\compile_commands\debug.json .\
del compile_commands.json
ren debug.json compile_commands.json
popd
PAUSE
