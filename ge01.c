
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef USKIP
void Uskip_ ()
{
  q -= 1;
  (*(q + 1)->q) ();
}
#endif

#ifdef LKH
void lkh_epiloque ()
{
  char **lkh = (q--)->l;
  ip = *lkh;
  q -= 1;
  (*(q + 1)->q) ();
  (++q)->l = lkh;
  (++q)->q = lkh_epiloque;
}

void lkh_proloque ()
{
  char **lkh = (q--)->l;
  *lkh = ip;
  q -= 1;
  (*(q + 1)->q) ();
  (++q)->l = lkh;
  (++q)->q = lkh_proloque;
}
#endif

#ifdef UCUT
void Ucut ()
{
  int *cut_set = (q--)->i;
  *cut_set = 1;
  q -= 1;
  (*(q + 1)->q) ();
  (++q)->i = cut_set;
  (++q)->q = Ucut;
}
#endif

#ifdef UIDENTIFIER
void Uidentifier ()
{                               /* identifier */

  register cont *rq = q;
  char *rip = ip, *rc = c;
  register affix *A_0 = (rq + 0)->a;
  {
    if (Didentifier (A_0))
    {
      q = rq + -2;
      (*(rq + -1)->q) ();
      rq = q - -1;
      if ((mip < ip) && (ip < limitip) && (ip > input))
      {
        change_line_file = true;
        mip = ip;
        set_ip_start_pos = set_line_pos;
        set_ip_start_num = set_line_num;
      }
    }
    ip = rip;
    c = rc;
    if (rc > cstore_top)
      cstore_overflow ();
  }
  (rq + 0)->a = A_0;
  (rq + 1)->q = Uidentifier;
  q = rq + 1;
}
#endif
#define TESTletter  (isalpha (*rip) )
#define TESTmorename  (isalnum (*rip) ) || (*rip == '_') || (*rip == '\'')

#ifdef  DIDENTIFIER
int Didentifier (A_0)
register AFFIX A_0;
{
  register char *rip = ip, *rc = c;
  if (!TESTletter)
    return false;
  A_0->t = rc;
  A_0->r = nil;
  A_0->l = nil;
  do
  {
    *rc++ = *rip++;
  }
  while (TESTmorename);
  *rc++ = '\0';
  ip = rip;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return true;
}
#endif

#ifdef ULETTERDIGITSEQUENCE
void Uletterdigitsequence ()
{                               /* identifier */
  register cont *rq = q;
  char *rip = ip, *rc = c;
  register affix *A_0 = (rq + 0)->a;
  {
    if (Dletterdigitsequence (A_0))
    {
      q = rq + -2;
      (*(rq + -1)->q) ();
      rq = q - -1;
      if ((mip < ip) && (ip < limitip) && (ip > input))
      {
        change_line_file = true;
        mip = ip;
        set_ip_start_pos = set_line_pos;
        set_ip_start_num = set_line_num;
      }
    }
    ip = rip;
    c = rc;
    if (rc > cstore_top)
      cstore_overflow ();
  }
  (rq + 0)->a = A_0;
  (rq + 1)->q = Uletterdigitsequence;
  q = rq + 1;
}
#endif
#define TESTletter  (isalpha (*rip) )
#define TESTletdig  (isalnum (*rip) )

#ifdef DLETTERDIGITSEQUENCE
int Dletterdigitsequence (A_0)
register AFFIX A_0;
{
  register char *rip = ip, *rc = c;
  if (!TESTletter)
    return false;
  A_0->t = rc;
  A_0->r = nil;
  A_0->l = nil;
  do
  {
    *rc++ = *rip++;
  }
  while (TESTletdig);
  *rc++ = '\0';
  ip = rip;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  return true;
}
#endif

#ifdef UNLCR
void Unlcr ()
{                               /* nlcr */
  if ((*ip == '\0') || (Dterminal_ ("\n")))
  {
    CONTINUE;
  }
  (++q)->q = Unlcr;
}
#endif

#ifdef DNLCR
int Dnlcr ()
{
  return (*ip == '\0') || (*ip == '\n' && Dterminal_ ("\n")) || (*ip == '\r' && Dterminal_ ("\r\n"));
}
#endif
#ifdef UBSLASH
void Ubackslash ()
{                               /* quote */
  if (Dterminal_ ("\\"))
  {
    CONTINUE;
  }
  (++q)->q = Ubackslash;
}
#endif

#ifdef DBSLASH
int Dbackslash ()
{
  return (Dterminal_ ("\\"));
}
#endif

#ifdef UQUOTE
void Uquote ()
{                               /* quote */
  if (Dterminal_ ("\""))
  {
    CONTINUE;
  }
  (++q)->q = Uquote;
}
#endif

#ifdef DQUOTE
int Dquote ()
{
  return (Dterminal_ ("\""));
}
#endif

#ifdef UENDOFSENTENCE
void Uendofsentence ()
{                               /* endofsentence */
  if (*ip == '\0')
  {
    q -= 1;
    (*(*(q + 1)).q) ();
  }
  (++q)->q = Uendofsentence;
}
#endif

#ifdef DENDOFSENTENCE
int Dendofsentence ()
{
  return (*ip == '\0');
}
#endif
