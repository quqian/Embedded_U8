#include "BswDrv_nfc.h"

/****************************************************************************************/
/*名称：TypeA_Request																	*/
/*功能：TypeA_Request卡片寻卡															*/
/*输入：																				*/
/*       			    			     												*/
/*	       								 												*/
/*输出：																			 	*/
/*	       	pTagType[0],pTagType[1] =ATQA                                         		*/
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_Request(unsigned char *pTagType) 
{
    unsigned char   result, send_buff[1], rece_buff[2];
    unsigned int   rece_bitlen = 0;
    Clear_BitMask(TxModeReg, 0x80);//关闭TX CRC
    Clear_BitMask(RxModeReg, 0x80);//关闭RX CRC
    Set_BitMask(RxModeReg, 0x08);//关闭位接收
    Write_Reg(BitFramingReg, 0x07);
    send_buff[0] = 0x26;
    Pcd_SetTimer(1);

    result = Pcd_Comm(Transceive, send_buff, 1, rece_buff, &rece_bitlen);  //发送REQA进行轮询
    //PrintfData("quqian00TypeA_Request", rece_buff, 16);
    if ((result == OK) && (rece_bitlen == 2 * 8)) {
        *pTagType = rece_buff[0];
        *(pTagType + 1) = rece_buff[1];
        return OK;
    }
    return ERROR;
}
/****************************************************************************************/
/*名称：TypeA_WakeUp																	*/
/*功能：TypeA_WakeUp卡片寻卡															*/
/*输入：																				*/
/*       			    			     												*/
/*	       								 												*/
/*输出：																			 	*/
/*	       	pTagType[0],pTagType[1] =ATQA                                         		*/
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
//unsigned char TypeA_WakeUp(unsigned char *pTagType) {
//    unsigned char   result, send_buff[1], rece_buff[2];
//    unsigned int   rece_bitlen;
//    Clear_BitMask(TxModeReg, 0x80);//关闭TX CRC
//    Clear_BitMask(RxModeReg, 0x80);//关闭RX CRC
//    Set_BitMask(RxModeReg, 0x08);//关闭位接收
//    Clear_BitMask(Status2Reg, 0x08);
//    Write_Reg(BitFramingReg, 0x07);
//    send_buff[0] = 0x52;
//    Pcd_SetTimer(2);
//    result = Pcd_Comm(Transceive, send_buff, 1, rece_buff, &rece_bitlen);

