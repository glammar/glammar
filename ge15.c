
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef DEVALAFFIX
int Devalaffix (A, B)
register AFFIX A, B;
{
  register char *rc = c;
  if (((A->r) == nil) && ((A->l) == nil))
    rc = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  if (c > cstore_top)
    cstore_overflow ();
  B->t = rc;
  B->r = nil;
  B->l = nil;
  return true;
}
#endif

#ifdef UEVALAFFIX
void Uevalaffix ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Devalaffix (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  c = rc;
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Uevalaffix;
  q = rq + 1;
}
#endif

#ifdef UFAIL
void Ufail ()
{                               /* fail */
  (++q)->q = Ufail;
}

#endif

#ifdef DFAIL
int Dfail ()
{
  return false;
}
#endif

#ifdef USETEXITCODE
void Usetexitcode ()
{                               /* set exit code */
  exit_code = 2;
  CONTINUE;
  (++q)->q = Usetexitcode;
}
#endif

#ifdef DSETEXITCODE
int Dsetexitcode ()
{
  exit_code = 2;
  return true;
}
#endif

#ifdef DGETENV
char *getenv ();
int Dgetenv (A, B)
register AFFIX A, B;
{
  register char *rc = c;
  if (((A->r) == nil) && ((A->l) == nil))
    rc = (A->t);
  else
  {
    sprinta (A);
    *c++ = '\0';
  }
  if (c > cstore_top)
    cstore_overflow ();
  B->t = getenv (rc);
  if (!B->t)
    return false;
  B->r = nil;
  B->l = nil;
  return true;
}
#endif

#ifdef UGETENV
void Ugetenv ()
{
  register cont *rq = q;
  register AFFIX A_1 = rq->a;
  register AFFIX A_0 = (rq - 1)->a;
  char *rc = c;
  if (Dgetenv (A_0, A_1))
  {
    q = rq - 3;
    (*(rq - 2)->q) ();
    rq = q + 2;
  }
  c = rc;
  (rq - 1)->a = A_0;
  rq->a = A_1;
  (rq + 1)->q = Ugetenv;
  q = rq + 1;
}
#endif

#ifdef DERRORMESSAGE
int x_errline (b, e)
char *b, *e;
{
  if (b > e)
  {
    *c++ = '\n';
    return;
  }
  if (e > (b + 75))
  {
    b = e - 75;
    *c++ = '@';
  }
  while (b <= e)
    *c++ = *b++;
}

int x_underline (b, e)
char *b, *e;
{
  if (b > e)
  {
    *c++ = '\n';
    return;
  }
  if (e > b + 75)
    b = e - 76;
  for (; b <= e; b++)
  {
    if (b == mip)
    {
      int o = *mip;
      o &= 255;
      sprintf (c, "^(%o)\n", o);
      while (*++c);
      break;
    }
    else if (*b == '\t')
      *c++ = '\t';
    else
      *c++ = '-';
  }
}

int x_errmsg ()
{
  char *beginmsg, *endmsg, *count;
  int linenumber = 1, cc_error = false;


  if (*mip == '\0')
  {
    sprintf (c, "*** Context error, after parsing");
    while (*++c);
    cc_error = true;
  }
  else
  {
    /* say line number */
    linenumber = set_ip_start_num;
    for (count = set_ip_start_pos; count < mip;)
      if (*count++ == '\n')
        linenumber += 1;
    if (*count == '\n')
      linenumber += 1;
    sprintf (c, "*** line %d", linenumber);
    while (*++c);
  }

  /* say file */

  if (*current_file_name != '\0')
  {
    sprintf (c, ", file: %s", current_file_name);
    while (*++c);
  }

  if (rmax > 0)
  {
    sprintf (c, ", while parsing \"%s\"", sv_error_msg);
    while (*++c);
  }




  if (!cc_error)
  {
    *c++ = ':';


    for (beginmsg = mip; *beginmsg != '\n'; beginmsg -= 1);
    /* print x_errline */
    endmsg = mip;
    for (endmsg = mip; *endmsg != '\n'; endmsg += 1);
    x_errline (beginmsg, endmsg);
    x_underline (beginmsg + 1, endmsg);
  }
  else
  {
    *c++ = '.';
    *c++ = '\n';
  }
  *c++ = '\0';
  return true;
}

int Derrormessage (A_0)         /* newname */
register AFFIX A_0;
{
  A_0->t = c;
  A_0->l = nil;
  A_0->r = nil;
  emsg_count += 1;
  x_errmsg ();
}
#endif


#ifdef UERRORMESSAGE
void Uerrormessage ()
{
  register cont *rq = q;
  register affix *A_0 = rq->a;
  char *rc = c;
  if (Derrormessage (A_0))
  {
    q = rq - 2;
    (*(rq - 1)->q) ();
    rq = q + 1;
    emsg_count -= 1;
  }
  rq->a = A_0;
  (rq + 1)->q = Uerrormessage;
  q = rq + 1;
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
}
#endif
