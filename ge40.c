/*

    This file is a part of the GLAMMAR source Distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

/*
 * file: mp.c last modified: Fri May 18 09:57:36 MET DST 2012
 *
 */

#include <mp.h>
#include <stdio.h>
#include <ctype.h>

#define true 1
#define false 0

int64_t * dectoint ();

#define MAX_SHORT 32767
#define MIN_SHORT -32768

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


extern cont     words[],*q_top;
extern char    *input;
extern char    *cstore,*cstore_top;

extern cont    *q;
extern char *fast_list_acces;
extern char    *ip, *mip, *set_ip_start_pos, *limitip, *undefined, *empty, *c;

extern int      rcount, rmax, set_ip_start_num, cssize;
extern int      parsecount, nrofchars, level, eval_count, 
                interesting_level_number, backtrace;
extern char     in_file_name[256], out_file_name[256], current_file_name[256];
extern char    *thischar;
extern FILE    *inputfile,*output;
extern int             set_line_num;
extern char            *set_line_pos;
extern char            *set_file_name ;
extern char            *inputbuff;
extern int             change_line_file;


extern char *argv[11];

extern affix *af;

#define nil NULL
#define ASSIGN(x,tr,lf,rt) (*(x)).t = tr ;(*(x)).l = lf;(*(x)).r = rt
#define COPY(x,y) (x)->t = (y)->t ; (x)->l = (y)->l ; (x)->r = (y)->r
#define SAVE(x,y) x.t = (y)->t ; x.l = (y)->l ; x.r = (y)->r
#define RESTORE(x,y) (x)->t = y.t;  (x)->l = y.l;  (x)->r = y.r
#define SHARE(x,y) (x)->t = empty ; (x)->l = nil ; (x)->r = y
#define MAKE(x,y) if ( (y)->t == undefined) { SHARE(x,y);} else { COPY (x,y); }
#define CONTINUE  q -=1;(*(*(q+1)).q)()



#define GET_GINT(mint,str,r)\
{ register char *d = (char *) & mint;\
   register char *rc = str; \
   if ( (rc[0] == '\002') && (rc[9] == '\002') && (rc[10] == '\000')) {\
   d[0] = rc [1] ; d[1] = rc [2]; d [2] = rc [3];\
   d[3] = rc [4] ; d[4] = rc [5]; d [5] = rc [6];\
   d[6] = rc [7] ; d[7] = rc [8]; r = 1;\
   } else r = 0;\
}
#define PUT_GINT(mint)\
{ register char *d = (char *) & mint;\
   register char *rc = c; rc[0] = '\002' ;\
   rc[1] = d[0] ;\
   rc [2] = d[1]; rc [3] = d[2]; rc [4] =  d[3] ;\
   rc[5] = d[4] ;\
   rc [6] = d[5]; rc [7] = d[6]; rc [8] =  d[7] ;\
   rc [9] = '\002'; rc [10] = '\0'; c = rc+11;} 

#ifdef DMP

int64_t  szero = 0;

/*  intplusintisint   */
int Dintplusintisint(A,B,C)
register AFFIX A;
register AFFIX B;
register AFFIX C;
{
  char           *xs = c, *csave = c;
  int r=0;
  int64_t m1,m2,m3;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs,r);
  if ( !r)  return false;
  c = csave;
  if ((B->r == nil) && (B->l == nil)) xs = B->t;
  else {
    sprinta(B);
    *c++ = '\0';
  }
  if ( !r)  return false;
  c = csave;
  m3 = m1+m2;
  C->t = c;
  C->l = nil;
  C->r = nil;
  PUT_GINT(m3);
  return true;

}

/*  intsubintisint   */
int Dintsubintisint(A,B,C)
register AFFIX A;
register AFFIX B;
register AFFIX C;
{
  char           *xs = c, *csave = c;
  int64_t   m1,m2,m3;
  int r=0;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs,r);
  if ( (int) m1 == -1)  return false;
  c = csave;
  if ((B->r == nil) && (B->l == nil)) xs = B->t;
  else {
    sprinta(B);
    *c++ = '\0';
  }

  GET_GINT(m2,xs);
  if ( (int) m2 == -1)  return false;
  c = csave;
  m3 = itom(0);
  msub (m1,m2,m3);
  C->t = c;
  C->l = nil;
  C->r = nil;
  PUT_GINT(m3);
  return true;

}



