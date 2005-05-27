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

#ifndef SSL_H
#define SSL_H
#include <stdio.h>
#include <core.h>

struct ssl_st;
typedef struct ssl_st SSL;
struct ssl_ctx_st;
typedef struct ssl_ctx_st SSL_CTX;
struct ssl_method_st;
typedef struct ssl_method_st SSL_METHOD;
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
extern "C" void ERR_print_errors_fp(FILE @fp);
extern "C" int SSL_get_error(SSL @ssl, int ret);

#endif
