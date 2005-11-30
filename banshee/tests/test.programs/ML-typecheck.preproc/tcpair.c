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
struct TCPair *CreatePair(struct sub *S, struct typeExp *exp)
{
struct TCPair *tmp;
    tmp = (struct TCPair *)malloc(sizeof(struct TCPair));
    tmp -> substitution = S;
    tmp -> type = exp;
    return(tmp);
}
