#ifndef _CONTEXTS_H
#define _CONTEXTS_H

/* Context: an unsigned short with the following bits:
   0-3: size
   4-7: color -- 4 bits so we can have an abstract color
   8-9: underline value
   10: TT
   11: B
   12: EM
   13: I
   14: S

Pictorially:

        -     e   t u u c c c c s s s s
        - s i m b t l l l l l l z z z z
        8 4 2 1 8 4 2 1 8 4 2 1 8 4 2 1

*/

typedef unsigned short context_t;

#define S_MASK  0x4000
#define I_MASK  0x2000
#define EM_MASK 0x1000
#define B_MASK  0x0800
#define TT_MASK 0x0400
#define UL_MASK 0x0300
#define CL_MASK 0x00f0
#define SZ_MASK 0x000f

#define SZ(x)    ((x)&SZ_MASK)
#define CL(x)    (((x)&CL_MASK)>>4)
#define UL(x)    (((x)&UL_MASK)>>8)
#define isTT(x)  ((x)&TT_MASK)
#define isB(x)   ((x)&B_MASK)
#define isEM(x)  ((x)&EM_MASK)
#define isI(x)   ((x)&I_MASK)
#define isS(x)   ((x)&S_MASK)

#define SMALL(x) ((context_t)(x))
#define clrSZ(x) SMALL((x)&~SZ_MASK)
#define clrCL(x) SMALL((x)&~CL_MASK)
#define clrUL(x) SMALL((x)&~UL_MASK)
#define clrTT(x) SMALL((x)&~TT_MASK)
#define clrB(x)  SMALL((x)&~B_MASK)
#define clrEM(x) SMALL((x)&~EM_MASK)
#define clrI(x)  SMALL((x)&~I_MASK)
#define clrS(x)  SMALL((x)&~S_MASK)

#define setSZ(x,y) SMALL(clrSZ(x)|((y)&SZ_MASK))
#define setCL(x,y) SMALL(clrCL(x)|(((y)<<4)&CL_MASK))
#define setUL(x,y) SMALL(clrUL(x)|(((y)<<8)&UL_MASK))
#define setTT(x)   SMALL((x)|TT_MASK)
#define setB(x)    SMALL((x)|B_MASK)
#define setEM(x)   SMALL((x)|EM_MASK)
#define setI(x)    SMALL((x)|I_MASK)
#define setS(x)    SMALL((x)|S_MASK)

#define incUL(x)   SMALL(UL(x)==3?(x):setUL((x),UL(x)+1))

#define invertEM(x) SMALL((x)^EM_MASK)

#define setr(x)    SMALL(setCL(x,0))
#define setg(x)    SMALL(setCL(x,1))
#define setb(x)    SMALL(setCL(x,2))
#define setc(x)    SMALL(setCL(x,3))
#define setm(x)    SMALL(setCL(x,4))
#define sety(x)    SMALL(setCL(x,5))
#define setk(x)    SMALL(setCL(x,6))
#define setw(x)    SMALL(setCL(x,7))

#define doPL(x)    clrUL(clrS(clrEM(clrI(clrB(clrTT(x))))))
#define doS(x)     clrEM(setS(x))

#define SPACE(x)        ((x)==0x20||(x)==0x0d||(x)==0x0a||(x)==0x09)
#define SPACECONTEXT(x) (UL(x)==0?setw(clrS(clrEM(clrI(clrB(x))))):clrS(clrEM(clrI(clrB(x)))))

// Given 5MB input limit this should be sufficient
#define MAX_NEST 749000

#endif
