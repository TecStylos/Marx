echo off

echo Marx:
vendor\bin\LOC\LOC.exe Marx\include .h .cpp

echo Sandbox:
vendor\bin\LOC\LOC.exe Sandbox\src .h .cpp

echo DonHugo:
vendor\bin\LOC\LOC.exe DonHugo\src .h .cpp

pause