/*  inttimesintisint   */
int Dinttimesintisint(A,B,C)
register AFFIX A;
register AFFIX B;
register AFFIX C;
{
  char           *xs = c, *csave = c;
  int r=0;
  int64_t   *m1,*m2,*m3;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = csave;
  if ((B->r == nil) && (B->l == nil)) xs = B->t;
  else {
    sprinta(B);
    *c++ = '\0';
  }
  GET_GINT(m2,xs);
  if ( (int) m2 == -1)  return false;
  c = csave;
  m3 = itom(0);
  mult (m1,m2,m3);
  C->t = c;
  C->l = nil;
  C->r = nil;
  PUT_GINT(m3);
  return true;

}


/*  intdividedbyintisintwithremainderint   */
int Dintdividedbyintisintwithremainderint(A,B,C,D)
register AFFIX A;
register AFFIX B;
register AFFIX C;
register AFFIX D;
{
  char           *xs = c, *csave = c;
  int64_t   *m1,*m2,*m3,*m4;
  int r=0;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = csave;
  if ((B->r == nil) && (B->l == nil)) xs = B->t;
  else {
    sprinta(B);
    *c++ = '\0';
  }
  GET_GINT(m2,xs);
  if ( (int) m2 == -1)  return false;
  c = csave;
  m3 = itom(0);
  m4 = itom(0);
  mdiv (m1,m2,m3,m4);
  C->t = c;
  C->l = nil;
  C->r = nil;
  PUT_GINT(m3);
  D->t = c;
  D->l = nil;
  D->r = nil;
  PUT_GINT(m4);
  return true;
}


/*  intisint   */
int Dintisint(A,B,C)
register AFFIX A;
register AFFIX B;
register AFFIX C;
{
  char           *xs = c, *csave = c;
  int64_t   *m1,*m2;
  int r=0;
  int i;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = csave;
  if ((C->r == nil) && (C->l == nil)) xs = C->t;
  else {
    sprinta(C);
    *c++ = '\0';
  }
  GET_GINT(m2,xs);
  if ( (int) m2 == -1)  return false;
  c = csave;
  i = mcmp (m1,m2);
  if (i == 0)
     B->t = "0";
  else if (i < 0)
     B->t = "-1";
  else 
     B->t = "1";
  B->l = nil;
  B->r = nil;
  return true;

}

/*  intraisedbyintmodulointisint   */
int Dintraisedbyintmodulointisint(A,B,C,D)
register AFFIX A;
register AFFIX B;
register AFFIX C;
register AFFIX D;
{
  char           *xs = c, *csave = c;
  int64_t   *m1,*m2,*m3,*m4;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = csave;
  if ((B->r == nil) && (B->l == nil)) xs = B->t;
  else {
    sprinta(B);
    *c++ = '\0';
  }
  GET_GINT(m2,xs);
  if ( (int) m2 == -1)  return false;
  c = csave;
  if ((C->r == nil) && (C->l == nil)) xs = C->t;
  else {
    sprinta(C);
    *c++ = '\0';
  }
  GET_GINT(m3,xs);
  if ( (int) m3 == -1)  return false;
  c = csave;
  m4 = itom(0);
  pow (m1,m2,m3,m4);
  D->t = c;
  D->l = nil;
  D->r = nil;
  PUT_GINT(m4);
  return true;
}


