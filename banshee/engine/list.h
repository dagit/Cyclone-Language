/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#ifndef LIST_H
#define LIST_H

#include "regions.h"
#include "bool.h"

EXTERN_C_BEGIN

typedef void *list_data;
typedef void (*app_fn) (void *);
typedef bool (*eq_fn)(const void *);
typedef int (*comparator_fn)(const void *,const void *);

struct list;

typedef struct list_node_ *list_node;

struct list_scanner
{
  struct list *l;
  list_node cur;
}; /* Opaque type. Do not modify fields */


struct list *new_list(region r, int persist_kind);
struct list *new_persistent_list(int persist_kind);
int list_length(struct list *a);
struct list *list_cons(void *data, struct list *a);
struct list *list_append(struct list *a, struct list *b);
struct list *list_append_tail(void *data, struct list *a);
struct list *list_app(struct list *a,app_fn app);
void *list_find(struct list *a,eq_fn eq);
void *list_head(struct list *a);
void *list_last(struct list *a);
struct list *list_tail(struct list *a);
void list_drop(struct list *a, eq_fn eq);
struct list *list_filter(region r,struct list *a,eq_fn eq);
struct list *list_filter2(struct list *a,eq_fn eq);
struct list *list_keep(struct list *a,eq_fn eq);
struct list *list_copy(region r, struct list *a);
struct list *list_copy_upto(region r, struct list *a, int len);
struct list *list_sort(struct list *a, comparator_fn cmp);
struct list *list_merge(struct list *a,struct list *b, comparator_fn cmp);
void list_scan(struct list *a,struct list_scanner *scan);
bool list_next(struct list_scanner *scan, void **data);
bool list_empty(struct list *a);
bool list_member(struct list *a, void *data);
void list_clear(struct list *a);
struct list *list_reverse(struct list *a);
struct list *list_from_array(region r,void **data, int length);
void **array_from_list(region r,struct list *a);
int list_stamp(struct list *a);

