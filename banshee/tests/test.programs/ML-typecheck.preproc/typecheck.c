extern	struct	_iobuf {
	int	_cnt;
	unsigned char *_ptr;
	unsigned char *_base;
	int	_bufsiz;
	short	_flag;
	char	_file;		
} _iob[];
typedef struct _iobuf FILE;
extern struct _iobuf	*fopen(const char *, const char *);
extern struct _iobuf	*fdopen(int, const char *);
extern struct _iobuf	*freopen(const char *, const char *, FILE *);
extern struct _iobuf	*popen(const char *, const char *);
extern struct _iobuf	*tmpfile(void);
extern long	ftell(FILE *);
extern char	*fgets(char *, int, FILE *);
extern char	*gets(char *);
extern char	*sprintf(char *, const char *, ...);
extern char	*ctermid(char *);
extern char	*cuserid(char *);
extern char	*tempnam(const char *, const char *);
extern char	*tmpnam(char *);
struct typeExp {
  int nodetype;
  char *id;
  struct typeExp *kid1;
  struct typeExp *kid2;
};
struct typeVarList {
    char *name;
    struct typeVarList *next;
};
struct typeEnv {
  char *name;
  struct typeExp *type;
  struct typeEnv *next;
};
struct typeExp *CreateType(int type, char *name, struct typeExp *kid1,
			   struct typeExp *kid2);
void PrintType(struct typeExp *t);
struct typeExp *HandleGeneric(struct typeExp *t);
struct typeExp *Substitute(char *old, struct typeExp *new, struct typeExp *t);
char *NewTypeVar(void);
int OccursCheck(char *id, struct typeExp *t);
struct typeVarList *FreeVarsInType(struct typeExp *t);
int FreeInType(char *id, struct typeExp *t);
struct typeExp *BuildGeneric(struct typeExp *rho, struct typeVarList *freeVars,
			     struct typeEnv *A);