//    if ((result == OK) && (rece_bitlen == 2 * 8)) {
//        *pTagType = rece_buff[0];
//        *(pTagType + 1) = rece_buff[1];
//    }
//    return result;
//}
/****************************************************************************************/
/*名称：TypeA_Anticollision																*/
/*功能：TypeA_Anticollision卡片防冲突													*/
/*输入：selcode =0x93，0x95，0x97														*/
/*       			    			     												*/
/*	       								 												*/
/*输出：																			 	*/
/*	       	pSnr[0],pSnr[1],pSnr[2],pSnr[3]pSnr[4] =UID                            		*/
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_Anticollision(unsigned char selcode, unsigned char *pSnr) 
{
    unsigned char   result, send_buff[7], rece_buff[5], CollPos, NVB, Row, Col, Bit_Framing, Send_Len, Snr[5], i, Rece_Len;
    unsigned int   rece_bitlen = 0;

    memset(rece_buff, 0x00, 5);
    memset(Snr, 0x00, 5);//临时UID
    memset(pSnr, 0x00, 5);
    NVB = 0x20;//NVB初始值
    Bit_Framing = 0x00;//Bit_Framing初始值

    Row = 0;//行，0为无冲突，bit1~bit8
    Col = 0;//列，0为无冲突，1~4
    CollPos = 0;
    //4行8列，共32个位置，对应CollPos数值 0~31
    Clear_BitMask(TxModeReg, 0x80);
    Clear_BitMask(RxModeReg, 0x80);

    Write_Reg(CollReg, 0x80);//冲突位后的接收数据为0

    Write_Reg(MfRxReg, 0x00);//使能奇偶校验


    while (1) {
        send_buff[0] = selcode;

        send_buff[1] = NVB;

        Write_Reg(BitFramingReg, Bit_Framing);//设置发送、接收的bit个数

        memcpy(send_buff + 2, pSnr, Col);

        Send_Len = Col + 2;//设置发送字节数

        Pcd_SetTimer(1);

        result = Pcd_Comm(Transceive, send_buff, Send_Len, rece_buff, &rece_bitlen);
        //PrintfData("quqian101TypeA_Anticollision", rece_buff, 16);
        if (result == TIMEOUT_Err) {
            return ERROR;
        }


        if (result & 0x08)//CollErr
        {
            //发现冲突错误
            CollPos = Read_Reg(CollReg) & 0x1F;//读取冲突位置

            TypeA_Set_NVB(CollPos, &NVB, &Row, &Col);//计算下次交互需要的NVB，保存本次收到的bit行数、列数

            TypeA_Set_Bit_Framing(CollPos, &Bit_Framing);//计算下次交互需要发送、接收的bit个数

            memcpy(Snr, rece_buff, Col);//保存接收到的部分UID到Snr

            Snr[Col - 1] = Snr[Col - 1] & Row;//保存接收到的正确bit

            for (i = 0; i < Col; i++) {
                pSnr[i] = pSnr[i] | Snr[i];//组合Snr与pSnr，保存到pSnr
            }
        } else {
            //没有发现冲突
            if (rece_bitlen > 8)
                Rece_Len = rece_bitlen / 8;
            else
                Rece_Len = 1;

            if (Col == 0) {
                //上次交互无冲突
                if ((Rece_Len + Col) != 5) {
                    return ERROR;
                }
                memcpy(pSnr, rece_buff, Rece_Len);//保存rece buff到 pSnr
            } else {
                //上次交互有冲突
                if ((Rece_Len + Col) != 6) {

                    return ERROR;
                }


                memcpy(Snr + Col - 1, rece_buff, Rece_Len);//保存rece buff到 Snr

                for (i = 0; i < Rece_Len; i++) {
                    pSnr[i + Col - 1] = pSnr[i + Col - 1] | Snr[i + Col - 1];//组合Snr与pSnr，保存到pSnr
                }
            }
            if (pSnr[4] == (pSnr[0] ^ pSnr[1] ^ pSnr[2] ^ pSnr[3])) {
                return OK;
            } else {
                return ERROR;
            }
        }

    }
    // return result;
}


void TypeA_Set_NVB(unsigned char collpos, unsigned char *nvb, unsigned char *row, unsigned char *col) 
{
    if (collpos == 0) {
        *nvb = 0x58;
        *col = 4;
    }
    if ((collpos > 0) && (collpos < 9)) //1~8
    {
        *nvb = 0x20 + (collpos); //21~28
        *col = 1;
    }
    if ((collpos > 8) && (collpos < 17))//9~16
    {
        *nvb = 0x30 + (collpos - 8);//31~38
        *col = 2;
    }
    if ((collpos > 16) && (collpos < 25))//17~24
    {
        *nvb = 0x40 + (collpos - 16);//41~48
        *col = 3;
    }
    if ((collpos > 24) && (collpos < 32))//25~31
    {
        *nvb = 0x50 + (collpos - 24);//51~57
        *col = 4;
    }

    switch ((*nvb) & 0x07) {
    case 1:
        *row = 0x01;
        break;
    case 2:
        *row = 0x03;
        break;
    case 3:
        *row = 0x07;
        break;
    case 4:
        *row = 0x0F;
        break;
    case 5:
        *row = 0x1F;
        break;
    case 6:
        *row = 0x3F;
        break;
    case 7:
        *row = 0x7F;
        break;
    case 8:
        *row = 0xFF;
        break;
    }
    return;
}

