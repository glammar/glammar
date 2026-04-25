/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/*
    This heap is used to store the input and to evaluate affixes.
    It is not tested for overflow.
*/

/*
    This heap is used to store affixes .
    It is not tested for overflow.
    For complete non-deterministic grammars this stack stays empty.
*/

/*
INPUT_MEMO_SIZE
    This size specifies the number of integers (bits) to represent 
    the memoize table state  for the current input character.
    If an integer takes 32 bits and INPUT_MEMO_SIZE is set to 64
    it means that 64 * 32 = 2048 characters can be memoized.
    It is tested for overflow.
*/
    
#define CONTINUATION_STACK_SIZE 1512000

/*
    On this stack continuations are put on.
    In the beginning and end this stack is empty and less obvious also 
    in the middle (when found a parse) this stack is empty.
    It is tested for overflow.
    For deterministic grammars this stack is always empty.
*/
typedef  union { void (*q)() ; int *i;char *s,**l; AFFIX a; } cont;


#define COPY(x,y) (x)->t = (y)->t ; (x)->l = (y)->l ; (x)->r = (y)->r
#define SHARE(x,y) (x)->t = empty ; (x)->l = nil ; (x)->r = y 
#define MAKE(x,y) if ( (y)->t == undefined) { SHARE(x,y);} else { COPY (x,y); }

cont *q_stack,*q_top,*q;
long int q_size = CONTINUATION_STACK_SIZE;
char *input,*cstore, *mip,*limitip, *empty = "",
      *ct,*undefined = "undefined", *cstore_top,
     *fast_list_acces = "\001", *inputbuff = "",
      *set_ip_start_pos, *arg_zero, *pntname= "", *lastpntname;

affix fla = {"@@@",0,0};


#if defined(__GNUC__) && defined(__sparc__)
register char *c asm ("g7");
register char *ip asm ("g6");
register affix *af asm ("g5");
#else
char *c, *ip;
affix *af;
#endif

AFFIX affix_heap,afx_top;
int argc;
char ** arg_v; 
long int  set_ip_start_num ;
int  tabstop = 8 ;
long int rcount =0, rmax =0,parsecount=0,nrofchars,level=0,eval_count, emsg_count=0;
long int cssize;
long int afhsize;
#ifdef MEMOIZE
int    memorizing = 1;
 long memo_table[MEMOIZE][INPUT_MEMO_SIZE];
int clear_memo_tab (void) {
   int i,j; 
    for (i = 0; i< MEMOIZE; i++) 
     for (j = 0; j< INPUT_MEMO_SIZE;) 
        memo_table [i] [j++] = -1;
   return 1;
} 
                        
#else 
int memorizing = 0;
int clear_memo_tab (void) { return 0;}
#endif
extern void found(void);


#define nil 0
#define true 1
#define false 0


