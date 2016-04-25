/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

*/
/*
 * file: builtins.c last modified: Fri May 18 09:57:36 MET DST 2012
 *
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#if defined LINUX || defined LINUX64
#include <string.h>
#endif
#include <string.h>

#define true 1
#define false 0


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

extern int      rcount, rmax, set_ip_start_num, cssize;
extern int      parsecount, nrofchars, level, eval_count, emsg_count,
                interesting_level_number, backtrace;
extern char     in_file_name[256], out_file_name[256], current_file_name[256];
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

extern char *argv[102];

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

#ifndef EIGHT_BYTE_ADDR
#define EIGHT_BYTE_ADDR
#endif
#ifdef EIGHT_BYTE_ADDR
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
   register char *rc = c;rc[0] = '\001' ;rc[1] = d[0];rc[2] = d[1];\
rc[3] = d[2];rc[4] = d[3];rc[5] = d[4];rc[6] = d[5];\
rc[7] = d[6];rc[8] = d[7];rc[9] = '\001';rc[10] = '\0';c = rc+11;} 
#else   /* assume 4 byte addressing */
#define GET_CELL_ADDR(cell,str)\
{ register char *d = (char *) & cell;\
   register char *rc = str;\
   if ( (rc[0] == '\001') && (rc[5] == '\001') && (rc[6] == '\000')) {\
   d[0] = rc[1] ;d[1] = rc[2];d[2] = rc[3];\
      d[3] = rc[4];\
   } else cell = (AFFIX ) 0;\
}
#define PUT_CELL_ADDR(cell)\
 { register char *d = (char *) & cell;\
   register char *rc = c;rc[0] = '\001' ;rc[1] = d[0];rc[2] = d[1];\
rc[3] = d[2];rc[4] = d[3];rc[5] = '\001';rc[6] = '\0';c = rc+7;} 
#endif /* eight byte addressing */                                

extern AFFIX   afx_top;
extern AFFIX   affix_heap;
extern cont *q_stack; 
extern int  runtime_input_size, abq_level;
extern int ll_mode, ll_count, output_to_stdout, input_from_stdin;
extern int afhsize, q_size;
extern int  report_stacksize;
extern char** stack_bottum;
extern char sv_error_msg[4000];
extern int exit_code;
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
