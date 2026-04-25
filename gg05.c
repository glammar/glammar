
/*

   This file is a part of the GLAMMAR source distribution 
   and therefore subjected to the copy notice below. 

   Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

*/
/*  check context conditions */
#include "gg1.h"
#include "gg2.h"
#ifdef SEQUENT
#  include <sys/types.h>
#  include <sys/times.h>
#else
#if defined LINUX  || defined LINUX64
#  include <time.h>
#  include <sys/time.h>
#else
#  include <sys/time.h>
#endif /* LINUX || LINUX64 */
#endif /* SEQUENT */
time_t myvclock, *myclock;
char generation_time[64];
char generation_date[64];
long error = false;


/* exports 
   void linktodefs ();
   void meta2init ();
   void check_metagrammar ();
   */

static void newtop ();
static void lhs_affixes_defined (long alt, long rule);
static void rhs_members_affixes_defined (long alt, long rule);
static void multiple_defined (long rule);
static void delete_multiple_defined_hyperrules ();
static void check_affix_def (long rule);
static long definedin (long term, long alt);
static long leftindef (long term, long alt);
static long rightdvdef (long term, long alt);
static void meta_defined (long trm);
static void link_supernt ();
static void remove_multiple_defined_meta_affixes ();
static void overwrite_grammar_generation_meta_rules ();
static long meta_used (char *repr);
static void meta2initrule (long metarule);
static void meta2mintrule (long metarule);
static void eval_one_star_node ();
static void evalmetarule (long metarule);
static void check_metarule (long mrule);
static long metadefinition_of (long def_mrule,long term);


void linktodefs () 
{
  register long rule, alt, afx;
  newtop ();
  alt = SON (root);
  afx = AFFIXDEF (alt);
  if ((afx == nil) || (NODENAME (afx) != derived) || (BROTHER (afx) != nil))
  {
    error = true;
    if (input_from_partlist)
      fprintf (stderr, "In %s:\n", PART (root));
    fprintf (stderr, "line %ld: start rule should have at most ONE derived affix\n", LINE (root));
  }

  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
  {
    long alt = SON (rule);
    multiple_defined (rule);
    for (; alt != nil; alt = BROTHER (alt))
    {
      lhs_affixes_defined (alt, rule);
      rhs_members_affixes_defined (alt, rule);
    }
    check_affix_def (rule);
  }
  delete_multiple_defined_hyperrules ();
  if (error)
    if (!tree_flag)
      exit (10);
}

static void newtop () 
{
  if (AFFIXDEF (SON (root)) == nil)
  {
    long root_mem = nil;

    if (root != laststdpred)
    {
      newdefnode (ntnode, nil, nil, root, REPR (root));
      root_mem = brother;
    }

    newnode (affixtm, nil, nil, "");
    newnode (derived, nil, brother, "(nil)");
    newnode (brother, nil, root_mem, "(nil)");
    newrulenode (NODENAME (root), root, brother, 0, PART (root), "newroot");
    OLDSUM (brother) = OLDSUM (root);
    root = brother;
  }
}

static void lhs_affixes_defined (long alt, long rule) 
{
  register long afx, trm;
  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
    if (NODENAME (afx) == derived)
    {
      for (trm = SON (afx); trm != nil; trm = BROTHER (trm))
	if ((APPLY_BOUND_AFFIX (trm)) && (!definedin (trm, alt)) && LATTICE_DEF(trm) == nil)
	{
	  if (input_from_partlist)
	    fprintf (stderr, "In %s:\n", PART (rule));
	  fprintf (stderr,
	      "line %ld: `%s': affix `%s' in left-hand side is not defined\n",
	      LINEX(trm,alt), FREPR (rule), FREPR (trm));
	  error = true;
	}
    }
    else if (LATTICE (afx))
      if (!tree_flag)
	lattice_used (SON (afx), alt);
}

