
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.coml 

/* file  abstract syntax tree    */
#include "gg1.h"
#include "gg2.h"

int optlefthsd = 0, dfa, last_lattice = nil, firstnode = nil;

char *cur_part_name;
char *part_file;

char inputfilename[256];
compile ()
{
  int rule;
  initialize ();
  grammar ();
  line = 0;

  if (verbose_flag)
    dump_hash ();
  if (root == nil)
    root = laststdpred;
  if (root == laststdpred)
  {
    errmsg ("RULES");
  }
  if ((syntaxerrors > 0))
    exit (syntaxerrors);
#ifdef NEVER
  if (index_flag)
    fclose (indexfile);
#endif
  init_builtins ();
  meta2init ();
  list ();

  if (verbose_flag)
    fprintf (stderr, "no syntax errors detected.\
        \n%d name entries in hash table (max = %d).\
        \n%d chars in symbol table.\
        \n%d name clashes occured.\
        \nsetting meta affixes.\n", nr_names, maxnt, symbol_table_size + charindex, name_clashes);
  set_meta_affixes ();
  if (verbose_flag)
    fprintf (stderr, "match applications with definitions.\n");

  linktodefs ();

  if (parse_tree_flag)
  {
    parse_tree ();
    printtree ();
    exit (0);
  }

  if (id_tree_flag)
  {
    id_tree ();
    printtree ();
    exit (0);
  }

  if (cfg_flag)
  {
    print_cfg_tree ();
    exit (0);
  }

  if (verbose_flag)
    fprintf (stderr, "eliminate defining expressions.\n");

  eag ();

  if (verbose_flag)
  {
    fprintf (stderr, "hint on non-used hyperrules.\n");
    hint_on_non_used_hyperrules (0);
  }
  else
    hint_on_non_used_hyperrules (1);

  if (index_flag)
    better_index ();
  if (tagindex_flag)
    tag_index ();

  tr_lattice ();
  if (verbose_flag)
    fprintf (stderr, "eliminate implied equals.\n");
  ie ();

  if (verbose_flag)
    fprintf (stderr, "eliminate wheres.\n");

  wheres ();

  if (verbose_flag)
    fprintf (stderr, "compute \"is predicate\".\n");
  compute_predicates ();

  if (verbose_flag)
    fprintf (stderr, "tail recursion opt.\n");

  move_terms_up ();
  final_nestarset_removal ();
  tail_rec_opt ();

  if (verbose_flag)
    fprintf (stderr, "check metagrammar.\n");
  check_metagrammar ();


  if (fulltrace_flag || fullstat_flag)
  {
    if (verbose_flag)
      fprintf (stderr, "alias builtin for tracing.\n");
    trace ();
  }


  if (lift_flag && memo_flag)
  {
    if (verbose_flag)
      fprintf (stderr, "compute empty rules.\n");
    empty ();

    if (memopt_flag)
    {
      if (verbose_flag)
        fprintf (stderr, "memoizer optimizer.\n");
      memopt ();
    }
    if (verbose_flag)
      fprintf (stderr, "compute  recursive  rules.\n");
    recursive_ ();
    if (verbose_flag)
      fprintf (stderr, "check on left recursive rules.\n");
    left_rec ();
  }


  if (startname == NULL)
    startnode = root;
  else
  {
    for (rule = root; rule != nil; rule = BROTHER (rule))
      if (REPR (rule) == startname)
      {
        startnode = rule;
        break;
      }
    if (rule == nil)
    {
      fprintf (stderr, "STARTRULE `%s' not found and set to %s\n", full_repr (startname), FREPR (root));
      startnode = root;
    }
  }
  determ ();
  set_volatile ();
  if (memo_flag)
  {
    if (verbose_flag)
      fprintf (stderr, "set affix directed parsing affixes ; do a memo walk\n");
    adp_walk ();
  }
  if (verbose_flag)
    fprintf (stderr, "compute affix usage.\n");
  affixuse ();
  if (tree_flag)
  {
    printtree ();
    exit (0);
  }
  if ((memo_flag) && (MARKED (skip, deterministic)))
  {
    fprintf (stderr, "compiler error: skip deterministic?\n");
    exit (-1);
  }
  if (verbose_flag)
    fprintf (stderr, "code generation phase\n");
  if (separate_comp_flag)
    compile_parts ();
  else
  {
    if (input_from_partlist)
    {
      int rule;
      for (rule = root; rule != laststdpred; rule = BROTHER (rule))
        SET (rule, docompile);
    }
    code ();
  }
}

initialize ()
{
  lastmetarule = nil;
  first_lattice = nil;
  reccount = 0;
  astindex = 0;
  brother = 0;
  charindex = 0;
  prevcharindex = 0;
  chartable[0] = '\0';
  hashindex = 0;
  NtCount = 0;
  RuleCount = 0;
  nrterms = 0;
  nrntmems = 0;
  syntaxerrors = 0;
  usefullerrmsg = true;
  startname = NULL;
  initnametable ();
  stddefs ();
  underscore_allowed = false;
  laststdmetarule = lastmetarule;
}

newnode (name, brothers, sons, repr)
int name, brothers, sons;
char *repr;
{
  NODENAME (astindex) = name;
  BROTHER (astindex) = brothers;
  SON (astindex) = sons;
  DEF (astindex) = -1;
  if (input_from_partlist)
    PART (astindex) = cur_part_name;
  else
    PART (astindex) = inputfilename;
  LINE (astindex) = line;
  REPR (astindex) = repr;
  FLAGS (astindex) = 0;
  brother = astindex++;
  if (astindex >= max_item)
    realloc_ast ();
}

newdefnode (name, brothers, sons, def, repr)
int name, brothers, sons, def;
char *repr;
{
  NODENAME (astindex) = name;
  BROTHER (astindex) = brothers;
  SON (astindex) = sons;
  DEF (astindex) = def;
  LINE (astindex) = line;
  REPR (astindex) = repr;
  FLAGS (astindex) = 0;
  brother = astindex++;
  if (astindex >= max_item)
    realloc_ast ();
}

newrulenode (name, brothers, sons, line, part, repr)
int name, brothers, sons, line;
char *part, *repr;
{
  NODENAME (astindex) = name;
  BROTHER (astindex) = brothers;
  SON (astindex) = sons;
  LINE (astindex) = line;
  PART (astindex) = part;
  REPR (astindex) = repr;
  FLAGS (astindex) = 0;
  brother = astindex++;
  astindex++;
  if (astindex >= max_item)
    realloc_ast ();
}

