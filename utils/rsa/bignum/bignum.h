/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 *****************************************************************************
 * @file bignum.h
 * @author tanxt
 * @version v1.0.0
 ****************************************************************************/

#ifndef __BIGNUM_H__
#define __BIGNUM_H__

#define BN_MAX_DIGITS 65 // RSA_MAX_MODULUS_LEN + 1


void bn_decode(unsigned int *bn, unsigned int digits, unsigned char *hexarr, unsigned int size);
void bn_encode(unsigned char *hexarr, unsigned int size, unsigned int *bn, unsigned int digits);

// a = 0
void bn_assign_zero(unsigned int *a, unsigned int digits);

// a = b + c, return carry
unsigned int bn_add(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int digits);
// a = b - c, return borrow
unsigned int bn_sub(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int digits);
// a = b * c
void bn_mul(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int digits);

// a = b mod c
void bn_mod(unsigned int *a, unsigned int *b, unsigned int bdigits, unsigned int *c, unsigned int cdigits);
// a = b * c mod d
void bn_mod_mul(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int *d, unsigned int digits);
// a = b ^ c mod d
void bn_mod_exp(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int cdigits, unsigned int *d, unsigned int ddigits);

// returns sign of a - b
int bn_cmp(unsigned int *a, unsigned int *b, unsigned int digits);

// returns significant length of a in digits
unsigned int bn_digits(unsigned int *a, unsigned int digits);

#endif // __BIGNUM_H__
