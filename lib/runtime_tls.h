#ifndef _RUNTIME_TLS_H_
#define _RUNTIME_TLS_H_

#ifndef _CYC_DRIVER_
/* setjmp.h clone */

# define _SIGSET_NWORDS	(1024 / (8 * sizeof (unsigned long int)))
typedef struct
  {
    unsigned long int __val[_SIGSET_NWORDS];
  } __sigset_t;

typedef int __jmp_buf[6];

/* Calling environment, plus possibly a saved signal mask.  */
typedef struct __jmp_buf_tag	/* C++ doesn't like tagless structs.  */
{
  /* NOTE: The machine-dependent definitions of `__sigsetjmp'
     assume that a `jmp_buf' begins with a `__jmp_buf' and that
     `__mask_was_saved' follows it.  Do not move these members
     or add others before it.  */
  __jmp_buf __jmpbuf;		/* Calling environment.  */
  int __mask_was_saved;	/* Saved the signal mask?  */
    __sigset_t __saved_mask;	/* Saved signal mask.  */
} jmp_buf[1];

/* defines in setjmp.S asm file */

extern int setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int val);

#include "precore_c.h"

#endif


typedef struct {
  struct _RuntimeStack *current_frame;
  struct _xtunion_struct *exn_thrown;
  const char *exn_filename;
  int exn_lineno;
}tls_record_t;

typedef struct {
  unsigned int pid;
  unsigned int usage_count;
  tls_record_t *record;
}tls_slot_t;

extern tls_record_t* cyc_runtime_lookup_tls_record();
#endif