/*  intisthegcdofintandint   */
int Dintisthegcdofintandint(C,A,B)
register AFFIX C;
register AFFIX A;
register AFFIX B;
{
  char           *xs = c, *csave = c;
  int64_t   *m1,*m2,*m3;
  int r=0;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = csave;
  if ((B->r == nil) && (B->l == nil)) xs = B->t;
  else {
    sprinta(B);
    *c++ = '\0';
  }
  GET_GINT(m2,xs);
  if ( (int) m2 == -1)  return false;
  c = csave;
  m3 = itom(0);
  gcd (m1,m2,m3);
  C->t = c;
  C->l = nil;
  C->r = nil;
  PUT_GINT(m3);
  return true;

}

/*  intisthesquarerootofintwithremainderint   */
int Dintisthesquarerootofintwithremainderint(C,A,B)
register AFFIX C;
register AFFIX A;
register AFFIX B;
{
 
  char           *xs = c, *csave = c;
  int64_t   *m1,*m2,*m3;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = csave;
  m2 = itom(0);
  m3 = itom(0);
  msqrt (m1,m2,m3);
  B->t = c;
  B->l = nil;
  B->r = nil;
  PUT_GINT(m2);
  C->t = c;
  C->l = nil;
  C->r = nil;
  PUT_GINT(m3);
  return true;

}

int Ddecisint(A,B)
register AFFIX A, B;
{
   int64_t *M ;
   register char *xs =c, *csave =c;
   if ((A->r == nil) && (A->l == nil)) xs = A->t;
   else {
      sprinta(A);
      *c++ = '\0';
   }
   M = dectoint(xs);
   c = csave;
   B->t = c;
   B->l = nil;
   B->r = nil;
   PUT_GINT(M); 
   return true;
}

int Dinitmint_(A,B)
register AFFIX A, B;
{
   int64_t *M ;
   register char *xs =c, *csave =c;
   if ((B->r == nil) && (B->l == nil)) xs = B->t;
   else {
      sprinta(B);
      *c++ = '\0';
   }
   M = dectoint(xs);
   c = csave;
   A->t = c;
   A->l = nil;
   A->r = nil;
   PUT_GINT(M); 
   return true;
}
/*  intisdec   */
int Dintisdec(A,B)
register AFFIX A, B;
{
   register char           *xs = c, *rc =c;
   int64_t   *m1;
   if ((A->r == nil) && (A->l == nil)) xs = A->t;
   else {
      sprinta(A);
      *c++ = '\0';
   }
   B ->t = rc;
   B ->r = nil;
   B ->l = nil;
   if (*xs == '\002') { 
       int64_t *cpy =  itom(0), *dmy = itom(0);
       
       GET_GINT(m1,xs);
       madd (m1,dmy,cpy);
      inttodec(cpy);
      mfree(cpy);
      *c++ = '\0';
      return true;
   }
   return false;
}


#define Base 10000
inttodec(A)
int64_t *A;
{
   int len = A -> len;
   short *val = A -> val;
   int64_t *R;
   if (len < 0) {
      len = -len;
      A -> len = len;
      *c++ = '-';
   }
   if (len == 1) {
      register char * rc =c;
      sprintf(rc,"%d",(int) *val);
      while (*++rc !=  '\0') ;
      c = rc;
      return;
   }
   else if (len == 0) {
      *c++ =  '0';
      *c =  '\0';
      return;
   }
   R = itom(0);
   sdiv (A,Base,R,val);
   inttodec(R);
   mfree(R);
   {
      short v = *val;
      register char * rc =c;
      if (v < 1000)
         *rc++ = '0';
      if (v < 100)
         *rc++ = '0';
      if (v < 10)
         *rc++ = '0';
      sprintf(rc,"%d", (int) v);
      while (*++rc !=  '\0') ;
      c = rc;
      return;
   }
}

static int64_t * Mint_10, *R; 

int first_time_dec_to_mint =1;

int64_t * dectoint (str)
register  char *str;
{
  int64_t * bignum;
  int negate;
  if (first_time_dec_to_mint) {
      Mint_10 = itom(10);
      R  = itom(0);
      first_time_dec_to_mint =0;
  }
  if (str[0] == '-') {
    str++;
    negate = 1;

  } else {
    negate = 0;
  }

  bignum = itom(0);
  while (*str) {
    if (*str < '0' || *str > '9')
      return bignum;
    mult(Mint_10,bignum,R);
    madd(R , itom(*str++ - '0'),bignum);
  }
  if (negate) bignum  -> len = - (bignum -> len);
  return (bignum);
}


