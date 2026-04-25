/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

*/
/*
 * file: builtins.c last modified: Fri May 18 09:57:36 MET DST 2012
 *
 */

#define _GNU_SOURCE

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#if defined LINUX || defined LINUX64
#include <string.h>
#endif
#include <string.h>

#include <unistd.h>

#define true 1
#define false 0

#define GM_ARG_SIZE 1000

#define CSTORE_INPUT_HEAP_SIZE 100000000
#define AFFIX_HEAP_SIZE         30000000
#define MIN_AFH_SIZE             1000000
#define MIN_CSI_SIZE             8000000
#define LONGBITS sizeof(long)*8

typedef struct affix_struct {
  char           *t;
  struct affix_struct  *l, *r;
} affix,*AFFIX;

extern AFFIX first_meta,last_meta; 

typedef union {
  void             (*q) ();
  int           *i;
  char           *s;
  char          **l;
  AFFIX   a;
}               cont;


extern affix fla;

extern struct eaglist {
  AFFIX   table;
  AFFIX   key;
  AFFIX   value;
  struct eaglist *left;
  struct eaglist *right;
  struct eaglist *next;
}       initial_l,*root;


extern cont     words[],*q_top;
extern char    *input;
extern char    *cstore,*cstore_top;

extern int     memorizing; 
extern char *fast_list_acces;
extern char    *mip, *set_ip_start_pos, *limitip, *undefined, *empty;

extern long int      rcount, rmax, set_ip_start_num, cssize;
extern long int      parsecount, nrofchars, level, eval_count, emsg_count,
                interesting_level_number, backtrace;
extern char     in_file_name[GM_ARG_SIZE], out_file_name[GM_ARG_SIZE], current_file_name[GM_ARG_SIZE];
extern char    *thischar;
extern char    *arg_zero;
extern FILE    *inputfile,*output;
extern int             set_line_num;
extern int             tabstop;
extern char            *set_line_pos;
extern char            *set_file_name ;
extern char            *inputbuff;
extern char            *pntname;
extern char            *lastpntname;
extern int             change_line_file;

extern char *argv[GM_ARG_SIZE+1];

extern cont    *q;

#if defined(__GNUC__) && defined(__sparc__)
register char *c asm ("g7");
register char *ip asm ("g6");
register affix *af asm ("g5");
#else
extern char *c, *ip;
extern affix *af;
#endif


#define nil NULL
#define ASSIGN(x,tr,lf,rt) (*(x)).t = tr ;(*(x)).l = lf;(*(x)).r = rt
#define COPY(x,y) (x)->t = (y)->t ; (x)->l = (y)->l ; (x)->r = (y)->r
#define SAVE(x,y) x.t = (y)->t ; x.l = (y)->l ; x.r = (y)->r
#define RESTORE(x,y) (x)->t = y.t;  (x)->l = y.l;  (x)->r = y.r
#define SHARE(x,y) (x)->t = empty ; (x)->l = nil ; (x)->r = y
#define MAKE(x,y) if ( (y)->t == undefined) { SHARE(x,y);} else { COPY (x,y); }
#define CONTINUE  q -=1;(*(*(q+1)).q)()

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
   if (rc + 12 > cstore_top) cstore_overflow (); \
   rc[0] = '\001' ;rc[1] = d[0];rc[2] = d[1];\
rc[3] = d[2];rc[4] = d[3];rc[5] = d[4];rc[6] = d[5];\
rc[7] = d[6];rc[8] = d[7];rc[9] = '\001';rc[10] = '\0';c = rc+11;} 

extern AFFIX   afx_top;
extern AFFIX   affix_heap;
extern cont *q_stack; 
extern long int  runtime_input_size, abq_level;
extern long int ll_mode, ll_count, output_to_stdout, input_from_stdin;
extern long int afhsize, q_size;
extern long int  report_stacksize;
extern char** stack_bottom;
extern char sv_error_msg[GM_ARG_SIZE*10];
extern long int exit_code;
extern char *ipstart, *ipend, *ipln;
int clear_memo_tab();

#define AFX_DONT_CARE A_9999999

extern AFFIX A_9999999;

#define GLAMMAR_Q1(action, continuation)\
void continuation ()\
{\
   register cont *rq = q;\
   register AFFIX A_0 = rq->a;\
   char *rc = c, *rip = ip;\
   if ( action (A_0)) {\
      q = rq -2;\
      (*(rq -1)->q)();\
      rq = q +1;\
   }\
   rq->a = A_0;\
   (rq + 1)->q = continuation;\
   q = rq + 1;\
   c = rc;\
   ip = rip;\
}