#ifdef NVER
newrulenode (name, brothers, sons, def, repr)
int name, brothers, sons, def;
char *repr;
{
  NODENAME (astindex) = name;
  BROTHER (astindex) = brothers;
  SON (astindex) = sons;
  DEF (astindex) = def;
  REPR (astindex) = repr;
  FLAGS (astindex) = 0;
  brother = astindex++;
  astindex++;
  if (astindex >= max_item)
    realloc_ast ();
}
#endif
realloc_ast ()
{
  max_item <<= 1;
  ast = (AST *) realloc (ast, (max_item + 2) * sizeof (AST));
  if (ast == NULL)
  {
    fprintf (stderr, "glammar fatal msg: no %d bytes available for heap space\n", max_item * 20);
    exit (1);
  }
  if (verbose_flag)
    fprintf (stderr, "realloc ast: %d bytes available for heap space\n", max_item * 20);
}

grammar ()
{
  brother = nil;
  pieces ();
  if (!feof (input))
    errmsg ("RULES");
}

pieces ()
{
  if (input_from_partlist)
  {
    (void) sprintf (&chartable[charindex], "%s.p\0", thispart);
    part_file = &chartable[charindex];
    while (chartable[charindex++] != '\0');
    prevcharindex = charindex;
    partlist = fopen (part_file, "r");
    if (partlist == NULL)
    {
      fprintf (stderr, "glammar: %s: No such file\n", part_file);
      exit (24);
    }
  }
  sum = 0;
  if (input_from_partlist && (fscanf (partlist, "%s %d", thispart, &sum) != -1))
  {
    int comp = false;
    cur_part_name = &chartable[charindex];
    strcpy (cur_part_name, thispart);
    while (chartable[charindex++] != '\0');
    prevcharindex = charindex;
    input = fopen (thispart, "r");
    addpart ();
    if (input == NULL)
    {
      fprintf (stderr, "glammar: %s: No such file\n", thispart);
      exit (24);
    }
    line = 1;
    getfirstchar ();
    brother = nil;

    rules (rnode);
    getlastnode ();
    if (lastnode <= laststdpred)
    {
      fprintf (stderr, "In %s: HYPERrules expected.\n", cur_part_name);
      syntaxerrors += 1;
    }
    if (verbose_flag)
      fprintf (stderr, "glammar: %s linked to %s\n", FREPR (lastnode), FREPR (laststdpred));
    root = brother;
    PART (root) = cur_part_name;
    OLDSUM (root) = sum;
    brother = nil;
    morepieces ();
    getlastnode ();
    BROTHER (lastnode) = laststdpred;
  }
  else if (!input_from_partlist)
  {
    if (!input_from_stdin)
    {
      (void) sprintf (inputfilename, "%s.g\0", thispart);

      input = fopen (inputfilename, "r");
      if (verbose_flag)
        fprintf (stderr, "%s opened\n", inputfilename);
    }
    else
    {
      input = stdin;
      (void) sprintf (inputfilename, "@stdin@");
      if (verbose_flag)
        fprintf (stderr, "stdin opened\n");
    }

    if (input == NULL)
    {
      fprintf (stderr, "Glammar: (compiler error) cannot open single file '%s'\n", inputfilename);
      exit (12);
    }
    line = 1;
    getfirstchar ();
    brother = nil;
    rules (rnode + docompile);
    root = brother;
    if (root != nil)
    {
      getlastnode ();
      BROTHER (lastnode) = laststdpred;
    }
    else
    {
      if (verbose_flag)
        fprintf (stderr, "Glammar: empty file?\n");
    }
  }
  else
  {
    fprintf (stderr, "Glammar: specified partlist empty? Bye.\n");
    exit (12);
  }
}


morepieces ()
{
  int prevlastnode;
  sum = 0;
  if (fscanf (partlist, "%s %d", thispart, &sum) != -1)
  {
    int comp = false;
    fclose (input);
    input = fopen (thispart, "r");
    cur_part_name = &chartable[charindex];
    strcpy (cur_part_name, thispart);
    while (chartable[charindex++] != '\0');
    prevcharindex = charindex;
    addpart ();
    if (input == NULL)
    {
      fprintf (stderr, "Glammar: cannot open file: `%s' \n", thispart);
      exit (24);
    }
    line = 1;
    getnextchar ();
    prevlastnode = lastnode;
    brother = nil;
    comp = false;
    rules (rnode);
    getlastnode ();
    if (prevlastnode != lastnode)
    {
      if (verbose_flag)
        fprintf (stderr, "glammar: %s linked to %s\n", FREPR (prevlastnode), FREPR (firstnode));
      PART (firstnode) = cur_part_name;
      OLDSUM (firstnode) = (int) sum;
      BROTHER (prevlastnode) = firstnode;
    }
    else
    {
      fprintf (stderr, "In %s: HYPERrules expected.\n", cur_part_name);
      syntaxerrors += 1;
    }
    morepieces ();
  }
}

getlastnode ()
{
  int rule;
  for (rule = firstnode; rule != nil; rule = BROTHER (rule))
  {
    if (NODENAME (rule) != meta_prod_rule)
      if (NODENAME (rule) != 0)
        lastnode = rule;
  }
}

rules (ruletype)
int ruletype;
{
  int sons, ln, loc_glob = rnode;
  char *thisname = "??";

l:
  if (feof (input))
  {
    return;
  }
  ln = line;
  if (name_display_mix ())
  {
    thisname = ntname;
    if (define_symbol (&loc_glob));

    else if (startspecification ())
      goto l;

    else
    {
      errmsg ("DEFINE [: =] symbol");
      skiptopoint_symbol ();
      rules (rnode);
      usefullerrmsg = false;
    }
  }
  else
  {
    errmsg ("LEFT HANDSIDE");
    rulename = "??";
    skiptopoint_symbol ();
    rules (rnode);
    usefullerrmsg = false;
  }

  brother = nil;
  rulename = thisname;
  if (metarule (ruletype))
    goto l;
  else
  {
    if (index_flag)
    {
      if (fprintf (indexfile, "%s (", full_repr (thisname)) == EOF)
        fprintf (stderr, "glammar: Write to index file failed\n");
      printaffixtype (indexfile, lastaffixtree);
      if (input_from_partlist)
        fprintf (indexfile, "), file: `%s' %d\n", thispart, line);
      else
        fprintf (indexfile, "), %d\n", line);
    }
    alts (true);
    sons = brother;
    brother = nil;
    rules (ruletype);
    if (input_from_partlist)
      newrulenode (ruletype | loc_glob, brother, sons, ln, cur_part_name, thisname);
    else
      newrulenode (ruletype | loc_glob, brother, sons, ln, inputfilename, thisname);
    if (sons != nil)
      LINE (sons) = ln;
    firstnode = brother;
    NtCount += 1;
  }
}


