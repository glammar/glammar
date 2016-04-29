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
 int memo_table[MEMOIZE][INPUT_MEMO_SIZE];
int clear_memo_tab () {
   int i,j; 
    for (i = 0; i< MEMOIZE; i++) 
     for (j = 0; j< INPUT_MEMO_SIZE;) 
        memo_table [i] [j++] = -1;
   return 1;
} 
                        
#else 
int memorizing = 0;
int clear_memo_tab () { return 0;}
#endif
extern void found();


#define nil 0
#define true 1
#define false 0


void Uterminal_();
void Uchoice_();
void Uex_choice();
void Ux_star_ex_choice();
void Ux_star_choice();
void Ux_plus_ex_choice();
void Ux_plus_choice();
int Dterminal_();
int Dchoice_();
int Dex_choice();
int Dx_star_ex_choice();
int Dx_star_choice();
int Dx_plus_ex_choice();
int Dx_plus_choice();

void lkh_epiloque();
void lkh_proloque();

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

static inline int WHERE(A,B)
AFFIX A,B;
{
  MAKE(B,A);
  return true;
}
static inline int UNPAIR(A,B,C)
AFFIX A,B,C;
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
static inline int PAIR(A, C, D) /* pair */ 
register AFFIX A, C, D;
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
