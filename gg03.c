
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

/* hash names */
#include "gg1.h"
#include "gg2.h"
int nr_names = 0;
static char full_name[512];
initnametable ()
{
  register int count;
  count = 0;
  while (count < maxnt)
    nametable[count++] = NULL;
}

static int distinct_clashes = 0;
addname ()
{
  int count, limit;
  count = hashindex;
  limit = (hashindex - 1) & (maxnt - 1);
  if (count > maxnt)
  {
    fprintf (stderr, "compiler error in `addname': index too high (max = %d)\n", maxnt);
    fprintf (stderr, "   count = %d, limit = %d, line = %d\n ", count, limit, line);
    exit (3);
  }
  while (nametable[count] != NULL && count != limit)
  {
    if (mystrcmp (&(chartable[prevcharindex]), nametable[count]))
    {
      string = nametable[count];
      charindex = prevcharindex;
      return;
    }
    name_clashes += 1;
    if (++count == maxnt)
      count = 0;
  }
  if (count == limit)
  {
    fprintf (stderr, "compiler error: nametable overflow\n");
    fprintf (stderr, " -- index = %d, name = `%s', line = %d\n", count, &(chartable[prevcharindex]), line);
    exit (18);
  }
  if (count != hashindex)
    ++distinct_clashes;
  nr_names += 1;
  nametable[count] = &chartable[prevcharindex];
  string = nametable[count];
  chartable[charindex++] = '\0';
  {
    char *c;
    char pc = '\0';
    for (c = full_name; *c; c++)
    {
      if (*c == ' ' && pc == ' ')
        continue;
      pc = *c;
      chartable[charindex++] = pc;
    }
  }
  /* strcpy(chartable+charindex, full_name); */
  /* charindex += strlen(full_name); */
  if (chartable[charindex - 1] == ' ')
  {
    chartable[charindex - 1] = '\0';
  }
  else
  {
    chartable[charindex++] = '\0';
  }
  prevcharindex = charindex;
  if (verbose_flag)
    fprintf (stderr, "new name (%s,%s)\n", string, full_name);
  if (charindex >= maxchars - 256)
    alloc_chartable ();
  return;
}

mystrcmp (s1, s2)
register char *s1, *s2;
{
  while (*s1 == *s2++)
  {
    if (*s1++ == '\0')
      return true;
  }
  return false;
}


#define HASHC(c)      hashidx += (c<<(hashidx +c)%12)+c;
#define HASHTHIS      HASHC(thischar)

#define HASHTHIS1      hashidx = (hashidx << 5 +hashidx) + (thischar << (thischar&7))
#define HASHC1(c)      hashidx = (hashidx << 5 +hashidx) + (c << (c&7))
#define TESTletter  (isalpha (thischar) )
#define TESTmorename  (isalnum (thischar) )
name ()
{
  int curchar;
  int hashidx = 5381;
  char *fc = full_name;
  if (!TESTletter)
    return false;
  do
  {
    chartable[charindex++] = thischar;
    *fc++ = thischar;
    HASHTHIS;
    if (getnextchar () && TESTmorename)
      *fc++ = ' ';
  }
  while (TESTmorename);
  if (thischar == '\'')
  {
    chartable[charindex++] = thischar;
    *fc++ = thischar;
    HASHTHIS;
    getnextchar ();
  }
  hashidx += hashidx >> 12;
  hashidx += hashidx >> 24;
  hashidx %= nearest_prime_maxnt;
  hashindex = hashidx;
  chartable[charindex++] = '\0';
  *fc++ = '\0';
  addname ();
  return true;
}

#define USC  (underscore_allowed && (thischar == '_'))
#define ALFA  (isalpha(thischar))
#define ALNUM  (isalnum(thischar))
#define OPEN  (thischar == '(')
#define SEQ  (thischar == '*' || thischar == '+'  || thischar == '?'|| thischar == '\'')
#define TESTletter_open   (ALFA || OPEN || SEQ)
#define TESTmore_name_display   (ALNUM || OPEN || SEQ || USC)

