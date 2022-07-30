/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file bignum.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <string.h>

#include "bignum.h"

#define BN_MAX_DIGIT        0xFFFFFFFF
#define BN_DIGIT_BITS       32 // For unsigned int
#define DIGIT_2MSB(x)       (unsigned int)(((x) >> (BN_DIGIT_BITS - 2)) & 0x03)
#define BN_ASSIGN_DIGIT(a, b, digits) \
    {                                 \
        bn_assign_zero(a, digits);    \
        a[0] = b;                     \
    }


static unsigned int bn_sub_digit_mul(unsigned int *a, unsigned int *b, unsigned int c,
                                     unsigned int *d, unsigned int digits)
{
    static unsigned long long result;
    unsigned int borrow, rh, rl;
    unsigned int i;

    result = 0;
    if (c == 0)
        return 0;

    borrow = 0;
    for (i = 0; i < digits; i++)
    {
        result = (unsigned long long)c * d[i];
        rl = result & BN_MAX_DIGIT;
        rh = (result >> BN_DIGIT_BITS) & BN_MAX_DIGIT;
        if ((a[i] = b[i] - borrow) > (BN_MAX_DIGIT - borrow))
        {
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }
        if ((a[i] -= rl) > (BN_MAX_DIGIT - rl))
        {
            borrow++;
        }
        borrow += rh;
    }

    return borrow;
}

static unsigned int bn_digit_bits(unsigned int a)
{
    unsigned int i;

    for (i = 0; i < BN_DIGIT_BITS; i++)
    {
        if (a == 0)
            break;
        a >>= 1;
    }

    return i;
}

static unsigned int bn_add_digit_mul(unsigned int *a, unsigned int *b, unsigned int c,
                                     unsigned int *d, unsigned int digits)
{
    static unsigned long long result;
    unsigned int carry, rh, rl;
    unsigned int i;

    result = 0;
    if (c == 0)
        return 0;

    carry = 0;
    for (i = 0; i < digits; i++)
    {
        result = (unsigned long long)c * d[i];
        rl = result & BN_MAX_DIGIT;
        rh = (result >> BN_DIGIT_BITS) & BN_MAX_DIGIT;
        if ((a[i] = b[i] + carry) < carry)
        {
            carry = 1;
        }
        else
        {
            carry = 0;
        }
        if ((a[i] += rl) < rl)
        {
            carry ++;
        }
        carry += rh;
    }

    return carry;
}

void bn_decode(unsigned int *bn, unsigned int digits, unsigned char *hexarr,
               unsigned int size)
{
    unsigned int t;
    int j;
    unsigned int i, u;

    for (i = 0, j = size - 1; i < digits && j >= 0; i++)
    {
        t = 0;
        for (u = 0; j >= 0 && u < BN_DIGIT_BITS; j--, u += 8)
        {
            t |= ((unsigned int)hexarr[j]) << u;
        }
        bn[i] = t;
    }

    for (; i < digits; i++)
    {
        bn[i] = 0;
    }
}

void bn_encode(unsigned char *hexarr, unsigned int size, unsigned int *bn,
               unsigned int digits)
{
    unsigned int t;
    int j;
    unsigned int i, u;

    for (i = 0, j = size - 1; i < digits && j >= 0; i++)
    {
        t = bn[i];
        for (u = 0; j >= 0 && u < BN_DIGIT_BITS; j--, u += 8)
        {
            hexarr[j] = (unsigned char)(t >> u);
        }
    }

    for (; j >= 0; j--)
    {
        hexarr[j] = 0;
    }
}

// a = b
static void bn_assign(unsigned int *a, unsigned int *b, unsigned int digits)
{
    unsigned int i;

    for (i = 0; i < digits; i++)
    {
        a[i] = b[i];
    }
}

// a = 0
void bn_assign_zero(unsigned int *a, unsigned int digits)
{
    unsigned int i;

    for (i = 0; i < digits; i++)
    {
        a[i] = 0;
    }
}

unsigned int bn_add(unsigned int *a, unsigned int *b, unsigned int *c,
                    unsigned int digits)
{
    unsigned int ai, carry;
    unsigned int i;

    carry = 0;
    for (i = 0; i < digits; i++)
    {
        if ((ai = b[i] + carry) < carry)
        {
            ai = c[i];
        }
        else if ((ai += c[i]) < c[i])
        {
            carry = 1;
        }
        else
        {
            carry = 0;
        }
        a[i] = ai;
    }

    return carry;
}

unsigned int bn_sub(unsigned int *a, unsigned int *b, unsigned int *c,
                    unsigned int digits)
{
    unsigned int ai, borrow;
    unsigned int i;

    borrow = 0;
    for (i = 0; i < digits; i++)
    {
        if ((ai = b[i] - borrow) > (BN_MAX_DIGIT - borrow))
        {
            ai = BN_MAX_DIGIT - c[i];
        }
        else if ((ai -= c[i]) > (BN_MAX_DIGIT - c[i]))
        {
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }
        a[i] = ai;
    }

    return borrow;
}

void bn_mul(unsigned int *a, unsigned int *b, unsigned int *c, unsigned int digits)
{
    static unsigned int t[2 * BN_MAX_DIGITS] = {0};
    unsigned int bdigits = 0, cdigits = 0, i = 0;

    // Clear potentially sensitive information
    memset((unsigned char *)t, 0, sizeof(t));

    bn_assign_zero(t, (2 * digits));
    bdigits = bn_digits(b, digits);
    cdigits = bn_digits(c, digits);

    for (i = 0; i < bdigits; i++)
    {
        t[i + cdigits] += bn_add_digit_mul(&t[i], &t[i], b[i], c, cdigits);
    }

    bn_assign(a, t, 2 * digits);
}

// a = b << c (a = b * 2^c)
static unsigned int bn_shift_l(unsigned int *a, unsigned int *b, unsigned int c,
                               unsigned int digits)
{
    unsigned int bi, carry;
    unsigned int i, t;

    if (c >= BN_DIGIT_BITS)
        return 0;

    t = BN_DIGIT_BITS - c;
    carry = 0;
    for (i = 0; i < digits; i++)
    {
        bi = b[i];
        a[i] = (bi << c) | carry;
        carry = c ? (bi >> t) : 0;
    }

    return carry;
}

// a = b >> c (a = b / 2^c)
static unsigned int bn_shift_r(unsigned int *a, unsigned int *b, unsigned int c,
                               unsigned int digits)
{
    unsigned int bi, carry;
    int i;
    unsigned int t;

    if (c >= BN_DIGIT_BITS)
        return 0;

    t = BN_DIGIT_BITS - c;
    carry = 0;
    i = digits - 1;
    for (; i >= 0; i--)
    {
        bi = b[i];
        a[i] = (bi >> c) | carry;
        carry = c ? (bi << t) : 0;
    }

    return carry;
}