void TypeA_Set_Bit_Framing(unsigned char collpos, unsigned char *bit_framing) 
{
    if (collpos == 0) {
        *bit_framing = 0x00;
        return;
    }
    if ((collpos > 0) && (collpos < 9)) //1~8
    {
        *bit_framing = ((collpos) << 4) + (collpos);
        return;
    }
    if ((collpos > 8) && (collpos < 17))//9~16
    {
        *bit_framing = (((collpos - 8)) << 4) + (collpos - 8);
        return;
    }
    if ((collpos > 16) && (collpos < 25))//17~24
    {
        *bit_framing = (((collpos - 16)) << 4) + (collpos - 16);

        return;
    }
    if ((collpos > 24) && (collpos < 32))//25~31
    {
        *bit_framing = (((collpos - 24)) << 4) + (collpos - 24);
        return;
    }
    return;
}


/****************************************************************************************/
/*名称：TypeA_Select																	*/
/*功能：TypeA_Select卡片选卡															*/
/*输入：selcode =0x93，0x95，0x97														*/
/*      pSnr[0],pSnr[1],pSnr[2],pSnr[3]pSnr[4] =UID 			    			     	*/
/*	       								 												*/
/*输出：																			 	*/
/*	       	pSak[0],pSak[1],pSak[2] =SAK			                            		*/
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_Select(unsigned char selcode, unsigned char *pSnr, unsigned char *pSak) 
{
    unsigned char   result, i, send_buff[7], rece_buff[5];
    unsigned int   rece_bitlen;
    Write_Reg(BitFramingReg, 0x00);
    Set_BitMask(TxModeReg, 0x80);
    Set_BitMask(RxModeReg, 0x80);

    send_buff[0] = selcode;
    send_buff[1] = 0x70;

    for (i = 0; i < 5; i++) {
        send_buff[i + 2] = *(pSnr + i);
    }

    Pcd_SetTimer(1);
    result = Pcd_Comm(Transceive, send_buff, 7, rece_buff, &rece_bitlen);
    //PrintfData("quqian11TypeA_Select", rece_buff, 16);
    if (result == OK) {
        *pSak = rece_buff[0];
    }
    return result;
}
/****************************************************************************************/
/*名称：TypeA_Halt																		*/
/*功能：TypeA_Halt卡片停止																*/
/*输入：																				*/
/*       			    			     												*/
/*	       								 												*/
/*输出：																			 	*/
/*	       											                            		*/
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_Halt(void) 
{
    unsigned char   result, send_buff[2], rece_buff[1];
    unsigned int   rece_bitlen;
    send_buff[0] = 0x50;
    send_buff[1] = 0x00;

    Write_Reg(BitFramingReg, 0x00);
    Set_BitMask(TxModeReg, 0x80);
    Set_BitMask(RxModeReg, 0x80);
    Clear_BitMask(Status2Reg, 0x08);
    Pcd_SetTimer(1);
    result = Pcd_Comm(Transceive, send_buff, 2, rece_buff, &rece_bitlen);
    //PrintfData("quqian22TypeA_Halt", rece_buff, 16);
    return result;
}
/****************************************************************************************/
/*名称：TypeA_CardActivate																*/
/*功能：TypeA_CardActivate卡片激活														*/
/*输入：																				*/
/*       			    			     												*/
/*	       								 												*/
/*输出：	pTagType[0],pTagType[1] =ATQA 											 	*/
/*	       	pSnr[0],pSnr[1],pSnr[2],pSnr[3]pSnr[4] =UID 		                   		*/
/*	       	pSak[0],pSak[1],pSak[2] =SAK			                            		*/
/*       	OK: 应答正确                                                              	*/
/*	 		ERROR: 应答错误																*/
/****************************************************************************************/
unsigned char TypeA_CardActivate(unsigned char *pTagType, unsigned char *pSnr, unsigned char *pSak) 
{
    unsigned char   result;

    result = TypeA_Request(pTagType);//寻卡 Standard	 send request command Standard mode
    
    if (result != OK) {
        return ERROR;
    }
    if (pTagType[1] != 0x00) {
        return ERROR;
    }
    if ((pTagType[0] & 0xC0) == 0x00)   //M1卡
    {
        result = TypeA_Anticollision(0x93, pSnr);//防冲突验证
        if (result != OK) {
            return ERROR;
        }
        result = TypeA_Select(0x93, pSnr, pSak);//选卡并继续防冲突验证
        if (result != OK) {
            return ERROR;
        }
    }
    return result;
}