static void rhs_members_affixes_defined (long alt, long rule)
{
  register long afx, trm, member, rrule, prev;
  prev = nil;
  for (member = SON (alt); member != nil; member = BROTHER (member))
  {
    if (TERMINAL (member))
      DEF (member) = nlcr;
    else
    {
      for (afx = AFFIXTREE (member); afx != nil; afx = BROTHER (afx))
      {
	if (NODENAME (afx) == inherited)
	  for (trm = SON (afx); trm != nil; trm = BROTHER (trm))
	    if ((APPLY_BOUND_AFFIX (trm)) && (!definedin (trm, alt)) && LATTICE_DEF(trm) == nil)
	    {
	      if (input_from_partlist)
		fprintf (stderr, "In %s:\n", PART (rule));
	      fprintf (stderr,
		  "line %ld: `%s': `%s': affix `%s' is not defined\n",
		  LINEX(trm,alt), FREPR (rule), FREPR (member), FREPR (trm));
	      error = true;
	    }
      }
      if (REPR (member) == redirect)
      {
	DEF (member) = nlcr;
	continue;
      }

      for (rrule = root; rrule != nil && REPR (member) != REPR (rrule); rrule = BROTHER (rrule));

      if (rrule == nil)
      {
	if (input_from_partlist)
	  fprintf(stderr, "In %s:\n", PART (rule));
	fprintf (stderr, "line %ld: lhs `%s': member `%s' is not defined\n",LINEX(member,alt), FREPR (rule), FREPR (member));
	error = true;
	break;
      }
      if (!valid_overloaded_affixtype (AFFIXDEF (SON (rrule)), AFFIXTREE (member), member))
      {

	if (input_from_partlist)
	  fprintf (stderr, "In %s:\n", PART (rule));
	fprintf (stderr, "line %ld: `%s',`%s': ", LINEX(member,alt), FREPR (rule), FREPR (member));
	affixerrmsg (AFFIXDEF (SON (rrule)), AFFIXTREE (member));
	error = true;
      }
      DEF (member) = rrule;
      if (rrule == add_to)
	add_to_used = 1;
      SET (rrule, rule_used);
      if ((rrule == cut) && (BROTHER (alt) == nil))
      {

	while (BROTHER (member) != nil && DEF (BROTHER (member)) == cut)
	  member = BROTHER (member);
	if (prev == nil)
	  SON (alt) = BROTHER (member);
	else
	  BROTHER (prev) = BROTHER (member);
      }
      if (PART (rule) != PART (rrule) && input_from_partlist)
	if (!MARKED (rrule, external))
	{
	  if (input_from_partlist)
	    fprintf (stderr, "In %s:\n", PART (rrule));
	  fprintf (stderr, "line %ld: `%s': hyper rule local defined\n", LINE ((rrule)), FREPR (member));
	  error = true;
	}
    }
    prev = member;
  }
}

static void multiple_defined (long rule) 
{
  long node;

  for (node = BROTHER (rule); node > laststdpred; node = BROTHER (node))
    if (REPR (rule) == REPR (node))
    {
      if (!eag_flag)
      {
	if (input_from_partlist)
	{
	  fprintf (stderr, "In %s:\n", PART (rule));
	  fprintf (stderr, "line %ld: '%s' multiple defined\n", LINE ((rule)), FREPR (rule));
	  fprintf (stderr, "In %s:\n", PART (node));
	  fprintf (stderr, "line %ld: '%s' multiple defined\n", LINE ((node)), FREPR (node));
	}
	else
	{
	  fprintf (stderr, "line %ld: '%s' multiple defined\n", LINE ((rule)), FREPR (rule));
	  fprintf (stderr, "line %ld: '%s' multiple defined\n", LINE ((node)), FREPR (node));
	}
	error = true;
      }
      else
      {
	long alt;
	for (alt = SON (rule); BROTHER (alt) != nil; alt = BROTHER (alt));
	BROTHER (alt) = SON (node);
	return;
      }
    }
}