alts (firstalt)
int firstalt;
{
  int asons, lefths, errline, locaffixtree, memaffixtree, settype;
  char *thisname = "LHS";
  int lkh = 0;
  lefths = false;
  asons = nil;
  errline = line;
  setlookahead (&lkh);
  if (firstalt)
  {
    defaffixtree = lastaffixtree;
    dfa = defaffixtree;
  }
  locaffixtree = defaffixtree;
  if ((name_display_mix ()) || (cut_symbol ()))
  {
    int l_g;
    thisname = ntname;
    if (define_symbol (&l_g))
    {
      lefths = true;
      if (firstalt)
        errmsg ("only one left-hand side");
      locaffixtree = lastaffixtree;
      defaffixtree = locaffixtree;
      brother = nil;
      if (lkh == lookahead)
        errmsg ("no LOOKAHEAD symbol");
      members ();
    }
    else if (!comma_symbol ())
    {
      newnode (ntnode | lkh, nil, brother, thisname);
      nrntmems += 1;
    }
    else
    {
      memaffixtree = brother;
      brother = nil;
      members ();
      newnode (ntnode | lkh, brother, memaffixtree, thisname);
      nrntmems += 1;
    }
  }
  else if (terminal ())
  {
    if (!comma_symbol ())
    {
      if (strlen (string) == 0);
      else
      {
        newnode (tnode | lkh, nil, nil, string);
        nrterms += 1;
      }
    }
    else
    {
      thisname = string;
      members ();
      if (strlen (thisname) == 0);
      else
      {
        newnode (tnode | lkh, brother, nil, thisname);
        nrterms += 1;
      }
    }
  }
  else if (set ())
  {
    thisname = string;
    if (!display ())
    {
      newnode (affixnt, nil, nil, dont_care);
      newnode (derived, nil, brother, "(nil)");
      lastaffixtree = brother;
    }
    else if ((NODENAME (brother) != derived) || (BROTHER (brother) != nil))
      errmsg ("ONE DERIVED AFFIX");

    if (!comma_symbol ())
    {
      newnode (cnode | lastsettype | lkh, nil, brother, thisname);
      nrterms += 1;
    }
    else
    {
      memaffixtree = brother;
      settype = lastsettype;
      members ();
      newnode (cnode | settype | lkh, brother, memaffixtree, thisname);
      nrterms += 1;
    }
  }
  else if (square_open_symbol ())
  {
    if (lkh)
      errmsg ("No lookahead");
    brother = nil;
    terms ();
    newnode (inherited, nil, brother, "(nil)");
    lastaffixtree = brother;
    if (!square_close_symbol ())
      errmsg ("Redirection Close (])");
    memaffixtree = brother;
    brother = nil;
    members ();
    newnode (ntnode, brother, memaffixtree, redirect);
  }
  if ((!lefths) && (optlefthsd == 0) && (!firstalt))
  {
    optlefthsd = 1;
    if (input_from_partlist)
      fprintf (stderr, "In %s:\n", cur_part_name);
    fprintf (stderr, "line %d: (message) no left-hand side in `%s'\n", errline, full_repr (rulename));
  }
  asons = brother;
  brother = nil;
  if ((!lefths) && (!firstalt))
  {
    copy_display ();
    locaffixtree = brother;
  }
  brother = nil;
  if (goon_symbol ())
  {
    alts (false);
    newnode (locaffixtree, brother, asons, (char *) "");
    LINE (brother) = errline;
    RuleCount += 1;
    if (lefths)
    {
      if (rulename != thisname)
      {
        if (input_from_partlist)
          fprintf (stderr, "In %s:\n", cur_part_name);
        fprintf (stderr, "line %d: LHS '%s' expected\n", errline, full_repr (rulename));
        syntaxerrors += 1;
        rulename = thisname;
        dfa = locaffixtree;
      }
      else if (!consistentaffixtype (dfa, locaffixtree))
      {
        if (input_from_partlist)
          fprintf (stderr, "In %s:\n", cur_part_name);
        fprintf (stderr, "line %d: LHS '%s' :", errline, full_repr (rulename));
        affixerrmsg (locaffixtree, dfa);
        syntaxerrors += 1;
      }
    }
  }
  else if (point_symbol ())
  {
    newnode (locaffixtree, brother, asons, "");
    LINE (brother) = errline;
    RuleCount += 1;
    if (lefths)
    {
      if (rulename != thisname)
      {
        if (input_from_partlist)
          fprintf (stderr, "In %s:\n", cur_part_name);
        fprintf (stderr, "line %d: LHS '%s' expected\n", errline, full_repr (rulename));
        rulename = thisname;
        dfa = locaffixtree;
        syntaxerrors += 1;
      }
      else if (!consistentaffixtype (dfa, locaffixtree))
      {
        if (input_from_partlist)
          fprintf (stderr, "In %s:\n", cur_part_name);
        fprintf (stderr, "line %d: LHS '%s' :", errline, full_repr (rulename));
        affixerrmsg (locaffixtree, dfa);
        syntaxerrors += 1;
      }
    }
  }
  else
  {
    errmsg ("MEMBERS");
    skiptopoint_symbol ();
    rules (rnode);
    usefullerrmsg = false;
  }
}


#define LOOKahead_symbol ( thischar == '?' )

setlookahead (lkh)
int *lkh;
{
  if (LOOKahead_symbol)
  {
    *lkh = lookahead;
    getnextchar ();
  }
}


