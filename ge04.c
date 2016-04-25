
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef UADDTO
void Uaddto ()
{                               /* addto */
  struct eaglist item;
  register AFFIX A = (*(q - 2)).a, B = (*(q - 1)).a;
  char *csave = c;
  affix list;
  item.table = A;
  item.key = B;
  item.value = (*q).a;
  item.next = nil;
  item.left = nil;
  item.right = nil;
  q -= 4;
  SAVE (list, A);
  {
    struct eaglist *link = root, *prev = nil;
    register char *xs = c, *ys;
    char *sxs;
    int left = true;
    if ((list.r == nil) && (list.l == nil));
    else
    {
      sprinta (A);
      A->t = xs;
      A->l = nil;
      A->r = nil;
      *c++ = '\0';
    }
    xs = A->t;
    for (sxs = xs; link != nil; xs = sxs)
    {
      prev = link;
      ys = link->table->t;
      left = afxcmp (xs, ys);
      if (left == 0)
      {
        item.next = link->next;
        link->next = (struct eaglist *) &item;
        (*(*(q + 1)).q) ();
        link->next = item.next;
        goto done;
      }
      if (left < 0)
        link = link->left;
      else
        link = link->right;
    }
    if (left < 0)
    {
      prev->left = (struct eaglist *) &item;
      (*(*(q + 1)).q) ();
      prev->left = nil;
    }
    else
    {
      prev->right = (struct eaglist *) &item;
      (*(*(q + 1)).q) ();
      prev->right = nil;
    }
  }
done:
  RESTORE (A, list);
  (++q)->a = A;
  (++q)->a = B;
  (++q)->a = item.value;
  (++q)->q = Uaddto;
  c = csave;
}
#endif

#ifdef DADDTO
int Daddto (A, B, C)
register AFFIX A, B, C;
{
  struct eaglist *item;
  item = (struct eaglist *) malloc (sizeof (struct eaglist));
  item->table = A;
  item->key = B;
  item->value = C;
  item->next = nil;
  item->left = nil;
  item->right = nil;
  {
    struct eaglist *link = root, *prev = nil;
    register char *xs = c, *ys;
    char *sxs;
    int left = true;
    if ((A->r == nil) && (A->l == nil))
      xs = A->t;
    else
    {
      sprinta (A);
      *c++ = '\0';
      item->table = ++af;
      af->t = xs;
      af->r = nil;
      af->l = nil;
    }
    for (sxs = xs; link != nil; xs = sxs)
    {
      prev = link;
      ys = link->table->t;
      left = afxcmp (xs, ys);
      if (left == 0)
      {
        item->next = link->next;
        link->next = (struct eaglist *) item;
        return true;
      }
      if (left < 0)
        link = link->left;
      else
        link = link->right;
    }
    if (left < 0)
    {
      prev->left = (struct eaglist *) item;
    }
    else
    {
      prev->right = (struct eaglist *) item;
    }
  }
  return true;
}
#endif

#ifdef DVALUEOF
int Dvalueof (A, B, C)
register AFFIX A, B, C;
{
  register struct eaglist *link = root;
  register char *xs = c, *ys;
  char *sxs, *list, *key;
  int left;
  C->t = empty;
  C->l = nil;
  C->r = nil;
  if ((B->r == nil) && (B->l == nil))
    key = B->t;
  else
  {
    key = c;
    sprinta (B);
    *c++ = '\0';
  }
  xs = c;
  if ((A->r == nil) && (A->l == nil))
    list = A->t;
  else
  {
    list = c;
    sprinta (A);
    *c++ = '\0';
  }
  xs = list;
  ys = key;
  for (sxs = xs; link != nil; xs = sxs)
  {
    ys = link->table->t;
    left = afxcmp (xs, ys);
    if (left == 0)
    {
      for (; link != nil; link = link->next)
      {
        affix *lB = link->key;
        ys = c;
        xs = key;
        if ((lB->r == nil) && (lB->l == nil))
          ys = lB->t;
        else
        {
          sprinta (link->key);
          *c++ = '\0';
        }
        while (*xs == *ys++)
          if (*xs++ == '\0')
          {
            MAKE (C, link->value);
            return true;
          }
      }
      return true;
    }
    else if (left < 0)
      link = link->left;
    else
      link = link->right;
  }
  return true;
}
#endif