/*  freeint   */
int Dfreeint(A)
register AFFIX A;
{
  register char *rc = c,  *xs = rc;
  int64_t   *m1;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 != -1)  mfree(m1);
  c = rc;
  A->t = rc;
  A->l = nil;
  A->r = nil;
  PUT_GINT(mzero);
  return true; 
}


/*  Decrementint   */
int Ddecrementsmallint(A)
register AFFIX A;
{
  register char *rc = c,  *xs = rc;
  int64_t   *m1;
  short *val;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  
  val = m1 -> val; 
  if (m1 -> len  == 1)  {
       if (*val == 0)  {
            *val = 1;
            m1 -> len = -1;
       }
       else  
          *val -= 1; 
  } else if (m1 -> len  == -1)  {
          *val += 1; 
  } else return false;
  
  c = rc;
  return true; 
}

   
/*  incrementint   */
int Dincrementsmallint(A)
register AFFIX A;
{
  register char *rc = c,  *xs = rc;
  int64_t   *m1;
  short *val;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
 
  val = m1 -> val; 
  if (m1 -> len  == 1)  {
       if (*val == 0)  {
            *val = 1;
            m1 -> len = 1;
       }
       else  
          *val += 1; 
  } else if (m1 -> len  == -1)  {
       if (*val == 0)  {
            *val = 1;
            m1 -> len = 1;
       }
       else  
          m1 -> val -= 1; 
  } else return false;
  
  c = rc;
  return true; 
}

   

/*  smalldecisint   */
int Dsmalldecisint(A,B)
register AFFIX A;
register AFFIX B;
{
  register char *rc = c,  *xs = rc;
  int64_t   *m1;
  int i;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  c = rc;
  i = atoi(xs);
  if (i > MAX_SHORT)
        return false;
  if  (i < MIN_SHORT) 
        return false;
  sprintf(c,"%d",i);
  c += 6;
  m1 = itom((short) i);
  B -> t  = c;
  B -> r = nil;
  B -> l = nil;
  PUT_GINT(m1);
  return true;
}

/*  intisnegative   */
int Dintisnegative(A)
register AFFIX A;
{
  register char *rc = c,  *xs = rc;
  int64_t   *m1;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = rc;
  return (m1->len <0);
}


/*  negateint   */
int Dnegateint(A)
register AFFIX A;
{
  register char *rc = c,  *xs = rc;
  int64_t   *m1;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = rc;
  m1->len = -m1->len;
  return true;
}



/*  intispositive   */
int Dintispositive(A)
register AFFIX A;
{
  register char *rc = c,  *xs = rc;
  int64_t   m1;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = rc;
  return (m1->len >0);
}



/*  intiszero   */
int Dintiszero(A)
register AFFIX A;
{
  register char *rc = c,  *xs = rc;
  int64_t   m1;
  if ((A->r == nil) && (A->l == nil)) xs = A->t;
  else {
    sprinta(A);
    *c++ = '\0';
  }
  GET_GINT(m1,xs);
  if ( (int) m1 == -1)  return false;
  c = rc;
  return (m1->len == 1 && *(m1->val) == 0);
}
#endif

#ifdef UMP
void Uintplusintisint () {
   register cont *rq = q;
register AFFIX C = (rq+0)-> a;
register AFFIX B = (rq+-1)-> a;
register AFFIX A = (rq+-2)-> a;
   if (Dintplusintisint(A,B,C))
   {
      q=rq+-4;
      (*(rq+-3)->q)();
      rq=q- -3;
   }
   (rq+-2)->a = A;
   (rq+-1)->a = B;
   (rq+0)->a = C;
   (rq+1)-> q = Uintplusintisint;
   q = rq+1;
}

