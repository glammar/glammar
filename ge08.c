
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef USETINPUTPTRTO
void Usetinputptrto_ ()
{
  register cont *rq = q;
  char *rc = c;
  register AFFIX A_0 = (rq + 0)->a;
  register AFFIX IP = (rq + -1)->a;
  {
    if (ip >= ipstart && ip <= ipend)
      ipln = ip;
    A_0->t = ip;
#ifdef NEVER
    /* 
       This is commented out because of the predicate last that assumes there is a null before the start of the input */
    if ((IP->r == nil) && (IP->l == nil))
    {
      ip = IP->t;
    }
    else
#endif
    {
      *c++ = '\0';
      sprinta (IP);
      ip = rc + 1;
      *c++ = '\0';
    }
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q - -2;
    ip = A_0->t;
    c = rc;
    if (rc > cstore_top)
      cstore_overflow ();
  }
  (rq - 1)->a = IP;
  (rq + 0)->a = A_0;
  (rq + 1)->q = Usetinputptrto_;
  q = rq + 1;
}
#endif

#ifdef URESETINPUTPTR
void Uresetinputptr_ ()
{
  register cont *rq = q;
  register char *a;
  register AFFIX A_0 = (rq + 0)->a;
  {
    if (*ip != '\0')
      goto done;
    a = ip;
    ip = A_0->t;
    q = rq + -2;
    (*(rq + -1)->q) ();
    rq = q - -1;
    ip = a;
  }
done:
  (rq + 0)->a = A_0;
  (rq + 1)->q = Uresetinputptr_;
  q = rq + 1;
}

void Udetnestarset ()
{
  register cont *rq = q;
  char *term, *rc = c, *rip = ip, *sip = ip, *pip;
  register AFFIX A_0 = (rq + 0)->a;
  register AFFIX A_1 = (rq + -1)->a;
  affix sA0;
  sA0 = *A_0;
  term = A_0->t;
  if (((A_0->r) == nil) && ((A_0->l) == nil));
  else
  {
    term = c;
    sprinta (A_0);
    *c++ = '\0';
  }
  for (pip = ip; ip && ip <= pip;)
  {
    ip = strstr (pip, term);
    if (!ip)
      break;
    pip = ip + 1;
    rip = sip;

    A_1->t = c;
    A_1->r = nil;
    A_1->l = nil;
    while (rip < ip)
      *c++ = *rip++;
    *c++ = '\0';
    q = rq + -3;
    (*(rq + -2)->q) ();
  }
  c = rc;
  ip = sip;
  *A_0 = sA0;
  (rq + -1)->a = A_0;
  (rq + 0)->a = A_1;
  (rq + 1)->q = Udetnestarset;
  q = rq + 1;
}

#endif

#ifdef UNESTARALT
void Unestaralt_ ()
{
  register cont *rq = q;
  register AFFIX A_0 = (rq - 1)->a;
  register AFFIX A_1 = (rq + 0)->a;
  register char *sip = ip;
  {
    A_0->t = sip;
    A_0->r = nil;
    A_0->l = nil;
    ip = A_1->t;
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q - -2;
  }
  ip = sip;
  (rq - 1)->a = A_0;
  (rq + 0)->a = A_1;
  (rq + 1)->q = Unestaralt_;
  q = rq + 1;
}

GLAMMAR_Q3 (Ddetprefix, Udetprefix) GLAMMAR_Q3 (Ddetprefix2_, Udetprefix2_)
#endif
#ifdef UWAS
                                        GLAMMAR_Q1 (Dgetlist_, Ugetlist_)
GLAMMAR_Q1 (Dwas, Uwas) GLAMMAR_Q1 (Dwas10, Uwas10) GLAMMAR_Q1 (Dwas100, Uwas100) GLAMMAR_Q1 (Dwas1000, Uwas1000)
#endif /* UWAS */
#ifdef UNESTARSET
void Unestarset ()
{
  register char *csave = c, *sip = ip;
  register AFFIX A_0 = (q--)->a;
  {

    A_0->t = csave;
    A_0->r = nil;
    A_0->l = nil;
    do
    {
      register char *rip = ip, *rc = c, special;
      if (*ip == '\001' || *ip == '\002')
      {
        special = *ip;
        *c++ = '\0';
        q -= 1;
        (*(*(q + 1)).q) ();
        rc[0] = special;
        rc[1] = rip[1];
        rc[2] = rip[2];
        rc[3] = rip[3];
        rc[4] = rip[4];
        rc[5] = rip[5];
#ifndef EIGHT_BYTE_ADDR
        rc[6] = '\0';
        ip += 5;
        c += 7;
#else
        rc[6] = rip[6];
        rc[7] = rip[7];
        rc[8] = rip[8];
        rc[9] = rip[9];
        rc[10] = '\0';
        ip += 9;
        c += 11;
#endif
      }
      else
      {
        *c++ = '\0';
        q -= 1;
        (*(*(q + 1)).q) ();
        *(c - 1) = *ip;
      }
    }
    while (*ip++ != '\0');
  }
  ip = sip;
  c = csave;
  (++q)->a = A_0;
  (++q)->q = Unestarset;
}
#endif