members ()
{
  char *repr;
  int locaffixtree, settype, lkh = 0;
  setlookahead (&lkh);
  if (terminal ())
  {
    if (!comma_symbol ())
    {
      if (strlen (string) == 0);
      else
      {
        newnode (tnode + lkh, nil, nil, string);
        nrterms += 1;
      }
    }
    else
    {
      repr = string;
      members ();
      if (strlen (repr) == 0);
      else
      {
        newnode (tnode + lkh, brother, nil, repr);
        nrterms += 1;
      }
    }
  }
  else if (set ())
  {
    repr = string;
    if (!display ())
    {
      newnode (affixnt, nil, nil, dont_care);
      newnode (derived, nil, brother, "(nil)");
      lastaffixtree = brother;
    }
    else if ((NODENAME (brother) != derived) || (BROTHER (brother) != nil))
      errmsg ("ONE DERIVED AFFIX");
    if (!comma_symbol ())
    {
      newnode (cnode | lastsettype | lkh, nil, brother, repr);
      nrterms += 1;
    }
    else
    {
      settype = lastsettype;
      locaffixtree = brother;
      members ();
      newnode (cnode | settype | lkh, brother, locaffixtree, repr);
      nrterms += 1;
    }
  }
  else if ((name_display_mix ()) || (cut_symbol ()))
  {
    repr = ntname;
    if (!comma_symbol ())
    {
      newnode (ntnode | lkh, nil, brother, repr);
      nrntmems += 1;
    }
    else
    {
      locaffixtree = lastaffixtree;
      members ();
      newnode (ntnode | lkh, brother, locaffixtree, repr);
      nrntmems += 1;
    }
  }
  else if (square_open_symbol ())
  {
    if (lkh)
      errmsg ("No lookahead");
    brother = nil;
    terms ();
    newnode (inherited, nil, brother, "(nil)");
    if (!square_close_symbol ())
      errmsg ("Redirection Close (])");
    locaffixtree = brother;
    brother = nil;
    members ();
    newnode (ntnode, brother, locaffixtree, redirect);
  }
}


#define GETnextchar thischar = getc(input)
#define SKIPset_symbol thischar = getc(input)

set ()
{
  lastsettype = 0;
  if (complement_symbol ())
  {
    if (charindex > maxchars - 256)
      alloc_chartable ();
    lastsettype = complset;
    if (thischar != '{' && thischar != '!')
    {
      errmsg ("SET");
      skiptopoint_symbol ();
      rules (rnode);
      usefullerrmsg = false;
    }
    else if (thischar == '!')
      lastsettype |= morelongset ();

    else
    {
      lastsettype |= moreset ();
    }
  }
  else
  {
    if (thischar != '{' && thischar != '!')
      return false;
    if (charindex > maxchars - 256)
      alloc_chartable ();
    if (thischar == '!')
      lastsettype |= morelongset ();
    else
      lastsettype |= moreset ();
  }
  lastsettype |= sequenceroption ();
  if (lastsettype & nset)
  {
    if (thischar == '!')
    {
      getnextchar ();
      lastsettype &= ~nset;
      lastsettype |= longset;
    }
  }
  return true;
}


morelongset ()
{
  SKIPset_symbol;
  while (thischar != '!')
  {
    if (thischar == '"')
      chartable[charindex++] = '\\';
    else if (thischar == '\\')
    {
      GETnextchar;
      switch (thischar)
      {
      case '\n':
        GETnextchar;
        line += 1;
        continue;
      case '"':
        chartable[charindex++] = '\\';
        chartable[charindex++] = '\\';
      case '0':
      case 'r':
      case 't':
      case 'n':
      case 'f':
      case 'v':
      case 'b':
      case '\\':
        chartable[charindex++] = '\\';
        break;
      case '!':
        break;
      default:
        chartable[charindex++] = '\\';
        chartable[charindex++] = '\\';
        break;
      }
    }
    chartable[charindex++] = thischar;
    GETnextchar;
    if (thischar == '\n')
    {
      errmsg ("not a NEW LINE char in a string");
      return longset;
    }
  }
  SKIPset_symbol;
  string = &chartable[prevcharindex];
  chartable[charindex++] = '\0';
  checkdups ();
  prevcharindex = charindex;
  return longset;
}


moreset ()
{
  do
  {
    SKIPset_symbol;
    while (thischar != '}')
    {
      chartable[charindex++] = thischar;
      GETnextchar;
      if (thischar == '\n')
      {
        errmsg ("not a NEW LINE char in a set");
        line += 1;
        break;
      }
    }

    SKIPset_symbol;
    if (thischar == '}')
      chartable[charindex++] = thischar;

  }
  while (thischar == '}');
  string = &chartable[prevcharindex];
  chartable[charindex++] = '\0';
  prevcharindex = charindex;
  return nset;
}


sequenceroption ()
{
  skiplayout ();
  if (thischar == '*')
  {
    getnextchar ();
    return starset;
  }
  if (thischar == '+')
  {
    getnextchar ();
    return plusset;
  }
  if (thischar == ')')
  {
    getnextchar ();
    return 0;
  }
  return 0;
}


#define QUOTE thischar == '"'
#define NotQUOTE thischar != '"'
#define GETnextTerminal thischar = getc(input)
#define SKIPquote thischar = getc(input)

terminal ()
{
  if (QUOTE)
  {
    if (charindex > maxchars - 256)
      alloc_chartable ();
    SKIPquote;
    mterminal ();
    return true;
  }
  return false;
}


mterminal ()
{
  while (NotQUOTE)
  {
    if (thischar == '\\')
    {
      GETnextchar;
      switch (thischar)
      {
      case '\n':
      case '\f':
      case '\r':
        GETnextchar;
        line += 1;
        continue;
      case '"':
      case '0':
      case 'r':
      case 't':
      case 'n':
      case 'f':
      case 'v':
      case 'b':
      case '\\':
        chartable[charindex++] = '\\';
        break;
      default:
        chartable[charindex++] = '\\';
        chartable[charindex++] = '\\';
        break;
      }
    }
    chartable[charindex++] = thischar;
    GETnextchar;
    if (thischar == '\n')
    {
      errmsg ("not a NEW LINE char in a string");
      return;
    }
  }
  SKIPquote;
  string = &chartable[prevcharindex];
  chartable[charindex++] = '\0';
  prevcharindex = charindex;
  skiplayout ();
}

cut_symbol ()
{
  if (thischar == '-')
  {
    getnextchar ();
    if (thischar == '>')
      getnextchar ();

    else
      errmsg ("SLASH symbol");

    string = REPR (cut);
    ntname = string;
    lastaffixtree = nil;
    brother = nil;
    return true;
  }
  else
    return false;
}


comma_symbol ()
{
  if (thischar == ',')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}

mint_symbol ()
{
  return (isdigit (thischar));
}

super_symbol ()
{
  if (thischar == '@')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}

lattice_symbol ()
{
  if (thischar == '{')
  {
    getnextchar ();
    getnextchar ();
    return true;
  }
  else
    return false;
}