void Uskip_ (void);
void lkh_epiloque (void);
void lkh_proloque (void);
void Ucut (void);
void Uidentifier (void);
int Didentifier (AFFIX A_0);
void Uletterdigitsequence (void);
int Dletterdigitsequence (AFFIX A_0);
void Unlcr (void);
int Dnlcr (void);
void Ubackslash (void);
int Dbackslash (void);
void Uquote (void);
int Dquote (void);
void Uendofsentence (void);
int Dendofsentence (void);
int Ddequal (AFFIX A, AFFIX B);
int Dequal (AFFIX A, AFFIX B);
int Dequalsempty (AFFIX A);
int Dnotequal (AFFIX A, AFFIX B);
int Ddfqual (AFFIX A, AFFIX B);
int Dfqual (AFFIX A, AFFIX B);
int Dfotequal (AFFIX A, AFFIX B);
int Dcompare (AFFIX A, AFFIX B, AFFIX tailx, AFFIX taily);
void Ulinenum (void);
int Dlinenum (affix *D_0);
void Usetlinefile (void);
void Ugetlinefile (void);
int Dsetlinefile (AFFIX L, AFFIX F);
int Dgetlinefile (AFFIX L, AFFIX F);
int Dcharnum (AFFIX D_0);
int Dsettabstop (AFFIX F);
int Dsomename (AFFIX A_0);
void Uaddto (void);
int Daddto (AFFIX A, AFFIX B, AFFIX C);
int Dvalueof (AFFIX A, AFFIX B, AFFIX C);
int Dincludes (AFFIX A, AFFIX B);
int Dexcludes (AFFIX A, AFFIX B);
int Dlookup (AFFIX A, AFFIX B, AFFIX C);
int Dcontentsof (AFFIX A, AFFIX B, AFFIX C, AFFIX D);
int Dsize (AFFIX A, AFFIX B);
void Udelete (void);
void Uselect (void);
int Dgetnextoption (AFFIX A_0);
void Ugetnextoption (void);
int Dlayout (AFFIX A_0);
int Dwhitespace (AFFIX A_0);
void Ulayout (void);
int Dlength (AFFIX A, AFFIX B);
void Ulength (void);
void  base64encode(char *clear, int len, char *code );
int Dbase64encode (AFFIX A);
int Dbase64addencode (AFFIX A);
int Dbase64intencode (AFFIX A, AFFIX B);
int Dbase64decode (AFFIX A, AFFIX B);
int Dtypein (AFFIX A_0);
int Dtypeout (AFFIX I_0);
int Drealtoint (AFFIX I_0, AFFIX D_1);
int Ddecimaltounary (AFFIX I_0, AFFIX D_1);
int Dkeyword (AFFIX I);
int Duppercase (AFFIX I, AFFIX D);
int Dreverse (AFFIX I, AFFIX D);
int Dlowercase (AFFIX I, AFFIX D);
int Dcompute (AFFIX I_0, AFFIX D_1);
int expression (signed long long *val);
int forced_expression (signed long long *val);
int simple_expression (signed long long *val);
int forced_simple_expression (signed long long *val);
int term (signed long long *val);
int forced_term (signed long long *val);
int factor (signed long long *val);
long forced_close_symbol (void);
int more_expression (signed long long *val);
int more_simple_expression (signed long long *val);
int more_term (signed long long *val);
long is_plus_symbol (void);
long is_minus_symbol (void);
int is_number (signed long long *val);
long is_open_symbol (void);
long is_not_symbol (void);
long is_close_symbol (void);
long is_equal_symbol (void);
long is_not_equal_symbol (void);
long is_less_symbol (void);
long is_greater_symbol (void);
long is_times_symbol (void);
long is_divide_symbol (void);
void Usetinputptrto_ (void);
void Uresetinputptr_ (void);
void Udetnestarset (void);
void Unestaralt_ (void);
void Unestarset (void);
void Umetaterminal (void);
int Dsetinputptrto_ (AFFIX IP, AFFIX A_0);
int Dresetinputptr_ (AFFIX A_0);
int Ddetprefix (AFFIX A_2, AFFIX A_1, AFFIX A_0);
int Ddetprefix2_ (AFFIX A_2, AFFIX A_1, AFFIX A_0);
int Ddetnestarset (AFFIX A_1, AFFIX A_0);
int Dnestaralt_ (AFFIX A_0, AFFIX A_1);
int Dgetlist_ (AFFIX A_0);
int Dgetlastlist_ (AFFIX A_0, AFFIX A_1, AFFIX A_2);
int Dmetaterminal (AFFIX A_0);
int Dmetaterminal2_ (AFFIX A_0, AFFIX A_1);
int Dwas (AFFIX afx);
int Xwas (int nr, AFFIX afx);
int Dwas10 (AFFIX afx);
int Dwas100 (AFFIX afx);
int Dwas1000 (AFFIX afx);
int Dfind (AFFIX A_0);
int Dpair (AFFIX A, AFFIX C, AFFIX D);
int Dunpair (AFFIX A, AFFIX B, AFFIX C);
int Dunpair2 (AFFIX A, AFFIX *B, AFFIX *C);
int Drepair (AFFIX A_0, AFFIX A_1, AFFIX A_2);
void Urepair (void);
int Dwhere (AFFIX A_0, AFFIX A_1);
int Devalmeta (AFFIX A_0);
void Uevalmeta (void);
int Dinitmeta (AFFIX A_0, AFFIX A_1);
void Uinitmeta (void);
int Dassign (AFFIX A_0, AFFIX A_1, AFFIX A_2);
void Uassign (void);
int Dsetupconnectiontounix (AFFIX B, AFFIX C);
int Dtalktounix (AFFIX B, AFFIX A_0, AFFIX A_1);
int talkto (char *cmd);
int Dgetfromunix (AFFIX A_0, AFFIX A_1);
int Dwritefile (AFFIX A_0, AFFIX A_1);
int Dsystem (AFFIX A_0);
void Usetupconnectiontounix (void);
void Utalktounix (void);
void Ugetfromunix (void);
void Uwritefile (void);
int Dtoutc (AFFIX A_0, AFFIX A_1);
void Uequal (void);
void Unotequal (void);
void Ufqual (void);
void Ufotequal (void);
void Ucompare (void);
void Ucontentsof (void);
void Ucharnum (void);
void Usettabstop (void);
void Usomename (void);
void Uvalueof (void);
void Ulookup (void);
void Uincludes (void);
void Uexcludes (void);
void Upair (void);
void Uunpair (void);
void Uwhere (void);
void Ucompute (void);
void Usize (void);
void Urealtoint (void);
void Udecimaltounary (void);
void Ureverse (void);
void Uuppercase (void);
void Ulowercase (void);
void Ukeyword (void);
void Utypeout (void);
void Utypein (void);
int Dlexicalless (AFFIX A, AFFIX B);
int Dlexicalmore (AFFIX A, AFFIX B);
int Dlexicallessequal (AFFIX A, AFFIX B);
int Dlexicalmoreequal (AFFIX A, AFFIX B);
void Ulexicalless (void);
void Ulexicalmore (void);
void Ulexicallessequal (void);
void Ulexicalmoreequal (void);
int Devalaffix (AFFIX A, AFFIX B);
void Uevalaffix (void);
void Ufail (void);
int Dfail (void);
void Usetexitcode (void);
void Uexit (void);
int Dsetexitcode (void);
int Dexit (void);
int Dgetenv (AFFIX A, AFFIX B);
void Ugetenv (void);
int x_errline (char *b, char *e);
int x_underline (char *b, char *e);
int x_errmsg (void);
int Derrormessage (AFFIX A_0);
void Uerrormessage (void);
int Dtransformlattice_ (AFFIX A, AFFIX B, AFFIX C);
int Dtransformlatticeterm_ (AFFIX A, AFFIX B, AFFIX C);
int Dexplintersect_ (AFFIX A, AFFIX B);
void Uexplintersect_ (void);
int Dintersect_ (AFFIX A, AFFIX B);
void Uintersect_ (void);
void Utransformlattice_ (void);
void Utransformlatticeterm_ (void);
void Utltraditional_ (void);
void Utltraditionalterm_ (void);
void afx_overflow (void);
void need_more_qstack (void);
void cstore_overflow (void);
void set_and_test_limits (int stk_size, int memo_flag);
int nt_name_push (char *nt_name);
int nt_name_ipop (int pushed );
char *nt_name_pop (int pushed );
int begin1_trace (char *ntname);
void begin2_trace (void);
void trace_affix (int builtin_affix, AFFIX afx);
void trace_lattice (AFFIX afx);
void int_trace_affix (int paren, int n, AFFIX afx);
int try_trace_list_affix (int paren, int n, AFFIX A);
int traceterm (char *term, int rc, char *pre, char *post);
long endtrace (int pct, char *pntsave, char *ntname);
void ambiguous_trace (int *pct, int *act, char *ntname);
void found (void);
void readinput (int sarg_count, char **sarguments, int read_input);
int errline (char *b, char *e);
int underline (char *b, char *e);
int errmsg (void);
void result (void);
int Dreport (AFFIX afx);
void Ureport (void);
void printa (FILE *outdir, AFFIX afx);
void sprinta (AFFIX afx);
long set_errmsg (char *ntname);
long afxcmp (char *x, char *y);
long lexafxcmp (char *x, char *y);
int Dterminal_ (char *sterm);
int Dchoice_ (char *choice_set, AFFIX D0);
int Dex_choice (char *choice_set, AFFIX D0);
int Dx_star_choice (char *choice_set, AFFIX D0);
int Dx_star_ex_choice (char *choice_set, AFFIX D0);
int Dx_plus_choice (char *choice_set, AFFIX D0);
int Dx_plus_ex_choice (char *choice_set, AFFIX D0);
void Uterminal_ (void);
void Uchoice_ (void);
void Uex_choice (void);
void Ux_star_ex_choice (void);
void Ux_star_choice (void);
void Ux_plus_ex_choice (void);
void Ux_plus_choice (void);
int Dintplusintisint(AFFIX A, AFFIX B, AFFIX C);
int Dintsubintisint(AFFIX A, AFFIX B, AFFIX C);
int Dinttimesintisint(AFFIX A, AFFIX B, AFFIX C);
int Dintdividedbyintisintwithremainderint(AFFIX A, AFFIX B, AFFIX C, AFFIX D);
int Dintisint(AFFIX A, AFFIX B, AFFIX C);
int Dintraisedbyintmodulointisint(AFFIX A, AFFIX B, AFFIX C, AFFIX D);
int Dintisthegcdofintandint(AFFIX C, AFFIX A, AFFIX B);
int Dintisthesquarerootofintwithremainderint(AFFIX C, AFFIX A, AFFIX B);
int Ddecisint(AFFIX A, AFFIX B);
int Dinitmint_(AFFIX A, AFFIX B);
int Dintisdec(AFFIX A, AFFIX B);
void inttodec(int64_t *A);
int Dfreeint(AFFIX A);
int Ddecrementsmallint(AFFIX A);
int Dincrementsmallint(AFFIX A);
int Dsmalldecisint(AFFIX A, AFFIX B);
int Dintisnegative(AFFIX A);
int Dnegateint(AFFIX A);
int Dintispositive(AFFIX A);
int Dintiszero(AFFIX A);
void Uintplusintisint (void);
void Uintsubintisint (void);
void Uinttimesintisint (void);
void Uintdividedbyintisintwithremainderint (void);
void Uintisint (void);
void Uintraisedbyintmodulointisint (void);
void Uintisthegcdofintandint (void);
void Uintisthesquarerootofintwithremainderint (void);
void Udecisint (void);
void Uintisdec (void);
void Ufreeint (void);
void Udecrementsmallint (void);
void Uincrementsmallint (void);
void Usmalldecisint (void);
void Uintisnegative (void);
void Unegateint (void);
void Uintispositive (void);
void Uintiszero (void);
void loop_line_mode (void (*rroot) (void));

