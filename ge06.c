
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef DTYPEIN
int Dtypein (AFFIX A_0) 
{
  char *rc = c;
  if (feof (stdin))
    return false;
  A_0->t = rc;
  A_0->r = nil;
  A_0->l = nil;
  while (!feof (stdin))
  {
    *rc = getc (stdin);
    if (*rc++ == '\n')
    {
      *(rc - 1) = '\0';
      c = rc;
      return true;
    }
    if (rc > cstore_top)
        cstore_overflow ();
  }
  *rc++ = '\0';
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return true;
}
#endif

#ifdef DTYPEOUT
int Dtypeout (AFFIX I_0) 
{
  AFFIX _affix = I_0;
  if ((interesting_level_number > level) && (interesting_level_number != -1))
    fprintf (stderr, "%ld ", level);
  (void) printa (stderr, _affix);
  return true;
}
#endif

#ifdef DREALTOINT
int Drealtoint (AFFIX I_0, AFFIX D_1) 
{
  float real = 0;
  char *expr = c;
  if ((I_0->r == nil) && (I_0->l == nil))
    expr = I_0->t;
  else
  {
    sprinta (I_0);
    *c++ = '\0';
  }
  D_1->t = c;
  D_1->l = nil;
  D_1->r = nil;
  sscanf (expr, "%f", &real);
  (void) sprintf (c, "%d", (int) real);
  c += 20;
  if (c > cstore_top)
    cstore_overflow ();
  return true;
}
#endif

#ifdef DDECIMALTOUN
int Ddecimaltounary (AFFIX I_0, AFFIX D_1) 
{
  long dec;
  char *rc = c, *sv = c;
  char *eptr;
  if ((I_0->r == nil) && (I_0->l == nil))
    rc = I_0->t;
  else
  {
    sprinta (I_0);
    *c++ = '\0';
  }
  dec = strtol (rc, &eptr, 10);

  if (eptr == rc || *eptr != '\0' ||  dec > 10000)
    return false;
  rc = sv;
  D_1->t = rc;
  D_1->l = nil;
  D_1->r = nil;
  for (; dec-- > 0;)
  {
     if (rc > cstore_top)
     {
       c = rc;
       cstore_overflow ();
     }
    *rc++ = '1';
  }
  *rc++ = '\0';
  c = rc;
  if (rc > cstore_top)
  {
       c = rc;
       cstore_overflow ();
  }
  return true;
}

#endif
#ifdef DKEYWORD
int Dkeyword (AFFIX I) 
{
  register char *term, *rip = ip;
  term = I->t;
  for (; *term != '\0'; rip++)
  {
    char x = *rip;
    if (islower (x))
      x = toupper (x);
    if (*term++ != x)
    {
      if ((mip < ip) && (ip < limitip) && (ip > input))
      {
        mip = ip;
        change_line_file = true;
        set_ip_start_pos = set_line_pos;
        set_ip_start_num = set_line_num;
      }
      return false;
    }
  }
  ip = rip;
  if ((mip < ip) && (ip < limitip) && (ip > input))
  {
    mip = rip;
    change_line_file = true;
    set_ip_start_pos = set_line_pos;
    set_ip_start_num = set_line_num;
  }
  return true;
}
#endif

#ifdef DUPPERCASE
int Duppercase (AFFIX I, AFFIX D) 
{
  register char *rs, *rd;
  char *dest;
  if ((I->r == nil) && (I->l == nil))
    rs = I->t;
  else
  {
    rs = c;
    sprinta (I);
    *c++ = '\0';
  }
  dest = c;
  for (rd = dest; *rs != '\0';)
  {
    char tc = *rs++;
    if (islower (tc))
      *rd++ = toupper (tc);
    else
      *rd++ = tc;
  }
  *rd++ = '\0';
  c = rd;
  D->t = dest;
  D->l = nil;
  D->r = nil;
  return true;
}
#endif

#ifdef DREVERSE
int Dreverse (AFFIX I, AFFIX D) 
{
  register char *rs, *rd;
  rs = c;
  D->t = rs;
  sprinta (I);

  for (rd = c - 1; rs < rd;)
  {
    char tc = *rs;
    *rs++ = *rd;
    *rd-- = tc;
  }
  *c++ = '\0';
  D->l = nil;
  D->r = nil;
  return true;
}
#endif

#ifdef DLOWERCASE
int Dlowercase (AFFIX I, AFFIX D) 
{
  register char *rs, *rd;
  char *dest;
  if ((I->r == nil) && (I->l == nil))
    rs = I->t;
  else
  {
    rs = c;
    sprinta (I);
    *c++ = '\0';
  }
  dest = c;
  for (rd = dest; *rs != '\0';)
  {
    char tc = *rs++;
    if (isupper (tc))
      *rd++ = tolower (tc);
    else
      *rd++ = tc;
  }
  *rd++ = '\0';
  c = rd;
  D->t = dest;
  D->l = nil;
  D->r = nil;
  return true;
}
#endif