static void delete_multiple_defined_hyperrules () 
{
  long mrule, dubble_rules = 0;

  for (mrule = root; mrule > laststdpred; mrule = BROTHER (mrule))
  {
    long prev = mrule, this;

    for (this = BROTHER (prev); this > laststdpred; this = BROTHER (prev))
    {
      if (REPR (mrule) == REPR (this))
      {
	BROTHER (prev) = BROTHER (this);
	if (verbose_flag && eag_flag)
	{
	  if (input_from_partlist)
	    fprintf (stderr, "In %s:\n", PART (mrule));
	  fprintf (stderr, "glammar: `%s': Definitions of hyperrule grouped together\n", FREPR (mrule));
	  dubble_rules += 1;
	}
      }
      else
	prev = this;
    }
  }
  if (dubble_rules > 0 && eag_flag)
    fprintf (stderr, "glammar:  Some definitions where grouped together\n");
}

static void check_affix_def (long rule) 
{
  long alt, ref = AFFIXDEF (SON (rule));

  for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    if (!consistentaffixtype (AFFIXDEF (alt), ref))
    {
      if (input_from_partlist)
	fprintf (stderr, "In %s:\n", PART (rule));
      fprintf (stderr, "line %ld lhs `%s': ", LINE (alt), FREPR (rule));
      affixerrmsg (ref, AFFIXDEF (alt));
      error = true;
    }
}


static long definedin (long term, long alt) 
{
  if (!leftindef (term, alt))
    if (!rightdvdef (term, alt))
    {
      return false;
    }
  return true;
}


static long leftindef (long term, long alt) 
{
  long affix, trm;

  for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
  {
    if (!DERIVED (affix))
      for (trm = SON (affix); trm != nil; trm = BROTHER (trm))
	if (REPR (trm) == REPR (term))
	  if ((NODENAME (trm) < metaffix) || (LATTICE (affix)))
	    return true;
  }
  return false;
}


static long rightdvdef (long term, long alt) 
{
  long affix, mem_item, trm;

  for (mem_item = SON (alt); mem_item != nil; mem_item = BROTHER (mem_item))
  {
    for (affix = AFFIXTREE (mem_item); affix != nil; affix = BROTHER (affix))
      if (!INHERITED (affix))
      {
	for (trm = SON (affix); trm != nil; trm = BROTHER (trm))
	  if (REPR (trm) == REPR (term))
	    if ((NODENAME (trm) < metaffix) || (LATTICE (affix)))
	      return true;
      }
  }
  return false;
}

long cur_line = 0;

long set_meta_affixes () 
{
  register long term, mem, affix;
  long rule, alt;

  overwrite_grammar_generation_meta_rules ();
  remove_multiple_defined_meta_affixes ();
  link_supernt ();
  link_lattice ();

  for (rule = root; rule != laststdpred; rule = BROTHER (rule))
  {
    for (alt = SON (rule); alt != nil; alt = BROTHER (alt))
    {
      cur_line = LINE (alt);
      for (affix = AFFIXDEF (alt); affix != nil; affix = BROTHER (affix))
	for (term = SON (affix); term != nil; term = BROTHER (term))
	  if (NODENAME (term) == affixnt)
	  {
	    if (LATTICE (affix))
	      lattice_defined (rule, alt, term);
	    else
	      /* meta_defined (term, NODENAME (affix) == inherited); */
	      meta_defined (term);
	  }
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
	for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
	  for (term = SON (affix); term != nil; term = BROTHER (term))
	    if (NODENAME (term) == affixnt)
	    {
	      if (LATTICE (affix))
		lattice_defined (rule, alt, term);
	      else
		/* meta_defined (term, NODENAME (affix) == derived); */
		meta_defined (term);
	    }
    }
  }
  return 0;
}

/*
   meta_defined(trm,defining)
   register long  trm,defining;
   {
   register long    mrule;
   register char  *term = REPR(trm);

   for (mrule = lastmetarule; mrule != nil; mrule = BROTHER(mrule))
   if (REPR(mrule) == term) 
   switch (NODENAME(SON(mrule))  {
   case Super:
   if (defining) {
   long rrule  = DEF (SON (mrule));
   NODENAME (trm) = superaffix;
   DEF (trm) = rrule;
   if (DEF(mrule) == docompile)
   if (!MARKED(rrule,docompile))
   if (!MARKED(rrule, external)) {
   if (input_from_partlist) 
   fprintf(stderr,"In %s:\n",PART(mrule));
   fprintf(stderr,"line %ld, `%s': not defined as external (=)\n",
   cur_line,REPR(mrule));
   if (!tree_flag)
   exit(-1);
   }
   }
   break;
   case OneStar:
   if (defining) 
   fprintf(stderr,
   "line %ld, (Warning) Metalist `%s' used on a defining position\n",
   cur_line,REPR(mrule));
   case  One:  
   case  Mint:  
   NODENAME (trm) = metaffix;
   break;
   default:;
   }
   return 0;
   }
   */