void Uintsubintisint () {
   register cont *rq = q;
register AFFIX C = (rq+0)-> a;
register AFFIX B = (rq+-1)-> a;
register AFFIX A = (rq+-2)-> a;
   if (Dintsubintisint(A,B,C))
   {
      q=rq+-4;
      (*(rq+-3)->q)();
      rq=q- -3;
   }
   (rq+-2)->a = A;
   (rq+-1)->a = B;
   (rq+0)->a = C;
   (rq+1)-> q = Uintsubintisint;
   q = rq+1;
}

void Uinttimesintisint () {
   register cont *rq = q;
register AFFIX C = (rq+0)-> a;
register AFFIX B = (rq+-1)-> a;
register AFFIX A = (rq+-2)-> a;
   if (Dinttimesintisint(A,B,C))
   {
      q=rq+-4;
      (*(rq+-3)->q)();
      rq=q- -3;
   }
   (rq+-2)->a = A;
   (rq+-1)->a = B;
   (rq+0)->a = C;
   (rq+1)-> q = Uinttimesintisint;
   q = rq+1;
}

void Uintdividedbyintisintwithremainderint () {
   register cont *rq = q;
register AFFIX D = (rq+0)-> a;
register AFFIX C = (rq+-1)-> a;
register AFFIX B = (rq+-2)-> a;
register AFFIX A = (rq+-3)-> a;
   if (Dintdividedbyintisintwithremainderint(A,B,C,D))
   {
      q=rq+-5;
      (*(rq+-4)->q)();
      rq=q- -4;
   }
   (rq+-3)->a = A;
   (rq+-2)->a = B;
   (rq+-1)->a = C;
   (rq+0)->a = D;
   (rq+1)-> q = Uintdividedbyintisintwithremainderint;
   q = rq+1;
}

void Uintisint () {
   register cont *rq = q;
register AFFIX C = (rq+0)-> a;
register AFFIX B = (rq+-1)-> a;
register AFFIX A = (rq+-2)-> a;
   if (Dintisint(A,B,C))
   {
      q=rq+-4;
      (*(rq+-3)->q)();
      rq=q- -3;
   }
   (rq+-2)->a = A;
   (rq+-1)->a = B;
   (rq+0)->a = C;
   (rq+1)-> q = Uintisint;
   q = rq+1;
}

void Uintraisedbyintmodulointisint () {
   register cont *rq = q;
register AFFIX D = (rq+0)-> a;
register AFFIX C = (rq+-1)-> a;
register AFFIX B = (rq+-2)-> a;
register AFFIX A = (rq+-3)-> a;
   if (Dintraisedbyintmodulointisint(A,B,C,D))
   {
      q=rq+-5;
      (*(rq+-4)->q)();
      rq=q- -4;
   }
   (rq+-3)->a = A;
   (rq+-2)->a = B;
   (rq+-1)->a = C;
   (rq+0)->a = D;
   (rq+1)-> q = Uintraisedbyintmodulointisint;
   q = rq+1;
}

void Uintisthegcdofintandint () {
   register cont *rq = q;
register AFFIX C = (rq+0)-> a;
register AFFIX B = (rq+-1)-> a;
register AFFIX A = (rq+-2)-> a;
   if (Dintisthegcdofintandint(A,B,C))
   {
      q=rq+-4;
      (*(rq+-3)->q)();
      rq=q- -3;
   }
   (rq+-2)->a = A;
   (rq+-1)->a = B;
   (rq+0)->a = C;
   (rq+1)-> q = Uintisthegcdofintandint;
   q = rq+1;
}


void Uintisthesquarerootofintwithremainderint() {
   register cont *rq = q;
register AFFIX C = (rq+0)-> a;
register AFFIX B = (rq+-1)-> a;
register AFFIX A = (rq+-2)-> a;
   if (Dintisthesquarerootofintwithremainderint(A,B,C))
   {
      q=rq+-4;
      (*(rq+-3)->q)();
      rq=q- -3;
   }
   (rq+-2)->a = A;
   (rq+-1)->a = B;
   (rq+0)->a = C;
   (rq+1)-> q = Uintisthesquarerootofintwithremainderint;
   q = rq+1;
}

