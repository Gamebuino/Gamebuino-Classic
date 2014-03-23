@del /Q obj\*.hex
@del /Q *.hex
set PATH=%PATH%;C:\arduino-1.0.5\hardware\tools\avr\utils\bin
make all
copy obj\*.hex
dir *.hex
pause