/*****************************************************************************************/
/*名称：Mifare_Auth																		 */
/*功能：Mifare_Auth卡片认证																 */
/*输入：mode，认证模式（0：key A认证，1：key B认证）；sector，认证的扇区号（0~15）		 */
/*		*mifare_key，6字节认证密钥数组；*card_uid，4字节卡片UID数组						 */
/*输出:																					 */
/*		OK    :认证成功																	 */
/*		ERROR :认证失败																  	 */
/*****************************************************************************************/
unsigned char Mifare_Auth(unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid)
{
    unsigned char   send_buff[12],rece_buff[1],result;
    unsigned int   rece_bitlen;
    if(mode==0x0)
        send_buff[0]=0x60;//kayA认证
    if(mode==0x1)
        send_buff[0]=0x61;//keyB认证
    send_buff[1]=sector*4;
    send_buff[2]=mifare_key[0];
    send_buff[3]=mifare_key[1];
    send_buff[4]=mifare_key[2];
    send_buff[5]=mifare_key[3];
    send_buff[6]=mifare_key[4];
    send_buff[7]=mifare_key[5];
    send_buff[8]=card_uid[0];
    send_buff[9]=card_uid[1];
    send_buff[10]=card_uid[2];
    send_buff[11]=card_uid[3];

    Pcd_SetTimer(1);
    Clear_FIFO();
    result =Pcd_Comm(MFAuthent,send_buff,12,rece_buff,&rece_bitlen);//Authent认证
    //PrintfData("quqian33Mifare_Auth", rece_buff, 16);

    if (result==OK)
    {
        if(Read_Reg(Status2Reg) & 0x08)//判断加密标志位，确认认证结果
            return OK;
        else
            return ERROR;
    }
    return ERROR;
}
/*****************************************************************************************/
/*名称：Mifare_Blockset																	 */
/*功能：Mifare_Blockset卡片数值设置														 */
/*输入：block，块号；*buff，需要设置的4字节数值数组										 */
/*																						 */
/*输出:																					 */
/*		OK    :设置成功																	 */
/*		ERROR :设置失败																	 */
/*****************************************************************************************/
unsigned char Mifare_Blockset(unsigned char block,unsigned char *buff)
{
    unsigned char   block_data[16],result;
    block_data[0]=buff[3];
    block_data[1]=buff[2];
    block_data[2]=buff[1];
    block_data[3]=buff[0];
    block_data[4]=~buff[3];
    block_data[5]=~buff[2];
    block_data[6]=~buff[1];
    block_data[7]=~buff[0];
    block_data[8]=buff[3];
    block_data[9]=buff[2];
    block_data[10]=buff[1];
    block_data[11]=buff[0];
    block_data[12]=block;
    block_data[13]=~block;
    block_data[14]=block;
    block_data[15]=~block;
    result= Mifare_Blockwrite(block,block_data);
    return result;
}