#define DECLARE_OPAQUE_LIST(name,type) \
typedef struct list_scanner name ## _scanner; \
typedef void (* name ## _app_fn) (type); \
typedef bool (* name ## _eq_fn) (const type); \
typedef int (* name ## _comparator_fn)(const type,const type); \

#define DECLARE_LIST(name,type) \
typedef struct name ## _a *name; \
typedef struct list_scanner name ## _scanner; \
typedef void (* name ## _app_fn) (type); \
typedef bool (* name ## _eq_fn) (const type); \
typedef int (* name ## _comparator_fn)(const type,const type); \
name new_ ## name(region r); \
name new_persistent_ ## name(); \
int name ## _length(name a); \
name name ## _cons(type data, name a); \
name name ## _append(name a, name b); \
name name ## _append_tail(type data, name a); \
name name ## _app(name a, name ## _app_fn app); \
type name ## _find(name a, name ## _eq_fn eq); \
type name ## _head(name a); \
type name ## _last(name a); \
name name ## _tail(name a); \
void name ## _drop(name a, name ## _eq_fn eq); \
name name ## _filter(region r,name a, name ## _eq_fn eq); \
name name ## _filter2(name a, name ## _eq_fn eq); \
name name ## _keep(name a, name ## _eq_fn eq); \
name name ## _copy(region r, name a); \
name name ## _copy_upto(region r, name a, int l);		\
name name ## _sort(name a, name ## _comparator_fn cmp); \
name name ## _merge(name a,name b, name ## _comparator_fn cmp); \
void name ## _scan(name a, name ##_scanner *scan); \
bool name ## _next(name ##_scanner *scan, type *data); \
bool name ## _empty(name a); \
void name ## _clear(name a); \
bool name ## _member(name a, type data); \
name name ## _reverse(name a); \
int name ## _stamp(name a); \
name name ## _from_array(region r,type data[], int length); \
type *name ##_array_from_list(region r, name a);

#define DEFINE_LIST(name,type) \
name new_ ## name(region r) \
{ \
  return (name)new_list(r,BANSHEE_PERSIST_KIND_ ## type);\
} \
name new_persistent_ ## name() \
{ \
  return (name)new_list(NULL,BANSHEE_PERSIST_KIND_ ## type);\
} \
int name ## _length(name a) \
{ \
 return list_length((struct list *)a); \
} \
name name ## _cons(type data, name a) \
{ \
 return (name)list_cons((void *)data,(struct list *) a ); \
}\
name name ## _append(name a, name b) \
{ \
 return (name)list_append((struct list *)a,(struct list *)b);  \
} \
name name ## _append_tail(type data, name a) \
{\
  return (name)list_append_tail((void *)data,(struct list *) a); \
} \
name name ## _app(name a, name ## _app_fn app) \
{ \
 return (name)list_app((struct list *) a, (app_fn) app); \
} \
type name ## _find(name a, name ## _eq_fn eq) \
{ \
 return (type)list_find((struct list *)a, (eq_fn) eq); \
} \
name name ## _tail(name a) \
{\
 return (name)list_tail((struct list *)a);\
}\
type name ## _head(name a) \
{ \
 return (type)list_head((struct list *)a); \
} \
type name ## _last(name a) \
{\
  return (type)list_last((struct list *)a);\
}\
void name ## _drop(name a, name ## _eq_fn eq) \
{ \
 list_drop((struct list *)a, (eq_fn) eq); \
} \
name name ## _filter(region r,name a, name ## _eq_fn eq) \
{ \
 return (name)list_filter(r,(struct list *)a, (eq_fn) eq); \
} \
name name ## _keep(name a, name ## _eq_fn eq) \
{ \
 return (name)list_keep((struct list *)a, (eq_fn) eq); \
} \
name name ## _filter2(name a, name ## _eq_fn eq) \
{ \
 return (name)list_filter2((struct list *)a, (eq_fn) eq); \
} \
name name ## _copy(region r, name a) \
{ \
 return (name)list_copy(r,(struct list *) a); \
} \
name name ## _copy_upto(region r, name a, int l)	\
{ \
  return (name)list_copy_upto(r,(struct list *) a,l); \
} \
name name ## _sort(name a, name ## _comparator_fn cmp) \
{ \
 return (name)list_sort((struct list *)a,(comparator_fn) cmp); \
} \
name name ## _merge(name a,name b, name ## _comparator_fn cmp) \
{ \
 return (name)list_merge((struct list *)a,(struct list *)b,(comparator_fn)cmp); \
} \
void name ## _scan(name a, name ##_scanner *scan) \
{ \
 list_scan((struct list *)a,(struct list_scanner *)scan);\
}\
bool name ## _next(name ##_scanner *scan, type *data) \
{ \
 return list_next((struct list_scanner *)scan, (void **)data); \
} \
bool name ## _empty(name a) \
{ \
 return list_empty((struct list *)a); \
} \
void name ## _clear(name a) \
{ \
 return list_clear((struct list *)a); \
} \
bool name ## _member(name a, type data) \
{ \
 return list_member((struct list *)a,(void *)data); \
} \
name name ## _reverse(name a) \
{\
 return (name)list_reverse((struct list *)a);\
}\
name name ## _from_array(region r,type data[], int length) \
{\
 return (name)list_from_array(r,(void **)data,length); \
}\
int name ## _stamp(name a) \
{\
  return list_stamp((struct list*)a);\
}\
type *name ##_array_from_list(region r, name a) \
{\
 return (type *)array_from_list(r,(struct list *)a);\
}

#define DEFINE_KIND_LIST(name,type,kind)		\
name new_ ## name(region r) \
{ \
  return (name)new_list(r,kind);\
} \
name new_persistent_ ## name() \
{ \
  return (name)new_list(NULL,kind);\
} \
int name ## _length(name a) \
{ \
 return list_length((struct list *)a); \
} \
name name ## _cons(type data, name a) \
{ \
 return (name)list_cons((void *)data,(struct list *) a ); \
}\
name name ## _append(name a, name b) \
{ \
 return (name)list_append((struct list *)a,(struct list *)b);  \
} \
name name ## _append_tail(type data, name a) \
{\
  return (name)list_append_tail((void *)data,(struct list *) a); \
} \
name name ## _app(name a, name ## _app_fn app) \
{ \
 return (name)list_app((struct list *) a, (app_fn) app); \
} \
type name ## _find(name a, name ## _eq_fn eq) \
{ \
 return (type)list_find((struct list *)a, (eq_fn) eq); \
} \
name name ## _tail(name a) \
{\
 return (name)list_tail((struct list *)a);\
}\
type name ## _head(name a) \
{ \
 return (type)list_head((struct list *)a); \
} \
type name ## _last(name a) \
{\
  return (type)list_last((struct list *)a);\
}\
void name ## _drop(name a, name ## _eq_fn eq) \
{ \
 list_drop((struct list *)a, (eq_fn) eq); \
} \
name name ## _filter(region r,name a, name ## _eq_fn eq) \
{ \
 return (name)list_filter(r,(struct list *)a, (eq_fn) eq); \
} \
name name ## _keep(name a, name ## _eq_fn eq) \
{ \
 return (name)list_keep((struct list *)a, (eq_fn) eq); \
} \
name name ## _filter2(name a, name ## _eq_fn eq) \
{ \
 return (name)list_filter2((struct list *)a, (eq_fn) eq); \
} \
name name ## _copy(region r, name a) \
{ \
 return (name)list_copy(r,(struct list *) a); \
} \
name name ## _copy_upto(region r, name a, int l)	\
{ \
  return (name)list_copy_upto(r,(struct list *) a,l); \
} \
name name ## _sort(name a, name ## _comparator_fn cmp) \
{ \
 return (name)list_sort((struct list *)a,(comparator_fn) cmp); \
} \
name name ## _merge(name a,name b, name ## _comparator_fn cmp) \
{ \
 return (name)list_merge((struct list *)a,(struct list *)b,(comparator_fn)cmp); \
} \
void name ## _scan(name a, name ##_scanner *scan) \
{ \
 list_scan((struct list *)a,(struct list_scanner *)scan);\
}\
bool name ## _next(name ##_scanner *scan, type *data) \
{ \
 return list_next((struct list_scanner *)scan, (void **)data); \
} \
bool name ## _empty(name a) \
{ \
 return list_empty((struct list *)a); \
} \
void name ## _clear(name a) \
{ \
 return list_clear((struct list *)a); \
} \
bool name ## _member(name a, type data) \
{ \
 return list_member((struct list *)a,(void *)data); \
} \
name name ## _reverse(name a) \
{\
 return (name)list_reverse((struct list *)a);\
}\
name name ## _from_array(region r,type data[], int length) \
{\
 return (name)list_from_array(r,(void **)data,length); \
}\
int name ## _stamp(name a) \
{\
  return list_stamp((struct list*)a);\
}\
type *name ##_array_from_list(region r, name a) \
{\
 return (type *)array_from_list(r,(struct list *)a);\
}

#define DEFINE_NONPTR_LIST(name,type)		\
name new_ ## name(region r) \
{ \
  return (name)new_list(r,0);\
} \
name new_persistent_ ## name() \
{ \
  return (name)new_list(NULL,0);\
} \
int name ## _length(name a) \
{ \
 return list_length((struct list *)a); \
} \
name name ## _cons(type data, name a) \
{ \
 return (name)list_cons((void *)data,(struct list *) a ); \
}\
name name ## _append(name a, name b) \
{ \
 return (name)list_append((struct list *)a,(struct list *)b);  \
} \
name name ## _append_tail(type data, name a) \
{\
  return (name)list_append_tail((void *)data,(struct list *) a); \
} \
name name ## _app(name a, name ## _app_fn app) \
{ \
 return (name)list_app((struct list *) a, (app_fn) app); \
} \
type name ## _find(name a, name ## _eq_fn eq) \
{ \
 return (type)list_find((struct list *)a, (eq_fn) eq); \
} \
name name ## _tail(name a) \
{\
 return (name)list_tail((struct list *)a);\
}\
type name ## _head(name a) \
{ \
 return (type)list_head((struct list *)a); \
} \
type name ## _last(name a) \
{\
  return (type)list_last((struct list *)a);\
}\
void name ## _drop(name a, name ## _eq_fn eq) \
{ \
 list_drop((struct list *)a, (eq_fn) eq); \
} \
name name ## _filter(region r,name a, name ## _eq_fn eq) \
{ \
 return (name)list_filter(r,(struct list *)a, (eq_fn) eq); \
} \
name name ## _keep(name a, name ## _eq_fn eq) \
{ \
 return (name)list_keep((struct list *)a, (eq_fn) eq); \
} \
name name ## _filter2(name a, name ## _eq_fn eq) \
{ \
 return (name)list_filter2((struct list *)a, (eq_fn) eq); \
} \
name name ## _copy(region r, name a) \
{ \
 return (name)list_copy(r,(struct list *) a); \
} \
name name ## _copy_upto(region r, name a, int l)	\
{ \
  return (name)list_copy_upto(r,(struct list *) a,l); \
} \
name name ## _sort(name a, name ## _comparator_fn cmp) \
{ \
 return (name)list_sort((struct list *)a,(comparator_fn) cmp); \
} \
name name ## _merge(name a,name b, name ## _comparator_fn cmp) \
{ \
 return (name)list_merge((struct list *)a,(struct list *)b,(comparator_fn)cmp); \
} \
void name ## _scan(name a, name ##_scanner *scan) \
{ \
 list_scan((struct list *)a,(struct list_scanner *)scan);\
}\
bool name ## _next(name ##_scanner *scan, type *data) \
{ \
 return list_next((struct list_scanner *)scan, (void **)data); \
} \
bool name ## _empty(name a) \
{ \
 return list_empty((struct list *)a); \
} \
void name ## _clear(name a) \
{ \
 return list_clear((struct list *)a); \
} \
bool name ## _member(name a, type data) \
{ \
 return list_member((struct list *)a,(void *)data); \
} \
name name ## _reverse(name a) \
{\
 return (name)list_reverse((struct list *)a);\
}\
name name ## _from_array(region r,type data[], int length) \
{\
 return (name)list_from_array(r,(void **)data,length); \
}\
int name ## _stamp(name a) \
{\
  return list_stamp((struct list*)a);\
}\
type *name ##_array_from_list(region r, name a) \
{\
 return (type *)array_from_list(r,(struct list *)a);\
}

void list_init();
void list_reset();

EXTERN_C_END

#endif /* LIST_H */
