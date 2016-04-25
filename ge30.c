
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#define TRACE_RETURN(trm, rc, pre, post)  \
    if (interesting_level_number == -1)  \
       return rc;\
    else \
      return traceterm(trm, rc, pre, post);



#ifdef DTERMINAL
int Dterminal_ (sterm)
register char *sterm;
{
  register char *sp, *term, *rip = ip;
  term = sterm;
  for (; *term != '\0'; rip++)
    if (*term++ != *rip)
    {
      if ((mip < rip) && (rip < limitip) && (rip > input))
      {
        mip = rip;
        change_line_file = true;
      }
      TRACE_RETURN (sterm, false, "\"", "\"");
    }
  if ((mip < rip) && (rip < limitip) && (rip > input))
  {
    mip = rip;
    change_line_file = true;
  }
  ip = rip;
  TRACE_RETURN (sterm, true, "\"", "\"");
}
#endif


#ifdef DCHOICE
int Dchoice_ (choice_set, D0)
register char *choice_set;
register AFFIX D0;
{
  register char *cp = choice_set;
  D0->t = c;
  D0->l = nil;
  D0->r = nil;
  while (*cp != '\0')
    if (*ip == *cp++)
    {
      *c++ = *ip++;
      *c++ = '\0';
      if (c > cstore_top)
        cstore_overflow ();
      if ((mip < ip) && (ip < limitip) && (ip > input))
      {
        mip = ip;
        change_line_file = true;
      }
      TRACE_RETURN (choice_set, true, "{", "}");
    }
  TRACE_RETURN (choice_set, false, "{", "}");
}
#endif

#ifdef DEX_CHOICE
int Dex_choice (choice_set, D0)
register char *choice_set;
register AFFIX D0;
{
  register char *cp = choice_set;
  D0->t = c;
  D0->l = nil;
  D0->r = nil;
  while ((*ip != '\0') && (*ip != *cp))
    if (*cp++ == '\0')
    {
      *c++ = *ip++;
      *c++ = '\0';
      if (c > cstore_top)
        cstore_overflow ();
      if ((mip < ip) && (ip < limitip) && (ip > input))
      {
        mip = ip;
        change_line_file = true;
      }
      TRACE_RETURN (choice_set, true, "^{", "}");
    }
  TRACE_RETURN (choice_set, false, "^{", "}");
}
#endif


#ifdef DX_STAR_CHOICE
int Dx_star_choice (choice_set, D0)
register char *choice_set;
register AFFIX D0;
{
  register char *cp = choice_set;
  D0->t = c;
  D0->l = nil;
  D0->r = nil;

  while (*cp != '\0')
    if (*ip == *cp++)
    {
      *c++ = *ip++;
      cp = choice_set;
    }
  *c++ = '\0';
  if (c > cstore_top)
    cstore_overflow ();
  if ((mip < ip) && (ip < limitip) && (ip > input))
  {
    mip = ip;
    change_line_file = true;
  }
  TRACE_RETURN (choice_set, true, "{", "}*");
}
#endif

#ifdef DX_STAR_EX_CHOICE
int Dx_star_ex_choice (choice_set, D0)
register char *choice_set;
register AFFIX D0;
{
  register char *cp = choice_set, *rip = ip;
  D0->t = c;
  D0->l = nil;
  D0->r = nil;
  if (*cp == '\0')
  {
    D0->t = rip;
    while (*rip++ != '\0');
    ip = rip - 1;
    TRACE_RETURN (choice_set, true, "^{", "}*");
  }
  else
  {
    register char *rc = c;
    D0->t = rc;
    while ((*rip != '\0') && (*rip != *cp))
      if (*cp++ == '\0')
      {
        *rc++ = *rip++;
        cp = choice_set;
      }
    *rc++ = '\0';
    ip = rip;
    c = rc;
    if (c > cstore_top)
      cstore_overflow ();
    if (rc > cstore_top)
      cstore_overflow ();
  }
  if ((mip < ip) && (ip < limitip) && (ip > input))
  {
    mip = ip;
    change_line_file = true;
  }
  TRACE_RETURN (choice_set, true, "!", "!*");
}
#endif


#ifdef DX_PLUS_CHOICE
int Dx_plus_choice (choice_set, D0)
register char *choice_set;
register AFFIX D0;
{
  register char *cp = choice_set, *sip = ip;
  D0->t = c;
  D0->l = nil;
  D0->r = nil;
  while (*cp != '\0')
    if (*ip == *cp++)
    {
      *c++ = *ip++;
      cp = choice_set;
    }
  if (ip == sip)
    TRACE_RETURN (choice_set, false, "{", "}+");
  *c++ = '\0';
  if (c > cstore_top)
    cstore_overflow ();
  if ((mip < ip) && (ip < limitip) && (ip > input))
  {
    mip = ip;
    change_line_file = true;
  }
  TRACE_RETURN (choice_set, true, "{", "}+");
}
#endif

