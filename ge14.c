
/*

    This file is a part of the GLAMMAR source Distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef DLEXICALLESS
int Dlexicalless (A, B)
register AFFIX A, B;
{
  register char *rc = c, *xs = rc, *ys;
  if (A == B)
    return false;
  else if (((A->r) == nil) && ((A->l) == nil))
    xs = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  ys = c;
  if (((B->r) == nil) && ((B->l) == nil))
    ys = B->t;
  else
  {
    sprinta (B);
    *c++ = '\0';
  }
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return lexafxcmp (xs, ys) < 0;
}
#endif

#ifdef DLEXICALMORE
int Dlexicalmore (A, B)         /* lifted equal */
register AFFIX A, B;
{
  register char *rc = c, *xs = rc, *ys;
  if (A == B)
    return false;
  else if (((A->r) == nil) && ((A->l) == nil))
    xs = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  ys = c;
  if (((B->r) == nil) && ((B->l) == nil))
    ys = B->t;
  else
  {
    sprinta (B);
    *c++ = '\0';
  }
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return lexafxcmp (xs, ys) > 0;
}
#endif

#ifdef DLEXICALLESSEQUAL
int Dlexicallessequal (A, B)    /* lifted equal */
register AFFIX A, B;
{
  register char *rc = c, *xs = rc, *ys;
  if (A == B)
    return false;
  else if (((A->r) == nil) && ((A->l) == nil))
    xs = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  ys = c;
  if (((B->r) == nil) && ((B->l) == nil))
    ys = B->t;
  else
  {
    sprinta (B);
    *c++ = '\0';
  }
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return lexafxcmp (xs, ys) <= 0;
}
#endif

#ifdef DLEXICALMOREEQUAL
int Dlexicalmoreequal (A, B)    /* lifted equal */
register AFFIX A, B;
{
  register char *rc = c, *xs = rc, *ys;
  if (A == B)
    return true;
  else if (((A->r) == nil) && ((A->l) == nil))
    xs = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  ys = c;
  if (((B->r) == nil) && ((B->l) == nil))
    ys = B->t;
  else
  {
    sprinta (B);
    *c++ = '\0';
  }
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return lexafxcmp (xs, ys) >= 0;
}
#endif

#ifdef ULEXICALLESS
void Ulexicalless ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dlexicalless (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ulexicalless;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef ULEXICALMORE
void Ulexicalmore ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dlexicalmore (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ulexicalmore;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef ULEXICALLESSEQUAL
void Ulexicallessequal ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dlexicallessequal (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ulexicallessequal;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef ULEXICALMOREEQUAL
void Ulexicalmoreequal ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dlexicalmoreequal (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ulexicalmoreequal;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif
