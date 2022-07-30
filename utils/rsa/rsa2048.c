/*****************************************************************************
 * Copyright (c) 2020, TBOX Electronics Co., Ltd
 * All rights , including trade secret rights , reserved .
 * ***************************************************************************
 * @file rsa2048.c
 * @author tanxt
 * @version v1.0.0
 * **************************************************************************/

#include <string.h>

#include "rsa2048.h"
#include "bignum.h"
#include "keys.h"

#define PADDING_SIZE 11
#define KEY_M_BITS 2048

static unsigned char pkcs_block[RSA_MAX_MODULUS_LEN] = {0};
static int status;

static rsa_pk_t pk = {0};
static rsa_sk_t sk = {0};

static void rsa_load_key(rsa_pk_t *public_key, rsa_sk_t *private_key)
{
    // copy keys.h message about public key and private key to the flash RAM
    public_key->bits = KEY_M_BITS;
    memcpy(&public_key->modulus         [RSA_MAX_MODULUS_LEN-sizeof(key_m) ],  key_m,  sizeof(key_m ));
    memcpy(&public_key->exponent        [RSA_MAX_MODULUS_LEN-sizeof(key_e) ],  key_e,  sizeof(key_e ));
    private_key->bits = KEY_M_BITS;
    memcpy(&private_key->modulus        [RSA_MAX_MODULUS_LEN-sizeof(key_m) ],  key_m,  sizeof(key_m ));
    memcpy(&private_key->public_exponet [RSA_MAX_MODULUS_LEN-sizeof(key_e) ],  key_e,  sizeof(key_e ));
    memcpy(&private_key->exponent       [RSA_MAX_MODULUS_LEN-sizeof(key_ex)],  key_ex, sizeof(key_ex));
    memcpy(&private_key->prime1         [RSA_MAX_PRIME_LEN - sizeof(key_p1)],  key_p1, sizeof(key_p1));
    memcpy(&private_key->prime2         [RSA_MAX_PRIME_LEN - sizeof(key_p2)],  key_p2, sizeof(key_p2));
    memcpy(&private_key->prime_exponent1[RSA_MAX_PRIME_LEN - sizeof(key_e1)],  key_e1, sizeof(key_e1));
    memcpy(&private_key->prime_exponent2[RSA_MAX_PRIME_LEN - sizeof(key_e2)],  key_e2, sizeof(key_e2));
    memcpy(&private_key->coefficient    [RSA_MAX_PRIME_LEN - sizeof(key_c) ],  key_c,  sizeof(key_c ));
}

static void generate_rand(unsigned char *block, unsigned int block_len)
{
    unsigned int i;

    for (i = 0; i < block_len; i++)
    {
        block[i] = i;
    }
}

static int public_block_operation(unsigned char *out, unsigned int *out_len,
                                  unsigned char *in, unsigned int in_len, rsa_pk_t *pk)
{
    unsigned int edigits = 0, ndigits = 0;
    static unsigned int c[BN_MAX_DIGITS] = {0}, e[BN_MAX_DIGITS] = {0};
    static unsigned int m[BN_MAX_DIGITS] = {0}, n[BN_MAX_DIGITS] = {0};

    bn_decode(m, BN_MAX_DIGITS, in, in_len);
    bn_decode(n, BN_MAX_DIGITS, pk->modulus, RSA_MAX_MODULUS_LEN);
    bn_decode(e, BN_MAX_DIGITS, pk->exponent, RSA_MAX_MODULUS_LEN);

    ndigits = bn_digits(n, BN_MAX_DIGITS);
    edigits = bn_digits(e, BN_MAX_DIGITS);

    if (bn_cmp(m, n, ndigits) >= 0)
    {
        return ERR_WRONG_DATA;
    }

    bn_mod_exp(c, m, e, edigits, n, ndigits);

    *out_len = (pk->bits) >> 3;
    bn_encode(out, *out_len, c, ndigits);

    // Clear potentially sensitive information
    memset((unsigned char *)c, 0, sizeof(c));
    memset((unsigned char *)m, 0, sizeof(m));

    return 0;
}

