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
struct tree {
 int type;
 char *name;
 struct tree *kid1;
 struct tree *kid2;
 struct tree *kid3;
};
struct tree *CreateTreeNode(int nodetype, char *id, struct tree *child1, struct tree *child2, struct tree *child3);
struct tree *CreateLeaf(int leaftype, char *id);
struct tree *CreateTreeNode(int nodetype, char *id, struct tree *child1, struct tree *child2, struct tree *child3)
{
    struct tree *tmp;
    tmp = (struct tree *)malloc(sizeof(struct tree));
    tmp -> type = nodetype;
    tmp -> name = id;
    tmp -> kid1 = child1;
    tmp -> kid2 = child2;
    tmp -> kid3 = child3;
    return(tmp);
}
struct tree *CreateLeaf(int leaftype, char *id)
{
    struct tree *tmp;
    tmp = (struct tree *)malloc(sizeof(struct tree));
    tmp -> type = leaftype;
    tmp -> name = id;
    return(tmp);
}
void PrintTree(struct tree *t)
{
    if (t != 0) {
        switch (t->type) {
	case 1:
	case 2:
	case 3:     printf("%s",t->name);
	               break;
	case 4:
	case 5:
	case 6: printf("[%s]",t->name);
	               break;
	case 7: printf("(");
	            PrintTree(t->kid1);
		    printf(" ");
		    PrintTree(t->kid2);
		    printf(")");
		    break;
	case 8: printf("L %s . ",t->name);
	          PrintTree(t->kid1);
		  break;
	case 9:    printf("if ");
	              PrintTree(t->kid1);
		      printf(" then ");
		      PrintTree(t->kid2);
		      if (t->kid3 != 0) {
		          printf(" else ");
			  PrintTree(t->kid3);
		      }
		      printf(" fi");
		      break;
	case 10:   printf("let ");
	              printf("%s",t->name);
		      printf(" = ");
	              PrintTree(t->kid1);
		      printf(" in ");
		      PrintTree(t->kid2);
		      break;
	case 11: printf("fix ");
 	               printf("%s",t->name);
		       printf(" . ");
	               PrintTree(t->kid1);
		       break;
	default: printf("internal error: missing case for type %d in PrintTree\n", t->type);
	        exit(1);
	}
    }
}
