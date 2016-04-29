/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

*/
typedef  
       union { void (*q)() ; int *i;char *s,**l; AFFIX a; } cont;


#define COPY(x,y) (x)->t = (y)->t ; (x)->l = (y)->l ; (x)->r = (y)->r
#define SHARE(x,y) (x)->t = empty ; (x)->l = nil ; (x)->r = y 
#define MAKE(x,y) if ( (y)->t == undefined) { SHARE(x,y);} else { COPY (x,y); }

#if defined(__GNUC__) && defined(__sparc__)
register char *c asm ("g7");       
register char *ip asm ("g6");       
register affix *af asm ("g5");                                                 
#else                                                                          
extern char *c, *ip;
extern affix *af;
#endif                                                                         

extern cont *q,*q_stack,*q_top;
extern int memorizing;
extern char *input,*cstore,*cstore_top, *mip,*limitip, *empty , 
             *undefined, *arg_zero, *pntname; 

extern char *ipstart, *ipend, *ipln;
extern AFFIX affix_heap,afx_top;
 
extern  long int rcount, rmax,parsecount,nrofchars,level,eval_count, q_size;
#ifdef MEMOIZE
extern  int memo_table[MEMOIZE][INPUT_MEMO_SIZE];
#endif
#define nil 0
#define true 1
#define false 0

void lkh_epiloque();
void lkh_proloque();


extern void Uterminal_();
extern void Uchoice_();
extern void Uex_choice();
extern void Ux_star_ex_choice();
extern void Ux_star_choice();
extern void Ux_plus_ex_choice();
extern void Ux_plus_choice();
extern int Dterminal_();
extern int Dchoice_();
extern int Dex_choice();
extern int Dx_star_ex_choice();
extern int Dx_star_choice();
extern int Dx_plus_ex_choice();
extern int Dx_plus_choice();

#define nil 0
#define MEMTEMPS int m1= (rip-input)>>5, m2= (0X1 << ((rip-input)&31))
#define SET(pr) (memo_table[pr][m1] & m2) != 0
#define CLEAR(pr) memo_table[pr][m1] &= ~m2

extern AFFIX A_9999999;
#define GET_CELL_ADDR(cell,str)\
{ register char *d = (char *) & cell;\
   register char *rc = str; \
   if ( (rc[0] == '\001') && (rc[9] == '\001') && (rc[10] == '\0')) {\
   d[0] = rc[1] ;d[1] = rc[2];d[2] = rc[3];d[3] = rc[4];\
   d[4] = rc[5] ;d[5] = rc[6];d[6] = rc[7];d[7] = rc[8];\
   } else cell = (AFFIX) 0;\
}

static inline UNPAIR(A,B,C)
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