#ifdef INPUT_MEMO_SIZE
int runtime_input_size = INPUT_MEMO_SIZE * 8 * sizeof(int);
#else 
int runtime_input_size =  200000;
#endif 
#define nil 0
#define MEMTEMPS int m1= (rip-input)>>5, m2= (0X1 << ((rip-input)&31))
#define SET(pr) (memo_table[pr][m1] & m2) != 0
#define CLEAR(pr) memo_table[pr][m1] &= ~m2
#define GM_ARG_SIZE  1000
char            in_file_name[GM_ARG_SIZE+1] = {""},
                out_file_name[GM_ARG_SIZE+1] = {""},
                current_file_name[GM_ARG_SIZE+1] = {""};
char            sv_error_msg[GM_ARG_SIZE*10+1];
char           *thischar;
FILE           *inputfile;
FILE           *output =0;
long int        interesting_level_number = -1,
                output_to_stdout = 0, input_from_stdin = 0;
int             set_line_num;
char            *set_line_pos; 
char            *set_file_name = "\0"; 
int             change_line_file = 0;
int             exit_code = 0;

char *ipstart, *ipend, *ipln;

char *argv[GM_ARG_SIZE+1];

long int             ll_mode = false,  /* loop line mode */  ll_count=0;
long int             abq_level = INT_MAX, report_stacksize = false;
char **stack_bottom;

