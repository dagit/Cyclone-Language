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
int NumDigits(int i);
char *IntToStr(int i);
struct typeVarList *NewList(void);
struct typeVarList *AddToList(struct typeVarList *L, char *id);
struct typeVarList *ConcatList(struct typeVarList *L1, struct typeVarList *L2);
struct typeVarList *UnionList(struct typeVarList *L1, struct typeVarList *L2);
struct typeVarList *CopyList(struct typeVarList *L);
int ListMember(char *id, struct typeVarList *L);
void PrintTypeVarList(struct typeVarList *L);
typedef	int		sigset_t;	
typedef	unsigned int	speed_t;	
typedef	unsigned long	tcflag_t;	
typedef	unsigned char	cc_t;		
typedef	int		pid_t;		
typedef	unsigned short	mode_t;		
typedef	short		nlink_t;	
typedef	long		clock_t;	
typedef	long		time_t;		
typedef	int		size_t;		
typedef int		ptrdiff_t;	
typedef	unsigned short	wchar_t;	
extern unsigned int _mb_cur_max;
extern int	abort(void);
extern int	abs(int);
extern double	atof(const char *);
extern int	atoi(const char *);
extern long int	atol(const char *);
extern char *	bsearch(const void *, const void *, size_t, size_t, int (*) (const void *, const void *));
extern char *	calloc(size_t, size_t);
extern int	exit(int);
extern int	free(void *);
extern char *	getenv(const char *);
extern char *	malloc(size_t);
extern int	qsort(void *, size_t, size_t, int (*) (const void *, const void *));
extern int	rand(void);
extern char *	realloc(void *, size_t);
extern int	srand(unsigned int);
extern int    mbtowc(wchar_t *, const char *, size_t);
extern int    wctomb(char *, wchar_t);
extern size_t mbstowcs(wchar_t *, const char *, size_t);
extern size_t wcstombs(char *, const wchar_t *, size_t);
extern char *	strcat(char *, const char *);
extern char *	strchr(const char *, int);
extern int	strcmp(const char *, const char *);
extern char *	strcpy(char *, const char *);
extern size_t	strcspn(const char *, const char *);
extern char *	strdup(const char *);
extern size_t	strlen(const char *);
extern char *	strncat(char *, const char *, size_t);
extern int	strncmp(const char *, const char *, size_t);
extern char *	strncpy(char *, const char *, size_t);
extern char *	strpbrk(const char *, const char *);
extern char *	strrchr(const char *, int);
extern size_t	strspn(const char *, const char *);
extern char *	strstr(const char *, const char *);
extern char *	strtok(char *, const char *);
int typenum = 0;
struct typeExp *CreateType(int type, char *name, struct typeExp *kid1, struct typeExp *kid2)
{
struct typeExp *tmp;
    tmp = (struct typeExp *)malloc(sizeof(struct typeExp));
    tmp -> nodetype = type;
    tmp -> id = name;
    tmp -> kid1 = kid1;
    tmp -> kid2 = kid2;
    return(tmp);
}
void PrintType(struct typeExp *t)
{
    if (t == (struct typeExp *)-3) printf("Failure type");
    else switch (t->nodetype) {
        case 1: printf("bool");
	            break;
        case 2: printf("int");
	           break;
	case 3: printf("%s",t->id);
	             break;
	case 4: printf("(");
	            PrintType(t->kid1);
	            printf(" -> ");
		    PrintType(t->kid2);
		    printf(")");
		    break;
	case 5: printf("(");
	            PrintType(t->kid1);
	            printf(" X ");
		    PrintType(t->kid2);
		    printf(")");
		    break;
	case 6: PrintType(t->kid1);
	           printf("-list");
		   break;
	case 7: printf("forall %s . ", t->id);
	             PrintType(t->kid1);
		     break;
	default: printf("internal error: no case for type %s in PrintType",
			t->nodetype);
	         exit(1);
    }
}
struct typeExp *HandleGeneric(struct typeExp *t)
{
struct typeExp *tmp;
    if (t == (struct typeExp *)-3) return(t);
    if (t -> nodetype != 7) return(t);
    tmp = Substitute(t->id,CreateType(3,NewTypeVar(),0,0),t->kid1);
    return(HandleGeneric(tmp));
}
struct typeExp *Substitute(char *old, struct typeExp *new, struct typeExp *t)
{
    if ((t == (struct typeExp *)-3)||(new == (struct typeExp *)-3)) return((struct typeExp *)-3);
    switch (t->nodetype) {
      case 1:  
      case 2:   return(t);
      case 3: if (strcmp(t->id,old) == 0) return(new);
		   else return(t);
      case 4: return(CreateType(4,
				    "",
				    Substitute(old,new,t->kid1),
				    Substitute(old,new,t->kid2)));
      case 5: return(CreateType(5,
				    "",
				    Substitute(old,new,t->kid1),
				    Substitute(old,new,t->kid2)));
      case 6: return(CreateType(6,
				    "",
				    Substitute(old,new,t->kid1),
				    0));
      case 7: if (strcmp(t->id,old) == 0) return(t);
                   return(CreateType(7,
				     t->id,
				     Substitute(old,new,t->kid1),
				     0));
      default: printf("internal error: missing case for type %d in Substitute\n",t->nodetype);
               exit(1);
      }
}
char *NewTypeVar(void)
{
extern int typenum;
char *tmp;
    if (typenum < 0) {
        printf("internal error: typenum < 0 in NewType\n");
	exit(1);
    }
    typenum++;
    tmp = (char *)malloc(NumDigits(typenum)+2); 
    tmp = strcpy(tmp,"t");
    tmp = strcat(tmp,IntToStr(typenum));
    return(tmp);
}
int OccursCheck(char *id, struct typeExp *t)
{
    if (t == (struct typeExp *)-3) return(0);
    switch (t->nodetype) {
      case 1:
      case 2: return(0);
      case 3: if (strcmp(id,t->id) == 0) return(1);
                   else return(0);
      case 4:
      case 5:
      case 7: if (strcmp(t->id,id)==0) return(0);
                   if (OccursCheck(id,t->kid1)) return(1);
                   else return(OccursCheck(id,t->kid2));
      case 6: return(OccursCheck(id,t->kid1));
    }
}
struct typeVarList *FreeVarsInType(struct typeExp *t)
{
static struct typeVarList *FreeVarsInTypePrime(struct typeExp *t,
					       struct typeVarList *notFree);
    return(FreeVarsInTypePrime(t,NewList()));
}
static struct typeVarList *FreeVarsInTypePrime(struct typeExp *t, struct typeVarList *notFree)
{
struct typeVarList *list;
    list = NewList();
    if (t == (struct typeExp *)-3) return(list);
    switch (t->nodetype) {
    case 1:
    case 2:   return(list);
    case 3: if (ListMember(t->id,notFree)) return(list);
		 else return(AddToList(list,t->id));
    case 4: 
    case 5:  return(UnionList(FreeVarsInTypePrime(t->kid1,notFree),
				  FreeVarsInTypePrime(t->kid2,notFree)));
    case 6:   return(FreeVarsInTypePrime(t->kid1,notFree));
    case 7: return(FreeVarsInTypePrime(t->kid1,AddToList(notFree,t->id)));
    }
}
int FreeInType(char *id, struct typeExp *t)
{
    if (t == (struct typeExp *)-3) return(0);
    switch (t->nodetype) {
    case 1:
    case 2:   return(0);
    case 3: return(strcmp(id,t->id)==0);
    case 4: 
    case 5: if (FreeInType(id,t->kid1)) return(1);
                else return(FreeInType(id,t->kid2));
    case 6: return(FreeInType(id,t->kid1));
    case 7: if (strcmp(id,t->id)==0) return(0);
                 else return(FreeInType(id,t->kid1));
    }
}
struct typeExp *BuildGeneric(struct typeExp *rho, struct typeVarList *freeVars, struct typeEnv *A)
{
struct typeExp *tmp;
struct typeVarList *L;
    if ((rho == (struct typeExp *)-3)||(A == (struct typeEnv *)-2)) return((struct typeExp *)-3);
    tmp = rho;
    L = freeVars;
    while (L != 0) {
        if (!FreeInEnv(L->name,A)) tmp = CreateType(7,L->name,tmp,0);
	L = L->next;
    }
    return(tmp);
}
struct typeVarList *NewList(void)
{
return(0);
}
struct typeVarList *AddToList(struct typeVarList *L, char *id)
{
struct typeVarList *new;
    new = (struct typeVarList *)malloc(sizeof(struct typeVarList));
    new->name = id;
    new->next = L;
    return(new);
}
struct typeVarList *ConcatList(struct typeVarList *L1, struct typeVarList *L2)
{
struct typeVarList *tmp;
    if (L1 == 0) return(L2);
    if (L2 == 0) return(L1);
    tmp = L1;
    while (tmp->next != 0) tmp = tmp->next;
    tmp->next = L2;
    return(L1);
}
struct typeVarList *UnionList(struct typeVarList *L1, struct typeVarList *L2)
{
struct typeVarList *tmp, *t2;
    if (L1 == 0) {
      tmp = CopyList(L2);
      return(tmp);
    }
    if (L2 == 0) {
      tmp = CopyList(L1);
      return(tmp);
    }
    tmp = CopyList(L1);
    t2 = L2;
    while (t2 != 0) {
      if (!ListMember(t2->name, tmp)) tmp = AddToList(tmp, t2->name);
      t2 = t2->next;
    }
    return(tmp);
}
struct typeVarList *CopyList(struct typeVarList *L)
{
struct typeVarList *tmp, *t;
    tmp = NewList();
    t = L;
    while(t != 0) {
      tmp = AddToList(tmp, t->name);
      t = t->next;
    }
    return(tmp);
}
int ListMember(char *id, struct typeVarList *L)
{
struct typeVarList *tmp;
    tmp = L;
    while (tmp != 0) {
        if (strcmp(tmp->name,id)==0) return(1);
	tmp = tmp->next;
    }
    return(0);
}
void PrintTypeVarList(struct typeVarList *L)
{
struct typeVarList *tmp;
    tmp = L;
    while (tmp != 0)     {
        printf("%s  ",tmp->name);
	tmp = tmp->next;
    }
}