// a = b / c, d = b % c
static void bn_div(unsigned int *a, unsigned int *b, unsigned int *c,
                   unsigned int cdigits, unsigned int *d, unsigned int ddigits)
{
    static unsigned long long tmp;;
    static unsigned int cc[2 * BN_MAX_DIGITS + 1] = {0}, dd[BN_MAX_DIGITS] = {0};
    int i;
    unsigned int ai, t, dddigits, shift;

    tmp = 0;
    // Clear potentially sensitive information
    memset((unsigned char *)cc, 0, sizeof(cc));
    memset((unsigned char *)dd, 0, sizeof(dd));

    dddigits = bn_digits(d, ddigits);
    if (dddigits == 0)
        return;

    shift = BN_DIGIT_BITS - bn_digit_bits(d[dddigits - 1]);
    bn_assign_zero(cc, dddigits);
    cc[cdigits] = bn_shift_l(cc, c, shift, cdigits);
    bn_shift_l(dd, d, shift, dddigits);
    t = dd[dddigits - 1];

    bn_assign_zero(a, cdigits);
    i = cdigits - dddigits;
    for (; i >= 0; i--)
    {
        if (t == BN_MAX_DIGIT)
        {
            ai = cc[i + dddigits];
        }
        else
        {
            tmp = cc[i + dddigits - 1];
            tmp += (unsigned long long)cc[i + dddigits] << BN_DIGIT_BITS;
            ai = tmp / (t + 1);
        }

        cc[i + dddigits] -= bn_sub_digit_mul(&cc[i], &cc[i], ai, dd, dddigits);
        while (cc[i + dddigits] || (bn_cmp(&cc[i], dd, dddigits) >= 0))
        {
            ai++;
            cc[i + dddigits] -= bn_sub(&cc[i], &cc[i], dd, dddigits);
        }
        a[i] = ai;
    }

    bn_assign_zero(b, ddigits);
    bn_shift_r(b, cc, shift, dddigits);
}

// a = b mod c
void bn_mod(unsigned int *a, unsigned int *b, unsigned int bdigits,
            unsigned int *c, unsigned int cdigits)
{
    static unsigned int t[2 * BN_MAX_DIGITS] = {0};

    // Clear potentially sensitive information
    memset((unsigned char *)t, 0, sizeof(t));

    bn_div(t, a, b, bdigits, c, cdigits);
}

void bn_mod_mul(unsigned int *a, unsigned int *b, unsigned int *c,
                unsigned int *d, unsigned int digits)
{
    static unsigned int t[2 * BN_MAX_DIGITS]={0};

    // Clear potentially sensitive information
    memset((unsigned char *)t, 0, sizeof(t));

    bn_mul(t, b, c, digits);
    bn_mod(a, t, (2 * digits), d, digits);
}

void bn_mod_exp(unsigned int *a, unsigned int *b, unsigned int *c,
                unsigned int cdigits, unsigned int *d, unsigned int ddigits)
{
    static unsigned int bpower[3][BN_MAX_DIGITS] = {0}, t[BN_MAX_DIGITS] = {0};
    int i;
    unsigned int ci = 0, ci_bits = 0, j, s;

    // Clear potentially sensitive information
    memset((unsigned char *)bpower, 0, sizeof(bpower));
    memset((unsigned char *)t, 0, sizeof(t));

    bn_assign(bpower[0], b, ddigits);
    bn_mod_mul(bpower[1], bpower[0], b, d, ddigits);
    bn_mod_mul(bpower[2], bpower[1], b, d, ddigits);

    BN_ASSIGN_DIGIT(t, 1, ddigits);

    cdigits = bn_digits(c, cdigits);
    i = cdigits - 1;

    for (; i >= 0; i--)
    {
        ci = c[i];
        ci_bits = BN_DIGIT_BITS;

        if (i == (int)(cdigits - 1))
        {
            while (!DIGIT_2MSB(ci))
            {
                ci <<= 2;
                ci_bits -= 2;
            }
        }

        for (j = 0; j < ci_bits; j += 2)
        {
            bn_mod_mul(t, t, t, d, ddigits);
            bn_mod_mul(t, t, t, d, ddigits);
            if ((s = DIGIT_2MSB(ci)) != 0)
            {
                bn_mod_mul(t, t, bpower[s - 1], d, ddigits);
            }
            ci <<= 2;
        }
    }
    bn_assign(a, t, ddigits);
}

int bn_cmp(unsigned int *a, unsigned int *b, unsigned int digits)
{
    int i;

    for (i = digits - 1; i >= 0; i--)
    {
        if (a[i] > b[i])
            return 1;
        if (a[i] < b[i])
            return -1;
    }

    return 0;
}

unsigned int bn_digits(unsigned int *a, unsigned int digits)
{
    int i;

    for (i = digits - 1; i >= 0; i--)
    {
        if (a[i])
            break;
    }

    return (i + 1);
}