struct eaglist {
   AFFIX   table;
   AFFIX   key;
   AFFIX   value;
   struct eaglist *left;
   struct eaglist *right;
   struct eaglist *next;
}       initial_l= {&fla,&fla,&fla,0,0,0},*root = &initial_l;

affix _dont_care  = {"!@!",0,0}, *A_9999999 = &_dont_care;

#define GET_CELL_ADDR(cell,str)\
{ register char *d = (char *) & cell;\
   register char *rc = str; \
   if ( (rc[0] == '\001') && (rc[9] == '\001') && (rc[10] == '\0')) {\
   d[0] = rc[1] ;d[1] = rc[2];d[2] = rc[3];d[3] = rc[4];\
   d[4] = rc[5] ;d[5] = rc[6];d[6] = rc[7];d[7] = rc[8];\
   } else cell = (AFFIX) 0;\
}
#define PUT_CELL_ADDR(cell)\
 { register char *d = (char *) & cell;\
   register char *rc = c;\
  if (rc +12 > cstore_top) cstore_overflow (); \
   rc[0] = '\001' ;rc[1] = d[0];rc[2] = d[1];\
rc[3] = d[2];rc[4] = d[3];rc[5] = d[4];rc[6] = d[5];\
rc[7] = d[6];rc[8] = d[7];rc[9] = '\001';rc[10] = '\0';c = rc+11;} 

static inline int WHERE(AFFIX A,AFFIX B)
{
  MAKE(B,A);
  return true;
}
static inline int UNPAIR(AFFIX A,AFFIX B,AFFIX C)
{
  AFFIX   cell = 0;
  char *xs = A->t;

  if (A->r || A->l) return Dunpair(A,B,C);
  GET_CELL_ADDR(cell,xs); 
  if (cell) {
    COPY(B , cell->l);
    COPY(C , cell->r);
    return true;
  }
  return false;
}
static inline int PAIR(AFFIX A, AFFIX C, AFFIX D) /* pair */ 
{
  AFFIX B = ++af;
  A->t = c;
  A->l = nil;
  A->r = nil;
  B->t = fast_list_acces;
  B->l = C;
  B->r = D;
  PUT_CELL_ADDR(B);
  return true;
}
