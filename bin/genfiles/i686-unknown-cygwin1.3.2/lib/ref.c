 struct Cyc_Ref_Ref{ void* v; } ; extern struct Cyc_Ref_Ref* Cyc_Ref_new_ref(
void*); extern void* Cyc_Ref_get( struct Cyc_Ref_Ref*); extern void Cyc_Ref_set(
struct Cyc_Ref_Ref*, void*); struct Cyc_Ref_Ref* Cyc_Ref_new_ref( void* x){
return({ struct Cyc_Ref_Ref* _temp0=( struct Cyc_Ref_Ref*) _cycalloc( sizeof(
struct Cyc_Ref_Ref)); _temp0->v=( void*) x; _temp0;});} void* Cyc_Ref_get(
struct Cyc_Ref_Ref* r){ return( void*) r->v;} void Cyc_Ref_set( struct Cyc_Ref_Ref*
r, void* x){( void*)( r->v=( void*) x);}
