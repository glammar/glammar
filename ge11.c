
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef UEQUAL

void Uequal ()
{                               /* equal */
  register AFFIX B = q->a;
  register AFFIX A = (q - 1)->a;
  affix x, y;
  char *csave = c;
  register char *xs = c, *ys;
  q -= 2;
  SAVE (x, A);
  SAVE (y, B);
  if (A == B)
  {
    CONTINUE;
  }
  else
  {
    if ((x.r == nil) && (x.l == nil))
      xs = x.t;
    else
    {
      sprinta (A);
      A->t = xs;
      A->l = nil;
      A->r = nil;
      *c++ = '\0';
    }
    ys = c;
    if ((y.r == nil) && (y.l == nil))
      ys = y.t;
    else
    {
      sprinta (B);
      B->t = ys;
      B->l = nil;
      B->r = nil;
      *c++ = '\0';
    }
    if (afxcmp (xs, ys) == 0)
    {
      CONTINUE;
    }
  }
  c = csave;
  RESTORE (A, x);
  RESTORE (B, y);
  (q + 1)->a = A;
  (q + 2)->a = B;
  (q + 3)->q = Uequal;
  q += 3;
}
#endif

#ifdef UNOTEQUAL
void Unotequal ()
{                               /* notequal */

  register AFFIX B = (q)->a;
  register AFFIX A = (q - 1)->a;
  affix x, y;
  char *csave = c;
  register char *xs = c, *ys;
  q -= 2;
  SAVE (x, A);
  SAVE (y, B);
  if (A == B)
    goto done;
  if ((x.r == nil) && (x.l == nil))
    xs = x.t;
  else
  {
    sprinta (A);
    A->t = xs;
    A->l = nil;
    A->r = nil;
    *c++ = '\0';
  }
  ys = c;
  if ((y.r == nil) && (y.l == nil))
    ys = y.t;
  else
  {
    sprinta (B);
    B->t = ys;
    B->l = nil;
    B->r = nil;
    *c++ = '\0';
  }
  if (afxcmp (xs, ys) == 0)
    goto done;
go:
  CONTINUE;
done:
  RESTORE (A, x);
  RESTORE (B, y);
  (q + 1)->a = A;
  (q + 2)->a = B;
  (q + 3)->q = Unotequal;
  q += 3;
  c = csave;
}
#endif

#ifdef UFQUAL
void Ufqual ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Ddfqual (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ufqual;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UFOTEQUAL
void Ufotequal ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (!Ddfqual (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ufotequal;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UCOMPARE
void Ucompare ()
{
  register cont *rq = q;
  register AFFIX A_3 = rq->a;
  register AFFIX A_2 = (rq - 1)->a;
  register AFFIX A_1 = (rq - 2)->a;
  register AFFIX A_0 = (rq - 3)->a;
  char *rc = c;
  if (Dcompare (A_0, A_1, A_2, A_3))
  {
    q = rq - 5;
    (*(rq - 4)->q) ();
    rq = q + 4;
  }
  (rq - 3)->a = A_0;
  (rq - 2)->a = A_1;
  (rq - 1)->a = A_2;
  rq->a = A_3;
  (rq + 1)->q = Ucompare;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UCONTENTSOF
void Ucontentsof ()
{
  register cont *rq = q;
  register AFFIX A_3 = rq->a;
  register AFFIX A_2 = (rq - 1)->a;
  register AFFIX A_1 = (rq - 2)->a;
  register AFFIX A_0 = (rq - 3)->a;
  char *rc = c;
  if (Dcontentsof (A_0, A_1, A_2, A_3))
  {
    q = rq - 5;
    (*(rq - 4)->q) ();
    rq = q + 4;
  }
  (rq - 3)->a = A_0;
  (rq - 2)->a = A_1;
  (rq - 1)->a = A_2;
  rq->a = A_3;
  (rq + 1)->q = Ucontentsof;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UCHARNUM
void Ucharnum ()
{
  register cont *rq = q;
  register AFFIX A_0 = rq->a;
  char *rc = c;
  if (Dcharnum (A_0))
  {
    q = rq - 2;
    (*(rq - 1)->q) ();
    rq = q + 1;
  }
  rq->a = A_0;
  (rq + 1)->q = Ucharnum;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef USETTABSTOP
void Usettabstop ()
{
  register cont *rq = q;
  register AFFIX A_0 = rq->a;
  char *rc = c;
  int tabstop_save = tabstop;
  if (Dsettabstop (A_0))
  {
    q = rq - 2;
    (*(rq - 1)->q) ();
    rq = q + 1;
  }
  rq->a = A_0;
  (rq + 1)->q = Usettabstop;
  q = rq + 1;
  c = rc;
  tabstop = tabstop_save;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef USOMENAME
void Usomename ()
{
  register cont *rq = q;
  register AFFIX A_0 = rq->a;
  char *rc = c;
  if (Dsomename (A_0))
  {
    q = rq - 2;
    (*(rq - 1)->q) ();
    rq = q + 1;
  }
  rq->a = A_0;
  (rq + 1)->q = Usomename;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif
