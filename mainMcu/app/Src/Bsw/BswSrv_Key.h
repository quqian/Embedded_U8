#ifndef _BSWSRV_KEY_H__
#define _BSWSRV_KEY_H__


#define KEY_TP0		    0x01
#define KEY_TP1		    0x02
#define KEY_TP2		    0x04
#define KEY_TP3		    0x08
#define KEY_TP4		    0x10
#define KEY_TP5		    0x20
#define KEY_TP6		    0x40
#define KEY_TP7		    0x80

#define KEY_TP8		    0x01
#define KEY_TP9		    0x02
#define KEY_TP10		0x04
#define KEY_TP11		0x08

void BswSrv_Key_Loop(void);

void BswSrv_Key_Init(void);

#endif