define_symbol (l_g)             /* local or global rule */
int *l_g;
{
  if (thischar == '=')
  {
    *l_g = external;
    getnextchar ();
    return true;
  }
  else if (thischar == ':')
  {
    if (all_external_flag)
      *l_g = external;
    else
      *l_g = rnode;
    *l_g = 0;                   /* not always external */
    getnextchar ();
    return true;
  }
  else
    return false;
}


start_symbol ()
{
  if (thischar == '!')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}


point_symbol ()
{
  if (thischar == '.')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}


dontcare_symbol ()
{
  if (thischar == '_')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}

fact_symbol ()
{
  if (thischar == '*')
  {
    getnextchar ();
    return true;
  }
  else if ((isalpha (thischar)) || (thischar == '"'))
  {
    if (input_from_partlist)
      fprintf (stderr, "In %s:\n", cur_part_name);
    fprintf (stderr, "line %d: * symbol inserted\n", line);
    return true;
  }
  else
    return false;
}

negate_symbol ()
{
  if (thischar == '-')
  {
    getnextchar ();
    return true;
  }
  return false;
}

pos_symbol ()
{
  if (thischar == '+')
  {
    getnextchar ();
    return true;
  }
  return false;
}

plus_symbol ()
{
  if (thischar == '+')
  {
    getnextchar ();
    return true;
  }
  else if ((isalpha (thischar)) || (thischar == '"'))
  {
    if (input_from_partlist)
      fprintf (stderr, "In %s:\n", cur_part_name);
    fprintf (stderr, "line %d: + symbol inserted\n", line);
    return true;
  }
  else
    return false;
}


close_symbol ()
{
  if (thischar == ')')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}

square_open_symbol ()
{
  if (thischar == '[')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}

square_close_symbol ()
{
  if (thischar == ']')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}


open_symbol ()
{
  if (thischar == '(')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}


complement_symbol ()
{
  if (thischar == '^' || thischar == '~')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}


flow_symbol ()
{
  if (thischar == '>')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}


goon_symbol ()
{
  if (thischar == ';')
  {
    getnextchar ();
    return true;
  }
  else
    return false;
}

getfirstchar ()
{
  thischar = getc (input);
  glm_options ();
}

getnextchar ()
{
  thischar = getc (input);
  return skiplayout ();
}


skiplayout ()
{
  int space_skipped = 0;
  while ((thischar == '\t' || thischar == ' ' || thischar == '\n') && (!feof (input)))
  {
    if (thischar == '\n')
      line += 1;
    thischar = getc (input);
    space_skipped = 1;
  }

  if (thischar == '#')
  {
    do
    {
      thischar = getc (input);
    }
    while ((thischar != '#' && thischar != '\n') && (!feof (input)));

    skiplayout ();
  }
  if (thischar == '%')
  {
    do
    {
      thischar = getc (input);
      if (thischar == '\n')
        line += 1;

    }
    while ((thischar != '%') && (!feof (input)));

    getnextchar ();
  }
  return space_skipped;
}


displayoption ()
{
  if (display ())
    lastaffixtree = brother;
  else
  {
    lastaffixtree = nil;
    brother = nil;
  }
}