#ifdef DX_PLUS_EX_CHOICE
int Dx_plus_ex_choice (choice_set, D0)
register char *choice_set;
register AFFIX D0;
{
  register char *cp = choice_set, *sip = ip;
  D0->t = c;
  D0->l = nil;
  D0->r = nil;

  if (*cp == '\0')
  {
    while (*ip != '\0')
      *c++ = *ip++;
    *c++ = '\0';
    if (c > cstore_top)
      cstore_overflow ();
    TRACE_RETURN (choice_set, true, "^{", "}+");
  }
  else
    while ((*ip != '\0') && (*ip != *cp))
      if (*cp++ == '\0')
      {
        *c++ = *ip++;
        cp = choice_set;
      }
  if (ip == sip)
    TRACE_RETURN (choice_set, false, "!", "!+");
  *c++ = '\0';
  if (c > cstore_top)
    cstore_overflow ();
  if ((mip < ip) && (ip < limitip) && (ip > input))
  {
    mip = ip;
    change_line_file = true;
  }
  TRACE_RETURN (choice_set, true, "!", "!+");
}
#endif


#ifdef UTERMINAL
void Uterminal_ ()
{
  register cont *rq = q;
  register char *A_0 = (rq + 0)->s;
  char *sip = ip;
  if (Dterminal_ (A_0))
  {
    q = rq + -2;
    (*(rq + -1)->q) ();
    rq = q + 1;
  }
  ip = sip;
  (rq + 0)->s = A_0;
  (rq + 1)->q = Uterminal_;
  q = rq + 1;
}
#endif


#ifdef UCHOICE
void Uchoice_ ()
{
  register cont *rq = q;
  register char *A_0 = (rq + 0)->s;
  register AFFIX A_1 = (rq - 1)->a;
  if (Dchoice_ (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q + 2;
  }
  (rq + -1)->a = A_1;
  (rq + 0)->s = A_0;
  (rq + 1)->q = Uchoice_;
  q = rq + 1;
}
#endif

#ifdef UEX_CHOICE
void Uex_choice ()
{
  register cont *rq = q;
  register char *A_0 = (rq + 0)->s;
  register AFFIX A_1 = (rq - 1)->a;
  if (Dex_choice (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q + 2;
  }
  (rq + -1)->a = A_1;
  (rq + 0)->s = A_0;
  (rq + 1)->q = Uex_choice;
  q = rq + 1;
}
#endif

#ifdef UX_STAR_EX_CHOICE
void Ux_star_ex_choice ()
{
  register cont *rq = q;
  register char *A_0 = (rq + 0)->s;
  register AFFIX A_1 = (rq - 1)->a;
  if (Dx_star_ex_choice (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q + 2;
  }
  (rq + -1)->a = A_1;
  (rq + 0)->s = A_0;
  (rq + 1)->q = Ux_star_ex_choice;
  q = rq + 1;
}
#endif


#ifdef UX_STAR_CHOICE
void Ux_star_choice ()
{
  register cont *rq = q;
  register char *A_0 = (rq + 0)->s;
  register AFFIX A_1 = (rq - 1)->a;
  if (Dx_star_choice (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q + 2;
  }
  (rq + -1)->a = A_1;
  (rq + 0)->s = A_0;
  (rq + 1)->q = Ux_star_choice;
  q = rq + 1;
}
#endif

#ifdef UX_PLUS_EX_CHOICE
void Ux_plus_ex_choice ()
{
  register cont *rq = q;
  register char *A_0 = (rq + 0)->s;
  register AFFIX A_1 = (rq - 1)->a;
  if (Dx_plus_ex_choice (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q + 2;
  }
  (rq + -1)->a = A_1;
  (rq + 0)->s = A_0;
  (rq + 1)->q = Ux_plus_ex_choice;
  q = rq + 1;
}
#endif

#ifdef UX_PLUS_CHOICE
void Ux_plus_choice ()
{
  register cont *rq = q;
  register char *A_0 = (rq + 0)->s;
  register AFFIX A_1 = (rq - 1)->a;
  if (Dx_plus_choice (A_0, A_1))
  {
    q = rq + -3;
    (*(rq + -2)->q) ();
    rq = q + 2;
  }
  (rq + -1)->a = A_1;
  (rq + 0)->s = A_0;
  (rq + 1)->q = Ux_plus_choice;
  q = rq + 1;
}
#endif
