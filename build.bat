@echo off
REM Make sure you have Visual Studio 2013 (or 2015, untested) installed and on your path

REM Modify the next line as you see fit
call "C:\code\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64

cl src\jest.c /O2 /nologo /Fe:bin\jest.exe /Fo:tmp\ /TC
