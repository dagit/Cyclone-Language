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
typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern int yyleng;
extern struct _iobuf *yyin, *yyout;
	extern int yywrap (void);
struct yy_buffer_state
	{
	struct _iobuf *yy_input_file;
	char *yy_ch_buf;		
	char *yy_buf_pos;		
	int yy_buf_size;
	int yy_n_chars;
	int yy_is_interactive;
	int yy_fill_buffer;
	int yy_buffer_status;
	};
static YY_BUFFER_STATE yy_current_buffer = 0;
static char yy_hold_char;
static int yy_n_chars;		
int yyleng;
static char *yy_c_buf_p = (char *) 0;
static int yy_init = 1;		
static int yy_start = 0;	
static int yy_did_buffer_switch_on_eof;
static void yyunput (int c, register char *yy_bp);
void yyrestart (FILE *input_file);
void yy_switch_to_buffer (YY_BUFFER_STATE new_buffer);
void yy_load_buffer_state (void);
YY_BUFFER_STATE yy_create_buffer (FILE *file, int size);
void yy_delete_buffer (YY_BUFFER_STATE b);
void yy_init_buffer (YY_BUFFER_STATE b, FILE *file);
static int yy_start_stack_ptr = 0;
static int yy_start_stack_depth = 0;
static int *yy_start_stack = 0;
static void yy_push_state (int new_state);
static void yy_pop_state (void);
static int yy_top_state (void);
static void *yy_flex_alloc (unsigned int size);
static void *yy_flex_realloc (void *ptr, unsigned int size);
static void yy_flex_free (void *ptr);
typedef unsigned char YY_CHAR;
typedef int yy_state_type;
struct _iobuf *yyin = (struct _iobuf *) 0, *yyout = (struct _iobuf *) 0;
extern char *yytext;
static int input (void);
static yy_state_type yy_get_previous_state (void);
static yy_state_type yy_try_NUL_trans (yy_state_type yy_current_state);
static int yy_get_next_buffer (void);
static void yy_fatal_error (char *msg);
static  short int yy_accept[49] =
    {   0,
        0,    0,   25,   23,    1,   24,   15,   16,   21,   19,
       13,   22,   20,   14,   17,   18,   12,   12,   12,   12,
       12,   12,   12,   12,   13,   12,   12,   12,    5,    2,
        8,   12,    9,   12,   12,   12,   12,   12,    6,    7,
       12,   12,    4,   12,    3,   10,   11,    0
    } ;
static  int yy_ec[256] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    2,    3,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    2,    1,    1,    1,    1,    1,    1,    1,    4,
        5,    1,    1,    6,    1,    7,    1,    8,    8,    8,
        8,    8,    8,    8,    8,    8,    8,    1,    9,    1,
       10,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,   11,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
       12,    1,   13,    1,    1,    1,   14,   15,   16,   15,
       17,   18,   15,   19,   20,   15,   15,   21,   15,   22,
       15,   15,   15,   23,   24,   25,   26,   15,   15,   15,
       15,   15,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1
    } ;
static  int yy_meta[27] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    2,    2,    2,    2,    2,    2,    2,
        2,    2,    2,    2,    2,    2
    } ;
static  short int yy_base[50] =
    {   0,
        0,    0,   51,   52,   52,   52,   52,   52,   52,   52,
       42,   52,   52,   52,   52,   52,    0,   28,   13,   10,
       31,   27,   29,   11,   37,    0,   20,   22,    0,    0,
        0,   17,    0,   25,   23,   13,   21,   13,    0,    0,
       14,   18,    0,   14,    0,    0,    0,   52,   27
    } ;
static  short int yy_def[50] =
    {   0,
       48,    1,   48,   48,   48,   48,   48,   48,   48,   48,
       48,   48,   48,   48,   48,   48,   49,   49,   49,   49,
       49,   49,   49,   49,   48,   49,   49,   49,   49,   49,
       49,   49,   49,   49,   49,   49,   49,   49,   49,   49,
       49,   49,   49,   49,   49,   49,   49,    0,   48
    } ;
static  short int yy_nxt[79] =
    {   0,
        4,    5,    6,    7,    8,    9,   10,   11,   12,   13,
       14,   15,   16,   17,   17,   17,   18,   19,   17,   20,
       21,   22,   23,   17,   24,   17,   28,   30,   26,   35,
       47,   31,   29,   36,   46,   45,   44,   43,   42,   41,
       40,   39,   38,   37,   25,   34,   33,   32,   27,   25,
       48,    3,   48,   48,   48,   48,   48,   48,   48,   48,
       48,   48,   48,   48,   48,   48,   48,   48,   48,   48,
       48,   48,   48,   48,   48,   48,   48,   48
    } ;