#ifdef DINCLUDES
int Dincludes (A, B)
register AFFIX A, B;
{
  register struct eaglist *link = root;
  register char *xs = c, *ys, *key, *list;
  char *sxs;
  int left;
  if ((B->r == nil) && (B->l == nil))
    key = B->t;
  else
  {
    key = c;
    sprinta (B);
    *c++ = '\0';
  }
  xs = c;
  if ((A->r == nil) && (A->l == nil))
    list = A->t;
  else
  {
    list = c;
    sprinta (A);
    *c++ = '\0';
  }
  xs = list;
  ys = key;
  for (sxs = xs; link != nil; xs = sxs)
  {
    ys = link->table->t;
    left = afxcmp (xs, ys);
    if (left == 0)
    {
      for (; link != nil; link = link->next)
      {
        affix *lB = link->key;
        ys = c;
        xs = key;
        if ((lB->r == nil) && (lB->l == nil))
          ys = lB->t;
        else
        {
          sprinta (link->key);
          *c++ = '\0';
        }
        while (*xs == *ys++)
          if (*xs++ == '\0')
          {
            return true;
          }
      }
      return false;
    }
    else if (left < 0)
      link = link->left;
    else
      link = link->right;
  }
  return false;
}
#endif

#ifdef DEXCLUDES
int Dexcludes (A, B)
register AFFIX A, B;
{
  return !Dincludes (A, B);
}
#endif

#ifdef DLOOKUP
extern AFFIX fist_meta, last_meta;
int Dlookup (A, B, C)
register AFFIX A, B, C;
{
  register struct eaglist *link = root;
  register char *xs = c, *ys, *key, *list;
  char *sxs;
  int left;
  if ((B->r == nil) && (B->l == nil))
    key = B->t;
  else
  {
    key = c;
    sprinta (B);
    *c++ = '\0';
  }
  xs = c;
  if ((A->r == nil) && (A->l == nil))
    list = A->t;
  else
  {
    list = c;
    sprinta (A);
    *c++ = '\0';
  }
  xs = list;
  ys = key;
  for (sxs = xs; link != nil; xs = sxs)
  {
    ys = link->table->t;
    left = afxcmp (xs, ys);
    if (left == 0)
    {
      for (; link != nil; link = link->next)
      {
        affix *lB = link->key;
        ys = c;
        xs = key;
        if ((lB->r == nil) && (lB->l == nil))
          ys = lB->t;
        else
        {
          sprinta (link->key);
          *c++ = '\0';
        }
        while (*xs == *ys++)
          if (*xs++ == '\0')
          {
            if (link->value <= last_meta && link->value >= first_meta)
            {
              SHARE (C, link->value);
            }
            else
            {
              MAKE (C, link->value);
            }
            return true;
          }
      }
      return false;
    }
    else if (left < 0)
      link = link->left;
    else
      link = link->right;
  }
  return false;
}
#endif

#ifdef DCONTENTSOF
int Dcontentsof (A, B, C, D)
register AFFIX A, B, C, D;
{
  register struct eaglist *link = root;
  register char *xs = c, *ys, *list;
  register AFFIX raf = af;
  char *sxs;
  int left, acount = 0;
  xs = c;
  if ((A->r == nil) && (A->l == nil))
    list = A->t;
  else
  {
    list = c;
    sprinta (A);
    *c++ = '\0';
  }
  xs = list;
  D->t = empty;
  D->r = nil;
  D->l = nil;
  for (sxs = xs; link != nil; xs = sxs)
  {
    ys = link->table->t;
    left = afxcmp (xs, ys);
    if (left == 0)
    {
      affix *Last;
      for (; link != nil; link = link->next)
      {
        register AFFIX E = raf + ++acount;
        D->r = raf + ++acount;
        D = D->r;
        D->t = empty;
        D->r = nil;
        D->l = E;
        D->r = raf + ++acount;
        D = D->r;
        D->t = empty;
        D->r = nil;
        D->l = B;
        Last = D;
        E->t = empty;
        E->l = link->key;
        E->r = raf + ++acount;
        E = E->r;
        E->t = empty;
        E->l = C;
        E->r = link->value;
      }
      af += acount;
      Last->l = nil;
      return true;
    }
    else if (left < 0)
      link = link->left;
    else
      link = link->right;
  }
  return false;
}
#endif

