@echo off
:: Change to the name of your project in VS
set project=G11_Project
:: Change if you want to test different passwords
set pass=SECRET
:: Change to the milestone level you want to test
set m=1

IF EXIST debug\%project%.exe (
    rem exists
) ELSE (
    echo Can't find executable %project%.exe in the debug folder,
    echo make sure you change the project variable in the file to your VS project name
    pause
    exit
)

debug\%project%.exe -e 1_testASCII.txt -k Key.dat -p %pass% -m %m% > 0
debug\%project%.exe -d 1_testASCII.txt.enc -k Key.dat -p %pass% -m %m% > 0
:: Compares input and output files
fc 1_testASCII.txt 1_testASCII.txt.enc.dec > 0
if errorlevel 1 goto error

echo SUCCESS, no differences found
pause
exit

:error
echo on
@echo FAILED with password = %pass% and m = %m%
@echo --- Original file ---
@type 1_testASCII.txt
@echo
@echo --- Decrypted file ---
@type 1_testASCII.txt.enc.dec
@echo
@pause


