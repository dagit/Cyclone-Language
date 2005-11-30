struct tree {
 int type;
 char *name;
 struct tree *kid1;
 struct tree *kid2;
 struct tree *kid3;
};
struct tree *CreateTreeNode(int nodetype, char *id, struct tree *child1, struct tree *child2, struct tree *child3);
struct tree *CreateLeaf(int leaftype, char *id);
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
struct TCPair *TypeCheck(struct typeEnv *A, struct tree *e);
struct sub *Unify(struct sub *S, struct typeExp *t1, struct typeExp *t2);
struct sub *Extend(struct sub *S, char *id, struct typeExp *exp);
char *tmpstr;
struct typeEnv *env;
struct TCPair *result;
typedef union  {
    char *sval;
    struct tree *tval;
    } YYSTYPE;
extern int yychar;
extern int yyerrflag;
YYSTYPE yylval, yyval;
int yywrap(void)
{
  return(1);
}
int yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
	};
int yyact[]={
    12,    13,    14,    18,    11,    43,    21,    42,    47,    19,
    38,    28,    36,    20,    12,    13,    14,    18,    11,    23,
    21,    34,    41,    19,    40,    29,    33,    20,    39,    26,
    32,    59,    12,    13,    14,    18,    11,    17,    21,    31,
    16,    19,    33,    34,    15,    20,     4,     2,    30,    57,
    12,    13,    14,    18,    11,     3,    21,     1,    10,    19,
    22,     9,     8,    20,     7,     6,    55,    12,    13,    14,
    18,    11,     0,    21,    49,     0,    19,     0,     0,     0,
    20,     0,    48,    51,    12,    13,    14,    18,    11,     0,
    21,     0,     0,    19,     0,     0,    56,    20,    12,    13,
    14,    18,    11,     0,    21,     0,     0,    19,     0,    50,
     0,    20,    12,    13,    14,    18,    11,     0,    21,     0,
     0,    19,    37,     0,     0,    20,    12,    13,    14,    18,
    11,    35,    21,     0,     0,    19,     0,     0,     0,    20,
    12,    13,    14,    18,    11,    24,    21,     0,     5,    19,
     5,     0,     0,    20,     0,     0,     0,    25,     0,     0,
     0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    44,    45,    46,     0,     0,     0,     0,     0,
     0,     0,     0,    52,     0,     0,    53,    54,     0,     0,
     0,    58 };
int yypact[]={
 -1000, -1000,  -117, -1000,  -117,  -257, -1000, -1000, -1000, -1000,
 -1000,  -117, -1000, -1000, -1000, -1000, -1000, -1000,  -228,  -117,
  -246,  -216, -1000, -1000, -1000,  -131,  -253,  -145,  -262,  -229,
 -1000,  -240,  -242,  -268,  -270, -1000,  -117,  -117,  -117,  -264,
 -1000, -1000,  -232,  -238,  -117,  -159,  -190,  -117, -1000, -1000,
  -117,  -117,  -207,  -173,  -225,  -117, -1000, -1000,  -243, -1000 };
int yypgo[]={
     0,   145,    65,    64,    62,    61,    58,    57,    47,    55,
    46,    44,    40,    37,    39,    30 };
int yyr1[]={
     0,     8,     7,     9,     9,    10,     1,     1,     1,     1,
     1,     1,     2,     2,     2,     2,     2,     2,     3,     4,
     5,     6,     6,    11,    12,    14,    14,    13,    15,    15 };
int yyr2[]={
     0,     1,     4,     2,     4,     5,     2,     2,     2,     2,
     2,     7,     3,     3,     3,     3,     3,     3,     5,     9,
    15,    15,    17,     4,     6,     2,     6,     6,     2,     6 };
int yychk[]={
 -1000,    -7,    -8,    -9,   -10,    -1,    -2,    -3,    -4,    -5,
    -6,   261,   257,   258,   259,   -11,   -12,   -13,   260,   266,
   270,   263,    -9,   276,    -1,    -1,   257,    -1,   257,   271,
   264,   -14,   -15,   258,   259,   262,   265,   267,   272,   257,
   264,   264,   275,   275,    -1,    -1,    -1,   272,   -14,   -15,
   268,   273,    -1,    -1,    -1,   273,   269,   274,    -1,   274 };
int yydef[]={
     1,    -2,     0,     2,     3,     0,     6,     7,     8,     9,
    10,     0,    12,    13,    14,    15,    16,    17,     0,     0,
     0,     0,     4,     5,    18,     0,     0,     0,     0,     0,
    23,     0,     0,    25,    28,    11,     0,     0,     0,     0,
    24,    27,     0,     0,    19,     0,     0,     0,    26,    29,
     0,     0,     0,     0,     0,     0,    20,    21,     0,    22 };
