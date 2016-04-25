
/*

    This file is a part of the GLAMMAR source Distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef    DDEQUAL
int Dequalsempty ();
int Ddequal (A, B)              /* lifted equal */
register AFFIX A, B;
{
  char *xs = c;
  char *ys;
  if (A == B)
    return true;
  else if (!*(A->t) && A->r == nil && A->l == nil)
    return Dequalsempty (B);
  else if (!*(B->t) && B->r == nil && B->l == nil)
    return Dequalsempty (A);
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
  if (afxcmp (xs, ys) == 0)
    return true;
  return false;
}
#endif

#ifdef         DEQUAL
int Dequal (A, B)               /* lifted not equal */
register AFFIX A, B;
{
  register char *rc = c;
  if (Ddequal (A, B))
  {
    c = rc;
    if (rc > cstore_top)
      cstore_overflow ();
    return true;
  }
  return false;


}

int Dequalsempty (A)
register AFFIX A;
{
  if (A == nil)
    return true;
  if ((A->t == undefined) || *(A->t) || (A->l && *(A->l->t)) || (A->r && *(A->r->t)))
  {
    return false;
  }
  return Dequalsempty (A->l) && Dequalsempty (A->r);
}

GLAMMAR_Q1 (Dequalsempty, Uequalsempty)
#endif
#ifdef DNOTEQUAL
int Dnotequal (A, B)            /* lifted not equal */
register AFFIX A, B;
{
  return !Dequal (A, B);
}
#endif

#ifdef DDFQUAL
int Ddfqual (A, B)              /* lifted equal */
register AFFIX A, B;
{
  register char *xs = c, *ys;
  if (A == B)
    return true;
  else if ((A >= first_meta) && (B >= first_meta) && (A <= last_meta) && (B <= last_meta))
  {
    return false;
  }
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
  if (afxcmp (xs, ys) == 0)
    return true;
  return false;
}
#endif

#ifdef   DFQUAL
int Dfqual (A, B)               /* lifted not equal */
register AFFIX A, B;
{
  register char *rc = c;
  if (Ddfqual (A, B))
  {
    c = rc;
    if (rc > cstore_top)
      cstore_overflow ();
    return true;
  }
  return false;


}
#endif

#ifdef DFOTEQUAL
int Dfotequal (A, B)            /* lifted not equal */
register AFFIX A, B;
{
  return !Dfqual (A, B);
}
#endif

#ifdef DCOMPARE
int Dcompare (A, B, tailx, taily)       /* compare */
register AFFIX A, B, tailx, taily;
{
  register char *xs = c, *ys;
  if ((A->r == nil) && (A->l == nil))
    xs = A->t;
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  ys = c;
  if ((B->r == nil) && (B->l == nil))
    ys = B->t;
  else
  {
    sprinta (B);
    *c++ = '\0';
  }
  while ((*xs == *ys++))
    if (*xs++ == '\0')
    {
      ASSIGN (tailx, empty, nil, nil);
      ASSIGN (taily, empty, nil, nil);
      return true;
    }
  ASSIGN (tailx, xs, nil, nil);
  ASSIGN (taily, --ys, nil, nil);
  return true;
}
#endif

#ifdef ULINENUM
void Ulinenum ()
{
  register cont *rq = q;
  register AFFIX A_0 = rq->a;
  char *rc = c;
  int save_l = set_line_num;
  char *save_p = set_line_pos;
  if (Dlinenum (A_0))
  {
    q = rq - 2;
    (*(rq - 1)->q) ();
    rq = q + 1;
  }
  rq->a = A_0;
  (rq + 1)->q = Ulinenum;
  q = rq + 1;
  c = rc;
  set_line_pos = save_p;
  set_line_num = save_l;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef DLINENUM
int Dlinenum (D_0)
register affix *D_0;
{
  register char *rip = set_line_pos, *tip = ip;
  int line = set_line_num;
  D_0->t = c;
  D_0->l = nil;
  D_0->r = nil;
  if (tip > ipend || tip < ipstart)
  {
    tip = ipln;
    rip = ipstart;
    line = 1;

  }
  if (tip < set_line_pos)
  {
    rip = ipstart;
    line = 1;
  }

  while (rip < tip)
    if (*rip++ == '\n')
      line += 1;
  (void) sprintf (c, "%d\0", line);
  set_line_pos = tip;
  set_line_num = line;
  c[7] = '\0';
  c += 8;
  return true;
}
#endif
