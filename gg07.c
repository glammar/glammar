
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

/* file: lattices */
#include "gg1.h"
#include "gg2.h"

int group = 0, count, top, unique_lattice_count = 0, prev_group = -1;
char repr_group[1000][6], *lattice_substitution_name, *lattice_val_repr;
int max_group, prev_mem;
static int lline;
static int lat_error = 0;

set_lattice_groups ()
{
  int ag;
  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
  {
    if (DEF (ag) == -1)
    {
      count = 0;
      DEF (ag) = group;
      top = ag;
      (void) sprintf (repr_group[group], "%d", group);
      set_group (ag);
      group += 1;
    }
  }
  max_group = group;

/*  fprintf (stderr,"%d lattice catogories\n",group); */
}


set_group (ag)
int ag;
{
  int mem = SON (ag);
  if (REPR (mem) == REPR (ag))
  {
    NODENAME (mem) = 1 << count++;
    NODENAME (ag) = NODENAME (mem);
    DEF (mem) = -1;
    if (count > 30)
    {
      fprintf (stderr, "lattice rule %s: to many lattice elements (>31)", FREPR (ag));
      exit (1);
    }
    return;
  }
  for (mem = SON (ag); mem != nil; mem = BROTHER (mem))
  {
    int def = DEF (mem);
    if (def == -1)
      set_mem_lattice (mem);
    else if (DEF (def) == -1)
    {
      DEF (def) = group;
      set_group (def);
    }
    else if (DEF (def) != group)
    {
      fprintf (stderr, "lattice rule %s used in different groups\n", FREPR (mem));
      exit (1);
    }
  }
  NODENAME (ag) = 0;
  for (mem = SON (ag); mem != nil; mem = BROTHER (mem))
    if (DEF (mem) == -1)
      NODENAME (ag) |= NODENAME (mem);
    else
      NODENAME (ag) |= NODENAME (DEF (mem));

}

set_mem_lattice (mem)
{
  register int ag, member;
  register char *repr = REPR (mem);
  for (ag = top; ag != nil; ag = BROTHER (ag))
    for (member = SON (ag); member != nil; member = BROTHER (member))
      if (mem == member)
      {
        NODENAME (mem) = 1 << count++;
        if (count > 30)
        {
          fprintf (stderr, "lattice rule %s: to much elements (>31)", FREPR (ag));
          exit (1);
        }
        return;
      }
      else if (REPR (member) == repr)
      {
        NODENAME (mem) = NODENAME (member);
        return;
      }
}

link_lattice ()
{
  register int ag, member;

  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
    for (member = SON (ag); member != nil; member = BROTHER (member))
    {
      register int r_ag = first_lattice;
      while (REPR (member) != REPR (r_ag))
      {
        if (r_ag == nil)
        {
          DEF (member) = -1;
          break;
        }
        else
          r_ag = BROTHER (r_ag);
      }
      if (r_ag != nil)
        DEF (member) = r_ag;
    }
  set_lattice_groups ();
}


lattice_defined (rule, alt, trm)
int alt, trm;
{
  register int ag, l;
  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
    if (REPR (trm) == REPR (ag))
    {
      LATTICE_DEF (trm) = ag;
      return;
    }
  l = LINE (alt);
  if (separate_comp_flag)
    fprintf (stderr, "In %s:\n", PART (rule));
  if (first_lattice != nil)
  {
    fprintf (stderr, "line %d: LATTICE affix %s not defined\n", l, FREPR (trm));
    fprintf (stderr, "line %d, affix %s: FLOW SYMBOL expected\n", l, FREPR (trm));
  }
  else
    fprintf (stderr, "line %d,affix %s: FLOW SYMBOL expected\n", l, FREPR (trm));
  exit (1);
}


