
/*

    This file is a part of the GLAMMAR source Distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef DPAIR
int Dpair (A, C, D)             /* pair */
register AFFIX A, C, D;
{
  AFFIX B = ++af;
  A->t = c;
  A->l = nil;
  A->r = nil;
  B->t = fast_list_acces;
  B->l = C;
  B->r = D;
  PUT_CELL_ADDR (B);
  return true;
}
#endif

#ifdef DUNPAIR
int Dunpair (A, B, C)           /* lifted unpair */
register AFFIX A, B, C;
{
  char *xs = c;
  AFFIX cell = 0;
  if ((A->r == nil) && (A->l == nil))
    xs = A->t;
  else
  {
    sprinta (A);
    *c++ = '\0';
  }

  GET_CELL_ADDR (cell, xs);
  if (cell)
  {
    COPY (B, cell->l);
    COPY (C, cell->r);
    return true;
  }
  return false;
}

int Dunpair2 (A, B, C)          /* lifted unpair */
register AFFIX A, *B, *C;
{
  char *xs = c;
  AFFIX cell = 0;
  if ((A->r == nil) && (A->l == nil))
    xs = A->t;
  else
  {
    sprinta (A);
    *c++ = '\0';
  }

  GET_CELL_ADDR (cell, xs);
  if (cell)
  {
    B = cell->l;
    C = cell->r;
    return true;
  }
  return false;
}
#endif

#ifdef DREPAIR
int Drepair (A_0, A_1, A_2)     /* lifted unpair */
register AFFIX A_0, A_1, A_2;
{
  AFFIX cell = nil;
  register char *xs = c;
  if ((A_0->r == nil) && (A_0->l == nil))
    xs = A_0->t;
  else
  {
    sprinta (A_0);
    *c++ = '\0';
  }
  GET_CELL_ADDR (cell, xs);
  if (cell)
  {
    cell->l = A_1;
    cell->r = A_2;
    return true;
  }
  return false;
}
#endif

#ifdef UREPAIR
void Urepair ()
{                               /* repair */

  register cont *rq = q;
  register AFFIX A_2 = (rq + 0)->a;
  register AFFIX A_1 = (rq + -1)->a;
  register AFFIX A_0 = (rq + -2)->a;
  affix x, *cell = nil, old;
  char *xs = c;
  SAVE (x, A_0);
  if ((x.r == nil) && (x.l == nil));
  else
  {
    sprinta (A_0);
    A_0->t = xs;
    A_0->l = nil;
    A_0->r = nil;
    *c++ = '\0';
  }
  GET_CELL_ADDR (cell, A_0->t);
  if (cell)
  {
    SAVE (old, cell);
    cell->l = A_1;
    cell->r = A_2;
    q = rq + -4;
    (*(rq + -3)->q) ();
    rq = q + 3;
    RESTORE (cell, old);
  }

  RESTORE (A_0, x);
  (rq + -2)->a = A_0;
  (rq + -1)->a = A_1;
  (rq + 0)->a = A_2;
  (rq + 1)->q = Urepair;
  q = rq + 1;
  c = xs;
}
#endif

#ifdef DWHERE
int Dwhere (A_0, A_1)           /* where */
register AFFIX A_0;
register AFFIX A_1;
{
  MAKE (A_1, A_0);
  return true;
}
#endif

#ifdef DEVALMETA
int Devalmeta (A_0)
register AFFIX A_0;
{
  register char *rc = c;
  if (((A_0->r) == nil) && ((A_0->l) == nil))
    return;
  else
  {
    sprinta (A_0);
    *c++ = '\0';
  }
  A_0->t = rc;
  A_0->r = nil;
  A_0->l = nil;
  return true;
}
#endif

#ifdef UEVALMETA
void Uevalmeta ()
{
  register cont *rq = q;
  register AFFIX A_0 = rq->a;
  char *rc = c;
  q = rq - 2;
  if (((A_0->r) == nil) && ((A_0->l) == nil))
  {
    (*(rq - 1)->q) ();
  }
  else
  {
    affix S_0;
    S_0.t = A_0->t;
    S_0.r = A_0->r;
    S_0.l = A_0->l;
    A_0->t = c;
    A_0->r = nil;
    A_0->l = nil;
    sprinta (A_0);
    *c++ = '\0';
    if (c > cstore_top)
      cstore_overflow ();
    (*(rq - 1)->q) ();
    c = A_0->t;
    A_0->t = S_0.t;
    A_0->r = S_0.r;
    A_0->l = S_0.r;
  }
  rq = q + 1;
  rq->a = A_0;
  (rq + 1)->q = Uevalmeta;
  q = rq + 1;
}
#endif

#ifdef DINITMETA
int Dinitmeta (A_0, A_1)
register AFFIX A_0;
register AFFIX A_1;
{
  A_0->t = "";
  A_0->r = A_1;
  A_0->l = nil;
  return true;
}
#endif

#ifdef UINITMETA
void Uinitmeta ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dinitmeta (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Uinitmeta;
  q = rq + 1;
  c = rc;
}
#endif

#ifdef DASSIGN
extern AFFIX first_meta, last_meta;

int Dassign (A_0, A_1, A_2)
register AFFIX A_0;
register AFFIX A_1;
register AFFIX A_2;
{
  A_2->t = A_0->t;
  A_2->r = A_0->r;
  A_2->l = A_0->l;
  if (A_0 >= first_meta && A_0 <= last_meta)
  {
    A_0->t = A_1->t;
    A_0->r = A_1->r;
    A_0->l = A_1->l;
  }
  else if ((!*(A_0->t)) && A_0->l == nil && (A_0->r >= first_meta && A_0->r <= last_meta))
  {
    A_0->r->t = A_1->t;
  }
  return true;
}
#endif

#ifdef UASSIGN
void Uassign ()
{
  register cont *rq = q;
  register AFFIX A_2 = (rq + 0)->a;
  register AFFIX A_1 = (rq + -1)->a;
  register AFFIX A_0 = (rq + -2)->a;
  {
    A_2->t = A_0->t;
    A_2->r = A_0->r;
    A_2->l = A_0->l;
    A_0->t = A_1->t;
    A_0->r = A_1->r;
    A_0->l = A_1->l;
    q = rq + -4;
    (*(rq + -3)->q) ();
    rq = q + 3;
    A_0->t = A_2->t;
    A_0->r = A_2->r;
    A_0->l = A_2->l;
  }
  (rq + -2)->a = A_0;
  (rq + -1)->a = A_1;
  (rq + 0)->a = A_2;
  (rq + 1)->q = Uassign;
  q = rq + 1;
}
#endif
