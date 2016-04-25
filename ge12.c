
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef UVALUEOF
void Uvalueof ()
{
  register cont *rq = q;
  register AFFIX A_2 = rq->a;
  register AFFIX A_1 = (rq - 1)->a;
  register AFFIX A_0 = (rq - 2)->a;
  char *rc = c;
  if (Dvalueof (A_0, A_1, A_2))
  {
    q = rq - 4;
    (*(rq - 3)->q) ();
    rq = q + 3;
  }
  (rq - 2)->a = A_0;
  (rq - 1)->a = A_1;
  rq->a = A_2;
  (rq + 1)->q = Uvalueof;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef ULOOKUP
void Ulookup ()
{
  register cont *rq = q;
  register AFFIX A_2 = rq->a;
  register AFFIX A_1 = (rq - 1)->a;
  register AFFIX A_0 = (rq - 2)->a;
  char *rc = c;
  if (Dlookup (A_0, A_1, A_2))
  {
    q = rq - 4;
    (*(rq - 3)->q) ();
    rq = q + 3;
  }
  (rq - 2)->a = A_0;
  (rq - 1)->a = A_1;
  rq->a = A_2;
  (rq + 1)->q = Ulookup;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UINCLUDES
void Uincludes ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dincludes (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Uincludes;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UEXCLUDES
void Uexcludes ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (!Dincludes (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Uexcludes;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UPAIR
void Upair ()
{
  register cont *rq = q;
  register AFFIX A_2 = rq->a;
  register AFFIX A_1 = (rq - 1)->a;
  register AFFIX A_0 = (rq - 2)->a;
  char *rc = c;
  if (Dpair (A_0, A_1, A_2))
  {
    q = rq - 4;
    (*(rq - 3)->q) ();
    rq = q + 3;
  }
  (rq - 2)->a = A_0;
  (rq - 1)->a = A_1;
  rq->a = A_2;
  (rq + 1)->q = Upair;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UUNPAIR
void Uunpair ()
{
  register cont *rq = q;
  register AFFIX A_2 = rq->a;
  register AFFIX A_1 = (rq - 1)->a;
  register AFFIX A_0 = (rq - 2)->a;
  char *rc = c;
  if (Dunpair (A_0, A_1, A_2))
  {
    q = rq - 4;
    (*(rq - 3)->q) ();
    rq = q + 3;
  }
  (rq - 2)->a = A_0;
  (rq - 1)->a = A_1;
  rq->a = A_2;
  (rq + 1)->q = Uunpair;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UWHERE
void Uwhere ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dwhere (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Uwhere;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UCOMPUTE
void Ucompute ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dcompute (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ucompute;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef USIZE
void Usize ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dsize (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Usize;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif
