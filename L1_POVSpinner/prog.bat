:start
atprogram -cl 200khz -v -d attiny4313 -t atmelice -i isp chiperase
atprogram -cl 200khz -v -d attiny4313 -t atmelice -i isp program -f L12024POV\Release\L12024POV.elf
pause
goto start
