
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef DCOMPUTE
int Dcompute (AFFIX I_0, AFFIX D_1) 
{
  char *expr = c;
  signed long long val = 0;
  if ((I_0->r == nil) && (I_0->l == nil))
  {
    expr = I_0->t;
  }
  else
  {
    sprinta (I_0);
    *c++ = '\0';
  }

  if (c +51 > cstore_top)
    cstore_overflow ();
  thischar = expr;
  expression (&val);
  D_1->t = c;
  D_1->l = nil;
  D_1->r = nil;
  (void) sprintf (c, "%lld", val);
  c += strlen(c)+1;
  return true;
}

int expression (signed long long *val) 
{
  if (simple_expression (val))
  {
    more_expression (val);
    return true;
  }
  return false;
}

int forced_expression (signed long long *val) 
{
  if (!expression (val))
    fprintf (stderr, "Compute: expression expected");
  return true;
}

int simple_expression (signed long long *val) 
{
  if (is_plus_symbol ())
  {
    forced_term (val);
    more_simple_expression (val);
    return true;
  }
  else if (is_minus_symbol ())
  {
    forced_term (val);
    *val = *val * -1;
    more_simple_expression (val);
    return true;
  }
  else if (term (val))
  {
    more_simple_expression (val);
    return true;
  }
  return false;
}

int forced_simple_expression (signed long long *val) 
{
  if (!simple_expression (val))
  {
    fprintf (stderr, "Compute: simple expression expected\n");
    *val = 0;
    return false;
  }
  return true;
}

int term (signed long long *val) 
{
  if (factor (val))
  {
    more_term (val);
    return true;
  }
  return false;
}

int forced_term (signed long long *val) 
{
  if (!term (val))
  {
    fprintf (stderr, "Compute: term expected\n");
    *val = 0;
  }
  return true;
}

int factor (signed long long *val) 
{
  if (is_number (val))
    return true;
  if (is_open_symbol ())
  {
    forced_expression (val);
    forced_close_symbol ();
    return true;
  }
  if (is_not_symbol ())
  {
    forced_expression (val);
    *val = !*val;
    return true;
  }
  return true;
}

long forced_close_symbol ()
{
  if (!is_close_symbol ())
    fprintf (stderr, "Compute: close symbol expected\n");
  return true;
}

int more_expression (signed long long *val) 
{
  signed long long v2;
  if (is_not_equal_symbol ())
  {
    forced_simple_expression (&v2);
    *val = (*val != v2);
  }
  else if (is_less_equal_symbol ())
  {
    forced_simple_expression (&v2);
    *val = (*val <= v2);
  }
  else if (is_greater_equal_symbol ())
  {
    forced_simple_expression (&v2);
    *val = (*val >= v2);
  }
  else if (is_equal_symbol ())
  {
    forced_simple_expression (&v2);
    *val = (*val == v2);
  }
  else if (is_not_equal_symbol ())
  {
    forced_simple_expression (&v2);
    *val = (*val != v2);
  }
  else if (is_less_symbol ())
  {
    forced_simple_expression (&v2);
    *val = (*val < v2);
  }
  else if (is_greater_symbol ())
  {
    forced_simple_expression (&v2);
    *val = (*val > v2);
  }
 return true;
}

int more_simple_expression (signed long long *val) 
{
  signed long long v2;
  if (is_plus_symbol ())
  {
    forced_term (&v2), *val += v2;
    more_simple_expression (val);
  }
  if (is_minus_symbol ())
  {
    forced_term (&v2);
    *val -= v2;
    more_simple_expression (val);
  }
 return true;
}

int more_term (signed long long *val) 
{
  signed long long v2;
  if (is_times_symbol ())
  {
    forced_term (&v2);
    *val *= v2;
    more_term (val);
  }
  if (is_divide_symbol ())
  {
    forced_term (&v2);
    *val /= v2;
    more_term (val);
  }
 return true;
}


long is_plus_symbol ()
{
  if (*thischar == '+')
  {
    thischar++;
    return true;
  }
  return false;
}

long is_minus_symbol ()
{
  if (*thischar == '-')
  {
    thischar++;
    return true;
  }
  return false;
}

int is_number (signed long long *val) 
{
  signed long long pval;
  char *st = thischar;
  if (isdigit (*thischar))
  {
    *val = 0;
    do
    {
      pval = *val;
      switch (*thischar)
      {
      case '0':
        *val *= 10;
        break;
      case '1':
        *val = *val * 10 + 1;
        break;
      case '2':
        *val = *val * 10 + 2;
        break;
      case '3':
        *val = *val * 10 + 3;
        break;
      case '4':
        *val = *val * 10 + 4;
        break;
      case '5':
        *val = *val * 10 + 5;
        break;
      case '6':
        *val = *val * 10 + 6;
        break;
      case '7':
        *val = *val * 10 + 7;
        break;
      case '8':
        *val = *val * 10 + 8;
        break;
      case '9':
        *val = *val * 10 + 9;
        break;
      }
      if (pval > *val)
      {
        fprintf (stderr, "glammar: compute integer overflow %.*s\n", (int)(thischar - st), st);
        exit (1);
      }
    }
    while (isdigit (*++thischar));
    return true;
  }
  return false;
}

long is_open_symbol ()
{
  if (*thischar == '(')
  {
    thischar++;
    return true;
  }
  return false;
}

long is_not_symbol ()
{
  if (*thischar == '~')
  {
    thischar++;
    return true;
  }
  return false;
}

long is_close_symbol ()
{
  if (*thischar == ')')
  {
    thischar++;
    return true;
  }
  return false;
}

long is_equal_symbol ()
{
  if (*thischar == '=')
  {
    thischar++;
    return true;
  }
  return false;
}

long is_not_equal_symbol ()
{
  if (*thischar == '#')
  {
    thischar++;
    return true;
  }
  else if (*thischar == '!' && thischar[1] == '=')
  {
    thischar++;
    thischar++;
    return true;
  }
  return false;
}

long is_less_equal_symbol ()
{
  if (*thischar == '<' && thischar[1] == '=')
  {
    thischar++;
    thischar++;
    return true;
  }
  return false;
}
long is_less_symbol ()
{
  if (*thischar == '<')
  {
    thischar++;
    return true;
  }
  return false;
}

long is_greater_symbol ()
{
  if (*thischar == '>')
  {
    thischar++;
    return true;
  }
  return false;
}
long is_greater_equal_symbol ()
{
  if (*thischar == '>' && thischar[1] == '=')
  {
    thischar++;
    thischar++;
    return true;
  }
  return false;
}

long is_times_symbol ()
{
  if (*thischar == '*')
  {
    thischar++;
    return true;
  }
  return false;
}

long is_divide_symbol ()
{
  if (*thischar == '/')
  {
    thischar++;
    return true;
  }
  return false;
}
#endif
