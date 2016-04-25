
/*

    This file is a part of the GLAMMAR source Distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
struct char_ptr_list
{
  char *l[32];
};
extern struct char_ptr_list groups[];

#ifdef  DTRANSFORMLATTICE
int Dtransformlattice_ (A, B, C)        /* lifted tranform */
register AFFIX A, B, C;
{
  char *rc = c;
  int i;
  int x, y = (long) B->t;
  x = atoi (A->t);
  C->t = rc;
  C->r = nil;
  C->l = nil;
  *rc++ = '{';
  for (i = 0; i < 32; i++)
    if (((1 << i) & y) != 0)
    {
      register char *rr = groups[x].l[i];
      while ((rr != 0) && (*rr != '\0'))
        *rc++ = *rr++;
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
int Dtransformlatticeterm_ (A, B, C)    /* lifted tranform */
register AFFIX A, B, C;
{
  char *rc = c;
  int i;
  int x, y;
  x = atoi (A->t);
  y = atoi (B->t);
  C->t = rc;
  C->r = nil;
  C->l = nil;
  *rc++ = '{';
  for (i = 0; i < 32; i++)
    if (((1 << i) & y) != 0)
    {
      register char *rr = groups[x].l[i];
      while ((rr != 0) && (*rr != '\0'))
        *rc++ = *rr++;
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
int Dexplintersect_ (A, B)
register AFFIX A, B;
{

  char *cx = B->t;
  long y = (long) A->t;
  long x = atoi (cx);
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
int Dintersect_ (A, B)
register AFFIX A, B;
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
  if (Dransformlatticeterm_ (A_0, A_1, A_2))
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
  int i;
  int x, y = (long) A_1->t;
  x = atoi (A_0->t);
  A_2->t = rc;
  A_2->r = nil;
  A_2->l = nil;
  for (i = 0; i < 32; i++)
    if (((1 << i) & y) != 0)
    {
      register char *rr = groups[x].l[i];
      rc = csave;
      while ((rr != 0) && (*rr != '\0'))
        *rc++ = *rr++;
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
  int i;
  int x, y = atoi (A_1->t);
  x = atoi (A_0->t);
  A_2->t = rc;
  A_2->r = nil;
  A_2->l = nil;
  for (i = 0; i < 32; i++)
    if (((1 << i) & y) != 0)
    {
      register char *rr = groups[x].l[i];
      rc = csave;
      while ((rr != 0) && (*rr != '\0'))
        *rc++ = *rr++;
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