void Udecisint () {
   register cont *rq = q;
register AFFIX B = (rq+0)-> a;
register AFFIX A = (rq+-1)-> a;
   if (Ddecisint(A,B))
   {
      q=rq+-3;
      (*(rq+-2)->q)();
      rq=q- -2;
   }
   (rq+-1)->a = A;
   (rq+0)->a = B;
   (rq+1)-> q = Udecisint;
   q = rq+1;
}
void Uintisdec () {
   register cont *rq = q;
register AFFIX B = (rq+0)-> a;
register AFFIX A = (rq+-1)-> a;
   if (Dintisdec(A,B))
   {
      q=rq+-3;
      (*(rq+-2)->q)();
      rq=q- -2;
   }
   (rq+-1)->a = A;
   (rq+0)->a = B;
   (rq+1)-> q = Uintisdec;
   q = rq+1;
}

void Ufreeint () { 
   register cont *rq = q;
register AFFIX A = (rq+0)-> a;
   if (Dfreeint(A))
   { 
      q=rq+-2;
      (*(rq+-1)->q)();
      rq=q- -1;
   }
   (rq+0)->a = A;
   (rq+1)-> q = Ufreeint; 
   q = rq+1; 
}

void Udecrementsmallint () { 
   register cont *rq = q;
register AFFIX A = (rq+0)-> a;
   if (Ddecrementsmallint(A))
   { 
      q=rq+-2;
      (*(rq+-1)->q)();
      rq=q- -1;
   }
   (rq+0)->a = A;
   (rq+1)-> q = Udecrementsmallint; 
   q = rq+1; 
}

void Uincrementsmallint () { 
   register cont *rq = q;
register AFFIX A = (rq+0)-> a;
   if (Dincrementsmallint(A))
   { 
      q=rq+-2;
      (*(rq+-1)->q)();
      rq=q- -1;
   }
   (rq+0)->a = A;
   (rq+1)-> q = Uincrementsmallint; 
   q = rq+1; 
}

void Usmalldecisint () { 
   register cont *rq = q;
register AFFIX B = (rq+0)-> a;
register AFFIX A = (rq+-1)-> a;
   if (Dsmalldecisint(A,B))
   { 
      q=rq+-3;
      (*(rq+-2)->q)();
      rq=q- -2;
   }
   (rq+-1)->a = A;
   (rq+0)->a = B;
   (rq+1)-> q = Usmalldecisint; 
   q = rq+1; 
}

void Uintisnegative () { 
   register cont *rq = q;
register AFFIX A = (rq+0)-> a;
   if (Dintisnegative(A))
   { 
      q=rq+-2;
      (*(rq+-1)->q)();
      rq=q- -1;
   }
   (rq+0)->a = A;
   (rq+1)-> q = Uintisnegative; 
   q = rq+1; 
}

void Unegateint () { 
   register cont *rq = q;
register AFFIX A = (rq+0)-> a;
   if (Dnegateint(A))
   { 
      q=rq+-2;
      (*(rq+-1)->q)();
      rq=q- -1;
   }
   (rq+0)->a = A;
   (rq+1)-> q = Unegateint; 
   q = rq+1; 
}

void Uintispositive () { 
   register cont *rq = q;
register AFFIX A = (rq+0)-> a;
   if (Dintispositive(A))
   { 
      q=rq+-2;
      (*(rq+-1)->q)();
      rq=q- -1;
   }
   (rq+0)->a = A;
   (rq+1)-> q = Uintispositive; 
   q = rq+1; 
}

void Uintiszero () { 
   register cont *rq = q;
register AFFIX A = (rq+0)-> a;
   if (Dintiszero(A))
   { 
      q=rq+-2;
      (*(rq+-1)->q)();
      rq=q- -1;
   }
   (rq+0)->a = A;
   (rq+1)-> q = Uintiszero; 
   q = rq+1; 
}
#endif