tr_lattice ()
{
  register int ag, mem, afx, alt, rule;
  if (lat_trad_flag)
  {
    transformlattice = tltraditional;
    transformlatticeterm = tltraditionalterm;
  }
  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
  {
    lline = LINE ((rule));
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
        if (LATTICE (afx))
          if (defining_occurence_of_lattice_affix (REPR (SON (afx)), SON (alt)))
            tr_lattice_to_afx (SON (afx), alt, SON (alt));
          else
            tr_lattice_term_to_afx (SON (afx), alt, SON (alt));
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
        if (DEF (mem) != transformlattice)
          for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
            if (LATTICE (afx))
              tr_lattice_to_afx (SON (afx), alt, mem);
    }
  }
  if (lat_error > 0)
    exit (1);
}

int defining_occurence_of_lattice_affix (repr, mem)
char *repr;
int mem;
{
  int afx;
  for (; mem != nil; mem = BROTHER (mem))
    for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
      if ((LATTICE (afx)) && (repr == REPR (SON (afx))))
        return true;
  return false;
}

int done;

tr_lattice_term_to_afx (trm, alt, member)
int trm, alt, member;
{
  register int afx, mem, term;
  register char *repr = REPR (LATTICE_DEF (trm));
  int lat_val = NODENAME (LATTICE_DEF (trm));


  group = DEF (LATTICE_DEF (trm));
  lattice_substitution_name = &chartable[++charindex];
  (void) sprintf (&chartable[charindex], "T_%d", unique_lattice_count++);
  charindex += 10;
  if (charindex > maxchars)
    alloc_chartable ();
  lattice_val_repr = &chartable[++charindex];
  (void) sprintf (&chartable[charindex], "%d", lat_val);
  charindex += 20;
  if (charindex > maxchars)
    alloc_chartable ();

  done = false;
  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
    if (!LATTICE (afx))
      for (term = SON (afx); term != nil; term = BROTHER (term))
        if (repr == REPR (term))
        {
          if (!done)
          {
            done = true;
            add_lattice_term_to_afx_node (term, alt);
          }
          else
            REPR (term) = lattice_substitution_name;
        }
  prev_mem = nil;
  for (mem = SON (alt); mem != nil; prev_mem = mem, mem = BROTHER (mem))
    if (DEF (mem) != transformlattice)
      for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
        if (!LATTICE (afx))
          for (term = SON (afx); term != nil; term = BROTHER (term))
            if (repr == REPR (term))
            {
              if (!done)
              {
                done = true;
                insert_lattice_term_to_afx_node (term, mem, alt);
                check_used_before_defined (mem, member, alt, repr);
              }
              else
                REPR (term) = lattice_substitution_name;
            }
}

tr_lattice_to_afx (trm, alt, member)
int trm, alt, member;
{
  register int afx, mem, term;
  register char *repr = REPR (LATTICE_DEF (trm));

  group = DEF (LATTICE_DEF (trm));
  lattice_substitution_name = &chartable[++charindex];
  (void) sprintf (&chartable[charindex], "T_%d", unique_lattice_count++);
  charindex += 10;
  if (charindex > maxchars)
    alloc_chartable ();
  done = false;
  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
    if (!LATTICE (afx))
      for (term = SON (afx); term != nil; term = BROTHER (term))
        if (repr == REPR (term))
        {
          if (!done)
          {
            done = true;
            add_lattice_to_afx_node (repr, term, alt);
          }
          else
            REPR (term) = lattice_substitution_name;
        }
  prev_mem = nil;
  for (mem = SON (alt); mem != nil; prev_mem = mem, mem = BROTHER (mem))
    if (DEF (mem) != transformlattice)
      for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
        if (!LATTICE (afx))
          for (term = SON (afx); term != nil; term = BROTHER (term))
            if (repr == REPR (term))
            {
              if (!done)
              {
                done = true;
                if (DERIVED (afx))
                  append_lattice_to_afx_node (trm, term, mem);
                else
                  insert_lattice_to_afx_node (trm, term, mem, alt);
                check_used_before_defined (mem, member, alt, repr);
              }
              else
                REPR (term) = lattice_substitution_name;
            }
}

