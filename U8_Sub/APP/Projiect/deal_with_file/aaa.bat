
echo off

cd /d %~dp0
del U8_App_With_Bin.bin
fsutil file createnew U8_App_With_Bin.bin 8192
cat App_Bin\U8_Sub_app.bin >>U8_App_With_Bin.bin
hbin boot_Bin\Boot_U8_Sub.bin U8_App_With_Bin.bin


copy ..\Objects\U8_Sub_app.hex App_Bin

echo on