#ifdef DSIZE
int Dsize (A, B)
register AFFIX A, B;
{
  register struct eaglist *link = root;
  register char *xs = c, *list;
  char *sxs, *ys;
  int left, count = 0;
  xs = c;
  if ((A->r == nil) && (A->l == nil))
    list = A->t;
  else
  {
    list = c;
    sprinta (A);
    *c++ = '\0';
  }
  xs = list;
  B->t = c;
  B->r = nil;
  B->l = nil;
  for (sxs = xs; link != nil; xs = sxs)
  {
    ys = link->table->t;
    left = afxcmp (xs, ys);
    if (left == 0)
    {
      for (; link != nil; link = link->next)
        if (link->key != nil)
          count += 1;
      break;
    }
    else if (left < 0)
      link = link->left;
    else
      link = link->right;
  }
  (void) sprintf (c, "%d", count);
  c += 10;
  return true;
}
#endif
#ifdef UDELETE
void Udelete ()
{                               /* delete */
  AFFIX C = (q--)->a;
  AFFIX B = (q--)->a;
  AFFIX A = (q--)->a;
  AFFIX val = C, key = B;
  char *csave = c, *sxs;
  affix lst;
  register struct eaglist *link = root, *top;
  register char *xs = c, *ys;
  int left;
  SAVE (lst, A);
  if ((lst.r == nil) && (lst.l == nil));
  else
  {
    sprinta (A);
    A->t = xs;
    A->l = nil;
    A->r = nil;
    *c++ = '\0';
  }
  xs = A->t;
  for (sxs = xs; link != nil; xs = sxs)
  {
    ys = link->table->t;
    left = afxcmp (xs, ys);
    if (left == 0)
    {
      for (top = link; link != nil;)
      {
        COPY (key, link->key);
        COPY (val, link->value);
        if (link->next != nil)
        {
          link->key = link->next->key;
          link->value = link->next->value;
          link->next = link->next->next;
        }
        else
        {
          top->key = nil;
          top->value = nil;
          top->next = nil;
          link = nil;
        }
        q -= 1;
        (*(*(q + 1)).q) ();
      }
      goto done;
    }
    else if (left < 0)
      link = link->left;
    else
      link = link->right;
  }
done:
  RESTORE (A, lst);
  (++q)->a = A;
  (++q)->a = B;
  (++q)->a = C;
  (++q)->q = Udelete;
  c = csave;
}
#endif
#ifdef USELECT
void Uselect ()
{                               /* select */
  AFFIX C = (q--)->a;
  AFFIX B = (q--)->a;
  AFFIX A = (q--)->a;
  AFFIX val = C, key = B;
  char *csave = c, *sxs;
  affix lst;
  register struct eaglist *link = root;
  register char *xs = c, *ys;
  int left;
  SAVE (lst, A);
  if ((lst.r == nil) && (lst.l == nil));
  else
  {
    sprinta (A);
    A->t = xs;
    A->l = nil;
    A->r = nil;
    *c++ = '\0';
  }
  xs = A->t;
  for (sxs = xs; link != nil; xs = sxs)
  {
    ys = link->table->t;
    left = afxcmp (xs, ys);
    if (left == 0)
    {
      for (; link != nil; link = link->next)
        if (link->key != nil)
        {
          MAKE (key, link->key);
          MAKE (val, link->value);
          q -= 1;
          (*(*(q + 1)).q) ();
        }
      goto done;
    }
    else if (left < 0)
      link = link->left;
    else
      link = link->right;
  }
done:
  RESTORE (A, lst);
  (++q)->a = A;
  (++q)->a = B;
  (++q)->a = C;
  (++q)->q = Uselect;
  c = csave;
}
#endif
