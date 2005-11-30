/* This file is part of the RC compiler.
   Copyright (C) 2000-2001 The Regents of the University of California.

RC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

RC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef ARRAY_H
#define ARRAY_H

/* A region-based growable array type */

struct array;

struct array *new_array(region r, size_t initialsize,
			size_t typesize, type_t typeinfo);
void *array_extend(struct array *a, int by);
void array_reset(struct array *a);
size_t array_length(struct array *a);
void *array_data(struct array *a);


#define DECLARE_ARRAY(name, type) \
typedef struct name ## _a *name; \
name new_ ## name(region r, size_t initialsize); \
type *name ## _extend(name a, int by); \
void name ## _reset(name a); \
size_t name ## _length(name a); \
type *name ## _data(name a);

#define DEFINE_ARRAY(name, type) \
name new_ ## name(region r, size_t initialsize) \
{ \
  return (name)new_array(r, initialsize, sizeof(type), rctypeof(type)); \
} \
type *name ## _extend(name a, int by) \
{ \
  return array_extend((struct array *)a, by); \
} \
void name ## _reset(name a) \
{ \
  return array_reset((struct array *)a); \
} \
size_t name ## _length(name a) \
{ \
  return array_length((struct array *)a); \
} \
type *name ## _data(name a) \
{ \
  return array_data((struct array *)a); \
}

#endif