#define GLAMMAR_Q2(action, continuation)\
void continuation () \
{ \
   register cont *rq = q;\
register AFFIX A_2 = rq->a;\
register AFFIX A_1 = (rq -1)->a;\
   char *rc = c; \
   if ( action ( A_1, A_2)) {\
      q = rq -3;\
      (*(rq -2)->q)(); \
      rq = q +2;\
   }\
   (rq -1)->a = A_1;\
   rq->a = A_2;\
   (rq + 1)->q = continuation; \
   q = rq + 1; \
   c = rc;\
  if (rc > cstore_top ) cstore_overflow();\
}
#define GLAMMAR_Q3(action, continuation)\
void continuation () \
{ \
   register cont *rq = q;\
register AFFIX A_2 = rq->a;\
register AFFIX A_1 = (rq -1)->a;\
register AFFIX A_0 = (rq -2)->a;\
   char *rc = c; \
   if ( action (A_0, A_1, A_2)) {\
      q = rq -4;\
      (*(rq -3)->q)(); \
      rq = q +3;\
   }\
   (rq -2)->a = A_0;\
   (rq -1)->a = A_1;\
   rq->a = A_2;\
   (rq + 1)->q = continuation; \
   q = rq + 1; \
   c = rc;\
  if (rc > cstore_top ) cstore_overflow();\
}