check_used_before_defined (use_mem, def_mem, alt, repr)
int use_mem, def_mem, alt;
char *repr;
{
  int mem;
  for (mem = SON (alt); (mem != nil) && (def_mem != mem); mem = BROTHER (mem))
    if (use_mem == mem)
    {
      fprintf (stderr,
               "line %d in %s: cannot delay evaluation of lattice term '%s' (defined at '%s')\n",
               lline, FREPR (use_mem), repr, FREPR (def_mem));
      lat_error += 1;
    }
}

/*  
 *  add node "transform  lattice (>group,>LATTICE, T_%d.>).
 */

add_lattice_to_afx_node (lattice, term, alt)
int term, alt;
char *lattice;
{
  int b, mem;

/*  T_%d> */
  REPR (term) = lattice_substitution_name;
  newnode (affixnt, nil, nil, lattice_substitution_name);
  newnode (derived, nil, brother, "(nil)");
  b = brother;

/*  LATTICE */
  newnode (affixnt, nil, nil, lattice);
  newnode (inherited, b, brother, "(nil)");
  b = brother;

/*  >"groupnr" */
  newnode (affixtm, nil, nil, repr_group[group]);
  newnode (inherited, b, brother, "(nil)");

/*  transform lattice */
  newdefnode (ntnode, nil, brother, transformlattice, REPR (transformlattice));

  mem = SON (alt);
  if (mem == nil)
  {
    SON (alt) = brother;
    return;
  }
  for (; BROTHER (mem) != nil; mem = BROTHER (mem));
  BROTHER (mem) = brother;
}

/*  
 *  append  node "transform  lattice (>group,LATTICE, T_%d.>).
     after mem
 */

append_lattice_to_afx_node (lattice, term, mem)
int lattice, term, mem;
{
  int b;
  REPR (term) = lattice_substitution_name;

/*  T_%d> */
  newnode (affixnt, nil, nil, lattice_substitution_name);
  newnode (derived, nil, brother, "(nil)");
  b = brother;

/*  LATTICE */
  newnode (affixnt, nil, nil, REPR (lattice));
  newnode (inherited, b, brother, "(nil)");
  b = brother;

/*  >"groupnr" */
  newnode (affixtm, nil, nil, repr_group[group]);
  newnode (inherited, b, brother, "(nil)");

/*  transform lattice */
  newdefnode (ntnode, nil, brother, transformlattice, REPR (transformlattice));
  b = BROTHER (mem);
  BROTHER (mem) = brother;
  BROTHER (brother) = b;

}

insert_lattice_to_afx_node (lattice, term, mem, alt)
int lattice, term, mem;
{
  int b;
  REPR (term) = lattice_substitution_name;

/*  T_%d> */
  newnode (affixnt, nil, nil, lattice_substitution_name);
  newnode (derived, nil, brother, "(nil)");
  b = brother;

/*  LATTICE */
  newnode (affixnt, nil, nil, REPR (lattice));
  newnode (inherited, b, brother, "(nil)");
  b = brother;

/*  >"groupnr" */
  newnode (affixtm, nil, nil, repr_group[group]);
  newnode (inherited, b, brother, "(nil)");

/*  transform lattice */
  newdefnode (ntnode, mem, brother, transformlattice, REPR (transformlattice));
  if (prev_mem == nil)
    SON (alt) = brother;
  else
    BROTHER (prev_mem) = brother;
  prev_mem = brother;
}

/*  
 *  add node "transform  lattice (>group,>"valLATTICE", T_%d.>).
 */

add_lattice_term_to_afx_node (term, alt)
int term, alt;
{
  int b, mem;

/*  T_%d> */
  REPR (term) = lattice_substitution_name;
  newnode (affixnt, nil, nil, lattice_substitution_name);
  newnode (derived, nil, brother, "(nil)");
  b = brother;

/*  LATTICE */
  newnode (affixtm, nil, nil, lattice_val_repr);
  newnode (inherited, b, brother, "(nil)");
  b = brother;

/*  >"groupnr" */
  newnode (affixtm, nil, nil, repr_group[group]);
  newnode (inherited, b, brother, "(nil)");

/*  transform lattice */
  newdefnode (ntnode, nil, brother, transformlatticeterm, REPR (transformlatticeterm));

  mem = SON (alt);
  if (mem == nil)
  {
    SON (alt) = brother;
    return;
  }
  for (; BROTHER (mem) != nil; mem = BROTHER (mem));
  BROTHER (mem) = brother;
}

