/*
 * The Shadow Simulator
 *
 * Copyright (c) 2010-2011 Rob Jansen <jansen@cs.umn.edu>
 *
 * This file is part of Shadow.
 *
 * Shadow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Shadow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shadow.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <dlfcn.h>

#include <openssl/aes.h>
#include <openssl/ssl.h>
#include "shadow.h"

#define CRYPTO_LIB_PREFIX "intercept_"

typedef void (*AES_encrypt_fp)(const unsigned char *, unsigned char *, const AES_KEY *);
typedef void (*AES_decrypt_fp)(const unsigned char *, unsigned char *, const AES_KEY *);
typedef int (*EVP_Cipher_fp)(EVP_CIPHER_CTX *ctx, unsigned char *out, const unsigned char *in, unsigned int inl);

/* pointer to our version */
static AES_encrypt_fp _intercept_AES_encrypt = NULL;
static AES_decrypt_fp _intercept_AES_decrypt = NULL;
static EVP_Cipher_fp _intercept_EVP_Cipher = NULL;

/* pointer to openssl's version */
static AES_encrypt_fp _AES_encrypt = NULL;
static AES_decrypt_fp _AES_decrypt = NULL;
static EVP_Cipher_fp _EVP_Cipher = NULL;

/* avoid multiple hash table lookups */
//static int _did_check_config = 0;
//static int _do_intercept = 0;

void AES_encrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key) {
	AES_encrypt_fp* func;
	char* funcName;

//	/* check config option to see if we should intercept the call */
//	if(_did_check_config == 0) {
//		_do_intercept = sysconfig_get_int("do_intercept_crypto");
//		_did_check_config = 1;
//	}
//
//	if(_do_intercept == 1) {
//		/* use our version */
//		fp_ptr = &_intercept_AES_encrypt;
//		f_name = CRYPTO_LIB_PREFIX "AES_encrypt";
//	} else {
//		/* use openssl's version */
//		fp_ptr = &_AES_encrypt;
//		f_name = "AES_encrypt";
//	}

	PRELOAD_DECIDE(func, funcName, "AES_encrypt", _AES_encrypt, CRYPTO_LIB_PREFIX, _intercept_AES_encrypt, 1);
	/* this is a void func, so no return val in the third param */
	PRELOAD_LOOKUP(func, funcName,);
	(*func)(in, out, key);
}

void AES_decrypt(const unsigned char *in, unsigned char *out, const AES_KEY *key) {
	AES_decrypt_fp* func;
	char* funcName;
	PRELOAD_DECIDE(func, funcName, "AES_decrypt", _AES_decrypt, CRYPTO_LIB_PREFIX, _intercept_AES_decrypt, 1);
	/* this is a void func, so no return val in the third param */
	PRELOAD_LOOKUP(func, funcName,);
	(*func)(in, out, key);
}

int EVP_Cipher(EVP_CIPHER_CTX *ctx, unsigned char *out, const unsigned char *in, unsigned int inl)
{
	EVP_Cipher_fp* func;
	char* funcName;
	PRELOAD_DECIDE(func, funcName, "EVP_Cipher", _EVP_Cipher, CRYPTO_LIB_PREFIX, _intercept_EVP_Cipher, 1);
	PRELOAD_LOOKUP(func, funcName, -1);
	return (*func)(ctx, out, in, inl);
}