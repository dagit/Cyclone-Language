#ifndef _REF_H_
#define _REF_H_
namespace Ref {
extern struct Ref<`a> {`a v;};
typedef struct Ref<`a> @ ref_t<`a>;
extern ref_t<`a> new_ref(`a);
extern `a get(ref_t<`a>);
extern void set(ref_t<`a>,`a);
}
#endif 