static  short int yy_chk[79] =
    {   0,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
        1,    1,    1,    1,    1,    1,   19,   20,   49,   24,
       44,   20,   19,   24,   42,   41,   38,   37,   36,   35,
       34,   32,   28,   27,   25,   23,   22,   21,   18,   11,
        3,   48,   48,   48,   48,   48,   48,   48,   48,   48,
       48,   48,   48,   48,   48,   48,   48,   48,   48,   48,
       48,   48,   48,   48,   48,   48,   48,   48
    } ;
static yy_state_type yy_last_accepting_state;
static char *yy_last_accepting_cpos;
char *yytext;
typedef union  {
    char *sval;
    struct tree *tval;
    } YYSTYPE;
extern YYSTYPE yylval;
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
int yylex (void)
{
	register yy_state_type yy_current_state;
	register char *yy_cp, *yy_bp;
	register int yy_act;
	if ( yy_init )
		{
		if ( ! yy_start )
			yy_start = 1;	
		if ( ! yyin )
			yyin = (&_iob[0]);
		if ( ! yyout )
			yyout = (&_iob[1]);
		if ( yy_current_buffer )
			yy_init_buffer( yy_current_buffer, yyin );
		else
			yy_current_buffer =
				yy_create_buffer( yyin, 16384 );
		yy_load_buffer_state();
		yy_init = 0;
		}
	while ( 1 )		
		{
		yy_cp = yy_c_buf_p;
		*yy_cp = yy_hold_char;
		yy_bp = yy_cp;
		yy_current_state = yy_start;
yy_match:
		do
			{
			register YY_CHAR yy_c = yy_ec[((unsigned int) (unsigned char) *yy_cp)];
			if ( yy_accept[yy_current_state] )
				{
				yy_last_accepting_state = yy_current_state;
				yy_last_accepting_cpos = yy_cp;
				}
			while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
				{
				yy_current_state = (int) yy_def[yy_current_state];
				if ( yy_current_state >= 49 )
					yy_c = yy_meta[(unsigned int) yy_c];
				}
			yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
			++yy_cp;
			}
		while ( yy_base[yy_current_state] != 52 );
yy_find_action:
		yy_act = yy_accept[yy_current_state];
		yytext = yy_bp; yyleng = yy_cp - yy_bp; yy_hold_char = *yy_cp; *yy_cp = '\0'; yy_c_buf_p = yy_cp;;
do_action:	
		switch ( yy_act )
	{ 
			case 0: 
			*yy_cp = yy_hold_char;
			yy_cp = yy_last_accepting_cpos;
			yy_current_state = yy_last_accepting_state;
			goto yy_find_action;
case 1:
{}
	break;
case 2:
{return(266);}
	break;
case 3:
{return(267);}
	break;
case 4:
{return(268);}
	break;
case 5:
{return(269);}
	break;
case 6:
{return(270);}
	break;
case 7:
{return(271);}
	break;
case 8:
{return(273);}
	break;
case 9:
{return(274);}
	break;
case 10:
{yylval.sval =
			     strcpy((char *)malloc(strlen(yytext)+1),yytext);
			 return(259);}
	break;
case 11:
{yylval.sval =
			     strcpy((char *)malloc(strlen(yytext)+1),yytext);
			 return(259);}
	break;
case 12:
{yylval.sval =
			     strcpy((char *)malloc(strlen(yytext)+1),yytext);
			 return(257);}
	break;
case 13:
{yylval.sval =
			     strcpy((char *)malloc(strlen(yytext)+1),yytext);
			 return(258);}
	break;
case 14:
{return(260);}
	break;
case 15:
{return(261);}
	break;
case 16:
{return(262);}
	break;
case 17:
{return(263);}
	break;
case 18:
{return(264);}
	break;
case 19:
{return(265);}
	break;
case 20:
{return(272);}
	break;
case 21:
{return(275);}
	break;
case 22:
{return(276);}
	break;
case 23:
{printf("\nignoring illegal char in input: %s\n",yytext);}
	break;
case 24:
(void) fwrite( yytext, yyleng, 1, yyout );
	break;
case (25 + 0 + 1):
	return 0;
	case 25:
		{
		int yy_amount_of_matched_text = yy_cp - yytext - 1;
		*yy_cp = yy_hold_char;
		if ( yy_current_buffer->yy_buffer_status == 0 )
			{
			yy_n_chars = yy_current_buffer->yy_n_chars;
			yy_current_buffer->yy_input_file = yyin;
			yy_current_buffer->yy_buffer_status = 1;
			}
		if ( yy_c_buf_p <= &yy_current_buffer->yy_ch_buf[yy_n_chars] )
			{ 
			yy_state_type yy_next_state;
			yy_c_buf_p = yytext + yy_amount_of_matched_text;
			yy_current_state = yy_get_previous_state();
			yy_next_state = yy_try_NUL_trans( yy_current_state );
			yy_bp = yytext + 0;
			if ( yy_next_state )
				{
				yy_cp = ++yy_c_buf_p;
				yy_current_state = yy_next_state;
				goto yy_match;
				}
			else
				{
							yy_cp = yy_c_buf_p;
				goto yy_find_action;
				}
			}
		else switch ( yy_get_next_buffer() )
			{
			case 1:
				{
				yy_did_buffer_switch_on_eof = 0;
				if ( yywrap() )
					{
					yy_c_buf_p = yytext + 0;
					yy_act = (25 + ((yy_start - 1) / 2) + 1);
					goto do_action;
					}
				else
					{
					if ( ! yy_did_buffer_switch_on_eof )
						yyrestart( yyin );
					}
				break;
				}
			case 0:
				yy_c_buf_p =
					yytext + yy_amount_of_matched_text;
				yy_current_state = yy_get_previous_state();
				yy_cp = yy_c_buf_p;
				yy_bp = yytext + 0;
				goto yy_match;
			case 2:
				yy_c_buf_p =
				&yy_current_buffer->yy_ch_buf[yy_n_chars];
				yy_current_state = yy_get_previous_state();
				yy_cp = yy_c_buf_p;
				yy_bp = yytext + 0;
				goto yy_find_action;
			}
		break;
		}
	default:
		yy_fatal_error( 
			"fatal flex scanner internal error--no action found"  );
	} 
		} 
	} 
