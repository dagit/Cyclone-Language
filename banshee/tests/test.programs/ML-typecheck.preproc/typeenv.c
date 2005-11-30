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
struct typeExp *ConsType(void);
struct typeExp *CarType(void);
struct typeExp *CdrType(void);
struct typeExp *NullType(void);
struct typeExp *PairType(void);
struct typeExp *SuccType(void);
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
static struct typeEnv *CopyEnvExceptId(struct typeEnv *env, char *id);
struct typeEnv *NewEnv(void)
{
    return(0);
}
struct typeEnv *AddType(struct typeEnv *env, char *id, struct typeExp *exp)
{
struct typeEnv *tmp;
struct typeEnv *newenv;
    if ((env == (struct typeEnv *)-2)||(exp == (struct typeExp *)-3)) return((struct typeEnv *)-2);
    if (Lookup(id, env) != (struct typeExp *)-3) {
        newenv = CopyEnvExceptId(env,id);
        }
    else newenv = env;
    tmp = (struct typeEnv *)malloc(sizeof(struct typeEnv));
    tmp -> name = id;
    tmp -> type = exp;
    tmp -> next = newenv;
    return(tmp);
}
struct typeExp *Lookup(char *id, struct typeEnv *env)
{
struct typeEnv *tmp;
    if (env == (struct typeEnv *)-2) return((struct typeExp *)-3);
    tmp = env;
    while (tmp != 0) {
        if (strcmp(tmp -> name,id) == 0 ) return(tmp -> type);
	tmp = tmp -> next;
        };
    return((struct typeExp *)-3);
}
void PrintEnv(struct typeEnv *env)
{
struct typeEnv *tmp;
    if (env == 0) printf("Empty environment");
    else if (env == (struct typeEnv *)-2) printf("Failure environment");
    else {
        tmp = env;
	while (tmp != 0) {
	    printf("%s:\t",tmp -> name);
	    PrintType(tmp -> type);
	    printf("\n");
	    tmp = tmp -> next;
	}
    }
}
int FreeInEnv(char *id, struct typeEnv *env)
{
struct typeEnv *tmp;
    if (env == (struct typeEnv *)-2) return(0);
    tmp = env;
    while (tmp != 0) {
        if (FreeInType(id,tmp->type)) return(1);
	else tmp = tmp->next;
      }
    return(0);
}
struct typeEnv *CopyTypeEnv(struct typeEnv *env)
{
struct typeEnv *newenv;
struct typeEnv *tmp;
    if (env == 0) return(0);
    if (env == (struct typeEnv *)-2) return ((struct typeEnv *)-2);
    newenv = NewEnv();
    tmp = env;
    while (tmp != 0) {
        newenv = AddType(newenv, tmp->name, tmp->type);
	tmp = tmp->next;
    }
    return(newenv);
}
struct typeEnv *InitialEnv(void)
{
struct typeEnv *tmp;
    tmp = NewEnv();
    tmp = AddType(tmp, "cons", ConsType());
    tmp = AddType(tmp, "cdr", CdrType());
    tmp = AddType(tmp, "null", NullType());
    tmp = AddType(tmp, "car", CarType());
    tmp = AddType(tmp, "pair", PairType());
    tmp = AddType(tmp, "succ", SuccType());
    return(tmp);
}
static struct typeEnv *CopyEnvExceptId(struct typeEnv *env, char *id)
{
struct typeEnv *newenv;
struct typeEnv *tmp;
    if (env == (struct typeEnv *)-2) return((struct typeEnv *)-2);
    if (env == 0) return(0);
    newenv = NewEnv();
    tmp = env;
    while (tmp != 0) {
        if (strcmp(tmp->name,id)!=0) newenv = AddType(newenv, tmp->name, tmp->type);
	tmp = tmp->next;
        }
    return(newenv);
}