/*  
 *  insert  node "transform  lattice (>group,>"valLATTICE", T_%d.>).
     after mem
 */

insert_lattice_term_to_afx_node (term, mem, alt)
int term, mem;
{
  int b;
  REPR (term) = lattice_substitution_name;

/*  T_%d> */
  newnode (affixnt, nil, nil, lattice_substitution_name);
  newnode (derived, nil, brother, "(nil)");
  b = brother;

/*  LATTICE */
  newnode (affixtm, nil, nil, lattice_val_repr);
  newnode (inherited, b, brother, "(nil)");
  b = brother;

/*  >"groupnr" */
  newnode (affixtm, nil, nil, repr_group[group]);
  newnode (inherited, b, brother, "(nil)");

/*  transform lattice */
  newdefnode (ntnode, mem, brother, transformlatticeterm, REPR (transformlatticeterm));
  if (prev_mem == nil)
    SON (alt) = brother;
  else
    BROTHER (prev_mem) = brother;
  prev_mem = brother;
}


/* code part */

int el_count;
conv_table ()
{
  int ag;
  if (!MARKED (root, docompile))
  {
    fprintf (output, "struct char_ptr_list { char *l[32];};\n");
    fprintf (output, "extern struct char_ptr_list groups[%d];\n", max_group);
    return;
  }
  group = -1;
  fprintf (output, "struct char_ptr_list { char *l[32];};\n");
  fprintf (output, "struct char_ptr_list groups[%d] = {\n", max_group);
  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
    if (top_def (ag))
    {
      fprintf (output, "{");
      group = DEF (ag);
      top = ag;
      count = 0;
      el_count = 1;
      code_group (ag);
      if (el_count - 1 != NODENAME (ag))
        fprintf (stderr, "glammar : lattice code generation error?\
           ag = %lx, el_count =%x\n", NODENAME (ag), el_count);

      fprintf (output, "},\n");
    }
  fprintf (output, "};\n");
}

int top_def (ag)
int ag;
{
  int g;

  for (g = first_lattice; g != nil; g = BROTHER (g))
    if (DEF (g) == DEF (ag))
      return g == ag;
  return false;
}

code_group (ag)
int ag;
{
  int mem;
  for (mem = SON (ag); mem != nil; mem = BROTHER (mem))
  {
    int def = DEF (mem);
    if (def == -1)
    {
      if (el_count == NODENAME (mem))
      {
        el_count <<= 1;
        code_lattice_el (mem);
      }
    }
    else
      code_group (def);
  }
}


code_lattice_el (mem)
{
  register int ag, member;
  register char *repr = REPR (mem);
  for (ag = top; ag != nil; ag = BROTHER (ag))
    for (member = SON (ag); member != nil; member = BROTHER (member))
      if (mem == member)
      {
        fprintf (output, "\"%s\",\n\t", repr);
        return;
      }
      else if (REPR (member) == repr)
      {
        fprintf (output, "\"%s\",\n\t", repr);
        return;
      }

}

/* 
 * Need to know if a lattice term occuring in the lefthandside
 * is used in the righthandside.
 * If it is then we can use the name of the term;
 * otherwise a terminal is made with its value converted 
 * to decimal as the terminals value.
 * Here we only make it a terminal; at code generation 
 * the value of the lattice-term is known and filled in.
 */

lattice_used (term, alt)
int term, alt;
{
  int affix, mem, trm;

  for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
  {
    for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
      if (LATTICE (affix))
      {
        trm = SON (affix);
        if (REPR (trm) == REPR (term))
          return;
      }
  }
  NODENAME (term) = affixtm;
}

int lattice_top (afx)
{
  register int ag, def, n, r;
  def = LATTICE_DEF (SON (afx));
  n = DEF (def);
  r = NODENAME (def);
  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
    if (DEF (ag) == n)
      return NODENAME (ag) == r;
  return false;
}
