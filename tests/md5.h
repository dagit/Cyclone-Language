/*
 **********************************************************************
 ** md5.h -- Header file for implementation of MD5 (Cyclone Version) **
 ** Derived from RSA Data Security, Inc.                             **
 ** MD5 Message Digest Algorithm.                                    **
 ** Created: May 10 2003                                             **
 ** Creator: I-Hsuan Huang <ihhuang@acm.org>                         **
 **********************************************************************
 */

/*
 ************************************************************************
 * Copyright (C) 2003 I-Hsuan Huang <ihhuang@acm.org>                   *
 *                                                                      *
 * This Program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU Gereral Public License as publisded by *
 * the Free Software Foundation; eigher version 2 of the License, or    *
 * (at your option) any laterversion.                                   *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program; if not, write to the Free Software          *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston,               *
 * MA 02111-1307, USA.                                                  *
 ************************************************************************
 */

/* typedef a 32 bit type */
typedef unsigned long int UINT4;

/* Data structure for MD5 (Message Digest) computation */
typedef struct {
  UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
  UINT4 buf[4];                                    /* scratch buffer */
  unsigned char in[64];                              /* input buffer */
  unsigned char digest[16];     /* actual digest after MD5Final call */
} MD5_CTX;

extern void MD5Init (MD5_CTX *mdContext);
extern void MD5Update (MD5_CTX *mdContext, unsigned char ?@nozeroterm inBuf, unsigned int
inLen);
extern void MD5Final (MD5_CTX *mdContext);
