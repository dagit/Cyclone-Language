#ifndef DFA_CYC_H_
#define DFA_CYC_H_

namespace DFACyc{

//char ?act2symb(act_t);
extern int is_dfa_final(st_t);
extern $(st_t,Semiring::weight_t) transitions(st_t,act_t);
extern act_t ?attributes(st_t);
extern $(int,Semiring::weight_t) is_final(st_t);
}

static int array_find(act_t ?arr,act_t a){
	let n = numelts(arr);
	for (int i=0; i<n; i++){
		if (arr[i] == a) return i;
	}
	return -1;
}

#define DFA_TY int
#define DFA_TRANS(dfa,s,a) ({let $(t,w) = DFACyc::transitions(s,a); t;})
#define DFA_TRANS_W(dfa,s,a) (DFACyc::transitions(s,a))

// XXX Need to implement
#define DFA_GET_REPEAT_ACT(dfa,s) 0
#define DFA_GET_REPEATEE_ACT(dfa,s) 0
#define DFA_R_EXTEND(dfa,nt,nt_start,nt_final) nt_final

#define DFA_IN_FINAL(dfa,nt,s) (array_find(DFACyc::attributes(s),nt) != -1)
#define DFA_IS_FINAL(dfa,s) (DFACyc::is_final(s).f0)
#define DFA_FINAL_WEIGHT(dfa,f) (DFACyc::is_final(f).f1)
#define DFA_FINAL_ATTRS(dfa,f) DFACyc::attributes(f)
#define DFA_GET_START(dfa) 1
#define DFA_IS_DFA_FINAL(dfa,s) ({let as = DFACyc::attributes(s); as && numelts(as) == 0;})

#endif /*DFA_CYC_H_*/