static int yy_get_next_buffer(void)
{
	register char *dest = yy_current_buffer->yy_ch_buf;
	register char *source = yytext - 1; 
	register int number_to_move, i;
	int ret_val;
	if ( yy_c_buf_p > &yy_current_buffer->yy_ch_buf[yy_n_chars + 1] )
		yy_fatal_error( 
		"fatal flex scanner internal error--end of buffer missed"  );
	if ( yy_current_buffer->yy_fill_buffer == 0 )
		{ 
		if ( yy_c_buf_p - yytext - 0 == 1 )
			{
			return 1;
			}
		else
			{
			return 2;
			}
		}
	number_to_move = yy_c_buf_p - yytext;
	for ( i = 0; i < number_to_move; ++i )
		*(dest++) = *(source++);
	if ( yy_current_buffer->yy_buffer_status == 2 )
		yy_n_chars = 0;
	else
		{
		int num_to_read =
			yy_current_buffer->yy_buf_size - number_to_move - 1;
		while ( num_to_read <= 0 )
			{ 
			YY_BUFFER_STATE b = yy_current_buffer;
			int yy_c_buf_p_offset = yy_c_buf_p - b->yy_ch_buf;
			b->yy_buf_size *= 2;
			b->yy_ch_buf = (char *)
				yy_flex_realloc( (void *) b->yy_ch_buf,
						 b->yy_buf_size );
			if ( ! b->yy_ch_buf )
				yy_fatal_error( 
				"fatal error - scanner input buffer overflow"  );
			yy_c_buf_p = &b->yy_ch_buf[yy_c_buf_p_offset];
			num_to_read = yy_current_buffer->yy_buf_size -
						number_to_move - 1;
			}
		if ( num_to_read > 8192 )
			num_to_read = 8192;
		if ( yy_current_buffer->yy_is_interactive ) { int c = (--( yyin )->_cnt>=0? ((int)*( yyin )->_ptr++):_filbuf( yyin )); 
			yy_n_chars = c == (-1) ? 0 : 1;  (&yy_current_buffer->yy_ch_buf[number_to_move])[0] = (char) c; } else if ( ((
			yy_n_chars = fread(  (&yy_current_buffer->yy_ch_buf[number_to_move]), 1,  num_to_read , yyin )) == 0) && ((( yyin )->_flag&040)!=0) ) yy_fatal_error(  "input in flex scanner failed"  );;
		}
	if ( yy_n_chars == 0 )
		{
		if ( number_to_move - 0 == 1 )
			{
			ret_val = 1;
			yyrestart( yyin );
			}
		else
			{
			ret_val = 2;
			yy_current_buffer->yy_buffer_status =
				2;
			}
		}
	else
		ret_val = 0;
	yy_n_chars += number_to_move;
	yy_current_buffer->yy_ch_buf[yy_n_chars] = 0;
	yy_current_buffer->yy_ch_buf[yy_n_chars + 1] = 0;
	yytext = &yy_current_buffer->yy_ch_buf[1];
	return ret_val;
	}
