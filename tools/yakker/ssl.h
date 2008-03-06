/* This file is part of the Cyclone Library.
   Copyright (C) 2005 Greg Morrisett, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

/*
   Interface to SSL library for Yakker
 */
#ifndef SSL_H
#define SSL_H
#include <stdio.h>
#include <core.h>
#include "ykbuf.h"

struct ssl_st;
typedef struct ssl_st SSL;
struct ssl_ctx_st;
typedef struct ssl_ctx_st SSL_CTX;
struct ssl_method_st;
typedef struct ssl_method_st SSL_METHOD;
struct bio_st;
typedef struct bio_st BIO;
#define SSLeay_add_ssl_algorithms() SSL_library_init()
extern "C" int SSL_library_init();
extern "C" int SSL_set_fd(SSL @s,int fd);
extern "C" int SSL_set_wfd(SSL @s,int fd);
extern "C" int SSL_set_rfd(SSL @s,int fd);
extern "C" int SSL_connect(SSL @ssl);
extern "C" void SSL_load_error_strings();
extern "C" SSL_CTX *SSL_CTX_new(SSL_METHOD @meth);
extern "C" SSL_METHOD @ SSLv2_client_method();
extern "C" SSL_METHOD @ SSLv3_client_method();
extern "C" SSL_METHOD @ SSLv23_client_method();
extern "C" SSL_METHOD @ TLSv1_client_method();
extern "C" SSL_METHOD @ SSLv2_server_method();
extern "C" SSL_METHOD @ SSLv3_server_method();
extern "C" SSL_METHOD @ SSLv23_server_method();
extern "C" SSL_METHOD @ TLSv1_server_method();
extern "C" SSL_METHOD @ SSLv2_method();
extern "C" SSL_METHOD @ SSLv3_method();
extern "C" SSL_METHOD @ SSLv23_method();
extern "C" SSL_METHOD @ TLSv1_method();
extern "C" SSL *SSL_new(SSL_CTX @ctx);
extern int SSL_read(SSL @ssl, char ? @nozeroterm buf, int count);
extern int SSL_write(SSL @, const char ? @nozeroterm, int);
// Unsafe so disabled for now; needs consume
//  extern "C" void SSL_free(SSL @ssl);
#define SSL_free(x) (0)
// Unsafe so disabled for now; needs consume
//  extern "C" void SSL_CTX_free(SSL_CTX @ctx);
#define SSL_CTX_free(x) (0)
extern void ERR_print_errors_fp(FILE @fp);
extern "C" int SSL_get_error(SSL @ssl, int ret);
extern "C" BIO *BIO_new_socket(int sock, int close_flag);
extern "C" int SSL_accept(SSL *ssl);
extern "C" void SSL_set_bio(SSL *s, BIO *rbio,BIO *wbio);

#define X509_FILETYPE_PEM 1
#define X509_FILETYPE_ASN1 2
#define SSL_FILETYPE_PEM X509_FILETYPE_PEM
#define SSL_FILETYPE_ASN1 X509_FILETYPE_ASN1

#define SSL_ERROR_NONE             0
#define SSL_ERROR_SSL              1
#define SSL_ERROR_WANT_READ        2
#define SSL_ERROR_WANT_WRITE       3
#define SSL_ERROR_WANT_X509_LOOKUP 4
#define SSL_ERROR_SYSCALL          5
#define SSL_ERROR_ZERO_RETURN      6
#define SSL_ERROR_WANT_CONNECT     7
#define SSL_ERROR_WANT_ACCEPT      8

extern "C" int SSL_CTX_use_certificate_file(SSL_CTX @ctx, const char @file, int type);
extern "C" int SSL_CTX_use_PrivateKey_file(SSL_CTX @ctx, const char @file, int type);
extern "C" int SSL_CTX_check_private_key(SSL_CTX @ctx);

extern ykbuf_t @ssl2ykbuf(SSL@`H);

#endif
