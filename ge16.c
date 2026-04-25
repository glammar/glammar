
/*

    This file is a part of the GLAMMAR source Distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
extern char *groups[][LONGBITS];

#ifdef  DTRANSFORMLATTICE
int Dtransformlattice_ (AFFIX A, AFFIX B, AFFIX C) 
{
  char *rc = c;
  long i;
  long x, y;
  x = atol (A->t);
  y = (long) (B->t);
  C->t = rc;
  C->r = nil;
  C->l = nil;
  *rc++ = '{';
  for (i = 0; i < LONGBITS; i++)
    if ((((long)1 << i) & y) != 0)
    {
      char *rr = groups[x][i];
      while ((rr != 0) && (*rr != '\0'))
      {
       if (rc > cstore_top)
          cstore_overflow ();
        *rc++ = *rr++;
      }
      *rc++ = ',';
      if (rr == NULL)
      {
        fprintf (stderr, "%s: runtime error in tranform lattice: evaluating undefined affix\n", arg_zero);
        exit (1);
      }
    }
  *(rc - 1) = '}';
  *rc++ = '\0';
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return true;
}
#endif

#ifdef DTRANSFORMLATTICETERM
int Dtransformlatticeterm_ (AFFIX A, AFFIX B, AFFIX C) 
{
  char *rc = c;
  long i;
  int m = LONGBITS;
  long x, y;
  x = atol (A->t);
  y = atol (B->t);
  C->t = rc;
  C->r = nil;
  C->l = nil;
  *rc++ = '{';
  for (i = 0; i < m; i++)
    if ((((long)1 << i) & y) != 0)
    {
      register char *rr = groups[x][i];
      while ((rr != 0) && (*rr != '\0'))
      {
        *rc++ = *rr++;
       if (rc > cstore_top)
          cstore_overflow ();
      }
      *rc++ = ',';
      if (rr == NULL)
      {
        fprintf (stderr, "%s: runtime error in tranform lattice: evaluating undefined affix\n", arg_zero);
        exit (1);
      }
    }
  *(rc - 1) = '}';
  *rc++ = '\0';
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return true;
}
#endif

#ifdef DEXPLINTERSECT
int Dexplintersect_ (AFFIX A, AFFIX B) 
{

  char *cx = B->t;
  long y = (long) A->t;
  long x = atol (cx);
  long z = x & y;

  if (z == 0)
    return false;
  A->t = (char *) z;
  return true;
}
#endif

#ifdef UEXPLINTERSECT
void Uexplintersect_ ()
{
  register cont *rq = q;
  register AFFIX A_1 = (rq + 0)->a;
  register AFFIX A_0 = (rq + -1)->a;
  char *rc = c;
  if (Dexplintersect_ (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q + 2;
  }
  (rq + -1)->a = A_0;
  (rq + 0)->a = A_1;
  (rq + 1)->q = Uexplintersect_;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif

#ifdef DINTERSECT
int Dintersect_ (AFFIX A, AFFIX B) 
{
  long x = (long) A->t;
  long y = (long) B->t;
  long z = x & y;

  if (z == 0)
    return false;
  B->t = (char *) z;
  return true;
}
#endif

#ifdef UINTERSECT
void Uintersect_ ()
{
  register cont *rq = q;
  register AFFIX A_1 = (rq + 0)->a;
  register AFFIX A_0 = (rq + -1)->a;
  char *a1 = A_1->t, *a0 = A_0->t, *rc = c;
  if (Dintersect_ (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q + 2;
  }
  (rq + -1)->a = A_0;
  (rq + 0)->a = A_1;
  (rq + 1)->q = Uintersect_;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  A_1->t = a1;
  A_0->t = a0;
}
#endif

#ifdef UTRANSFORMLATTICE
void Utransformlattice_ ()
{
  register cont *rq = q;
  register AFFIX A_2 = (rq + 0)->a;
  register AFFIX A_1 = (rq + -1)->a;
  register AFFIX A_0 = (rq + -2)->a;
  char *rc = c;
  if (Dtransformlattice_ (A_0, A_1, A_2))
  {
    q = rq - 4;
    (*(rq - 3)->q) ();
    rq = q + 3;
  }
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  (rq + -2)->a = A_0;
  (rq + -1)->a = A_1;
  (rq + 0)->a = A_2;
  (rq + 1)->q = Utransformlattice_;
  q = rq + 1;
}
#endif

#ifdef UTRANSFORMLATTICETERM
void Utransformlatticeterm_ ()
{
  register cont *rq = q;
  register AFFIX A_2 = (rq + 0)->a;
  register AFFIX A_1 = (rq + -1)->a;
  register AFFIX A_0 = (rq + -2)->a;
  char *rc = c;
  if (Dtransformlatticeterm_ (A_0, A_1, A_2))
  {
    q = rq - 4;
    (*(rq - 3)->q) ();
    rq = q + 3;
  }
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  (rq + -2)->a = A_0;
  (rq + -1)->a = A_1;
  (rq + 0)->a = A_2;
  (rq + 1)->q = Utransformlatticeterm_;
  q = rq + 1;
}
#endif

#ifdef UTLTRADITIONAL
void Utltraditional_ ()
{
  register cont *rq = q;
  register AFFIX A_2 = (rq + 0)->a;
  register AFFIX A_1 = (rq + -1)->a;
  register AFFIX A_0 = (rq + -2)->a;
  register char *rc = c;
  char *csave = c;
  long i;
  int x, y = (long) A_1->t;
  x = atoi (A_0->t);
  A_2->t = rc;
  A_2->r = nil;
  A_2->l = nil;
  for (i = 0; i < LONGBITS; i++)
    if ((((long)1 << i) & y) != 0)
    {
      register char *rr = groups[x][i];
      rc = csave;
      while ((rr != 0) && (*rr != '\0'))
      {
        *rc++ = *rr++;
       if (rc > cstore_top)
          cstore_overflow ();
      }
      if (rr == NULL)
      {
        fprintf (stderr, "%s: runtime error in tranform lattice: evaluating undefined affix\n", arg_zero);
        exit (1);
      }
      *rc++ = '\0';
      c = rc;
      if (rc > cstore_top)
        cstore_overflow ();
      q = rq - 4;
      (*(rq - 3)->q) ();
      rq = q + 3;
    }
  c = csave;
  (rq + -2)->a = A_0;
  (rq + -1)->a = A_1;
  (rq + 0)->a = A_2;
  (rq + 1)->q = Utltraditional_;
  q = rq + 1;
}
#endif

#ifdef UTLTRADITIONALTERM
void Utltraditionalterm_ ()
{
  register cont *rq = q;
  register AFFIX A_2 = (rq + 0)->a;
  register AFFIX A_1 = (rq + -1)->a;
  register AFFIX A_0 = (rq + -2)->a;
  register char *rc = c;
  char *csave = c;
  long i;
  int x, y = atoi (A_1->t);
  x = atoi (A_0->t);
  A_2->t = rc;
  A_2->r = nil;
  A_2->l = nil;
  for (i = 0; i < LONGBITS; i++)
    if ((((long)1 << i) & y) != 0)
    {
      register char *rr = groups[x][i];
      rc = csave;
      while ((rr != 0) && (*rr != '\0'))
      {
         if (rc > cstore_top)
            cstore_overflow ();
        *rc++ = *rr++;
      }
      if (rr == NULL)
      {
        fprintf (stderr, "%s: runtime error in tranform lattice: evaluating undefined affix\n", arg_zero);
        exit (1);
      }
      *rc++ = '\0';
      c = rc;
      if (rc > cstore_top)
        cstore_overflow ();
      q = rq - 4;
      (*(rq - 3)->q) ();
      rq = q + 3;
    }
  c = csave;
  (rq + -2)->a = A_0;
  (rq + -1)->a = A_1;
  (rq + 0)->a = A_2;
  (rq + 1)->q = Utltraditionalterm_;
  q = rq + 1;
}
#endif