name_display_mix ()
{
  int nmix = 0, hashidx = 5381;
  char nm[512];
  char fnm[512];
  char *fc = fnm;

  if (!TESTletter_open)
    return false;
  lastaffixtree = nil;
  brother = nil;
  while (TESTmore_name_display)
    if (mdisplay ())
    {
      if (fc != fnm && TESTmore_name_display)
        *fc++ = ' ';
    }
    else if (thischar == '\'')
    {
      *fc++ = '\'';
      nm[nmix++] = '_';
      nm[nmix++] = '0';
      HASHC ('\'');
      getnextchar ();
    }
    else if (thischar == '*')
    {
      *fc++ = '*';
      nm[nmix++] = '_';
      nm[nmix++] = '1';
      HASHC ('*');
      getnextchar ();
    }
    else if (thischar == '+')
    {
      *fc++ = '+';
      nm[nmix++] = '_';
      nm[nmix++] = '2';
      HASHC ('+');
      getnextchar ();
    }
    else if (thischar == '?')
    {
      *fc++ = '?';
      nm[nmix++] = '_';
      nm[nmix++] = '4';
      HASHC ('?');
      getnextchar ();
    }

    else
    {
      *fc++ = thischar;
      nm[nmix++] = thischar;
      HASHTHIS;
      if (getnextchar () && TESTmore_name_display)
        *fc++ = ' ';
    }
  hashidx += hashidx >> 12;
  hashidx += hashidx >> 24;
  hashidx %= nearest_prime_maxnt;
  hashindex = hashidx;
  nm[nmix] = '\0';
  if (*(fc - 1) == ' ')
    fc--;
  *fc++ = '\0';
  strcpy (full_name, fnm);
  for (nmix = 0; nm[nmix] != '\0';)
    chartable[charindex++] = nm[nmix++];
  chartable[charindex++] = '\0';
  addname ();
  ntname = string;
  return true;
}


mdisplay ()
{
  if (open_symbol ())
  {
    int first = lastaffixtree;
    brother = nil;
    affixes ();
    if (first == nil)
      lastaffixtree = brother;
    else
    {
      int afx;
      for (afx = first; BROTHER (afx) != nil; afx = BROTHER (afx));
      BROTHER (afx) = brother;
      lastaffixtree = first;
      brother = first;
    }
    if (close_symbol ());
    else
    {
      errmsg ("CLOSE symbol");
      skiptopoint_symbol ();
      rules (rnode);
      usefullerrmsg = false;
    }
    return true;
  }
  return false;
}

/* name_display()
{
   if (name() ) {
     ntname = string;
     displayoption();
     return true;
   }
   return false;
}
*/

alloc_chartable ()
{
  chartable = (char *) malloc ((maxchars + 132));
  charindex = 0;
  symbol_table_size += maxchars + 132;
  prevcharindex = 0;
  if (chartable == NULL)
  {
    fprintf (stderr, "glammar fatal msg: no %d bytes available for symbol table\n", maxchars);
    exit (1);
  }
  if (verbose_flag)
    fprintf (stderr, "alloc sym: %d bytes available for sym space\n", maxchars);
}

char *full_repr (char *s)
{
  if ((s > chartable) && (s < chartable + maxchars))
  {
    s += strlen (s);            /* full name follows name in symbol table */
    while (*s == '\0')
      s++;
  }
  if (!memcmp ("built_", s, 6))
    s += 6;
  return s;
}

dump_hash ()
{
  int c;
  fprintf (stderr, "%d hash mapping imperfections\n", distinct_clashes);
  for (c = 0; c < maxnt; c++)
    if (nametable[c])
      fprintf (stderr, "%05d %05d %s\n", get_hash (nametable[c]), c, nametable[c]);

    else
      fprintf (stderr, "--\n");
}

int get_hash (char *c)
{
  char t;
  int hashidx = 5381;
  while (t = *c++)
    HASHC (t);

  hashidx += hashidx >> 12;
  hashidx += hashidx >> 24;
  hashidx %= nearest_prime_maxnt;
  return hashidx;
}