void Uskip_ ();
void lkh_epiloque ();
void lkh_proloque ();
void Ucut ();
void Uidentifier ();
int Didentifier (AFFIX A_0);
void Uletterdigitsequence ();
int Dletterdigitsequence (AFFIX A_0);
void Unlcr ();
int Dnlcr ();
void Ubackslash ();
int Dbackslash ();
void Uquote ();
int Dquote ();
void Uendofsentence ();
int Dendofsentence ();
int Ddequal (AFFIX A, AFFIX B);
int Dequal (AFFIX A, AFFIX B);
int Dequalsempty (AFFIX A);
int Dnotequal (AFFIX A, AFFIX B);
int Ddfqual (AFFIX A, AFFIX B);
int Dfqual (AFFIX A, AFFIX B);
int Dfotequal (AFFIX A, AFFIX B);
int Dcompare (AFFIX A, AFFIX B, AFFIX tailx, AFFIX taily);
void Ulinenum ();
int Dlinenum (affix *D_0);
void Usetlinefile ();
void Ugetlinefile ();
int Dsetlinefile (AFFIX L, AFFIX F);
int Dgetlinefile (AFFIX L, AFFIX F);
int Dcharnum (AFFIX D_0);
int Dsettabstop (AFFIX F);
int Dsomename (AFFIX A_0);
void Uaddto ();
int Daddto (AFFIX A, AFFIX B, AFFIX C);
int Dvalueof (AFFIX A, AFFIX B, AFFIX C);
int Dincludes (AFFIX A, AFFIX B);
int Dexcludes (AFFIX A, AFFIX B);
int Dlookup (AFFIX A, AFFIX B, AFFIX C);
int Dcontentsof (AFFIX A, AFFIX B, AFFIX C, AFFIX D);
int Dsize (AFFIX A, AFFIX B);
void Udelete ();
void Uselect ();
int Dgetnextoption (AFFIX A_0);
void Ugetnextoption ();
int Dlayout (AFFIX A_0);
int Dwhitespace (AFFIX A_0);
void Ulayout ();
int Dlength (AFFIX A, AFFIX B);
void Ulength ();
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
long forced_close_symbol ();
int more_expression (signed long long *val);
int more_simple_expression (signed long long *val);
int more_term (signed long long *val);
long is_plus_symbol ();
long is_minus_symbol ();
int is_number (signed long long *val);
long is_open_symbol ();
long is_not_symbol ();
long is_close_symbol ();
long is_equal_symbol ();
long is_not_equal_symbol ();
long is_less_symbol ();
long is_less_equal_symbol ();
long is_greater_symbol ();
long is_greater_equal_symbol ();
long is_times_symbol ();
long is_divide_symbol ();
void Usetinputptrto_ ();
void Uresetinputptr_ ();
void Udetnestarset ();
void Unestaralt_ ();
void Unestarset ();
void Umetaterminal ();
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
void Urepair ();
int Dwhere (AFFIX A_0, AFFIX A_1);
int Devalmeta (AFFIX A_0);
void Uevalmeta ();
int Dinitmeta (AFFIX A_0, AFFIX A_1);
void Uinitmeta ();
int Dassign (AFFIX A_0, AFFIX A_1, AFFIX A_2);
void Uassign ();
int Dsetupconnectiontounix (AFFIX B, AFFIX C);
int Dtalktounix (AFFIX B, AFFIX A_0, AFFIX A_1);
int talkto (char *cmd);
int Dgetfromunix (AFFIX A_0, AFFIX A_1);
int Dwritefile (AFFIX A_0, AFFIX A_1);
int Dsystem (AFFIX A_0);
void Usetupconnectiontounix ();
void Utalktounix ();
void Ugetfromunix ();
void Uwritefile ();
int Dtoutc (AFFIX A_0, AFFIX A_1);
void Uequal ();
void Unotequal ();
void Ufqual ();
void Ufotequal ();
void Ucompare ();
void Ucontentsof ();
void Ucharnum ();
void Usettabstop ();
void Usomename ();
void Uvalueof ();
void Ulookup ();
void Uincludes ();
void Uexcludes ();
void Upair ();
void Uunpair ();
void Uwhere ();
void Ucompute ();
void Usize ();
void Urealtoint ();
void Udecimaltounary ();
void Ureverse ();
void Uuppercase ();
void Ulowercase ();
void Ukeyword ();
void Utypeout ();
void Utypein ();
int Dlexicalless (AFFIX A, AFFIX B);
int Dlexicalmore (AFFIX A, AFFIX B);
int Dlexicallessequal (AFFIX A, AFFIX B);
int Dlexicalmoreequal (AFFIX A, AFFIX B);
void Ulexicalless ();
void Ulexicalmore ();
void Ulexicallessequal ();
void Ulexicalmoreequal ();
int Devalaffix (AFFIX A, AFFIX B);
void Uevalaffix ();
void Ufail ();
int Dfail ();
void Usetexitcode ();
void Uexit ();
int Dsetexitcode ();
int Dexit ();
int Dgetenv (AFFIX A, AFFIX B);
void Ugetenv ();
int x_errline (char *b, char *e);
int x_underline (char *b, char *e);
int x_errmsg ();
int Derrormessage (AFFIX A_0);
void Uerrormessage ();
int Dtransformlattice_ (AFFIX A, AFFIX B, AFFIX C);
int Dtransformlatticeterm_ (AFFIX A, AFFIX B, AFFIX C);
int Dexplintersect_ (AFFIX A, AFFIX B);
void Uexplintersect_ ();
int Dintersect_ (AFFIX A, AFFIX B);
void Uintersect_ ();
void Utransformlattice_ ();
void Utransformlatticeterm_ ();
void Utltraditional_ ();
void Utltraditionalterm_ ();
void afx_overflow ();
void need_more_qstack ();
void cstore_overflow ();
void set_and_test_limits (int stk_size, int memo_flag);
int nt_name_push (char *nt_name);
int nt_name_ipop (int pushed );
char *nt_name_pop (int pushed );
int begin1_trace (char *ntname);
void begin2_trace (void);
void trace_affix (int builtin_affix, AFFIX afx);
void int_trace_affix (int paren, int n, AFFIX afx);
int try_trace_list_affix (int paren, int n, AFFIX A);
int traceterm (char *term, int rc, char *pre, char *post);
long endtrace (int pct, char *pntsave, char *ntname);
void ambiguous_trace (int *pct, int *act, char *ntname);
void found (void);
void readinput (int sarg_count, char **sarguments, int read_input);
int errline (char *b, char *e);
int underline (char *b, char *e);
int errmsg ();
void result ();
int Dreport (AFFIX A);
void Ureport ();
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
void Uterminal_ ();
void Uchoice_ ();
void Uex_choice ();
void Ux_star_ex_choice ();
void Ux_star_choice ();
void Ux_plus_ex_choice ();
void Ux_plus_choice ();
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
void Uintplusintisint ();
void Uintsubintisint ();
void Uinttimesintisint ();
void Uintdividedbyintisintwithremainderint ();
void Uintisint ();
void Uintraisedbyintmodulointisint ();
void Uintisthegcdofintandint ();
void Uintisthesquarerootofintwithremainderint ();
void Udecisint ();
void Uintisdec ();
void Ufreeint ();
void Udecrementsmallint ();
void Uincrementsmallint ();
void Usmalldecisint ();
void Uintisnegative ();
void Unegateint ();
void Uintispositive ();
void Uintiszero ();
