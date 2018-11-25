#ifndef __M1_CARD_H__
#define __M1_CARD_H__

#include "includes.h"
#include "fm175xx.h"

extern unsigned char TypeA_Request(unsigned char *pTagType);
extern unsigned char TypeA_WakeUp(unsigned char *pTagType);
extern unsigned char TypeA_Anticollision(unsigned char selcode,unsigned char *pSnr);
extern unsigned char TypeA_Select(unsigned char selcode,unsigned char *pSnr,unsigned char *pSak);
extern unsigned char TypeA_Halt(void);
extern unsigned char TypeA_CardActivate(unsigned char *pTagType,unsigned char *pSnr,unsigned char *pSak);
void TypeA_Set_NVB(unsigned char collpos,unsigned char *nvb,unsigned char *row,unsigned char *col);
void TypeA_Set_Bit_Framing(unsigned char collpos,unsigned char *bit_framing);

extern unsigned char Mifare_Transfer(unsigned char block);
extern unsigned char Mifare_Restore(unsigned char block);
extern unsigned char Mifare_Blockset(unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Blockinc(unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Blockdec(unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Blockwrite(unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Blockread(unsigned char block,unsigned char *buff);
extern unsigned char Mifare_Auth(unsigned char mode,unsigned char sector,unsigned char *mifare_key,unsigned char *card_uid);



#endif
