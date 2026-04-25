
/*

   This file is a part of the GLAMMAR source distribution 
   and therefore subjected to the copy notice below. 

   Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

*/
/* file: lattices */
#include "gg1.h"
#include "gg2.h"

long group = 0, count, top, unique_lattice_count = 0, prev_group = -1;
char repr_group[MAX_LATTICE_GROUPS][MAX_LATTICE_GROUPS_DIGITS], *lattice_substitution_name, *lattice_val_repr;
long max_group, prev_mem;
static long lline;
static long lat_error = 0;

/* exports
   void link_lattice ();
   void lattice_defined (long rule, long alt, long trm);
   void tr_lattice ();
   void lattice_used (long term, long alt);
   long lattice_top (long afx);
   */

static long  free_trm_lattice(long lat_trm, long lat_alt);
static void set_tr_lattice_group_val(long trm);
static void set_lattice_groups ();
static void set_group (long ag);
static void set_mem_lattice (long mem);
static long defining_occurence_of_lattice_affix (char *repr, long mem);
static void tr_lattice_term_to_afx (long trm, long alt, long member);
static void tr_lattice_to_afx (long trm, long alt, long member);
static void check_used_before_defined (long use_mem, long def_mem, long alt, char *repr);
static void add_lattice_to_afx_node (char *lattice, long term, long alt, int free);
static void append_lattice_to_afx_node (long lattice, long term, long mem, int free);
static void insert_lattice_to_afx_node (long lattice, long term, long mem, long alt, int free);
static void add_lattice_term_to_afx_node (long term, long alt, int free);
static void insert_lattice_term_to_afx_node (long term, long mem, long alt);
static long top_def (long ag);
static void code_group (long ag);
static void code_lattice_el (long mem);

static void set_lattice_groups () 
{
  long ag, lattice_count;
  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
  {
    if (DEF (ag) == -1)
    {
      count = 0;
      DEF (ag) = group;
      top = ag;
      (void) sprintf (repr_group[group], "%ld", group);
      set_group (ag);
      group += 1;
    }
  }
  max_group = group;

  if (verbose_flag)
    fprintf (stderr,"%ld lattice catogories\n",group); 
}