/*****************************************************************************************/
/*名称：Mifare_Blockread																 */
/*功能：Mifare_Blockread卡片读块操作													 */
/*输入：block，块号（0x00~0x3F）；buff，16字节读块数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockread(unsigned char block,unsigned char *buff)
{	
    unsigned char   send_buff[2],result;
    unsigned int   rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0]=0x30;//30 读块
    send_buff[1]=block;//块地址
    Clear_FIFO();
    result =Pcd_Comm(Transceive,send_buff,2,buff,&rece_bitlen);//
    // CL_LOG("rece_bitlen: %d ,result = %d \n", rece_bitlen, result);
    if ((result!=OK )||(rece_bitlen!=16*8)) //接收到的数据长度为16
        return ERROR;
    return OK;
}

/*****************************************************************************************/
/*名称：mifare_blockwrite																 */
/*功能：Mifare卡片写块操作																 */
/*输入：block，块号（0x00~0x3F）；buff，16字节写块数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/
unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *buff)
{
    unsigned char   result,send_buff[16],rece_buff[16];
    unsigned int   rece_bitlen;
    Pcd_SetTimer(1);
    send_buff[0] = 0xa0;//a0 写块
    send_buff[1] = block;//块地址

    result = Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);//
    //CL_LOG("rece_bitlen: %d ,result = %d ,rece_buff[0] = %d\n", rece_bitlen, result, rece_buff[0]);
    if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
    {
        //CL_LOG("Mifare_Blockwrite 错误1:\n");
        return(ERROR);
    }


    Pcd_SetTimer(5);
    result =Pcd_Comm(Transceive,buff,16,rece_buff,&rece_bitlen);//
    //CL_LOG("qa rece_bitlen: %d ,result = %d ,rece_buff[0] = %d\n", rece_bitlen, result, rece_buff[0]);
    if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A)) //如果未接收到0x0A，表示无ACK
    {
        //CL_LOG("Mifare_Blockwrite 错误2:\n");
        return(ERROR);
    }
    
    return OK;
}
/*****************************************************************************************/
/*名称：																				 */
/*功能：Mifare 卡片增值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节增值数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/

unsigned char Mifare_Blockinc(unsigned char block,unsigned char *buff)
{	
    unsigned char   result,send_buff[2],rece_buff[1];
    unsigned int   rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc1;//
    send_buff[1]=block;//块地址
    Clear_FIFO();
    result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    //PrintfData("quqian66Mifare_Blockinc1", rece_buff, 16);
    if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return ERROR;
    Pcd_SetTimer(5);
    Clear_FIFO();
    Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);
    //PrintfData("quqian66Mifare_Blockinc2", rece_buff, 16);
    return result;
}

/*****************************************************************************************/
/*名称：mifare_blockdec																	 */
/*功能：Mifare 卡片减值操作																 */
/*输入：block，块号（0x00~0x3F）；buff，4字节减值数据数组								 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/

unsigned char Mifare_Blockdec(unsigned char block,unsigned char *buff)
{	
    unsigned char   result,send_buff[2],rece_buff[1];
    unsigned int   rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc0;//
    send_buff[1]=block;//块地址
    Clear_FIFO();
    result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    //PrintfData("quqian77Mifare_Blockdec1", rece_buff, 16);
    if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return ERROR;
    Pcd_SetTimer(5);
    Clear_FIFO();
    Pcd_Comm(Transceive,buff,4,rece_buff,&rece_bitlen);
    //PrintfData("quqian77Mifare_Blockdec2", rece_buff, 16);
    return result;
}

/*****************************************************************************************/
/*名称：mifare_transfer																	 */
/*功能：Mifare 卡片transfer操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/

unsigned char Mifare_Transfer(unsigned char block)
{	
    unsigned char    result,send_buff[2],rece_buff[1];
    unsigned int    rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xb0;//
    send_buff[1]=block;//块地址
    Clear_FIFO();
    result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    //PrintfData("quqian88Mifare_Transfer", rece_buff, 16);
    if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return ERROR;
    return result;
}

/*****************************************************************************************/
/*名称：mifare_restore																	 */
/*功能：Mifare 卡片restore操作															 */
/*输入：block，块号（0x00~0x3F）														 */
/*输出:																					 */
/*		OK    :成功																		 */
/*		ERROR :失败																		 */
/*****************************************************************************************/

unsigned char Mifare_Restore(unsigned char block)
{	
    unsigned char    result,send_buff[4],rece_buff[1];
    unsigned int    rece_bitlen;
    Pcd_SetTimer(5);
    send_buff[0]=0xc2;//
    send_buff[1]=block;//块地址
    Clear_FIFO();
    result=Pcd_Comm(Transceive,send_buff,2,rece_buff,&rece_bitlen);
    //PrintfData("quqian99Mifare_Restore1", rece_buff, 16);
    if ((result!=OK )|((rece_buff[0]&0x0F)!=0x0A))	//如果未接收到0x0A，表示无ACK
        return ERROR;
    Pcd_SetTimer(5);
    send_buff[0]=0x00;
    send_buff[1]=0x00;
    send_buff[2]=0x00;
    send_buff[3]=0x00;
    Clear_FIFO();
    Pcd_Comm(Transceive,send_buff,4,rece_buff,&rece_bitlen);
    //PrintfData("quqian99Mifare_Restore2", rece_buff, 16);
    return result;
}

