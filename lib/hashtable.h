#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

namespace Hashtable {

extern struct table<`a,`b>;
typedef struct table<`a,`b> @Table<`a,`b>;

// if cmp(x,y) is 0 then it better be that hash(x) == hash(y) 
extern Table<`a,`b> create<`a,`b>(int sz, int cmp(`a,`a), int hash(`a));
extern void insert<`a,`b>(Table<`a,`b> t, `a key, `b val);
extern `b lookup<`a,`b>(Table<`a,`b> t, `a key); // raises Not_found
extern void resize<`a,`b>(Table<`a,`b> t);

// remove only most recent binding.  Silently return if none.  Does a
// destructive list update since this data structure is not functional
extern void remove<`a,`b>(Table<`a,`b> t, `a key);

extern int hash_string(string s);
extern int hash_stringptr(stringptr s);
extern void iter<`a,`b>(void f(`a,`b), Table<`a,`b> t);

// debugging
extern 
void print_table_map<`a,`b>(Table<`a,`b> t, void prn_key(`a), void prn_val(`b));

}
#endif
