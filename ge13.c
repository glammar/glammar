
/*

    This file is a part of the GLAMMAR source Distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef UREALTOINT
void Urealtoint ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Drealtoint (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Urealtoint;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UDECIMALTOUN
void Udecimaltounary ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Ddecimaltounary (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Udecimaltounary;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UREVERSE
void Ureverse ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dreverse (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ureverse;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UUPPERCASE
void Uuppercase ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Duppercase (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Uuppercase;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef ULOWERCASE
void Ulowercase ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dlowercase (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ulowercase;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UKEYWORD
void Ukeyword ()
{
  register cont *rq = q;
  register AFFIX A_0 = rq->a;
  char *rc = c;
  if (Dkeyword (A_0))
  {
    q = rq - 2;
    (*(rq - 1)->q) ();
    rq = q + 1;
  }
  rq->a = A_0;
  (rq + 1)->q = Ukeyword;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef UTYPEOUT
void Utypeout ()
{
  register cont *rq = q;
  register AFFIX A_0 = rq->a;
  char *rc = c;
  if (Dtypeout (A_0))
  {
    q = rq - 2;
    (*(rq - 1)->q) ();
    rq = q + 1;
  }
  rq->a = A_0;
  (rq + 1)->q = Utypeout;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  q = rq + 1;
}
#endif

#ifdef UTYPEIN
void Utypein ()
{
  register cont *rq = q;
  register AFFIX A_0 = rq->a;
  char *rc = c;
  if (Dtypein (A_0))
  {
    q = rq - 2;
    (*(rq - 1)->q) ();
    rq = q + 1;
  }
  rq->a = A_0;
  (rq + 1)->q = Utypein;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  q = rq + 1;
}
#endif
