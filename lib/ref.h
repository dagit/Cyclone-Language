#ifndef _REF_H_
#define _REF_H_
namespace Ref {
extern struct Reference<`a> {`a v;};
typedef struct Reference<`a> @ ref<`a>;
extern ref<`a> new_ref<`a>(`a);
extern `a get<`a>(ref<`a>);
extern void set<`a>(ref<`a>,`a);
}
#endif 
