/*aes.h
* 2017-10-23
* Copyright(C) 2017
* liutao@chargerlink.com
*/
#ifndef __AES_H__
#define __AES_H__

#include "includes.h"


/* This should be a hidden type, but EVP requires that the size be known */
struct aes_key_st {
    unsigned int rd_key[4 *(14 + 1)];
    int rounds;
};

typedef struct aes_key_st AES_KEY;


void AES_decrypt(unsigned char *in, unsigned char *out, AES_KEY *key);
void AES_encrypt(unsigned char *in, unsigned char *out, AES_KEY *key);
int AES_set_decrypt_key(unsigned char *userKey, int bits, AES_KEY *key);
int AES_set_encrypt_key(unsigned char *userKey, int bits, AES_KEY *key);

#endif //__AES_H__


