display ()
{
  if (open_symbol ())
  {
    affixes ();
    lastaffixtree = brother;
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


affixes ()
{
  int sons = 0;
  if (flow_symbol ())
  {
    terms ();
    sons = brother;
    if (comma_symbol ())
    {
      affixes ();
      newnode (inherited, brother, sons, "(nil)");
    }
    else
      newnode (inherited, nil, sons, "(nil)");
  }
  else
  {
    terms ();
    sons = brother;
    if (flow_symbol ())
    {
      if (comma_symbol ())
      {
        affixes ();
        newnode (derived, brother, sons, "(nil)");
      }
      else
        newnode (derived, nil, sons, "(nil)");
    }
    else
    {
      int term;
      if (comma_symbol ())
      {
        affixes ();
        newnode (lattice_affix, brother, sons, "(nil)");
      }
      else
        newnode (lattice_affix, nil, sons, "(nil)");
      term = SON (brother);;
      if ((BROTHER (term) != nil) || (NODENAME (term) != affixnt))
      {
        errmsg ("FLOW (>) symbol");
        syntaxerrors += 1;
      }
      if (SON (term) != nil)
      {
        errmsg ("FLOW (>) symbol");
        syntaxerrors += 1;
      }
    }
  }
}


terms ()
{
  int sons;
  char *afxnt, *afxtm;
  if ((name ()) || (dontcare ()) || affixpr ())
  {
    afxnt = string;
    if (plus_symbol ())
    {
      terms ();
      newnode (affixnt, brother, nil, afxnt);
    }
    else if (fact_symbol ())
    {
      factors ();
      newnode (affixnt, brother, nil, afxnt);
      sons = brother;
      if (plus_symbol ())
      {
        terms ();
        newnode (factor, brother, sons, "(nil)");
      }
      else
        newnode (factor, nil, sons, "(nil)");
    }
    else if (open_symbol ())
    {
      affixpr ();
      newnode (affixnt, brother, nil, afxnt);
      sons = brother;
      if (plus_symbol ())
      {
        terms ();
        newnode (factor, brother, sons, "(nil)");
      }
      else
        newnode (factor, nil, sons, "(nil)");
    }
    else
      newnode (affixnt, nil, nil, afxnt);
  }
  else if (terminal ())
  {
    afxtm = string;
    if (plus_symbol ())
    {
      terms ();
      newnode (affixtm, brother, nil, afxtm);
    }
    else if (fact_symbol ())
    {
      factors ();
      newnode (affixtm, brother, nil, afxtm);
      sons = brother;
      if (plus_symbol ())
      {
        terms ();
        newnode (factor, brother, sons, "(nil)");
      }
      else
        newnode (factor, nil, sons, "(nil)");
    }
    else
      newnode (affixtm, nil, nil, afxtm);
  }
  else
  {
    errmsg ("TERM");
    if (thischar == '>' || thischar == ',' || thischar == ')' || thischar == '+')
      newnode (affixtm, nil, nil, "");
    else
    {
      skiptopoint_symbol ();
      rules (rnode);
      usefullerrmsg = false;
    }
  }
}

char dc_repr[128][4] = {
  "x_a", "x_b", "x_c", "x_d", "x_e", "x_f", "x_g", "x_h", "x_i", "x_j",
  "x_k", "x_l", "x_m", "x_n", "x_o", "x_p", "x_q", "x_r", "x_s", "x_t",
  "x_u", "x_v", "x_w", "x_x", "x_y", "x_z", "x_A", "x_B", "x_C", "x_D",
  "x_E", "x_F", "x_G", "x_H", "x_I", "x_J", "x_K", "x_L", "x_M", "x_N",
  "x_O", "x_P", "x_Q", "x_R", "x_S", "x_T", "x_U", "x_V", "x_W", "x_X",
  "x_Y", "x_Z", "x_0", "x_1", "x_2", "x_3", "x_4", "x_5", "x_6", "x_7",
  "x_8", "x_9", "X_a", "X_b",
  "y_a", "y_b", "y_c", "y_d", "y_e", "y_f", "y_g", "y_h", "y_i", "y_j",
  "y_k", "y_l", "y_m", "y_n", "y_o", "y_p", "y_q", "y_r", "y_s", "y_t",
  "y_u", "y_v", "y_w", "y_x", "y_y", "y_z", "y_A", "y_B", "y_C", "y_D",
  "y_E", "y_F", "y_G", "y_H", "y_I", "y_J", "y_K", "y_L", "y_M", "y_N",
  "y_O", "y_P", "y_Q", "y_R", "y_S", "y_T", "y_U", "y_V", "y_W", "y_X",
  "y_Y", "y_Z", "y_0", "y_1", "y_2", "y_3", "y_4", "y_5", "y_6", "y_7",
  "y_8", "y_9", "Y_a", "Y_b",
};


int dc_cnt = 0;
int affixpr_root = 0;
affixpr ()
{
  int was_open = false;
  if ((was_open = open_symbol ()) || name ())
  {

    int save_brother = brother;
    char *alias_name;
    if (!was_open && !open_symbol ())
      return true;
    alias_name = dc_repr[dc_cnt++ & 127];
    brother = 0;
    terms ();
    if (close_symbol ())
    {
      affixpr_root = brother;
      brother = save_brother;
      string = alias_name;
    }
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

dontcare ()
{
  if (dontcare_symbol ())
  {
    string = dc_repr[dc_cnt++ & 127];
    return true;
  }
  return false;
}

dontcare2 ()
{
  string = dc_repr[dc_cnt++ & 127];
  return true;
}

factors ()
{
  char *afxnt, *afxtm;
  if ((name ()) || (dontcare ()))
  {
    afxnt = string;
    if (fact_symbol ())
    {
      factors ();
      newnode (affixnt, brother, nil, afxnt);
    }
    else
      newnode (affixnt, nil, nil, afxnt);
  }
  else if (terminal ())
  {
    afxtm = string;
    if (fact_symbol ())
    {
      factors ();
      newnode (affixtm, brother, nil, afxtm);
    }
    else
      newnode (affixtm, nil, nil, afxtm);
  }
  else
  {
    errmsg ("FACTOR");
    skiptopoint_symbol ();
    rules (rnode);
    usefullerrmsg = false;
  }
}


errmsg (msg)
char *msg;
{
  if (usefullerrmsg)
  {
    if (input_from_partlist)
      fprintf (stderr, "In %s:\n", cur_part_name);
    fprintf (stderr, "line %d: char `%c': %s expected\n", line, thischar, msg);
    syntaxerrors += 1;
    if (syntaxerrors > 7)
      exit (12);
  }
}

skiptopoint_symbol ()
{
  while ((thischar != '.') && (!feof (input)))
    getnextchar ();
  getnextchar ();
}

skip_term ()
{
  while ((thischar != '>' || thischar != ')') && (!feof (input)))
    getnextchar ();
}

startspecification ()
{
  if (start_symbol ())
  {
    if (startname == NULL)
      startname = string;
    else if (startname != string)
      errmsg ("no DOUBLE startspecification");
    return true;
  }
  return false;
}

consistentaffixtype (def, app)
int def, app;
{
  for (; def != nil; def = BROTHER (def), app = BROTHER (app))
    if (app == nil)
      return false;
    else if (NODENAME (def) == NODENAME (app))
    {
      if (NODENAME (def) == lattice_affix)
        if (DEF (LATTICE_DEF (SON (def))) != DEF (LATTICE_DEF (SON (app))))
        {
          if (input_from_partlist)
            fprintf (stderr, "In %s:\n", cur_part_name);
          fprintf (stderr, "`%s' and `%s' occur in different lattices\n", FREPR (SON (def)), FREPR (SON (app)));

          return false;
        }
    }
    else
      return false;

  if (app != nil)
    return false;
  return true;
}

valid_overloaded_affixtype (def, app, parent_app)
int def, app, parent_app;
{
  int app_afxtree;
  int def_afxtree;
  int count = 0;
  int prev_afxtree;

  if (consistentaffixtype (def, app))
    return true;

  /* allow missing all derived affixes only */
  for (app_afxtree = app; app_afxtree != nil; app_afxtree = BROTHER (app_afxtree))
    if (NODENAME (app_afxtree) == derived)
      return false;
    else
      count += 1;

  /* number of inherited affixes must match */
  for (def_afxtree = def; def_afxtree != nil; def_afxtree = BROTHER (def_afxtree))
  {
    if (NODENAME (def_afxtree) != derived)
      count -= 1;
  }

  if (count)
    return false;               /* number of inherited affixes don't match */

  /* check valid abreviated affix type */
  /* fill in missing dummy derived affixes */
  prev_afxtree = nil;
  for (def_afxtree = def, app_afxtree = app; def_afxtree != nil; def_afxtree = BROTHER (def_afxtree))
  {
    if (NODENAME (def_afxtree) == derived)
    {
      dontcare2 ();
      newnode (affixnt, nil, nil, string);
      newnode (derived, nil, brother, "");
      if (app_afxtree == nil)
      {
        if (prev_afxtree == nil)
          SON (parent_app) = brother;
        else
          BROTHER (prev_afxtree) = brother;

        prev_afxtree = brother;
      }
      else
      {
        /* insert before current node */
        if (prev_afxtree == nil)
        {
          SON (parent_app) = brother;
        }
        else
        {
          BROTHER (prev_afxtree) = brother;
        }
        BROTHER (brother) = app_afxtree;
      }
      prev_afxtree = brother;
      app_afxtree = BROTHER (brother);
    }
    else
    {
      prev_afxtree = app_afxtree;
      app_afxtree = BROTHER (app_afxtree);
    }
  }
  return true;
}

printaffixtype (channel, afxt)
FILE *channel;
int afxt;
{
  for (; afxt != nil; afxt = BROTHER (afxt))
    if (NODENAME (afxt) == derived)
      fprintf (channel, "d");
    else if (INHERITED (afxt))
      fprintf (channel, "i");
    else
      fprintf (channel, "a%ld", DEF (LATTICE_DEF (SON (afxt))));
}


affixerrmsg (original, bad_copy)
int original, bad_copy;
{
  fprintf (stderr, "Wrong affix type: (");
  printaffixtype (stderr, bad_copy);
  fprintf (stderr, ") expected affix type: (");
  printaffixtype (stderr, original);
  fprintf (stderr, ")\n");
}

stddefs ()
{
  int i_flag = index_flag;
  index_flag = false;
  get_stddefs ();
  input = fopen (stddefs_g, "r");
  if (input == NULL)
  {
    fprintf (stderr, "glammar installation error: %s: No such file\n", stddefs_g);
    exit (10);
  }
  line = 1;
  getnextchar ();
  rules (rnode);
  laststdpred = brother;
  if (hack_flag)
  {
    (void) sprintf (stddefs_g, "%s%sglext.g", hack_dir, dirsep);
    fclose (input);
    input = fopen (stddefs_g, "r");
    if (input == NULL)
    {
      fprintf (stderr, "glammar installation error: %s: No such file\n", stddefs_g);
      exit (10);
    }
    line = 1;
    getnextchar ();
    rules (rnode);
    firstnode = brother;
    getlastnode ();
    BROTHER (lastnode) = laststdpred;
    laststdpred = firstnode;

  }
  fclose (input);
  index_flag = i_flag;
  for (cut = laststdpred; (cut != nil) && (!mystrcmp (REPR (cut), "cut")); cut = BROTHER (cut));
  if (cut == nil)
  {
    fprintf (stderr, "glammar installation error: cut missing\n");
    exit (12);
  }
}

addpart ()
{
  char *x = thispart, *p_base;
  if (hide_flag)
  {
    strcat (partptr, ".glammar/");
    partptr += 9;
  }
  for (p_base = partptr; *x != '\0'; x++)
  {
    char ds = DIRSEP;
    if (*x == ds)
    {
      partptr = p_base;
    }
    else
    {
      *partptr++ = *x;
    }
  }
  *partptr++ = '.';
  *partptr++ = 'o';
  *partptr++ = ' ';
  *partptr = '\0';
}


copy_display ()
{
  copy_affixes (defaffixtree);
}

copy_affixes (afx)
int afx;
{
  int b;
  if (afx == nil)
    return;
  copy_affixes (BROTHER (afx));
  b = brother;
  brother = nil;
  copy_terms (SON (afx));
  newnode (NODENAME (afx), b, brother, "");
}

copy_terms (term)
int term;
{
  if (term == nil)
    return;
  copy_terms (BROTHER (term));
  newnode (NODENAME (term), brother, nil, REPR (term));
}


metarule (ruletype)
int ruletype;
{
  int type;
  char *thisname = string;
  int b = brother, ln = line;
  if (ismetarule (&type))
    switch (type)
    {
    case One:
      if (index_flag)
      {
        if (input_from_partlist)
          fprintf (indexfile, "%s (M), file: `%s' %d\n", full_repr (thisname), thispart, line);
        else
          fprintf (indexfile, "%s (M), %d\n", full_repr (thisname), line);
      }
      if (ruletype & docompile)
        newdefnode (meta_prod_rule, lastmetarule, SON (brother), docompile, rulename);
      else
        newnode (meta_prod_rule, lastmetarule, SON (brother), rulename);
      lastmetarule = brother;
      break;
    case OneStar:
      if (index_flag)
      {
        if (input_from_partlist)
          fprintf (indexfile, "%s (P), file: `%s' %d\n", full_repr (thisname), thispart, line);
        else
          fprintf (indexfile, "%s (P), %d\n", full_repr (thisname), line);

      }
      if (ruletype & docompile)
        newdefnode (meta_OneStar, lastmetarule, SON (brother), docompile, rulename);
      else
        newnode (meta_OneStar, lastmetarule, SON (brother), rulename);
      lastmetarule = brother;
      break;
    case Mult:
      if (index_flag)
      {
        if (input_from_partlist)
          fprintf (indexfile, "%s (S), file: `%s' %d\n", full_repr (thisname), thispart, line);
        else
          fprintf (indexfile, "%s (S), %d\n", full_repr (thisname), line);
      }
      fprintf (stderr, "line %d in metarule `%s':\n\
     Metaproductions can be implemented using\n\
     1) Superrules  (ie  superrule :: @ hyperrule).\n\
         Only applications on *defining* occurrences are recognized.\n\
     2) Lattices (i.e latticerule :: {} mem1; mem2 . ).\n\
     See the manual.\n\n", ln, full_repr (thisname));
      syntaxerrors += 1;
      break;
    case Super:
      if (index_flag)
      {
        if (input_from_partlist)
          fprintf (indexfile, "%s (S), file: `%s' %d\n", full_repr (thisname), thispart, line);
        else
          fprintf (indexfile, "%s (S), %d\n", full_repr (thisname), line);
      }
      if (ruletype & docompile)
        newdefnode (meta_prod_rule, lastmetarule, brother, docompile, rulename);
      else
        newnode (meta_prod_rule, lastmetarule, brother, rulename);
      lastmetarule = brother;
      break;

    case Lattice:
      if (index_flag)
      {
        if (input_from_partlist)
          fprintf (indexfile, "%s (A), file: `%s' %d\n", full_repr (thisname), thispart, line);
        else
          fprintf (indexfile, "%s (A), %d\n", full_repr (thisname), line);
      }
      newnode (0, nil, brother, rulename);
      if (first_lattice == nil)
        first_lattice = brother;
      else
        BROTHER (last_lattice) = brother;
      last_lattice = brother;
      break;
    case Mint:
      if (index_flag)
      {
        if (input_from_partlist)
          fprintf (indexfile, "%s (P), file: `%s' %d\n", full_repr (thisname), thispart, line);
        else
          fprintf (indexfile, "%s (P), %d\n", full_repr (thisname), line);
      }
      if (ruletype & docompile)
        newdefnode (meta_Mint, lastmetarule, brother, docompile, rulename);
      else
        newnode (meta_Mint, lastmetarule, brother, rulename);
      lastmetarule = brother;
      break;

    default:
      ;
    }
  else
    return false;
  brother = b;
  return true;
}

ismetarule (type)
int *type;
{
  int l_g;
  if (define_symbol (&l_g))
  {
    if (superrule ())
      *type = Super;
    else if (latticerule ())
      *type = Lattice;
    else if (numberrule ())
      *type = Mint;
    else
      *type = abstractionrule ();
    return true;
  }
  return false;
}

superrule ()
{
  if (super_symbol ())
  {
    if (name ())
    {
      char *repr = string;
      if (!display ())
      {
        newnode (affixnt, nil, nil, "x");
        newnode (derived, nil, brother, "");
      }
      else if ((NODENAME (brother) != derived) || (BROTHER (brother) != nil))
        errmsg ("ONE DERIVED AFFIX");
      newnode (supernt, nil, brother, repr);
    }
    else
      errmsg ("Super nonterminal");
    if (!point_symbol ())
    {
      skiptopoint_symbol ();
      errmsg ("Super metarule");
    }
    return true;
  }
  else
    return false;
}

#define DIGIT isdigit(thischar)
numberrule ()
{
  if (negate_symbol ())
  {
    chartable[charindex++] = '-';
    if (!DIGIT)
    {
      chartable[charindex++] = '\0';
      errmsg ("NUMBER");
      skiptopoint_symbol ();
      return true;
    }
  }
  if (pos_symbol ());
  if (!DIGIT)
    return false;
  while (DIGIT)
  {
    chartable[charindex++] = thischar;
    GETnextchar;
    if (thischar == '\\')
    {
      GETnextchar;
      GETnextchar;              /* skip nlcr */
    }
  }
  string = &chartable[prevcharindex];
  chartable[charindex++] = '\0';
  prevcharindex = charindex;
  skiplayout ();
  newnode (affixtm, nil, nil, string);
  if (!point_symbol ())
  {
    skiptopoint_symbol ();
    errmsg ("POINT symbol");
  }
  return true;
}


abstractionrule ()
{
  int type;                     /* One, OneStar, or Mult */

  /* 
   * One :: "a" + empty. 
   * OneStar :: "a" * One.
   * Mult :: OneStar; One.
   */
  type = meta_alts ();
  if (point_symbol ());
  else
  {
    errmsg ("POINT symbol");
    skiptopoint_symbol ();
    rules (rnode);
    exit (1);
  }
  return type;
}


meta_alts ()
{
  int sons, type = One;
  brother = nil;
  type = meta_terms ();
  sons = brother;
  if (goon_symbol ())
  {
    type = Mult;
    meta_alts ();
    newnode (meta_alt, brother, sons, "meta_alt");
  }
  else
    newnode (meta_alt, nil, sons, "meta_alt");
  return type;
}

meta_terms ()
{
  int sons, type = One;
  char *afxnt, *afxtm;
  if (name ())
  {
    afxnt = string;
    if (plus_symbol ())
    {
      type = meta_terms ();
      newnode (affixnt, brother, nil, afxnt);
    }
    else if (fact_symbol ())
    {
      type = OneStar;
      meta_factors ();
      newnode (affixnt, brother, nil, afxnt);
      sons = brother;
      if (plus_symbol ())
      {
        type = meta_terms ();
        newnode (factor, brother, sons, "(nil)");
      }
      else
        newnode (factor, nil, sons, "(nil)");
    }
    else
      newnode (affixnt, nil, nil, afxnt);
  }
  else if (terminal ())
  {
    afxtm = string;
    if (plus_symbol ())
    {
      type = meta_terms ();
      newnode (affixtm, brother, nil, afxtm);
    }
    else if (fact_symbol ())
    {
      type = OneStar;
      meta_factors ();
      newnode (affixtm, brother, nil, afxtm);
      sons = brother;
      if (plus_symbol ())
      {
        meta_terms ();
        newnode (factor, brother, sons, "(nil)");
      }
      else
        newnode (factor, nil, sons, "(nil)");
    }
    else
      newnode (affixtm, nil, nil, afxtm);
  }
  else if (set ())
  {
    type = Mult;
    if (plus_symbol ())
    {
      int lastst = lastsettype;
      char *repr = string;
      meta_terms ();
      newnode (cnode | lastst, brother, nil, repr);
    }
    else
      newnode (cnode | lastsettype, nil, nil, string);
  }
  else
    newnode (affixtm, nil, nil, "");
  return type;
}

meta_factors ()
{
  char *afxnt, *afxtm;
  if (name ())
  {
    afxnt = string;
    if (fact_symbol ())
    {
      meta_factors ();
      newnode (affixnt, brother, nil, afxnt);
    }
    else
      newnode (affixnt, nil, nil, afxnt);
  }
  else if (terminal ())
  {
    afxtm = string;
    if (fact_symbol ())
    {
      meta_factors ();
      newnode (affixtm, brother, nil, afxtm);
    }
    else
      newnode (affixtm, nil, nil, afxtm);
  }
  else
  {
    errmsg ("FACTOR");
    skiptopoint_symbol ();
    rules (rnode);
    usefullerrmsg = false;
  }
}

latticerule ()
{
  if (lattice_symbol ())
  {
    brother = nil;
    lattice_mems ();
    if (point_symbol ());
    else
    {
      errmsg ("POINT symbol");
      skiptopoint_symbol ();
      rules (rnode);
      exit (1);
    }
    return true;
  }
  return false;
}

lattice_mems ()
{
  char *afxnt, *afxtm;
  if (name ())
  {
    afxnt = string;
    if (goon_symbol ())
    {
      lattice_mems ();
      newnode (0, brother, nil, afxnt);
    }
    else
      newnode (0, nil, nil, afxnt);
  }
  else
  {
    errmsg ("LATTICE MEMBER");
    skiptopoint_symbol ();
    rules (rnode);
    exit (1);
  }
}

checkdups ()
{
  unsigned char *s = string;
  unsigned char c;
  unsigned char bv[255];

  if (underscore_allowed)
    return true;                /* parsing glammar.g */
  memset (bv, 0, 255);

  while (c = *s++)
  {
    if (c > 127)
    {
      errmsg ("Illegal char (>127) in set");
      return;
    }
    if (c == '\\')
      c = *s++ + 128;

    if (bv[c])
    {
      syntaxerrors += 1;
      if (input_from_partlist)
        fprintf (stderr, "In %s:\n", cur_part_name);
      if (c >= 128)
        fprintf (stderr, "line %d: Duplicate char '\\%c' in set\n", line, c - 128);
      else
        fprintf (stderr, "line %d: Duplicate char '%c' in set\n", line, c);
      return;
    }
    bv[c] = 1;
  }
}
