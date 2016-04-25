
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 


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
int error = false;

newtop ()
{
  if (AFFIXDEF (SON (root)) == nil)
  {
    int root_mem = nil;

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

linktodefs ()
{
  register int rule, alt, afx;
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
    int alt = SON (rule);
    check_affix_def (rule);
    multiple_defined (rule);
    for (; alt != nil; alt = BROTHER (alt))
    {
      lhs_affixes_defined (alt, rule);
      rhs_members_affixes_defined (alt, rule);
    }
  }
  delete_multiple_defined_hyperrules ();
  if (error)
    if (!tree_flag)
      exit (10);
}

lhs_affixes_defined (alt, rule)
int alt, rule;
{
  register int afx, trm;
  for (afx = AFFIXDEF (alt); afx != nil; afx = BROTHER (afx))
    if (NODENAME (afx) == derived)
    {
      for (trm = SON (afx); trm != nil; trm = BROTHER (trm))
        if ((APPLY_BOUND_AFFIX (trm)) && (!definedin (trm, alt)))
        {
          if (input_from_partlist)
            fprintf (stderr, "In %s:\n", PART (rule));
          fprintf (stderr,
                   "line %ld: `%s': affix `%s' in left-hand side is not defined\n",
                   LINE (alt), FREPR (rule), FREPR (trm));
        }
    }
    else if (LATTICE (afx))
      if (!tree_flag)
        lattice_used (SON (afx), alt);
}

rhs_members_affixes_defined (alt, rule)
int alt, rule;
{
  register int afx, trm, member, rrule, prev;
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
            if ((APPLY_BOUND_AFFIX (trm)) && (!definedin (trm, alt)))
            {
              if (input_from_partlist)
                fprintf (stderr, "In %s:\n", PART (rule));
              fprintf (stderr,
                       "line %ld: `%s': `%s': affix `%s' is not defined\n",
                       LINE (alt), FREPR (rule), FREPR (member), FREPR (trm));
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
          fprintf (stderr, "In %s:\n", PART (rule));
        fprintf (stderr, "line %ld: lhs `%s': member `%s' is not defined\n", LINE (alt), FREPR (rule), FREPR (member));
        error = true;
        break;
      }
      if (!valid_overloaded_affixtype (AFFIXDEF (SON (rrule)), AFFIXTREE (member), member))
      {

/*
         if (!consistentaffixtype(AFFIXDEF(SON(rrule)),
             AFFIXTREE(member))) {
*/
        if (input_from_partlist)
          fprintf (stderr, "In %s:\n", PART (rule));
        fprintf (stderr, "line %ld: `%s',`%s': ", LINE (alt), FREPR (rule), FREPR (member));
        affixerrmsg (AFFIXDEF (SON (rrule)), AFFIXTREE (member));
        error = true;
      }
      DEF (member) = rrule;
      if (rrule == add_to)
        add_to_used = 1;
      SET (rrule, rule_used);
      if ((rrule == cut) && (BROTHER (alt) == nil))
      {

/*
 *         fprintf(stderr, " line %d in %s: (hint) `->' in last production\n", 
 *              LINE(alt), REPR(rule));
 */
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

multiple_defined (rule)
int rule;
{
  int node;

  for (node = BROTHER (rule); node > laststdpred; node = BROTHER (node))
    if (REPR (rule) == REPR (node))
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
        int alt;
        for (alt = SON (rule); BROTHER (alt) != nil; alt = BROTHER (alt));
        BROTHER (alt) = SON (node);
        return;
      }

}

delete_multiple_defined_hyperrules ()
{
  int mrule, dubble_rules = 0;

  for (mrule = root; mrule > laststdpred; mrule = BROTHER (mrule))
  {
    int prev = mrule, this;

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

check_affix_def (rule)
int rule;
{
  int alt, ref = AFFIXDEF (SON (rule));
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


definedin (term, alt)
int term, alt;
{
  if (!leftindef (term, alt))
    if (!rightdvdef (term, alt))
    {
      error = true;
      return false;
    }
  return true;
}


leftindef (term, alt)
int term, alt;
{
  int affix, trm;

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


rightdvdef (term, alt)
int term, alt;
{
  int affix, mem_item, trm;

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

int cur_line = 0;
set_meta_affixes ()
{
  register int term, mem, affix;
  int rule, alt;

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
              meta_defined (term, NODENAME (affix) == inherited);
          }
      for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
        for (affix = AFFIXTREE (mem); affix != nil; affix = BROTHER (affix))
          for (term = SON (affix); term != nil; term = BROTHER (term))
            if (NODENAME (term) == affixnt)
              if (LATTICE (affix))
                lattice_defined (rule, alt, term);
              else
                meta_defined (term, NODENAME (affix) == derived);
    }
  }
}

/*
meta_defined(trm,defining)
register int  trm,defining;
{
  register int    mrule;
  register char  *term = REPR(trm);

  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER(mrule))
    if (REPR(mrule) == term) 
      switch (NODENAME(SON(mrule))  {
      case Super:
        if (defining) {
          int rrule  = DEF (SON (mrule));
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
}
*/

meta_defined (trm)
register int trm;
{
  register int mrule;
  register char *term = REPR (trm);

  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    if (REPR (mrule) == term)
    {
      if (MARKED (SON (mrule), supernt))
      {
        NODENAME (trm) = superaffix;
        DEF (trm) = DEF (SON (mrule));

      }
      else
        NODENAME (trm) = metaffix;
    }
}


link_supernt ()
{
  register int mrule;

  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    if (NODENAME (SON (mrule)) == supernt)
    {
      int rrule = root;
      int member = SON (mrule);
      while (REPR (member) != REPR (rrule))
      {
        if (rrule == nil)
        {
          fprintf (stderr, "`%s': `%s': Undefined super rule\n", FREPR (mrule), FREPR (member));
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
        fprintf (stderr, "`%s': `%s': ", FREPR (mrule), REPR (member));
        affixerrmsg (AFFIXDEF (SON (rrule)), AFFIXTREE (member));
        error = true;
      }
      DEF (member) = rrule;
    }
}


remove_multiple_defined_meta_affixes ()
{
  int mrule, dubble_rules = 0;
  char *warn = NULL;
  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
  {
    int prev = mrule, this;

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
    fprintf (stderr, "glammar (warning): %d multiple definitions of metanotions ignored\n", dubble_rules);
}

/* char *ctime_r(time_t *t, char *r); */

overwrite_grammar_generation_meta_rules ()
{
  char *gt, *gd;
  int mrule, gen_time;

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

meta_used (repr)
char *repr;
{
  int rule, alt, mem, afx, term;
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

char *lettergen ();

int lasthyperrule;
meta2super ()
{
  int l;
  /* find last user defined hyper rule */
  for (l = root; BROTHER (l) != laststdpred; l = BROTHER (l));
  lasthyperrule = l;
  for (l = lastmetarule; l != nil; l = BROTHER (l))
    if (NODENAME (l) == Mult)
      meta2superrule (l);
}

meta2superrule (metarule)
int metarule;
{
  register int mem, alt;
  newnode (rnode | external | docompile, laststdpred, SON (metarule), REPR (metarule));
  BROTHER (lasthyperrule) = brother;
  lasthyperrule = brother;

  for (alt = SON (metarule); alt != nil; alt = BROTHER (alt))
  {
    brother = nil;
    add_super_tree_term_lhs (SON (alt));
    newnode (derived, nil, brother, "");
    AFFIXDEF (alt) = brother;
    for (mem = SON (alt); mem != nil; mem = BROTHER (mem))
      if (NODENAME (mem) != affixtm)
      {
        if (NODENAME (mem) == affixnt)
          NODENAME (mem) = ntnode;
        newnode (affixnt, nil, nil, lettergen (mem));
        newnode (derived, nil, brother, "");
        AFFIXTREE (mem) = brother;
      }
      else
        NODENAME (mem) = tnode;
  }
  newnode (affixnt, nil, nil, "x");
  newnode (derived, nil, brother, "");
  newnode (supernt, nil, brother, REPR (lasthyperrule));
  SON (metarule) = brother;
}

add_super_tree_term_lhs (mem)
int mem;
{
  if (mem == nil)
    return;
  add_super_tree_term_lhs (BROTHER (mem));
  if (NODENAME (mem) != affixtm)
    newnode (affixnt, brother, nil, lettergen (mem));
  else
    newnode (affixtm, brother, nil, REPR (mem));
}

char *lettergen (mem)
int mem;
{
  switch (mem % 26)
  {
  case 0:
    return "x_a";
  case 1:
    return "x_b";
  case 3:
    return "x_c";
  case 4:
    return "x_d";
  case 5:
    return "x_f";
  case 6:
    return "x_g";
  case 7:
    return "x_h";
  case 8:
    return "x_i";
  case 9:
    return "x_j";
  case 10:
    return "x_k";
  case 11:
    return "x_l";
  case 12:
    return "x_m";
  case 13:
    return "x_n";
  case 14:
    return "x_o";
  case 15:
    return "x_p";
  case 16:
    return "x_q";
  case 17:
    return "x_r";
  case 18:
    return "x_s";
  case 19:
    return "x_t";
  case 20:
    return "x_u";
  case 21:
    return "x_v";
  case 22:
    return "x_w";
  case 23:
    return "x_x";
  case 24:
    return "x_y";
  case 25:
    return "x_z";
  default:
    return NULL;
  }
}

static int prev_mem;
static int LastAltInitOneStar;
static int LastAltEvalOneStar;
static int eval_one_star;

meta2init ()
{
  int l;

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

meta2initrule (metarule)
int metarule;
{
  int b;

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

meta2mintrule (metarule)
int metarule;
{
  int b;

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

eval_one_star_node ()
{
  newnode (ntnode, nil, nil, REPR (eval_one_star));
  if (prev_mem == nil)
    SON (LastAltInitOneStar) = brother;
  else
    BROTHER (prev_mem) = brother;
  prev_mem = brother;
}

char *metatab = "@!META!@";


evalmetarule (metarule)
int metarule;
{
  int b;

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

check_metagrammar ()
{
  int mrule;
  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    check_metarule (mrule);
  if (error)
    if (!tree_flag)
      exit (10);
}

check_metarule (mrule)
register int mrule;
{
  register int term;

  if (NODENAME (mrule) == -1)
    return;
  if (NODENAME (mrule) == eval_in_process)
  {
    fprintf (stderr, "%s: Cyclic metanotion.\n", FREPR (mrule));
    error = 1;
    return;
  }
  NODENAME (mrule) = eval_in_process;
  for (term = SON (mrule); term != nil; term = BROTHER (term))
  {
    if (NODENAME (term) == affixnt)
      check_metarule (metadefinition_of (term));
  }
  NODENAME (mrule) = meta_prod_rule;
}


metadefinition_of (term)
register int term;
{
  register int mrule;
  register char *rterm = REPR (term);

  for (mrule = lastmetarule; mrule != nil; mrule = BROTHER (mrule))
    if (REPR (mrule) == rterm)
      return mrule;
  fprintf (stderr, "%s: Undefined metanotion\n", rterm);
  error = 1;
  return -1;
}
