/* This file is part of the Cyclone Library.
   Copyright (C) 2004 Greg Morrisett

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

#ifndef _SEXP_H_
#define _SEXP_H_
#include <cycboot.h>
#include <hashtable.h>
/* This file defines an extensible form of data suitable for serializing
   to and from files.  Values are represented as objects that know how
   to print and parse themselves from strings.  New kinds of objects can
   be defined by defining a new class (with suitable methods) and registering
   that class.  Built-in classes include support for all of the integral
   types, floats, doubles, and tuples of values.  These are all hash-consed
   when created to ensure maximal sharing among the objects.

   This also demonstrates the use of many advanced features in Cyclone,
   including how to do class-based OO programming.
*/
namespace Sexp {
/* various forward declarations of types */
struct Class<`a::A>;
struct Obj<`a::A>;
struct Object<`e::E>;
struct Visitor<`a,`env,`r::E>;
typedef struct Class<`a> Class_t<`a>;
typedef struct Object<`H> Object_t;
typedef struct Obj<`a> Obj_t<`a>;
typedef struct Visitor<`a,`env,`r> Visitor_t<`a,`env,`r>;
typedef Visitor_t<`a,`env,`r>@`r visitor_t<`a,`env,`r>;

/* A parser is used to read values from some input source.  Parsers
 * for files and strings are provided. */
struct Parser<`p> {
  `p env;
  int (@getc)(`p);
  int (@ungetc)(int ch, `p);
  void (@error)<`r>(`p,int ch,const char ?`r) __attribute__((noreturn));
};

/* A printer is used to dump a value to some output sink.  Printers
 * for files and strings are provided. */
struct Printer<`p> {
  `p env;
  void (@print)(`p, const char ?`H);
};

/* A class provides a set of built-in methods for various objects. */
struct Class<`a::A> { : ALIASABLE >= aquals(`a)
  /* cast_value is used to support down-casts */
  Obj_t<`a> *cast_value;
  /* tag is a letter unique to the class used in printing and parsing. */
  const char tag;
  /* the name is used when printing error messages */
  const char ?name;
  /* print takes a printer as an argument and dumps the value to the sink */
  void (@print)<`p,`r>(Obj_t<`a>@`H self, struct Printer<`p>@`r);
  /* parse takes a parser as an argument and returns an object for this class*/
  Obj_t<`a> @(@parse)<`p,`r>(struct Parser<`p>@`r);
  /* cmp compares two objects of the same class. */
  int (@cmp)<`r1,`r2>(Obj_t<`a>@`r1,Obj_t<`a>@`r2);
  /* hash produces an integer hash code for a given object */
  int (@hash)<`r>(Obj_t<`a>@`r self);
  /* accept is used for visitors (see below) */
  `b (@accept)<`b,`env,`r>(Obj_t<`a>@ self, visitor_t<`b,`env,`r>,`env env : regions(`a) <= `H);
  /* the hash-table is used for hash-consing objects to maximize sharing */
  struct Hashtable::Table<Obj_t<`a>@,Obj_t<`a>@> *hash_table;
};

/* An Obj<`a> includes a suitable vtable pointer for its class
 * and an `a value.  Since `a is of kind A, it can be instantiated
 * with any size type, but operations that are polymorphic must 
 * manipulate pointers to Obj's. */
struct Obj<`a::A> { : ALIASABLE >= aquals(`a)
  Class_t<`a> @vtable;
  `a            v;
};

/* An Object abstracts the type of an Obj.*/
struct Object<`bd::E> {
  <`a> : regions(`a) <= `bd, ALIASABLE >= aquals(`a)
  Obj_t<`a> @self;
};

/* convert a Obj_t<`a> to an Object -- essentially an up-cast. */
extern Object_t up(Obj_t<`a>@`H self : regions(`a) <= `H);
/* convert an Object to an Obj_t<`C> given some class C -- returns
 * null upon failure, and otherwise returns a pointer to the underlying
 * object at the correct type -- effectively a down-cast. */
extern Obj_t<`c>* down(Class_t<`c>@`H C, Object_t x);

/* Built-in classes */
extern Class_t<unsigned char> uchar_class;
extern Class_t<signed char> schar_class;
extern Class_t<unsigned short> ushort_class;
extern Class_t<signed short> sshort_class;
extern Class_t<unsigned int> uint_class;
extern Class_t<signed int> sint_class;
extern Class_t<unsigned long long> ulonglong_class;
extern Class_t<signed long long> slonglong_class;
extern Class_t<float> float_class;
extern Class_t<double> double_class;
extern Class_t<const char ?> str_class;
extern Class_t<const char ?> symbol_class;
extern Class_t<Object_t?> tuple_class;

/* Various constructors for built-in classes -- these all hash-cons */
extern Obj_t<unsigned char>@ mk_uchar(unsigned char);
extern Obj_t<signed char>@ mk_schar(signed char);
extern Obj_t<unsigned short>@ mk_ushort(unsigned short);
extern Obj_t<signed short>@ mk_sshort(signed short);
extern Obj_t<unsigned int>@ mk_uint(unsigned int);
extern Obj_t<signed int>@ mk_sint(signed int);
extern Obj_t<unsigned long long>@ mk_ulonglong(unsigned long long);
extern Obj_t<signed long long>@ mk_slonglong(signed long long);
extern Obj_t<float>@ mk_float(float);
extern Obj_t<double>@ mk_double(double);
extern Obj_t<const char ?>@ mk_str(const char ?`H);
extern Obj_t<const char ?>@ mk_symbol(const char ?`H);
extern Obj_t<Object_t?>@ mk_tuple(Object_t?`H);

/* Composition of constructors with up-cast */
extern Object_t obj_uchar(unsigned char);
extern Object_t obj_schar(signed char);
extern Object_t obj_ushort(unsigned short);
extern Object_t obj_sshort(signed short);
extern Object_t obj_uint(unsigned int);
extern Object_t obj_sint(signed int);
extern Object_t obj_ulonglong(unsigned long long);
extern Object_t obj_slonglong(signed long long);
extern Object_t obj_float(float);
extern Object_t obj_double(double);
extern Object_t obj_str(const char ?`H);
extern Object_t obj_symbol(const char ?`H);
extern Object_t obj_tuple(Object_t?`H);
extern Object_t tuple(...Object_t);

/* A Visitor<`b,`env,`r> is used to fold some operation across a tree
 * of objects.  The typical accept method for an object simply invokes
 * the appropriate visitor member if present, and otherwise invokes the
 * default visitor method.  
 */
struct Visitor<`b,`env,`r> {
  `b (*visit_uchar)(`env,Obj_t<unsigned char>@self, visitor_t<`b,`env,`r>);
  `b (*visit_schar)(`env,Obj_t<signed char>@self, visitor_t<`b,`env,`r>);
  `b (*visit_ushort)(`env,Obj_t<unsigned short>@self, visitor_t<`b,`env,`r>);
  `b (*visit_sshort)(`env,Obj_t<signed short>@self, visitor_t<`b,`env,`r>);
  `b (*visit_uint)(`env,Obj_t<unsigned int>@self, visitor_t<`b,`env,`r>);
  `b (*visit_sint)(`env,Obj_t<signed int>@self, visitor_t<`b,`env,`r>);
  `b (*visit_ulonglong)(`env,Obj_t<unsigned long long>@self, visitor_t<`b,`env,`r>);
  `b (*visit_slonglong)(`env,Obj_t<signed long long>@self, visitor_t<`b,`env,`r>);
  `b (*visit_float)(`env,Obj_t<float>@self, visitor_t<`b,`env,`r>);
  `b (*visit_double)(`env,Obj_t<double>@self, visitor_t<`b,`env,`r>);
  `b (*visit_symbol)(`env,Obj_t<const char ?`H>@self, visitor_t<`b,`env,`r>);
  `b (*visit_str)(`env,Obj_t<const char ?`H>@self, visitor_t<`b,`env,`r>);
  `b (*visit_tuple)(`env,Obj_t<Object_t?`H>@self, visitor_t<`b,`env,`r>);
  `b (*visit_default)(`env, Object_t, visitor_t<`b,`env,`r>);
};

/* general visit */
extern `a visit(Object_t x, Visitor_t<`a,`env,`r>@`r v, `env env);

/* a visitor that is nowhere defined -- useful for building other 
 * visitors where you don't want to do anything special for most kinds 
 * of objects. */
extern Visitor_t<`b,`a,`r> empty_visitor();

/* default visitor -- same as above except that a default is given. */
extern Visitor_t<`b,`a,`r> default_visitor(`b (@`H def)(`a,Object_t,visitor_t<`b,`a,`r>));

/* compare two objects */
extern int cmp(Object_t x, Object_t y);
/* register a new class -- this is necessary to support parsing for
   new classes.  Built in classes are pre-registered. */
extern void register_class(struct Class<`a>@`H : regions(`a) <= `H);
/* print an object using the supplied printer. */
extern void print(struct Printer<`p>@, Object_t);
/* parse an object -- uses the registered classes' tags to determine
 * which class's parse method to invoke. */
extern Object_t parse(struct Parser<`p>@);

/* various printers */
extern void tofile(FILE @, Object_t);
extern void tofilename(const char @, Object_t);
extern const char ?tostring(Object_t);

/* various parsers */
extern Object_t fromfile(FILE @);
extern Object_t fromfilename(const char @);
extern Object_t fromstring(const char ?);
}
#endif
