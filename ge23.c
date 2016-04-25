
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/

#include "ge1.h"
#ifdef SET_ERRMSG
set_errmsg (ntname)
char *ntname;
{
  if (rmax < level)
  {
    strcpy (sv_error_msg, ntname);
    rmax = level;
  }
}
#endif




#ifdef AFXCMP
#ifdef EIGHT_BYTE_ADDR
#define GET_CELL_IN_STRING(cell,str)\
{ register char *d = (char *) & cell;\
   register char *rc = str; \
   d [0] = rc [1] ; d[1] = rc [2]; d [2] = rc [3]; d[3] = rc[4]; \
   d [0+4] = rc [1+4] ; d[1+4] = rc [2+4]; d [2+4] = rc [3+4]; d[3+4] = rc[4+4]; }
#else
#define GET_CELL_IN_STRING(cell,str)\
{ register char *d = (char *) & cell;\
   register char *rc = str; \
   d [0] = rc [1] ; d[1] = rc [2]; d [2] = rc [3]; d[3] = rc[4]; }
#endif /* EIGHT_BYTE_ADDR */

afxcmp (x, y)
register char *x, *y;
{
  if ((x == undefined) || (y == undefined))
  {
    if (rmax > 1)
      fprintf (stderr, "==> %d :\n", rmax);
    fprintf (stderr, "%s: Glammar message:\n\
      In afxcmp (\"%s\",\"%s\"): trying to evaluate uninstantiated affix.\n", arg_zero, x, y);
    parsecount = 0;
    errmsg ();
    exit (1);
  }
  for (; *y != '\0';)
    if (*x == '\001' || *x == '\002')
    {
      if (*x == *y)
        if ((*(x + 1) == *(y + 1)) && (*(x + 2) == *(y + 2)) && (*(x + 3) == *(y + 3)) && (*(x + 4) == *(y + 4)))
        {
          y += 6;
          x += 6;
        }
        else if ((*x == '\001') && (*(x + 5) == '\001') && (*(y + 5) == '\001'))
        {
          affix *ax, *ay;
          GET_CELL_IN_STRING (ax, x);
          GET_CELL_IN_STRING (ay, y);
          if (Dequal (ax->l, ay->l) && Dequal (ax->r, ay->r))
          {
            y += 6;
            x += 6;
          }
          else
            return 1;

        }
        else
          return 1;
      else
        return 1;
    }
    else if (*x++ != *y++)
      return 1;
  if (*x == '\0')
    return 0;
  return 1;
}

lexafxcmp (x, y)
register char *x, *y;
{
  for (; *y != '\0';)
    if (*x == '\001' || *x == '\002')
    {
      if (*x == *y)
        if (*(x + 1) == *(y + 1))
          if (*(x + 2) == *(y + 2))
            if (*(x + 3) == *(y + 3))
              if (*(x + 4) == *(y + 4))
              {
                y += 6;
                x += 6;
              }
              else
                return *(x + 4) - *(y + 4);
            else
              return *(x + 3) - *(y + 3);
          else
            return *(x + 2) - *(y + 2);
        else
          return *(x + 1) - *(y + 1);
      else
        return *x - *y;
    }
    else if (*x++ != *y++)
      return *(x - 1) - *(y - 1);
  return *x;
}
#endif