static yy_state_type yy_get_previous_state(void)
{
	register yy_state_type yy_current_state;
	register char *yy_cp;
	yy_current_state = yy_start;
	for ( yy_cp = yytext + 0; yy_cp < yy_c_buf_p; ++yy_cp )
		{
		register YY_CHAR yy_c = (*yy_cp ? yy_ec[((unsigned int) (unsigned char) *yy_cp)] : 1);
		if ( yy_accept[yy_current_state] )
			{
			yy_last_accepting_state = yy_current_state;
			yy_last_accepting_cpos = yy_cp;
			}
		while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
			{
			yy_current_state = (int) yy_def[yy_current_state];
			if ( yy_current_state >= 49 )
				yy_c = yy_meta[(unsigned int) yy_c];
			}
		yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
		}
	return yy_current_state;
	}
static yy_state_type yy_try_NUL_trans(yy_state_type yy_current_state)
{
	register int yy_is_jam;
	register char *yy_cp = yy_c_buf_p;
	register YY_CHAR yy_c = 1;
	if ( yy_accept[yy_current_state] )
		{
		yy_last_accepting_state = yy_current_state;
		yy_last_accepting_cpos = yy_cp;
		}
	while ( yy_chk[yy_base[yy_current_state] + yy_c] != yy_current_state )
		{
		yy_current_state = (int) yy_def[yy_current_state];
		if ( yy_current_state >= 49 )
			yy_c = yy_meta[(unsigned int) yy_c];
		}
	yy_current_state = yy_nxt[yy_base[yy_current_state] + (unsigned int) yy_c];
	yy_is_jam = (yy_current_state == 48);
	return yy_is_jam ? 0 : yy_current_state;
	}
static void yyunput(int c, register char *yy_bp)
{
	register char *yy_cp = yy_c_buf_p;
	*yy_cp = yy_hold_char;
	if ( yy_cp < yy_current_buffer->yy_ch_buf + 2 )
		{ 
		register int number_to_move = yy_n_chars + 2;
		register char *dest = &yy_current_buffer->yy_ch_buf[
					yy_current_buffer->yy_buf_size + 2];
		register char *source =
				&yy_current_buffer->yy_ch_buf[number_to_move];
		while ( source > yy_current_buffer->yy_ch_buf )
			*--dest = *--source;
		yy_cp += dest - source;
		yy_bp += dest - source;
		yy_n_chars = yy_current_buffer->yy_buf_size;
		if ( yy_cp < yy_current_buffer->yy_ch_buf + 2 )
			yy_fatal_error(  "flex scanner push-back overflow"  );
		}
	if ( yy_cp > yy_bp && yy_cp[-1] == '\n' )
		yy_cp[-2] = '\n';
	*--yy_cp = (char) c;
	yytext = yy_bp; yyleng = yy_cp - yy_bp; yy_hold_char = *yy_cp; *yy_cp = '\0'; yy_c_buf_p = yy_cp;; 
	}
static int input(void)
{
	int c;
	*yy_c_buf_p = yy_hold_char;
	if ( *yy_c_buf_p == 0 )
		{
		if ( yy_c_buf_p < &yy_current_buffer->yy_ch_buf[yy_n_chars] )
			*yy_c_buf_p = '\0';
		else
			{ 
			yytext = yy_c_buf_p;
			++yy_c_buf_p;
			switch ( yy_get_next_buffer() )
				{
				case 1:
					{
					if ( yywrap() )
						{
						yy_c_buf_p =
						yytext + 0;
						return (-1);
						}
					yyrestart( yyin );
					return input();
					}
				case 0:
					yy_c_buf_p = yytext + 0;
					break;
				case 2:
					yy_fatal_error( 
					"unexpected last match in input()"  );
				}
			}
		}
	c = *(unsigned char *) yy_c_buf_p;	
	*yy_c_buf_p = '\0';	
	yy_hold_char = *++yy_c_buf_p;
	return c;
	}