static void meta_defined (long trm)
{
  register long mrule;
  register char *term = REPR (trm);

  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    if (REPR (mrule) == term)
    {
      if (MARKED (SON (mrule), supernt))
      {
	NODENAME (trm) = superaffix;
	DEF (trm) = DEF (SON (mrule));
	FLAG_SET (trm, wasmeta);

      }
      else
	NODENAME (trm) = metaffix;
    }
  for (mrule = first_lattice;mrule != nil; mrule = BROTHER (mrule))
    if (REPR (mrule) == term)
    {
      LATTICE_DEF (trm) = mrule;
      FLAG_SET (trm, wasmeta);

    }
}


static void link_supernt () 
{
  register long mrule;

  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    if (NODENAME (SON (mrule)) == supernt)
    {
      long rrule = root;
      long member = SON (mrule);
      while (REPR (member) != REPR (rrule))
      {
	if (rrule == nil)
	{
	  if (input_from_partlist)
	    fprintf (stderr, "In %s:\n", PARTX (mrule,root));
	  fprintf (stderr, "line %ld,`%s': `%s': Undefined super rule\n", LINE(mrule),FREPR (mrule), FREPR (member));
	  error = true;
	  break;
	}
	else
	  rrule = BROTHER (rrule);
      }
      if (rrule == nil)
	break;
      if (!consistentaffixtype (AFFIXDEF (SON (rrule)), AFFIXTREE (member)))
      {
	if (input_from_partlist)
	  fprintf (stderr, "In %s:\n", PARTX(mrule,root));
	fprintf (stderr, "line %ld, `%s': `%s': ", LINE(mrule), FREPR (mrule), REPR (member));
	affixerrmsg (AFFIXDEF (SON (rrule)), AFFIXTREE (member));
	error = true;
      }
      DEF (member) = rrule;
    }
}


static void remove_multiple_defined_meta_affixes () 
{
  long mrule, dubble_rules = 0;
  char *warn = NULL;
  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
  {
    long prev = mrule, this;

    for (this = BROTHER (prev); this != nil; this = BROTHER (prev))
    {
      if (REPR (mrule) == REPR (this))
      {
	BROTHER (prev) = BROTHER (this);
	warn = REPR (mrule);
	fprintf (stderr, "glammar (warning): `%s': Multiple definition of metanotion ignored\n", warn);
	dubble_rules += 1;
      }
      else
	prev = this;
    }
  }
  if (dubble_rules > 0)
    fprintf (stderr, "glammar (warning): %ld multiple definitions of metanotions ignored\n", dubble_rules);
}

/* char *ctime_r(time_t *t, char *r); */

