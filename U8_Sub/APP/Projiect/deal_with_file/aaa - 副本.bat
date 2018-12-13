
echo off

del U8_App_With_Bin.bin
fsutil file createnew U8_App_With_Bin.bin 8192
E:\dingding\U8U8U8\u8\Embedded_U8\U8_Sub\APP\Src\deal_with_file\cat E:\dingding\U8U8U8\u8\Embedded_U8\U8_Sub\APP\Src\deal_with_file\U8_Sub_app.bin >>U8_App_With_Bin.bin
E:\dingding\U8U8U8\u8\Embedded_U8\U8_Sub\APP\Src\deal_with_file\hbin E:\dingding\U8U8U8\u8\Embedded_U8\U8_Sub\APP\Src\deal_with_file\Boot_U8_Sub.bin U8_App_With_Bin.bin


copy ..\..\Projiect\Objects\U8_Sub_app.hex ..\..\Projiect\Objects\bin
echo on