#ifdef UMETATERMINAL
void Umetaterminal ()
{
  register char *sp, *term, *rip = ip, *rc = c;
  AFFIX A_0 = q->a;
  term = A_0->t;
  sp = rip;
  if (((A_0->r) == nil) && ((A_0->l) == nil));
  else
  {
    *c++ = '\0';
    term = c;
    sprinta (A_0);
    *c++ = '\0';
  }
  for (; *term != '\0'; rip++)
    if (*term++ != *rip)
    {
      (++q)->q = Umetaterminal;
      return;
    }
  q -= 2;
  ip = rip;
  (*(*(q + 1)).q) ();
  (++q)->a = A_0;
  (++q)->q = Umetaterminal;
  ip = sp;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}

GLAMMAR_Q2 (Dmetaterminal2_, Umetaterminal2_)
#endif
#ifdef DSETINPUTPTRTO
int Dsetinputptrto_ (IP, A_0)
register AFFIX A_0;
register AFFIX IP;
{
  if (ip >= ipstart && ip <= ipend)
    ipln = ip;
  A_0->t = ip;
  if ((IP->r == nil) && (IP->l == nil))
  {
    ip = IP->t;
  }
  else
  {
    *c++ = '\0';
    *c++ = '\0';
    ip = c;
    sprinta (IP);
    if (c + 1 > cstore_top)
      cstore_overflow ();
    *c++ = '\0';
    *c++ = '\0';
  }
  return true;
}
#endif

#ifdef DRESETINPUTPTR
#include <strings.h>
int Dresetinputptr_ (A_0)
register AFFIX A_0;
{
  if (*ip != '\0')
    return false;
  ip = A_0->t;
  return true;
}

int Ddetprefix (A_2, A_1, A_0)
register AFFIX A_2;
register AFFIX A_1;
register AFFIX A_0;
{
  register char *term, *te, *rip, *re;
  term = A_0->t;
  if (((A_0->r) == nil) && ((A_0->l) == nil))
  {
    for (te = term; *te; te++);
  }
  else
  {
    term = c;
    sprinta (A_0);
    te = c;
    *c++ = '\0';
  }
  if (term == te)
  {
    COPY (A_1, A_2);
    return true;
  }
  rip = c;
  sprinta (A_2);
  re = c;
  *c++ = '\0';

  if (re - rip < te - term)
    return false;
  re--;
  te--;
  while (*te)
  {
    if (*re != *te)
    {
      return false;
    }
    re--;
    te--;
  }
  *++re = '\0';


  A_1->t = rip;
  A_1->r = nil;
  A_1->l = nil;
  return true;
}

int Ddetprefix2_ (A_2, A_1, A_0)
register AFFIX A_2;
register AFFIX A_1;
register AFFIX A_0;
{
  register char *term, *te, *rip, *ipe;
  long szi, szt, len;
  char *rc = c;

  term = A_0->t;
  if (((A_0->r) == nil) && ((A_0->l) == nil))
  {
    for (te = term; *te; te++);
  }
  else
  {
    term = c;
    sprinta (A_0);
    te = c;
    *c++ = '\0';
  }
  if (term == te)
  {
    A_1->t = ip;
    A_1->l = nil;
    A_2->l = nil;
    ip = A_2->t;
    return true;
  }
  rip = ip;
  ip = A_2->t;
  szi = strlen (rip);
  szt = te - term;
  len = szi - szt;
  ipe = rip + len;
  if (szt > szi)
  {
    c = rc;
    return false;
  }
  else if (szt == szi)
  {
    if (!strcmp (rip, term))
    {
      A_1->t = term;
      A_1->r = nil;
      A_1->l = nil;
      return true;
    }
    else
    {
      c = rc;
      return false;
    }
  }
  else if (!strcmp (ipe, term))
  {
    rc = c;
    while (rip < ipe)
      *c++ = *rip++;
    *c++ = 0;
    A_1->t = rc;
    A_1->r = nil;
    A_1->l = nil;
    return true;
  }
  c = rc;
  return false;
}