static void overwrite_grammar_generation_meta_rules () 
{
  char *gt;
  long mrule, gen_time;

  myvclock = time ((time_t *) 0);
  myclock = &myvclock;

  gt = ctime_r (myclock, generation_time);
  for (gt = generation_time; *gt != '\0'; gt += 1)
    if (*gt == '\n' || *gt == '\012')
    {
      *gt = '\0';
      break;
    }

  /*
     Thu Sep 27 16:58:35 2012
     */
#define Year_offset 20
#define Month_offset 4
#define Date_offset 8
  generation_date[0] = generation_time[0 + Month_offset];
  generation_date[1] = generation_time[1 + Month_offset];
  generation_date[2] = generation_time[2 + Month_offset];
  generation_date[3] = ' ';
  generation_date[4] = generation_time[0 + Date_offset];
  generation_date[5] = generation_time[1 + Date_offset];
  generation_date[6] = ',';
  generation_date[7] = ' ';
  generation_date[8] = generation_time[0 + Year_offset];
  generation_date[9] = generation_time[1 + Year_offset];
  generation_date[10] = generation_time[2 + Year_offset];
  generation_date[11] = generation_time[3 + Year_offset];

  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    if (strcmp (REPR (mrule), "generationtime") == 0)
    {
      REPR (SON (mrule)) = generation_time;
      gen_time = mrule;
    }
    else if (strcmp (REPR (mrule), "generationdate") == 0)
      REPR (SON (mrule)) = generation_date;
    else if (strcmp (REPR (mrule), "scriptname") == 0)
      REPR (SON (mrule)) = ex_filename;



  if ((separate_comp_flag) && !(meta_used (REPR (gen_time))))
  {
    REPR (SON (gen_time)) = "";
  }
}

static long meta_used (char *repr) 
{
  long rule, alt, mem, afx, term;
  for (rule = root; rule != init_one_star; rule = BROTHER (rule))
    for (alt = SON (rule); alt != nil; SUM_ALT, alt = BROTHER (alt))
    {
      for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
	for (term = SON (afx); term != nil; term = BROTHER (term))
	  if (REPR (term) == repr)
	    return true;
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
	for (afx = AFFIXTREE (mem); afx != nil; afx = BROTHER (afx))
	  for (term = SON (afx); term != nil; term = BROTHER (term))
	    if (REPR (term) == repr)
	      return true;
    }

  for (rule = lastmetarule; rule != nil; rule = BROTHER (rule))
    if (REPR (rule) == repr)
      continue;
    else
      for (term = SON (rule); term != nil; term = BROTHER (term))
	if (REPR (term) == repr)
	  return true;
  return false;
}


long lasthyperrule;

static long prev_mem;
static long LastAltInitOneStar;
static long LastAltEvalOneStar;
static long eval_one_star;

void meta2init () 
{
  long l;

  newnode (nil, nil, nil, NULL);
  LastAltEvalOneStar = brother;
  newrulenode (rnode | external | docompile | isvolatile, laststdpred, brother, 0, PART (root), "Eval_One_Star");
  eval_one_star = brother;
  SET (eval_one_star, generated_rule);
  SET (eval_one_star, deterministic);

  /* find last user defined hyper rule */

  for (l = root; BROTHER (l) != laststdpred; l = BROTHER (l));

  newnode (nil, nil, nil, NULL);
  LastAltInitOneStar = brother;
  prev_mem = nil;
  newrulenode (rnode | external | docompile | isvolatile, eval_one_star, brother, LINE (root),
      PART (root), "Init_One_Star");
  init_one_star = brother;
  SET (init_one_star, generated_rule);
  SET (init_one_star, deterministic);
  BROTHER (l) = brother;
  for (l = lastmetarule; l != nil; l = BROTHER (l))
    if ((NODENAME (l) == OneStar) || (NODENAME (l) == meta_prod_rule))
    {
      if (NODENAME (SON (l)) != supernt)
	meta2initrule (l);
    }
    else if (NODENAME (l) == meta_Mint)
      meta2mintrule (l);
  eval_one_star_node ();
  prev_mem = nil;
  for (l = lastmetarule; l != nil; l = BROTHER (l))
    if (NODENAME (l) == meta_prod_rule)
      if (NODENAME (SON (l)) != supernt)
	/* if ( (BROTHER(SON(l)) != nil) || (SON(l) == affixnt) ) */
	evalmetarule (l);
}

/* 
 *   add a rule of the form: init meta (>meta, >meta terms). 
 */

char *EmptyOneStar = "Empty One Star";

static void meta2initrule (long metarule) 
{
  long b;

  /*  >meta term */
  newnode (inherited, nil, SON (metarule), "(nil)");
  b = brother;

  /*  >meta */
  newnode (affixnt, nil, nil, REPR (metarule));
  newnode (inherited, b, brother, "(nil)");

  /*  init meta */
  newnode (ntnode, nil, brother, REPR (initmeta));
  if (prev_mem == nil)
    SON (LastAltInitOneStar) = brother;
  else
    BROTHER (prev_mem) = brother;
  prev_mem = brother;
  if (NODENAME (metarule) == OneStar)
  {
    newnode (affixtm, nil, nil, EmptyOneStar);
    SON (metarule) = brother;
    NODENAME (metarule) = meta_prod_rule;
  }
}