void yyrestart(FILE *input_file)
{
	if ( ! yy_current_buffer )
		yy_current_buffer = yy_create_buffer( yyin, 16384 );
	yy_init_buffer( yy_current_buffer, input_file );
	yy_load_buffer_state();
	}
void yy_switch_to_buffer(YY_BUFFER_STATE new_buffer)
{
	if ( yy_current_buffer == new_buffer )
		return;
	if ( yy_current_buffer )
		{
		*yy_c_buf_p = yy_hold_char;
		yy_current_buffer->yy_buf_pos = yy_c_buf_p;
		yy_current_buffer->yy_n_chars = yy_n_chars;
		}
	yy_current_buffer = new_buffer;
	yy_load_buffer_state();
	yy_did_buffer_switch_on_eof = 1;
	}
void yy_load_buffer_state(void)
{
	yy_n_chars = yy_current_buffer->yy_n_chars;
	yytext = yy_c_buf_p = yy_current_buffer->yy_buf_pos;
	yyin = yy_current_buffer->yy_input_file;
	yy_hold_char = *yy_c_buf_p;
	}
YY_BUFFER_STATE yy_create_buffer(FILE *file, int size)
{
	YY_BUFFER_STATE b;
	b = (YY_BUFFER_STATE) yy_flex_alloc( sizeof( struct yy_buffer_state ) );
	if ( ! b )
		yy_fatal_error(  "out of dynamic memory in yy_create_buffer()"  );
	b->yy_buf_size = size;
	b->yy_ch_buf = (char *) yy_flex_alloc( b->yy_buf_size + 2 );
	if ( ! b->yy_ch_buf )
		yy_fatal_error(  "out of dynamic memory in yy_create_buffer()"  );
	yy_init_buffer( b, file );
	return b;
	}
void yy_delete_buffer(YY_BUFFER_STATE b)
{
	if ( b == yy_current_buffer )
		yy_current_buffer = (YY_BUFFER_STATE) 0;
	yy_flex_free( (void *) b->yy_ch_buf );
	yy_flex_free( (void *) b );
	}
void yy_init_buffer(YY_BUFFER_STATE b, FILE *file)
{
	b->yy_input_file = file;
	b->yy_ch_buf[0] = '\n';
	b->yy_n_chars = 1;
	b->yy_ch_buf[1] = 0;
	b->yy_ch_buf[2] = 0;
	b->yy_buf_pos = &b->yy_ch_buf[1];
	b->yy_is_interactive = file ? isatty( ((file)->_file) ) : 0;
	b->yy_fill_buffer = 1;
	b->yy_buffer_status = 0;
	}
static void yy_push_state(int new_state)
{
	if ( yy_start_stack_ptr >= yy_start_stack_depth )
		{
		int new_size;
		yy_start_stack_depth += 25;
		new_size = yy_start_stack_depth * sizeof( int );
		if ( ! yy_start_stack )
			yy_start_stack = (int *) yy_flex_alloc( new_size );
		else
			yy_start_stack = (int *) yy_flex_realloc(
					(void *) yy_start_stack, new_size );
		if ( ! yy_start_stack )
			yy_fatal_error( 
			"out of memory expanding start-condition stack"  );
		}
	yy_start_stack[yy_start_stack_ptr++] = ((yy_start - 1) / 2);
	yy_start = 1 + 2 *(new_state);
	}
static void yy_pop_state(void)
{
	if ( --yy_start_stack_ptr < 0 )
		yy_fatal_error(  "start-condition stack underflow"  );
	yy_start = 1 + 2 *(yy_start_stack[yy_start_stack_ptr]);
	}
static int yy_top_state(void)
{
	return yy_start_stack[yy_start_stack_ptr - 1];
	}
static void yy_fatal_error(char *msg)
{
	(void) fprintf( (&_iob[2]), "%s\n", msg );
	exit( 1 );
	}
static void *yy_flex_alloc(unsigned int size)
{
	return (void *) malloc( size );
	}
static void *yy_flex_realloc(void *ptr, unsigned int size)
{
	return (void *) realloc( ptr, size );
	}
static void yy_flex_free(void *ptr)
{
	free( ptr );
	}