static int private_block_operation(unsigned char *out, unsigned int *out_len,
                                   unsigned char *in, unsigned int in_len, rsa_sk_t *sk)
{
    unsigned int cdigits = 0, ndigits = 0, pdigits = 0;
    static unsigned int  c[BN_MAX_DIGITS] = {0}, cp[BN_MAX_DIGITS] = {0}, cq[BN_MAX_DIGITS] = {0};
    static unsigned int dp[BN_MAX_DIGITS] = {0}, dq[BN_MAX_DIGITS] = {0};
    static unsigned int mp[BN_MAX_DIGITS] = {0}, mq[BN_MAX_DIGITS] = {0};
    static unsigned int  n[BN_MAX_DIGITS] = {0},  t[BN_MAX_DIGITS] = {0};
    static unsigned int  p[BN_MAX_DIGITS] = {0},  q[BN_MAX_DIGITS] = {0}, q_inv[BN_MAX_DIGITS] = {0};


    bn_decode(c, BN_MAX_DIGITS, in, in_len);
    bn_decode(n, BN_MAX_DIGITS, sk->modulus, RSA_MAX_MODULUS_LEN);
    bn_decode(p, BN_MAX_DIGITS, sk->prime1, RSA_MAX_PRIME_LEN);
    bn_decode(q, BN_MAX_DIGITS, sk->prime2, RSA_MAX_PRIME_LEN);
    bn_decode(dp, BN_MAX_DIGITS, sk->prime_exponent1, RSA_MAX_PRIME_LEN);
    bn_decode(dq, BN_MAX_DIGITS, sk->prime_exponent2, RSA_MAX_PRIME_LEN);
    bn_decode(q_inv, BN_MAX_DIGITS, sk->coefficient, RSA_MAX_PRIME_LEN);

    cdigits = bn_digits(c, BN_MAX_DIGITS);
    ndigits = bn_digits(n, BN_MAX_DIGITS);
    pdigits = bn_digits(p, BN_MAX_DIGITS);

    if (bn_cmp(c, n, ndigits) >= 0)
        return ERR_WRONG_DATA;

    bn_mod(cp, c, cdigits, p, pdigits);
    bn_mod(cq, c, cdigits, q, pdigits);
    bn_mod_exp(mp, cp, dp, pdigits, p, pdigits);
    bn_assign_zero(mq, ndigits);
    bn_mod_exp(mq, cq, dq, pdigits, q, pdigits);

    if (bn_cmp(mp, mq, pdigits) >= 0)
    {
        bn_sub(t, mp, mq, pdigits);
    }
    else
    {
        bn_sub(t, mq, mp, pdigits);
        bn_sub(t, p, t, pdigits);
    }

    bn_mod_mul(t, t, q_inv, p, pdigits);
    bn_mul(t, t, q, pdigits);
    bn_add(t, t, mq, ndigits);

    *out_len = (sk->bits) >> 3;
    bn_encode(out, *out_len, t, ndigits);

    // Clear potentially sensitive information
    memset((unsigned char *)c, 0, sizeof(c));
    memset((unsigned char *)cp, 0, sizeof(cp));
    memset((unsigned char *)cq, 0, sizeof(cq));
    memset((unsigned char *)dp, 0, sizeof(dp));
    memset((unsigned char *)dq, 0, sizeof(dq));
    memset((unsigned char *)mp, 0, sizeof(mp));
    memset((unsigned char *)mq, 0, sizeof(mq));
    memset((unsigned char *)p, 0, sizeof(p));
    memset((unsigned char *)q, 0, sizeof(q));
    memset((unsigned char *)q_inv, 0, sizeof(q_inv));
    memset((unsigned char *)t, 0, sizeof(t));

    return 0;
}

int rsa_public_encrypt(unsigned char *out, unsigned int *out_len,
                       unsigned char *in, unsigned int in_len, rsa_pk_t *pk)
{
    unsigned char byte;
    unsigned int i, modulus_len;

    modulus_len = (pk->bits) >> 3;
    if (in_len + PADDING_SIZE > modulus_len)
    {
        return ERR_WRONG_LEN;
    }

    pkcs_block[0] = 0;
    pkcs_block[1] = 2;
    for (i = 2; i < modulus_len - in_len - 1; i++)
    {
        do
        {
            generate_rand(&byte, 1);
        } while (byte == 0);
        pkcs_block[i] = byte;
    }

    pkcs_block[i++] = 0;

    memcpy((unsigned char *)&pkcs_block[i], (unsigned char *)in, in_len);
    status = public_block_operation(out, out_len, pkcs_block, modulus_len, pk);

    // Clear potentially sensitive information
    byte = 0;
    memset((unsigned char *)pkcs_block, 0, sizeof(pkcs_block));

    return status;
}