static void meta2mintrule (long metarule) 
{
  long b;

  /*  >meta term */
  newnode (affixtm, nil, nil, REPR (SON (metarule)));
  newnode (inherited, nil, brother, "(nil)");
  NODENAME (SON (metarule)) = One;
  b = brother;

  /*  >meta */
  newnode (affixnt, nil, nil, REPR (metarule));
  newnode (inherited, b, brother, "(nil)");

  /*  init meta */
  newnode (ntnode, nil, brother, REPR (initmint));
  if (prev_mem == nil)
    SON (LastAltInitOneStar) = brother;
  else
    BROTHER (prev_mem) = brother;
  prev_mem = brother;
  if (NODENAME (metarule) == OneStar)
  {
    newnode (affixtm, nil, nil, EmptyOneStar);
    SON (metarule) = brother;
    NODENAME (metarule) = meta_prod_rule;
  }
}

static void eval_one_star_node () 
{
  newnode (ntnode, nil, nil, REPR (eval_one_star));
  if (prev_mem == nil)
    SON (LastAltInitOneStar) = brother;
  else
    BROTHER (prev_mem) = brother;
  prev_mem = brother;
}

char *metatab = "@!META!@";


static void evalmetarule (long metarule) 
{
  long b;

  /*  >meta */
  newnode (affixnt, nil, nil, REPR (metarule));
  newnode (inherited, nil, brother, "(nil)");

  /*  eval meta */
  newdefnode (ntnode, nil, brother, evalmeta, REPR (evalmeta));
  if (prev_mem == nil)
    SON (LastAltEvalOneStar) = brother;
  else
    BROTHER (prev_mem) = brother;
  prev_mem = brother;


  /* add to >@METAS@, >"meta", >meta), */
  newnode (affixnt, nil, nil, REPR (metarule));
  newnode (inherited, nil, brother, "(nil)");
  b = brother;
  newnode (affixtm, nil, nil, REPR (metarule));
  newnode (inherited, b, brother, "(nil)");
  b = brother;
  newnode (affixtm, nil, nil, metatab);
  newnode (inherited, b, brother, "(nil)");

  newdefnode (ntnode, nil, brother, add_to, REPR (add_to));
  BROTHER (prev_mem) = brother;
  prev_mem = brother;

}


#define eval_in_process  111

void check_metagrammar () 
{
  long mrule;
  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    check_metarule (mrule);
  if (error)
    if (!tree_flag)
      exit (10);
}

static void check_metarule (long mrule)
{
  register long term;


  if (mrule == -1 || NODENAME(mrule) == -1)
    return;
  if (NODENAME (mrule) == eval_in_process)
  {
    if (input_from_partlist)
      fprintf (stderr, "In %s:\n", PARTX(mrule,root));
    fprintf (stderr, "line %ld, %s: Cyclic metanotion.\n", LINE(mrule),FREPR (mrule));
    error = 1;
    return;
  }
  NODENAME (mrule) = eval_in_process;
  for (term = SON (mrule); term != nil; term = BROTHER (term))
  {
    if (NODENAME (term) == affixnt)
      check_metarule (metadefinition_of (mrule,term));
  }
  NODENAME (mrule) = meta_prod_rule;
}


static long metadefinition_of (long def_mrule,long term) 
{
  register long mrule;
  register char *rterm = REPR (term);

  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    if (REPR (mrule) == rterm)
      return mrule;
  if (input_from_partlist)
    fprintf (stderr, "In %s:\n", PARTX(mrule,root));
  fprintf (stderr, "line %ld, %s: Undefined metanotion in definition of %s\n", LINE(term),rterm, REPR(def_mrule));
  error = 1;
  return (long) -1;
}
