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
struct sub *NewSub(void)
{
    return(0);
}
void PrintSub(struct sub *S)
{
struct sub *tmp;
    if (S == 0) printf("Identity substitution");
    else if (S == (struct sub *)-4) printf("Failure substitution");
    else {
        tmp = S;
	while (tmp != 0) {
	    printf("%s:\t",tmp -> name);
	    PrintType(tmp -> type);
	    printf("\n");
	    tmp = tmp -> next;
	}
    }
}
struct typeEnv *ApplySubToEnv(struct sub *S, struct typeEnv *A)
{
struct sub *stmp;
struct typeEnv *newA;
struct typeEnv *atmp;
    if (A == (struct typeEnv *)-2) return(A);
    if (S == (struct sub *)-4) return((struct typeEnv *)-2);
    stmp = S;
    newA = CopyTypeEnv(A);
    while (stmp != 0){
	atmp = newA;
	while (atmp != 0) {
	    atmp->type = Substitute(stmp->name,stmp->type,atmp->type);
	    atmp = atmp->next;
	}
	stmp = stmp->next;
    }
    return(newA);
}
struct typeExp *ApplySubToType(struct sub *S, struct typeExp *exp)
{
struct sub *tmp;
struct typeExp *newexp;
    if (exp == (struct typeExp *)-3) return(exp);
    if (S == (struct sub *)-4) return((struct typeExp *)-3);
    newexp = exp;
    tmp = S;
    while (tmp != 0) {
        newexp = Substitute(tmp->name,tmp->type,newexp);
	tmp = tmp->next;
    }
    return(newexp);
}
struct sub *Compose(struct sub *S1, struct sub *S2)
{
struct sub *new;
struct sub *tmp;
    if ((S1 == (struct sub *)-4)||(S2 == (struct sub *)-4)) return((struct sub *)-4);
    if (S2 == 0) return(S1);
    if (S1 == 0) return(S2);
    new = CopySub(S2);
    tmp = new;
    while (tmp != 0) {
        tmp->type = ApplySubToType(S1,tmp->type);
	tmp = tmp->next;
    }
    tmp = S1;
    while (tmp != 0) {
        new = AddSub(new,tmp->name,tmp->type);
	tmp = tmp->next;
    }
    return(new);
}
struct sub *CopySub(struct sub *S)
{
struct sub *newS;
struct sub *old;
    if (S == (struct sub *)-4) return((struct sub *)-4);
    newS = NewSub();
    old = S;
    while (old != 0) {
        newS = AddSub(newS,old->name, old->type);
	old = old->next;
    }
    return(newS);
}
struct sub *AddSub(struct sub *S, char *id, struct typeExp *exp)
{
struct sub *tmp;
    if (S == (struct sub *)-4) return((struct sub *)-4);
    if (InSub(id,S)) return(S);
    tmp = (struct sub *)malloc(sizeof(struct sub));
    tmp -> name = id;
    tmp -> type = exp;
    tmp -> next = S;
    return(tmp);
}
int InSub(char *id, struct sub *S)
{
struct sub *tmp;
    if ((S == (struct sub *)-4) || (S == 0)) return(0);
    tmp = S;
    while (tmp != 0) {
        if (strcmp(tmp->name,id) == 0) return(1);
	tmp = tmp->next;
    }
    return(0);
}
