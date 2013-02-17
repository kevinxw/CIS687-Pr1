@echo off

REM this file will re-compile the entire solution
echo Compiling... Please wait
devenv "./TypeDependencyAnalysis/TypeDependencyAnalysis.sln" /rebuild debug

REM this is for machines without including devenv.exe in their PATH variable
REM "C:/Program Files (x86)/Microsoft Visual Studio 11.0/Common7/IDE/devenv.exe" "./TypeDependencyAnalysis/TypeDependencyAnalysis.sln" /rebuild debug

echo Solution Compiled.  Enjoy!