int rsa_public_decrypt(unsigned char *out, unsigned int *out_len,
                       unsigned char *in, unsigned int in_len, rsa_pk_t *pk)
{
    unsigned int i, modulus_len, pkcs_block_len;

    modulus_len = (pk->bits) >> 3;
    if (in_len > modulus_len)
        return ERR_WRONG_LEN;

    status = public_block_operation(pkcs_block, &pkcs_block_len, in, in_len, pk);
    if (status != 0)
        return status;

    if (pkcs_block_len != modulus_len)
        return ERR_WRONG_LEN;

    if ((pkcs_block[0] != 0) || (pkcs_block[1] != 1))
        return ERR_WRONG_DATA;

    for (i = 2; i < modulus_len - 1; i++)
    {
        if (pkcs_block[i] != 0xFF)
            break;
    }

    if (pkcs_block[i++] != 0)
        return ERR_WRONG_DATA;

    *out_len = modulus_len - i;
    if (*out_len + PADDING_SIZE > modulus_len)
        return ERR_WRONG_DATA;

    memcpy((unsigned char *)out, (unsigned char *)&pkcs_block[i], *out_len);

    // Clear potentially sensitive information
    memset((unsigned char *)pkcs_block, 0, sizeof(pkcs_block));

    return status;
}

int rsa_private_encrypt(unsigned char *out, unsigned int *out_len,
                        unsigned char *in, unsigned int in_len, rsa_sk_t *sk)
{
    static unsigned char pkcs_block[RSA_MAX_MODULUS_LEN];
    unsigned int i, modulus_len;

    // Clear potentially sensitive information
    memset((unsigned char *)pkcs_block, 0, sizeof(pkcs_block));

    modulus_len = (sk->bits) >> 3;
    if (in_len + PADDING_SIZE > modulus_len)
        return ERR_WRONG_LEN;

    pkcs_block[0] = 0;
    pkcs_block[1] = 1;
    for (i = 2; i < modulus_len - in_len - 1; i++)
    {
        pkcs_block[i] = 0xFF;
    }

    pkcs_block[i++] = 0;

    memcpy((unsigned char *)&pkcs_block[i], (unsigned char *)in, in_len);

    status = private_block_operation(out, out_len, pkcs_block, modulus_len, sk);

    return status;
}

int rsa_private_decrypt(unsigned char *out, unsigned int *out_len,
                        unsigned char *in, unsigned int in_len, rsa_sk_t *sk)
{
    unsigned int i, modulus_len = 0, pkcs_block_len = 0;

    modulus_len = (sk->bits) >> 3;
    if (in_len > modulus_len)
        return ERR_WRONG_LEN;

    status = private_block_operation(pkcs_block, &pkcs_block_len, in, in_len, sk);
    if (status != 0)
        return status;

    if (pkcs_block_len != modulus_len)
        return ERR_WRONG_LEN;

    if ((pkcs_block[0] != 0) || (pkcs_block[1] != 2))
        return ERR_WRONG_DATA;

    for (i = 2; i < modulus_len - 1; i++)
    {
        if (pkcs_block[i] == 0)
            break;
    }

    i++;
    if (i >= modulus_len)
        return ERR_WRONG_DATA;
    *out_len = modulus_len - i;
    if (*out_len + PADDING_SIZE > modulus_len)
        return ERR_WRONG_DATA;
    memcpy((unsigned char *)out, (unsigned char *)&pkcs_block[i], *out_len);
    // Clear potentially sensitive information
    memset((unsigned char *)pkcs_block, 0, sizeof(pkcs_block));

    return status;
}

int rsa_decrypt(unsigned char *in, unsigned int in_len,
                unsigned char *out, unsigned int *out_len)
{
    rsa_load_key(&pk, &sk);
    status = rsa_public_decrypt(out, out_len, in, in_len, &pk);
    return status;
}
