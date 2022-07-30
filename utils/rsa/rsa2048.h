/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file rsa2048.h
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#ifndef __RSA_2048_H__
#define __RSA_2048_H__

// RSA key lengths
#define RSA_MAX_MODULUS_BITS                2048
#define RSA_MAX_MODULUS_LEN                 ((RSA_MAX_MODULUS_BITS) >> 3)
#define RSA_MAX_PRIME_BITS                  ((RSA_MAX_MODULUS_BITS) >> 1)
#define RSA_MAX_PRIME_LEN                   ((RSA_MAX_PRIME_BITS) >> 3)

// Error codes
#define ERR_WRONG_DATA                      0x1001
#define ERR_WRONG_LEN                       0x1002


typedef struct
{
    unsigned int  bits;
    unsigned char modulus[RSA_MAX_MODULUS_LEN];
    unsigned char exponent[RSA_MAX_MODULUS_LEN];
} rsa_pk_t;

typedef struct
{
    unsigned int  bits;
    unsigned char modulus[RSA_MAX_MODULUS_LEN];
    unsigned char public_exponet[RSA_MAX_MODULUS_LEN];
    unsigned char exponent[RSA_MAX_MODULUS_LEN];
    unsigned char prime1[RSA_MAX_PRIME_LEN];
    unsigned char prime2[RSA_MAX_PRIME_LEN];
    unsigned char prime_exponent1[RSA_MAX_PRIME_LEN];
    unsigned char prime_exponent2[RSA_MAX_PRIME_LEN];
    unsigned char coefficient[RSA_MAX_PRIME_LEN];
} rsa_sk_t;

int rsa_decrypt(unsigned char *in, unsigned int in_len,
                unsigned char *out, unsigned int *out_len);
#endif
