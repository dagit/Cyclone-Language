#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

// TODO: add region support

namespace Hashtable {

extern struct Table<`a,`b,`e1::E,`e2::E>;
typedef struct Table<`a,`b,`e1,`e2> @table_t<`a,`b,`e1,`e2>;

// if cmp(x,y) is 0 then it better be that hash(x) == hash(y) 
extern table_t<`a,`b,`e1,`e2> create(int sz, int cmp(`a,`a;`e1), 
                                   int hash(`a;`e2));
extern void insert(table_t<`a,`b,`e1,`e2> t, `a key, `b val);
extern `b lookup(table_t<`a,`b,`e1,`e2> t, `a key); // raises Not_found
extern void resize(table_t<`a,`b,`e1,`e2> t);

// remove only most recent binding.  Silently return if none.  Does a
// destructive list update since this data structure is not functional
extern void remove(table_t<`a,`b,`e1,`e2> t, `a key);

extern int hash_string(string s);
extern int hash_stringptr(stringptr s);
extern void iter(void f(`a,`b), table_t<`a,`b,`e1,`e2> t);

// debugging
extern 
void print_table_map(table_t<`a,`b,`e1,`e2> t, void prn_key(`a), 
                     void prn_val(`b));

}
#endif
