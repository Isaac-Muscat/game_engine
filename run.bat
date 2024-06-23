@echo off
call genVSProj.bat
call devenv .\game_engine.sln /Run