typedef struct { char *t_name; int t_val; } yytoktype;
int yydebug;			
static YYSTYPE *yyv;			
static int *yys;			
static YYSTYPE *yypv;			
static int *yyps;			
static int yystate;			
static int yytmp;			
int yynerrs;			
int yyerrflag;			
int yychar;			
int
yyparse(void)
{
	register YYSTYPE *yypvt;	
	unsigned yymaxdepth = 150;
	yyv = (YYSTYPE*)malloc(yymaxdepth*sizeof(YYSTYPE));
	yys = (int*)malloc(yymaxdepth*sizeof(int));
	if (!yyv || !yys)
	{
		yyerror( "out of memory" );
		return(1);
	}
	yypv = &yyv[-1];
	yyps = &yys[-1];
	yystate = 0;
	yytmp = 0;
	yynerrs = 0;
	yyerrflag = 0;
	yychar = -1;
	goto yystack;
	{
		register YYSTYPE *yy_pv;	
		register int *yy_ps;		
		register int yy_state;		
		register int  yy_n;		
	yynewstate:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
		goto yy_newstate;
	yystack:
		yy_pv = yypv;
		yy_ps = yyps;
		yy_state = yystate;
	yy_stack:
		if ( ++yy_ps >= &yys[ yymaxdepth ] )	
		{
			int yyps_index = (yy_ps - yys);
			int yypv_index = (yy_pv - yyv);
			int yypvt_index = (yypvt - yyv);
			yymaxdepth += 150;
			yyv = (YYSTYPE*)realloc((char*)yyv,
				yymaxdepth * sizeof(YYSTYPE));
			yys = (int*)realloc((char*)yys,
				yymaxdepth * sizeof(int));
			if (!yyv || !yys)
			{
				yyerror( "yacc stack overflow" );
				return(1);
			}
			yy_ps = yys + yyps_index;
			yy_pv = yyv + yypv_index;
			yypvt = yyv + yypvt_index;
		}
		*yy_ps = yy_state;
		*++yy_pv = yyval;
	yy_newstate:
		if ( ( yy_n = yypact[ yy_state ] ) <= (-1000) )
			goto yydefault;		
		if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
			yychar = 0;		
		if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= 202 ) )
			goto yydefault;
		if ( yychk[ yy_n = yyact[ yy_n ] ] == yychar )	
		{
			yychar = -1;
			yyval = yylval;
			yy_state = yy_n;
			if ( yyerrflag > 0 )
				yyerrflag--;
			goto yy_stack;
		}
	yydefault:
		if ( ( yy_n = yydef[ yy_state ] ) == -2 )
		{
			if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
				yychar = 0;		
			{
				register int *yyxi = yyexca;
				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					{ free(yys); free(yyv); return(0); };
			}
		}
		if ( yy_n == 0 )	
		{
			switch ( yyerrflag )
			{
			case 0:		
				yyerror( "syntax error" );
				goto skip_init;
			yyerrlab:
				yy_pv = yypv;
				yy_ps = yyps;
				yy_state = yystate;
				yynerrs++;
			skip_init:
			case 1:
			case 2:		
				yyerrflag = 3;
				while ( yy_ps >= yys )
				{
					yy_n = yypact[ *yy_ps ] + 256;
					if ( yy_n >= 0 && yy_n < 202 &&
						yychk[yyact[yy_n]] == 256)					{
						yy_state = yyact[ yy_n ];
						goto yy_stack;
					}
					yy_ps--;
					yy_pv--;
				}
				{ free(yys); free(yyv); return(1); };
			case 3:		
				if ( yychar == 0 )	
					{ free(yys); free(yyv); return(1); };
				yychar = -1;
				goto yy_newstate;
			}
		}
		yytmp = yy_n;			
		yypvt = yy_pv;			
		{
			register int yy_len = yyr2[ yy_n ];
			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				yyval = ( yy_pv -= yy_len )[1];	
				yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= 202 ||
					yychk[ yy_state =
					yyact[ yy_state ] ] != -yy_n )
				{
					yy_state = yyact[ yypgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			yyval = ( yy_pv -= yy_len )[1];	
			yy_state = yypgo[ yy_n = yyr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= 202 ||
				yychk[ yy_state = yyact[ yy_state ] ] != -yy_n )
			{
				yy_state = yyact[ yypgo[ yy_n ] ];
			}
		}
		yystate = yy_state;
		yyps = yy_ps;
		yypv = yy_pv;
	}
	switch( yytmp )
	{
case 1:
{env = InitialEnv();
		 printf("THE INITIAL ENVIRONMENT IS AS FOLLOWS:\n");
		 PrintEnv(env);
		 printf("\n");
		 } break;
case 5:
{result = TypeCheck(env,yypvt[-1].tval);
		    printf("\nTYPE OF EXPRESSION IS ");
		    PrintType(result->type);
		    printf("\n");
		    } break;
case 11:
{yyval.tval = yypvt[-1].tval;} break;
case 12:
{yyval.tval = CreateLeaf(1,yypvt[-0].sval);} break;
case 13:
{yyval.tval = CreateLeaf(2,"int");} break;
case 14:
{yyval.tval = CreateLeaf(3,"bool");} break;
case 15:
{yyval.tval = CreateLeaf(4,"");} break;
case 16:
{yyval.tval = CreateLeaf(5,"list-of-ints");} break;
case 17:
{yyval.tval = CreateLeaf(6,"list-of-bools");} break;
case 18:
{yyval.tval = CreateTreeNode(7, "", yypvt[-1].tval, yypvt[-0].tval, 0);} break;
case 19:
{yyval.tval = CreateTreeNode(8, yypvt[-2].sval, yypvt[-0].tval, 0, 0);} break;
case 20:
{yyval.tval = CreateTreeNode(9, "", yypvt[-5].tval, yypvt[-3].tval, yypvt[-1].tval);} break;
case 21:
{yyval.tval = CreateTreeNode(10, yypvt[-5].sval, yypvt[-3].tval, yypvt[-1].tval, 0);} break;
case 22:
{yyval.tval = CreateTreeNode(10,
		    			 yypvt[-5].sval,
					 CreateTreeNode(11,yypvt[-5].sval,yypvt[-3].tval,0,0),
					 yypvt[-1].tval,
					 0);} break;
	}
	goto yystack;		
}