int Ddetnestarset (A_1, A_0)
register AFFIX A_1;
register AFFIX A_0;
{
  register char *term, *rip = ip;
  term = A_0->t;
  if (((A_0->r) == nil) && ((A_0->l) == nil));
  else
  {
    term = c;
    sprinta (A_0);
    *c++ = '\0';
  }
  ip = strstr (rip, term);
  if (!ip)
  {
    ip = rip;
    return false;
  }
  A_1->t = c;
  A_1->r = nil;
  A_1->l = nil;
  while (rip < ip)
    *c++ = *rip++;
  *c++ = '\0';
  return true;

}
#endif
#ifdef DNESTARALT
int Dnestaralt_ (A_0, A_1)
register AFFIX A_0;
register AFFIX A_1;
{
  A_0->t = ip;
  A_0->r = nil;
  A_0->l = nil;
  ip = A_1->t;
  return true;
}


int Dgetlist_ (A_0)             /* dumpaffix */
register AFFIX A_0;
{
  AFFIX _affix = A_0;
  char *rc = c, *rip = ip;
  A_0->t = rc;
  A_0->r = nil;
  A_0->l = nil;
  if (*ip == '\001')
  {
    rc[0] = rip[0];
    rc[1] = rip[1];
    rc[2] = rip[2];
    rc[3] = rip[3];
    rc[4] = rip[4];
    rc[5] = rip[5];
    rc[6] = rip[6];
    rc[7] = rip[7];
    rc[8] = rip[8];
    rc[9] = rip[9];
    rc[10] = '\0';
    ip += 10;
    c += 11;
    if (c > cstore_top)
      cstore_overflow ();
    return true;
  }
  return false;
}
#endif

#ifdef DMETATERMINAL
int Dmetaterminal (A_0)
register AFFIX A_0;
{
  register char *term, *rip = ip;
  term = A_0->t;
  if (((A_0->r) == nil) && ((A_0->l) == nil));
  else
  {
    term = c;
    sprinta (A_0);
    *c++ = '\0';
  }
  for (; *term != '\0'; rip++)
    if (*term++ != *rip)
    {
      return false;
    }
  ip = rip;
  return true;
}

int Dmetaterminal2_ (A_0, A_1)
register AFFIX A_0;
register AFFIX A_1;
{
  register char *term, *rip = ip;
  term = A_0->t;
  if (((A_0->r) == nil) && ((A_0->l) == nil));
  else
  {
    term = c;
    sprinta (A_0);
    *c++ = '\0';
  }
  for (; *term != '\0'; rip++)
    if (*term++ != *rip)
    {
      return false;
    }
  if (!*rip)
  {
    ip = A_1->t;
    return true;
  }
  return false;
}
#endif


#ifdef DWAS
int Dwas (afx)
register AFFIX afx;
{
  *c = *(ip - 1);
  afx->t = c++;
  *c++ = '\0';
  afx->r = nil;
  afx->l = nil;
  return true;
}

int Xwas (nr, afx)
int nr;
register AFFIX afx;
{
  affix ax;
  char *i, sv, *rs, *rd;
  int j;
  afx->r = nil;
  afx->l = nil;
  afx->t = empty;

  for (j = nr, i = ip; *i != '\0' && --j; i--);

  if (i == ip)
    return true;

  sv = *ip;
  *ip = '\0';
  afx->t = i;
  i = c;
  sprinta (afx);
  *ip = sv;

  rs = i;
  afx->t = rs;

  for (rd = c - 1; rs < rd;)
  {
    char tc = *rs;
    *rs++ = *rd;
    *rd-- = tc;
  }
  *c++ = '\0';
  return true;
}

int Dwas10 (afx)
register AFFIX afx;
{
  return Xwas (10, afx);
}

int Dwas100 (afx)
register AFFIX afx;
{
  return Xwas (100, afx);
}

int Dwas1000 (afx)
register AFFIX afx;
{
  return Xwas (1000, afx);
}
#endif

#ifdef DFIND
#include <strings.h>
int Dfind (A_0)
register AFFIX A_0;
{
  register char *term, *rip;
  term = A_0->t;
  if (((A_0->r) == nil) && ((A_0->l) == nil));
  else
  {
    *c++ = '\0';
    term = c;
    sprinta (A_0);
    *c++ = '\0';
  }
  if (rip = strstr (ip, term))
  {
    ip = rip;
    return true;
  }
  return false;
}
#endif

#ifdef UFIND
GLAMMAR_Q1 (Dfind, Ufind)
#endif