static void set_group (long ag) 
{
  long mem = SON (ag);
  if (REPR (mem) == REPR (ag))
  {
    NODENAME (mem) = 1 << count++;
    NODENAME (ag) = NODENAME (mem);
    DEF (mem) = -1;
    if (count >= sizeof(long)*7 -1)
    {
      fprintf (stderr, "lattice rule %s: too many lattice elements (%ld)", FREPR (ag),sizeof(long)*8);
      exit (1);
    }
    return;
  }
  for (mem = SON (ag); mem != nil; mem = BROTHER (mem))
  {
    long def = DEF (mem);
    if (def == -1)
      set_mem_lattice (mem);
    else if (DEF (def) == -1)
    {
      DEF (def) = group;
      set_group (def);
    }
    else if (DEF (def) != group)
    {
      fprintf (stderr, "lattice %s used in multiple groups %ld and %ld\n", FREPR (mem),group, DEF(def));
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

static void set_mem_lattice (long mem)
{
  register long ag, member;
  register char *repr = REPR (mem);
  for (ag = top; ag != nil; ag = BROTHER (ag))
    for (member = SON (ag); member != nil; member = BROTHER (member))
      if (mem == member)
      {
	NODENAME (mem) = 1 << count++;
	if (count > MAX_LATTICES_PER_GROUP )
	{
	  fprintf (stderr, "lattice rule %s produced too many lattices (>%ld)", FREPR (ag),sizeof(long)*8 -1);
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

void link_lattice () 
{
  register long ag, member;

  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
    for (member = SON (ag); member != nil; member = BROTHER (member))
    {
      register long r_ag = first_lattice;
      while (r_ag != nil && REPR (member) != REPR (r_ag))
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


void lattice_defined (long rule, long alt, long trm) 
{
  register long ag, l;
  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
    if (REPR (trm) == REPR (ag))
    {
      LATTICE_DEF (trm) = ag;
      return;
    }
  l = LINEX (trm,alt);
  if (separate_comp_flag)
    fprintf (stderr, "In %s:\n", PART (rule));
  if (first_lattice != nil)
  {
    fprintf (stderr, "line %ld: LATTICE affix %s not defined\n", l, FREPR (trm));
    fprintf (stderr, "line %ld: affix %s: FLOW SYMBOL expected\n", l, FREPR (trm));
  }
  else
    fprintf (stderr, "line %ld: affix %s: FLOW SYMBOL expected\n", l, FREPR (trm));
  exit (1);
}


long tr_rule;
void tr_lattice () 
{
  register long mem, afx, alt, rule;
  int alt_nr, afx_nr;
  if (first_lattice == nil) return;
  if (lat_trad_flag)
  {
    transformlattice = tltraditional;
    transformlatticeterm = tltraditionalterm;
  }
  for (rule = root; rule != init_one_star; rule = BROTHER (rule))
  {
    tr_rule = rule;
    lline = LINE ((rule));
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
      {
	if (LATTICE (afx))
	{
	  if (defining_occurence_of_lattice_affix (REPR(SON(afx)), SON (alt)))
	  {
	    tr_lattice_to_afx (SON (afx), alt, SON (alt));
	  }
	  else
	  {
	    tr_lattice_term_to_afx (SON (afx), alt, SON (alt));;
	  }
	}
      }
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      {
	long defm = DEF (mem);
	if (defm != transformlattice && defm != transformlatticeterm 
	    && defm != explintersect && defm != intersect)
	  for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
	  {
	    long trm = SON(afx);
	    long  latdef = LATTICE_DEF(trm);
	    int is_lat = LATTICE(afx);
	    if (is_lat &&  latdef != nil)
	      tr_lattice_to_afx (SON (afx), alt, mem);
	  }
      }
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
      {
	long trm;

	if (!LATTICE(afx))
	  for (trm = SON (afx); trm != nil; trm = BROTHER (trm))
	    if (free_trm_lattice(trm,alt))
	    {
	      set_tr_lattice_group_val(trm);

	      if (verbose_flag)
		fprintf(stderr, "free_trm_lattice_lhs_uage:rule %s, line %ld: term %s group %ld\n", 
		    REPR(rule),LINE(trm),REPR(trm), group);
	      /* all lattice references in terms for this alt already have been resolved
	       *  if they occurred as a lattice in this alt
	       * so this one is a free occurance */         
	      prev_mem = nil;
	      if (DERIVED(afx))
		insert_lattice_to_afx_node (LATTICE_DEF(trm), trm, SON(alt),alt,1);
	      else 
		add_lattice_to_afx_node (REPR(trm), trm, alt,1);
	      LATTICE_DEF(trm) = nil;
	    }

      }
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      {
	long trm;
	long defm = DEF(mem);
	prev_mem = nil;
	if (defm != transformlattice && defm != transformlatticeterm 
	    && defm != explintersect && defm != intersect)
	  for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
	  {
	    if (!LATTICE (afx))
	      for (trm = SON (afx); trm != nil; trm = BROTHER (trm))
		if (free_trm_lattice(trm,alt))
		{
		  set_tr_lattice_group_val(trm);
		  if (verbose_flag)
		    fprintf(stderr, "free_trm_lattice_rhs_uage:mem %s, line %ld: term %s group %ld\n", REPR(mem),LINE(trm),REPR(trm), group);
		  /* all lattice references in terms for this alt already have been resolved
		   *  if they occurred as a lattice in this alt
		   * so this one is a free occurance */         
		  if (DERIVED(afx))
		    insert_lattice_to_afx_node (LATTICE_DEF(trm), trm, SON(alt),alt,1);
		  else 
		    add_lattice_to_afx_node (REPR(trm), trm, alt,1);
		  LATTICE_DEF(trm) = nil;
		}

	  }
      }
    }
  }
  if (lat_error > 0)
    exit (1);
}


static long  free_trm_lattice(long lat_trm, long lat_alt)
{
  long afx,trm,mem;

  if ( LATTICE_DEF(lat_trm) == nil) return false;

  for (afx=AFFIXDEF(lat_alt); afx != nil; afx=BROTHER(afx))
  {
    if (!LATTICE(afx)) continue;
    if (REPR(SON(afx)) == REPR(lat_trm))
      return false;
  }
  for (mem=SON(lat_alt); mem != nil; mem=BROTHER(mem))
    for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
    {
      if (!LATTICE(afx)) continue;
      if (REPR(SON(afx)) == REPR(lat_trm))
	return false;
    }
  return true;
}  

static long defining_occurence_of_lattice_affix (char *repr, long mem) 
{
  long afx;
  for (; mem != nil; mem = BROTHER (mem))
    for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
      if ((LATTICE (afx)) && (repr == REPR (SON (afx))))
	return true;
  return false;
}

long done;

static void set_tr_lattice_group_val(long trm)
{

  long lat_val = NODENAME (LATTICE_DEF (trm));
  group = DEF (LATTICE_DEF (trm));

  if (charindex +100> maxchars)
    alloc_chartable ();

  lattice_substitution_name = &chartable[++charindex];
  charindex +=  sprintf (&chartable[charindex], "T_%ld", unique_lattice_count++) +1;

  lattice_val_repr = &chartable[++charindex];
  charindex +=  sprintf (&chartable[charindex], "%ld", lat_val) +1;
}

static void tr_lattice_term_to_afx (long trm, long alt, long member) 
{
  register long afx, mem, term;
  register char *repr = REPR (LATTICE_DEF (trm));

  set_tr_lattice_group_val(trm);
  done = false;

  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
    if (!LATTICE (afx))
      for (term = SON (afx); term != nil; term = BROTHER (term))
	if (repr == REPR (term))
	{
	  if (!done)
	  {
	    done = true;
	    add_lattice_term_to_afx_node (term, alt, 0);
	  }
	  else
	  {
	    REPR (term) = lattice_substitution_name;
	  }
	  LATTICE_DEF(term) = nil;
	}
  prev_mem = nil;
  for (mem = SON (alt); mem != nil; prev_mem = mem, mem = BROTHER (mem))
  {
    long defm = DEF(mem);
    if (defm != transformlattice && defm != transformlatticeterm 
	&& defm != explintersect && defm != intersect)
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
	      {
		REPR (term) = lattice_substitution_name;
	      }
	      LATTICE_DEF(term) = nil;
	    }
  }
}

static void tr_lattice_to_afx (long trm, long alt, long member) 
{
  register long afx, mem, term;
  register char *repr = REPR (LATTICE_DEF (trm));

  set_tr_lattice_group_val(trm);
  done = false;

  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
    if (!LATTICE (afx))
      for (term = SON (afx); term != nil; term = BROTHER (term))
	if (repr == REPR (term))
	{
	  if (!done)
	  {
	    done = true;
	    add_lattice_to_afx_node (repr, term, alt,0);
	  }
	  else
	  {
	    REPR (term) = lattice_substitution_name;
	  }
	  LATTICE_DEF(term) = nil;
	}
  prev_mem = nil;
  for (mem = SON (alt); mem != nil; prev_mem = mem, mem = BROTHER (mem))
  {
    long defm = DEF (mem);
    if (defm != transformlattice && defm != transformlatticeterm 
	&& defm != explintersect && defm != intersect)
      for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
	if (!LATTICE (afx))
	  for (term = SON (afx); term != nil; term = BROTHER (term))
	    if (repr == REPR (term))
	    {
	      if (!done)
	      {
		done = true;
		if (DERIVED (afx))
		  append_lattice_to_afx_node (trm, term, mem,0);
		else
		  insert_lattice_to_afx_node (trm, term, mem,alt,0);
		check_used_before_defined (mem, member, alt, repr);
	      }
	      else
	      {
		REPR (term) = lattice_substitution_name;
	      }
	      LATTICE_DEF(term) = nil;
	    }
  }
}

static void check_used_before_defined (long use_mem, long def_mem, long alt, char *repr) 
{
  long mem;
  for (mem = SON (alt); (mem != nil) && (def_mem != mem); mem = BROTHER (mem))
    if (use_mem == mem)
    {
      fprintf (stderr,
	  "line %ld in %s: cannot delay evaluation of lattice term '%s' (defined at '%s')\n",
	  lline, FREPR (use_mem), repr, FREPR (def_mem));
      lat_error += 1;
    }
}

/*  
 *  add node "transform  lattice (>group,>LATTICE, T_%ld.>).
 */

static void add_lattice_to_afx_node (char *lattice, long term, long alt, int free) 
{
  long b, mem;

  if (unique_lattice_count > 100)
  {
    fprintf(stderr, "glammar: too many lattice substitutions > 100\n");
    exit(1);
  }

  fprintf(stderr, "Line %ld, %s, Adding transform lattice for %s, %ld %s\n", LINE(tr_rule), FREPR(tr_rule), lattice, group, repr_group[group]);
  /*  T_%ld> */
  REPR (term) = lattice_substitution_name;
  newnode (affixnt, nil, nil, lattice_substitution_name);
  newnode (derived, nil, brother, "(nil)");
  b = brother;

  /*  LATTICE */
  if (free)
    newnode (affixtm, nil, nil, lattice_val_repr);
  else
    newnode (affixnt, nil, nil, lattice);
  FLAG_SET(brother, transform_lattice_arg);
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
 *  append  node "transform  lattice (>group,LATTICE, T_%ld.>).
 after mem
 */

static void append_lattice_to_afx_node (long lattice, long term, long mem, int free) 
{
  long b;
  REPR (term) = lattice_substitution_name;

  /*  T_%ld> */
  newnode (affixnt, nil, nil, lattice_substitution_name);
  newnode (derived, nil, brother, "(nil)");
  b = brother;

  /*  LATTICE */
  if (free)
    newnode (affixtm, nil, nil, lattice_val_repr);
  else
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

static void insert_lattice_to_afx_node (long lattice, long term, long mem, long alt, int free) 
{
  long b;
  REPR (term) = lattice_substitution_name;

  /*  T_%ld> */
  newnode (affixnt, nil, nil, lattice_substitution_name);
  newnode (derived, nil, brother, "(nil)");
  b = brother;

  /*  LATTICE */
  if (free)
    newnode (affixtm, nil, nil, lattice_val_repr);
  else
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
 *  add node "transform  lattice (>group,>"valLATTICE", T_%ld.>).
 */

static void add_lattice_term_to_afx_node (long term, long alt, int free)
{
  long b, mem;

  /*  T_%ld> */
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
 *  insert  node "transform  lattice (>group,>"valLATTICE", T_%ld.>).
 after mem
 */

static void insert_lattice_term_to_afx_node (long term, long mem, long alt) 
{
  long b;
  REPR (term) = lattice_substitution_name;

  /*  T_%ld> */
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

long el_count;

long conv_table () 
{
  long ag;
  if (!MARKED (root, docompile))
  {
    fprintf (output, "extern char *groups[%ld][%ld];\n", max_group,sizeof(long)*8);
    return 0;
  }
  group = -1;
  fprintf (output, "char *groups[%ld][%ld] = {\n", max_group,sizeof(long)*8);
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
	    ag = %lx, el_count =%lx\n", NODENAME (ag), el_count);

      fprintf (output, "},\n");
    }
  fprintf (output, "};\n");
  return 0;
}

static long top_def (long ag) 
{
  long g;

  for (g = first_lattice; g != nil; g = BROTHER (g))
    if (DEF (g) == DEF (ag))
      return g == ag;
  return false;
}

static void code_group (long ag) 
{
  long mem;
  for (mem = SON (ag); mem != nil; mem = BROTHER (mem))
  {
    long def = DEF (mem);
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


static void code_lattice_el (long mem)
{
  long ag, member;
  char *repr = REPR (mem);
  char *frepr = FREPR (mem);
  for (ag = top; ag != nil; ag = BROTHER (ag))
    for (member = SON (ag); member != nil; member = BROTHER (member))
      if (mem == member)
      {
	fprintf (output, "\"%s\",\n\t", frepr);
	return;
      }
      else if (REPR (member) == repr)
      {
	fprintf (output, "\"%s\",\n\t", frepr);
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

void lattice_used (long term, long alt) 
{
  long affix, mem, trm;

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

long lattice_top (long afx)
{
  register long ag, def, n, r;
  def = LATTICE_DEF (SON (afx));
  n = DEF (def);
  r = NODENAME (def);
  for (ag = first_lattice; ag != nil; ag = BROTHER (ag))
    if (DEF (ag) == n)
      return NODENAME (ag) == r;
  return false;
}
