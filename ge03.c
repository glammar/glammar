
/*
    This file is a part of the GLAMMAR source Distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef USETLINEFILE
void Usetlinefile ()
{
  register AFFIX F = (q--)->a;
  register AFFIX L = (q--)->a;
  register char *rc = c;
  int line_save = set_line_num;
  char *pos_save = set_line_pos;
  char *file_save = set_file_name;
  if ((F->r == nil) && (F->l == nil));
  else
  {
    sprinta (F);
    F->t = rc;
    F->l = nil;
    F->r = nil;
    *c++ = '\0';
  }
  if ((L->r == nil) && (L->l == nil));
  else
  {
    char *rrc = c;
    sprinta (L);
    L->t = rrc;
    L->l = nil;
    L->r = nil;
    *c++ = '\0';
  }
  set_file_name = F->t;
  set_line_pos = ip;
  set_line_num = atoi (L->t);
  q -= 1;
  (*(*(q + 1)).q) ();
  c = rc;
  if (rc > cstore_top)
    cstore_overflow ();
  if (change_line_file)
  {
    set_ip_start_num = set_line_num;
    set_ip_start_pos = set_line_pos;
    strcpy (current_file_name, F->t);
    change_line_file = false;
  }
  set_line_num = line_save;
  set_line_pos = pos_save;
  set_file_name = file_save;
  (++q)->a = L;
  (++q)->a = F;
  (++q)->q = Usetlinefile;
}
#endif

#ifdef UGETLINEFILE
void Ugetlinefile ()
{
  register AFFIX F = (q--)->a;
  register AFFIX L = (q--)->a;
  char line_nm[8];
  register char *rip = set_line_pos;
  int line = set_line_num;
  int save_l = line;
  char *save_p = rip;
  L->t = line_nm;
  L->r = nil;
  L->l = nil;
  F->t = set_file_name;
  F->r = nil;
  F->l = nil;
  while (rip < ip)
    if (*rip++ == '\n')
      line += 1;
  (void) sprintf (line_nm, "%d", line);
  q -= 1;
  (*(*(q + 1)).q) ();
  set_line_pos = save_p;
  set_line_num = save_l;
  (++q)->a = L;
  (++q)->a = F;
  (++q)->q = Ugetlinefile;
}
#endif

#ifdef DSETLINEFILE
int Dsetlinefile (L, F)
register AFFIX L, F;
{
  register char *ll, *ff;
  if ((F->r == nil) && (F->l == nil))
    ff = F->t;
  else
  {
    ff = c;
    sprinta (F);
    *c++ = '\0';
  }
  if ((L->r == nil) && (L->l == nil))
    ll = L->t;
  else
  {
    ll = c;
    sprinta (L);
    *c++ = '\0';
  }
  set_file_name = ff;
  set_line_pos = ip;
  set_line_num = atoi (ll);
  strcpy (current_file_name, ff);
  return true;
}
#endif

#ifdef DGETLINEFILE
int Dgetlinefile (L, F)
register AFFIX L, F;
{
  register char *rip = set_line_pos;
  int line = set_line_num;
  L->t = c;
  L->r = nil;
  L->l = nil;
  F->t = set_file_name;
  F->r = nil;
  F->l = nil;
  while (rip < ip)
    if (*rip++ == '\n')
      line += 1;
  (void) sprintf (c, "%d", line);
  c += 8;
  return true;
}
#endif

#ifdef DCHARNUM
int Dcharnum (D_0)
register AFFIX D_0;
{                               /* charnum */
  int i = 0;
  D_0->t = c;
  D_0->l = nil;
  D_0->r = nil;
  {
    register char *rip = ip;
    for (; *--rip != '\n';);

    for (; ++rip < ip;)
      if (*rip >= ' ')
        i++;
      else if (*rip == '\t')
        i = (i + tabstop) & ~(tabstop - 1);
  }
  (void) sprintf (c, "%d", i);
  c += 8;
  return true;
}
#endif

#ifdef SETTABSTOP
int Dsettabstop (F)
register AFFIX F;
{
  register char *ff;
  if ((F->r == nil) && (F->l == nil))
    ff = F->t;
  else
  {
    ff = c;
    sprinta (F);
    *c++ = '\0';
  }
  tabstop = atoi (ff);
}
#endif

#ifdef DSOMENAME
int some_count = 0;
int Dsomename (A_0)             /* newname */
register AFFIX A_0;
{
  long lt = (long) c;
  int lta = (int) 'A';
  A_0->t = c;
  A_0->l = nil;
  A_0->r = nil;
  if (!some_count)
    some_count = clock ();
  (void) sprintf (c, "%c%c%X\0", (int) (lta + ((lt >> 3) & 15)), (int) (lta + ((lt >> 6) & 15)), some_count++);
  c += 12;
  return true;
}
#endif
