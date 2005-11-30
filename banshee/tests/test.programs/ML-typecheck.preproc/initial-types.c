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
struct typeExp *NullType(void)
{
    return(CreateType(7,
		      "t",
		      CreateType(4,
				 "",
				 CreateType(6,
					    "",
					    CreateType(3,
						       "t",
						       0,
						       0),
					    0),
				 CreateType(1,
					    "",
					    0,
					    0)),
		      0));
}
struct typeExp *ConsType(void)
{
    return(CreateType(7,
		      "t",
		      CreateType(4,
				 "",
				 CreateType(3,
					    "t",
					    0,
					    0),
                                 CreateType(4,
				            "",
					    CreateType(6,
						       "",
						       CreateType(3,
								  "t",
								  0,
								  0),
						       0),
				            CreateType(6,
					               "",
					               CreateType(3,
						                  "t",
						                  0,
						                  0),
					               0))),
		      0));
}
struct typeExp *CarType(void)
{
    return(CreateType(7,
		      "t",
		      CreateType(4,
				 "",
				 CreateType(6,
					    "",
					    CreateType(3,
						       "t",
						       0,
						       0),
					    0),
				 CreateType(3,
					    "t",
					    0,
					    0)),
		      0));
}
struct typeExp *CdrType(void)
{
    return(CreateType(7,
		      "t",
		      CreateType(4,
				 "",
				 CreateType(6,
					    "",
					    CreateType(3,
						       "t",
						       0,
						       0),
					    0),
		                 CreateType(6,
					    "",
					    CreateType(3,
						       "t",
						       0,
						       0),
					    0)),
		      0));
}
struct typeExp *PairType(void)
{
return(CreateType(7,
		  "a",
		   CreateType(7,
			      "b",
			      CreateType(4,
					 "",
					 CreateType(3,
						    "a",
						    0,
						    0),
					 CreateType(4,
						    "",
						    CreateType(3,
							       "b",
							       0,
							       0),
						    CreateType(5,
							       "",
							       CreateType(3,
									  "a",
									  0,
									  0),
							       CreateType(3,
									  "b",
									  0,
									  0)))),
				 0),
		      0));
}
struct typeExp *SuccType(void)
{
    return(CreateType(4,
		      "",
		      CreateType(2,
				 "",
				 0,
				 0),
		      CreateType(2,
				 "",
				 0,
				 0)));
}