struct typeEnv *InitialEnv(void);
struct typeEnv *NewEnv(void);
struct typeEnv *AddType(struct typeEnv *env, char *id, struct typeExp *exp);
struct typeExp *Lookup(char *id, struct typeEnv *env);
void PrintEnv(struct typeEnv *env);
int FreeInEnv(char *id, struct typeEnv *env);
struct typeEnv *CopyTypeEnv(struct typeEnv *env);
struct sub {
  char *name;
  struct typeExp *type;
  struct sub *next;
};
struct sub *NewSub(void);
void PrintSub(struct sub *S);
struct typeEnv *ApplySubToEnv(struct sub *S, struct typeEnv *A);
struct typeExp *ApplySubToType(struct sub *S, struct typeExp *exp);
struct sub *Compose(struct sub *S1, struct sub *S2);
struct sub *CopySub(struct sub *S);
struct sub *AddSub(struct sub *S, char *id, struct typeExp *exp);
int InSub(char *id, struct sub *S);
struct TCPair {
  struct sub *substitution;
  struct typeExp *type;
};
struct TCPair *CreatePair(struct sub *S, struct typeExp *exp);
struct sub *Unify(struct sub *S, struct typeExp *t1, struct typeExp *t2);
struct sub *Extend(struct sub *S, char *id, struct typeExp *exp);
struct tree {
 int type;
 char *name;
 struct tree *kid1;
 struct tree *kid2;
 struct tree *kid3;
};
struct TCPair *TypeCheck(struct typeEnv *A, struct tree *e);
struct tree *CreateTreeNode(int nodetype, char *id, struct tree *child1, struct tree *child2, struct tree *child3);
struct tree *CreateLeaf(int leaftype, char *id);
struct TCPair *TypeCheck(struct typeEnv *A, struct tree *e)
{
struct typeExp *tau;
struct typeExp *tmptype;
struct typeExp *beta;
struct typeExp *rho;
struct typeExp *rhoprime;
struct typeExp *sigma;
struct typeExp *sigmaprime;
struct TCPair *pair1;
struct TCPair *pair2;
struct sub *T;
struct sub *U;
struct sub *Uprime;
struct sub *R;
struct sub *S;
struct sub *Sprime;
struct typeEnv *URA;
struct typeEnv *RA;
struct typeVarList *freeVars;
    if (A == (struct typeEnv *)-2) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
    switch (e -> type) {
      case 1: tmptype = Lookup(e->name,A);
	          if (tmptype == (struct typeExp *)-3) {
		      printf("ERROR: missing type for id %s\n",e->name);
		      return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
		  }
		  else {
		      T = NewSub();
		      tau = HandleGeneric(tmptype);
		      return(CreatePair(T,tau));
		  }
      case 2:   return(CreatePair(NewSub(),CreateType(2,"",0,0)));
      case 3:  return(CreatePair(NewSub(),CreateType(1,"",0,0)));
      case 4: return(CreatePair(NewSub(),
				      CreateType(6,
					         "",
					         CreateType(3,
							    NewTypeVar(),
							    0,
							    0),
						 0)));
      case 5: return(CreatePair(NewSub(),
				      CreateType(6,
					         "",
					         CreateType(2,"",0,0),
						 0)));
      case 6: return(CreatePair(NewSub(),
				      CreateType(6,
					         "",
					         CreateType(1,"",0,0),
						 0)));
      case 7: 
	             pair1 = TypeCheck(A, e->kid1);
	             R = pair1->substitution;
		     rho = pair1->type;
		     if (rho == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
	             pair2 = TypeCheck(ApplySubToEnv(R,A),e->kid2);
		     S = pair2->substitution;
		     sigma = pair2->type;
		     if (sigma == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
		     beta = CreateType(3,NewTypeVar(),0,0);
		  U = Unify(NewSub(),
			    ApplySubToType(S,rho),
			    CreateType(4,"",sigma,beta));
		  T = Compose(U,Compose(S,R));
		  tau = ApplySubToType(U,beta);
		  return(CreatePair(T, tau));
      case 8:   beta = CreateType(3,NewTypeVar(),0,0);
                  A = AddType(A,e->name,beta);
		  if (1) {
		      printf("Lambda case is adding binding %s:",e->name);
		      PrintType(beta);
		      printf("\n");
		      }
                  pair1 = TypeCheck(A,e->kid1);
		  R = pair1->substitution;
		  rho = pair1->type;
		  if (1) {
		      printf("Lambda case discovered type of %s is ",e->name);
		      PrintType(ApplySubToType(R,beta));
		      printf("\n");
		      }
		  if (rho == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
		  T = R;
		  tau = CreateType(4,
				   "",
				    ApplySubToType(R,beta),
				    rho);
		  return(CreatePair(R, tau));
      case 9:  
                     pair1 = TypeCheck(A, e->kid1);
                     R = pair1->substitution;
		     rho = pair1->type;
		     if (rho == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
   	             U = Unify(NewSub(),rho,CreateType(1,"",0,0));
		     URA = ApplySubToEnv(U,ApplySubToEnv(R,A));
		     pair1 = TypeCheck(URA,e->kid2);
		     S = pair1->substitution;
		     sigma = pair1->type;
		     if (sigma == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
		     pair1 = TypeCheck(ApplySubToEnv(S,URA),e->kid3);
		     Sprime = pair1->substitution;
		     sigmaprime = pair1->type;
		     if (sigmaprime == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
		  Uprime = Unify(NewSub(),
			         ApplySubToType(Sprime,sigma),
				 sigmaprime);
                  if (1) {
		      printf("Cond case discovered types of branches to be ");
		      PrintType(ApplySubToType(Uprime,sigmaprime));
		      printf("\n");
		      }
                  T = Compose(Compose(Compose(Compose(Uprime,Sprime),S),U),R);
		  tau = ApplySubToType(Uprime,sigmaprime);
                  return(CreatePair(T,tau));
      case 10: pair1 = TypeCheck(A,e->kid1);
		  R = pair1->substitution;
		  rho = pair1->type;
		  if (rho == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
		  freeVars = FreeVarsInType(rho);
		  RA = ApplySubToEnv(R,A);
		  rhoprime = BuildGeneric(rho,freeVars,RA);
		  if (1) {
		      printf("Let case discovered type of %s is ",e->name);
		      PrintType(rhoprime);
		      printf("\n");
		      }
		  RA = AddType(RA,e->name,rhoprime);
		  pair2 = TypeCheck(RA,e->kid2);
		  S = pair2->substitution;
		  sigma = pair2->type;
		  if (sigma == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
		  T = Compose(S,R);
		  tau = sigma;
		  return(CreatePair(T,tau));
      case 11:   beta = CreateType(3,NewTypeVar(),0,0);
                  A = AddType(A,e->name,beta);
		  if (1) {
		      printf("Let rec case is adding binding %s:",e->name);
		      PrintType(beta);
		      printf("\n");
		      }
	          pair1 = TypeCheck(A,e->kid1);
		  R = pair1->substitution;
		  rho = pair1->type;
		  if (rho == (struct typeExp *)-3) return(CreatePair((struct sub *)-4,(struct typeExp *)-3));
		  U = Unify(NewSub(),ApplySubToType(R,beta),rho);
		  T = Compose(U,R);
		  tau = ApplySubToType(U,ApplySubToType(R,beta));
		  if (1) {
		      printf("Let rec case discovered type of %s is ",e->name);
		      PrintType(tau);
		      printf("\n");
		      }
		  return(CreatePair(T,tau));
      default: printf("internal error: missing case for type %d in TypeCheck\n", e->type);
               exit(1);
    }
}
struct sub *Unify(struct sub *S, struct typeExp *t1, struct typeExp *t2)
{
    if ((t1 == (struct typeExp *)-3)||(t2 == (struct typeExp *)-3)||(S == (struct sub *)-4)) return((struct sub *)-4);
    switch (t1->nodetype) {
      case 1:
      case 2: if (t2->nodetype == 3) return(Unify(S,t2,t1));
                 if (t2->nodetype != t1->nodetype) return((struct sub *)-4);
		 return(S);
      case 3: if (InSub(t1->id,S))
	               return(Unify(S, ApplySubToType(S,t1),ApplySubToType(S,t2)));
		   else return(Extend(S, t1->id, ApplySubToType(S,t2)));
      case 4: switch(t2->nodetype){
                    case 3: return(Unify(S,t2,t1));
		    case 4: return(Unify(Unify(S,t1->kid1,t2->kid1),t1->kid2,t2->kid2));
		    default: return((struct sub *)-4);
                  }
      case 6: switch(t2->nodetype) {
                   case 3: return(Unify(S,t2,t1));
		   case 6: return(Unify(S,t1->kid1,t2->kid1));
		   default: return((struct sub *)-4);
                 }
      case 5: switch(t2->nodetype){
                    case 3: return(Unify(S,t2,t1));
		    case 5: return(Unify(Unify(S,t1->kid1,t2->kid1),t1->kid2,t2->kid2));
		    default: return((struct sub *)-4);
                  }
      case 7: printf("Unify called on unsupported type FORALL\n");
                   return((struct sub *)-4);
    }
}
struct sub *Extend(struct sub *S, char *id, struct typeExp *exp)
{
struct sub *newS;
struct sub *tmp;
    if (S == (struct sub *)-4) return(S);
    if (exp == (struct typeExp *)-3) return((struct sub *)-4);
    if (exp->nodetype == 3)
        if (strcmp(exp->id,id)==0) return(S);
    if (OccursCheck(id,exp)) return((struct sub *)-4);
    tmp = NewSub();
    tmp = AddSub(tmp, id, exp);
    newS = Compose(tmp,S);
    return(newS);
}
