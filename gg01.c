
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

/* file    driver:  defines all global symbols, reads args, calls c-compiler */
#include "gg1.h"

AST *ast;

long endofsentence, nestaralt, nestarset, detnestarset, detprefix, detprefix2, equalsempty, meta_empty, setinputptrto, getlist_, resetinputptr, getip, falseip, restoreip, shadow, unshadow, firstshadow, nextshadow, metaterminal, metaterminal2, astindex, brother, root, startnode, underscore_allowed = true, lastnode, laststdpred, first_lattice = nil, symbol_table_size = 0, lastmetarule, laststdmetarule, charindex, hashindex, prevcharindex, usefullerrmsg, syntaxerrors, line, init_sum = 100,     /* current 
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   version 
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   number 
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 */
                                        sum = 0,
                                        nrterms,
                                        nrntmems,
                                        lastsettype,
                                        lastaffixtree,
                                        defaffixtree,
                                        ulhs,
                                        reccount,
                                        RuleCount,
                                        NtCount,
                                        qCount,
                                        aCount,
                                        input_from_partlist,
                                        fulltrace_flag,
                                        afx_opt_flag,
                                        memopt_flag,
                                        no_strip_flag = 0,
                                        AM_flag,
                                        lat_trad_flag,
                                        trace_flag,
                                        syntax_flag,
                                        errormsg_flag,
                                        stat_flag,
                                        fullstat_flag,
                                        memo_flag,
                                        nr_of_memo_alts,
                                        index_flag,
                                        tagindex_flag,
                                        hide_flag,
                                        input_from_stdin,
                                        lift_flag,
                                        meta_uniq_flag,
                                        det_flag,
                                        ambiguous_flag,
                                        separate_comp_flag,
                                        noerrmsg_flag,
                                        keep_flag,
                                        tree_flag,
                                        id_tree_flag,
                                        parse_tree_flag,
                                        hack_flag,
                                        cfg_flag,
                                        backtrace_mark,
                                        affix_trace,
                                        gnu_flag,
                                        link_flag,
                                        verbose_flag,
                                        eag_flag,
                                        name_clashes,
                                        optimize_flag,
                                        no_recursion_elm_flag = 1, all_external_flag, start_predicate_flag = 0;

unsigned max_item, maxchars = MAX_CHARS;


long runtime_stksize = 500000l;
int runtime_input_size;

char *getglammarenv ();
char *glm_libdir ();
char *glm_incdir ();
char thispart[256], outputfile[256], stddefs_g[256],
                                        thischar,
                                        *string,
                                        *ntname,
                                        *dont_care = "dont_care",
                                        *redirect = "redirection clause",
                                        **nametable,
                                        *chartable,
                                        commandbuffer[1024],
                                        tempbuf[1024],
                                        c_flags[256],
                                        indexfilename[256],
                                        tagindexfilename[256],
                                        ex_filename[256], hack_dir[256], sep_comp_part[256], *rulename, *startname;

FILE *output, *input, *partlist, *indexfile, *tagindexfile;

int nlcr, explintersect, intersect, transformlattice, tltraditional, assign,
                                        transformlatticeterm, tltraditionalterm,
                                        skip, cut, equal, init_one_star, add_to, add_to_used = 0,
                                        pair, unpair, repair, notequal, where, initmint, initmeta, evalmeta;
char parts[2000];
char *partptr = parts;
#ifdef ATARI
const long _stksize = 100000l;
#endif

char dirsep[2];

main (arg_count, arguments)
int arg_count;
char **arguments;
{
  int input_file_set = false;
  int output_file_set = false;
  char *arg_zero = *arguments;
  char *last_letter = *arguments;

  dirsep[0] = DIRSEP;
  dirsep[1] = '\0';
  AM_flag = true;
  affix_trace = true;
  all_external_flag = false;
  ambiguous_flag = false;
  backtrace_mark = 0;
  cfg_flag = false;
  det_flag = false;
  eag_flag = false;
  errormsg_flag = false;
  fulltrace_flag = true;
  gnu_flag = false;
  hack_flag = false;
  id_tree_flag = false;
  index_flag = false;
  tagindex_flag = true;
  {
    FILE *xxx = fopen (".glammar", "r");
    if (xxx == NULL)
      hide_flag = false;
    else
    {
      hide_flag = true;
      fclose (xxx);
    }
  }
  input_from_partlist = false;
  input_from_stdin = false;
  keep_flag = false;
  lift_flag = true;
  link_flag = false;
  memo_flag = false;
  memopt_flag = false;
  meta_uniq_flag = false;
  noerrmsg_flag = false;
  optimize_flag = false;
  afx_opt_flag = false;
  parse_tree_flag = false;
  separate_comp_flag = false;
  stat_flag = false;
  syntax_flag = false;
  trace_flag = true;
  tree_flag = false;

  max_item = MAX_ITEMS;

  chartable = (char *) malloc (maxchars);
  nametable = (char **) malloc (maxnt * sizeof (char *));
  if (arg_count == 1)
  {
    print_usage ();
    exit (1);
  }
  init_sum = 0;
  for (arguments += arg_count - 1; arg_zero != *arguments; arguments -= 1)
  {
    switch (**arguments)
    {
    case '-':
      switch (*(++*arguments))
      {
      case '\0':
        if (output_file_set && !input_file_set)
        {
          input_from_stdin = true;
          input_file_set = true;
        }
        else if (!output_file_set)
        {
          output = stdout;
          output_file_set = true;
        }
        else
        {
          fprintf (stderr, "glammar: `-' ignored\n");
        }
        break;
      case 'L':
        lat_trad_flag = true;
        init_sum += (int) 'L';
        break;
      case 'l':
        sscanf (*arguments + 1, "%ld", &runtime_stksize);
        runtime_stksize = 100000l * runtime_stksize;
        break;
      case 'A':
        afx_opt_flag = true;
        break;
      case 'a':
        init_sum += (int) 'a';
        ambiguous_flag = true;
        break;
      case 'C':
      case 'c':
        hack_flag = true;
        (void) strcpy (hack_dir, *arguments + 1);
        break;
      case 'E':
        init_sum += (int) 'E';
        errormsg_flag = true;
        det_flag = true;
        break;
      case 'e':
        eag_flag = true;
        break;
      case 'F':
      case 'f':
        lift_flag = false;
        init_sum += (int) 'F';
        break;
      case 'd':
        det_flag = true;
        break;

      case 'G':
      case 'g':
        gnu_flag = true;
        break;
      case 'H':
        hide_flag = true;
        break;
      case 'h':
      case '?':
        print_usage ();
        exit (0);
      case 'I':
        init_sum += (int) 'i';
        fullstat_flag = true;
      case 'i':
        stat_flag = true;
        init_sum += (int) 'i';
        break;
      case 'K':
      case 'k':
        keep_flag = true;
        break;
      case 'n':
        start_predicate_flag = true;
        break;
      case 'P':
        all_external_flag = true;
      case 'p':
        separate_comp_flag = true;
        input_from_partlist = true;
        input_file_set = true;
        break;
      case 'X':
        syntax_flag = true;
      case 'x':
        index_flag = true;
        break;
      case 'R':
        no_recursion_elm_flag = false;
        break;
      case 'r':
        switch (*(*arguments + 1))
        {
        case '\0':
          tree_flag = true;
          break;
        case 'i':
          id_tree_flag = true;
          break;
        case 'p':
          parse_tree_flag = true;
          break;
        case 'c':
          cfg_flag = true;
          break;
        default:;
        }
        tree_flag = true;
        break;
      case 'O':
        (void) sprintf (tempbuf, " -%c %s ", *(*arguments + 1), *arguments + 2);
        (void) strcat (c_flags, tempbuf);
        break;
      case 'o':
        (void) sprintf (tempbuf, " -%s ", *arguments + 1);
        (void) strcat (c_flags, tempbuf);
        init_sum += 9;
        break;
      case 'S':
        no_strip_flag = 1;
        break;
      case 's':
        syntax_flag = true;
        break;
      case 'T':
        fulltrace_flag = false;
        trace_flag = false;
        init_sum += (int) 'T';
        break;
      case 't':
        init_sum += (int) 't';
        trace_flag = true;
        break;
      case 'V':
        verbose_flag = true;
        break;
      case 'v':
        fprintf (stderr, "Glammar (c) %s\n", VERSION);
      case 'U':
        meta_uniq_flag = true;
        break;
      case 'M':
        AM_flag = false;        /* No advance memoizing */
      case 'm':
        memo_flag = true;
        sscanf (*arguments + 1, "%d", &runtime_input_size);
        if (runtime_input_size < 1)
          runtime_input_size = 2;
        runtime_input_size <<= 10;
        break;
      default:
        fprintf (stderr, "glammar: Unrecognized option `-%c'\n   \
If you want to pass this option to \"cc\" type: glammar -o%s\n", **arguments, *arguments);
      }
      break;
    default:
    {
      char *fname, *arg = *arguments;

      if (output_file_set && !input_file_set)
      {
        fname = thispart;
        while (*arg != '\0')
          *fname++ = *arg++;
        *fname++ = '.';
        last_letter = fname++;
        *fname++ = '\0';
        *last_letter = 'g';
        input = fopen (thispart, "r");
        if (input == NULL)
        {
          *last_letter = 'p';
          input = fopen (thispart, "r");
          if (input != NULL)
          {
            fclose (input);
            input_from_partlist = true;
            separate_comp_flag == true;
          }
          else
          {
            fprintf (stderr, "glammar: %s: No such file\n", thispart);
            exit (1);
          }
        }
        input_file_set = true;
        *--last_letter = '\0';
      }
      else if (!output_file_set)
      {
        fname = thispart;

        while (*arg != '\0')
          *fname++ = *arg++;

        *fname++ = '.';
        last_letter = fname++;
        *fname++ = '\0';
        *--last_letter = '_';
        *++last_letter = '.';
        *++last_letter = 'c';
        *++last_letter = '\0';
        output_file_set = true;
        (void) strcpy (outputfile, thispart);
        last_letter -= 4;
        *++last_letter = '.';
        *++last_letter = 'g';
        *++last_letter = '\0';
        input = fopen (thispart, "r");
        if (input == NULL)
        {
          *(last_letter - 1) = 'p';
          input = fopen (thispart, "r");
          if (input != NULL)
          {
            fclose (input);
            input_from_partlist = true;
            separate_comp_flag == true;
          }
          else
            *(last_letter - 1) = 'g';
        }
        else
          fclose (input);
        last_letter -= 3;
      }
      else
      {
        fprintf (stderr, "glammar: argument `%s' ignored\n", arguments);
      }
      break;
    }
    }
  }
  if (!output_file_set || (input_file_set && !input_from_partlist))
  {
    fprintf (stderr, "Usage: glammar [ options ] File | -\n");
    exit (1);
  }
  if (!input_file_set)
  {
    if (output == stdout)
    {
      input = stdin;
      input_from_stdin = true;
    }
    else
      input = fopen (thispart, "r");

    if (input == NULL)
    {
      fprintf (stderr, "glammar: %s: No such file or directory\n", thispart);
      exit (1);
    }
    input_file_set = true;
  }
  if (output != stdout)
  {
    *++last_letter = '\0';
    (void) sprintf (ex_filename, "%s", thispart);
  }
  if ((index_flag) && (output == stdout))
    indexfile = stdout;
  else if (index_flag)
  {
    (void) sprintf (indexfilename, "%s.x", thispart);
    indexfile = fopen (indexfilename, "w");
    if (indexfile == NULL)
    {
      fprintf (stderr, "Glammar fatal msg: cannot write to %s\n", indexfilename);
      exit (1);
    }
  }
  if ((tagindex_flag) && (output == stdout))
    tagindexfile = stdout;
  else if (tagindex_flag)
  {
    (void) sprintf (tagindexfilename, "gtags", thispart);
    tagindexfile = fopen (tagindexfilename, "a");
    if (tagindexfile == NULL)
    {
      fprintf (stderr, "Glammar warning msg: cannot write to %s\n", tagindexfilename);
    }
    fclose (tagindexfile);
  }
  /* do first pass */

  ast = (AST *) malloc ((max_item + 2) * sizeof (AST));

  if (ast == NULL)
  {
    fprintf (stderr, "Glammar fatal msg: not enough heap space available\n");
    exit (1);
  }
  if (verbose_flag)
    fprintf (stderr, "Arguments read and Storage allocated.\n");
  if (separate_comp_flag && !link_flag)
  {
    char *y = sep_comp_part, *x = outputfile, *x0;
    if (hide_flag)
    {
      (void) strcpy (outputfile, ".glammar/");
      x += 9;
    }
    for (x0 = x; *y != '\0'; y++)
      if (*y == DIRSEP)
        x = x0;
      else
        *x++ = *y;
    *x++ = '.';
    *x++ = 'c';
    *x = '\0';
  }
  compile ();
  free (ast);
  free (nametable);
  free (chartable);
  if (syntax_flag)
    exit (syntaxerrors ? 1 : 0);
  /* do C-assembler pass */
  if (output == stdout)
    exit (0);
  if (verbose_flag)
    fprintf (stderr, "C-compiler phase\n");

  (void) strcat (commandbuffer, CC);

  (void) strcat (commandbuffer, CCFLAGS);
#ifndef VMS
  if (separate_comp_flag && !link_flag)
    (void) strcat (commandbuffer, "-c ");
  else
  {
    (void) strcat (commandbuffer, "-o ");
    (void) strcat (commandbuffer, ex_filename);

  }
#endif
  (void) strcat (commandbuffer, c_flags);

#ifdef WARN
  (void) strcat (commandbuffer, " -w ");
#else
  (void) strcat (commandbuffer, " ");
#endif

#ifdef DEC_MIPS
  (void) strcat (commandbuffer, " -G 0 ");
#else
  (void) strcat (commandbuffer, " ");
#endif
  if (keep_flag)
    (void) strcat (commandbuffer, " -temp=. ");

  if (!link_flag)
  {
    (void) strcat (commandbuffer, outputfile);
    (void) strcat (commandbuffer, " ");
  }
  else
    (void) strcat (commandbuffer, parts);

#ifndef VMS
  if (!separate_comp_flag || link_flag)
  {
    if (hack_flag)
    {
      (void) strcat (commandbuffer, hack_dir);
      (void) strcat (commandbuffer, dirsep);
      (void) strcat (commandbuffer, "glext.o");
    }
    (void) strcat (commandbuffer, " ");
    (void) strcat (commandbuffer, glm_libdir ());
    (void) strcat (commandbuffer, dirsep);
    (void) strcat (commandbuffer, "glammar.a ");
#ifndef NOMP
#ifndef MP
    (void) strcat (commandbuffer, "-lmp ");
#else /* MP */
    (void) strcat (commandbuffer, MP);
#endif /* MP */
#endif /* NOMP */
  }
#endif /* VMS */

  if (verbose_flag)
    fprintf (stderr, "%s\n", commandbuffer);
  if (system (commandbuffer) > 0)
  {
    fprintf (stderr, "Glammar fatal msg: (C) assembler phase failed.\n");
    exit (1);
  }
#ifndef NOSTRIP
  if (!no_strip_flag)
  {
#ifndef CYGWIN
    (void) sprintf (commandbuffer, "strip %s", ex_filename);
#else
    (void) sprintf (commandbuffer, "strip %s.exe", ex_filename);
#endif
    if (verbose_flag)
      fprintf (stderr, "%s\n", commandbuffer);
    if (system (commandbuffer) > 0)
    {
      fprintf (stderr, "Glammar fatal msg: %s: strip failed\n", ex_filename);
      exit (1);
    }
  }
#endif

#ifdef RMOBJECT
  (void) sprintf (commandbuffer, "rm -f %s %s_.o", outputfile, ex_filename);
#else
  (void) sprintf (commandbuffer, "rm -f %s ", outputfile);
#endif

  if (verbose_flag)
    fprintf (stderr, "%s\n", commandbuffer);
  if (!syntax_flag)
    exit (system (commandbuffer));
}

char *getglammarenv (envvar)
char *envvar;
{
  char *getenv ();
  char *env = getenv (envvar);
  return ((env == (char *) NULL || *env == '\0') ? (char *) NULL : env);
}

char *glm_libdir ()
{
  char *fixlibdir = LIBDIR;
  char *envlibdir = getglammarenv (ENV_LIBDIR);
  return (envlibdir ? envlibdir : fixlibdir);
}

char *glm_incdir ()
{
  char *fixincdir = INCLUDEDIR;
  char *envincdir = getglammarenv (ENV_INCDIR);
  return (envincdir ? envincdir : fixincdir);
}

void glm_options ()
{
  while ((thischar == '\t' || thischar == ' ' || thischar == '\n') && (!feof (input)))
  {
    if (thischar == '\n')
      line += 1;
    thischar = getc (input);
  }

  if (thischar == '#')
  {
    thischar = getc (input);
    if (thischar != '-')
    {
      if (thischar != '\n')
        thischar = '#';
      skiplayout ();
      return;
    }

    read_glm_option ();
    thischar = '\n';
    glm_options ();
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

    getfirstchar ();
  }
}

read_glm_option ()
{

  char arg[256], skip[256];

  char *args = arg;
  fscanf (input, "%s", args);
  fgets (skip, 256, input);
  switch (*args)
  {
  case '-':
  case ' ':
  case '\t':
    break;
  case 'L':
    init_sum += (int) 'L';
    lat_trad_flag = true;
    break;
  case 'l':
    sscanf (args + 1, "%ld", &runtime_stksize);
    runtime_stksize = 100000l * runtime_stksize;
    break;
  case 'A':
    afx_opt_flag = true;
    break;
  case 'a':
    init_sum += (int) 'a';
    ambiguous_flag = true;
    break;
  case 'C':
  case 'c':
    hack_flag = true;
    (void) strcpy (hack_dir, args + 1);
    break;
  case 'E':
    init_sum += (int) 'E';
    errormsg_flag = true;
    det_flag = true;
    break;
  case 'e':
    eag_flag = true;
    break;
  case 'F':
  case 'f':
    lift_flag = false;
    break;
  case 'd':
    det_flag = true;
    break;

  case 'G':
  case 'g':
    gnu_flag = true;
    break;

  case 'H':
    hide_flag = true;
    break;
  case 'h':
  case '?':
    print_usage ();
    exit (0);
  case 'I':
    fullstat_flag = true;
    init_sum += (int) 'I';
  case 'i':
    stat_flag = true;
    init_sum += (int) 'i';
    break;
  case 'K':
  case 'k':
    keep_flag = true;
    break;
  case 'P':
    all_external_flag = true;
  case 'p':
    separate_comp_flag = true;
    input_from_partlist = true;
    break;
  case 'n':
    start_predicate_flag = true;
    break;
  case 'X':
    syntax_flag = true;
    index_flag = true;
    break;
  case 'x':
    tagindex_flag = true;
    break;
  case 'R':
    no_recursion_elm_flag = false;
    break;
  case 'r':
    switch (args[1])
    {
    case '\0':
      tree_flag = true;
      break;
    case 'i':
      id_tree_flag = true;
      break;
    case 'p':
      parse_tree_flag = true;
      break;
    case 'c':
      cfg_flag = true;
      break;
    default:
      ;
    }
    tree_flag = true;
    break;
  case 'O':
    (void) sprintf (tempbuf, " -%c %s ", args[1], args + 2);
    (void) strcat (c_flags, tempbuf);
    break;
  case 'o':
    (void) sprintf (tempbuf, " -%s ", args + 1);
    (void) strcat (c_flags, tempbuf);
    break;
  case 'S':
    no_strip_flag = 1;
    break;
  case 's':
    syntax_flag = true;
    break;
  case 'T':
    fulltrace_flag = true;
    init_sum += (int) 't';
  case 't':
    trace_flag = true;
    init_sum += (int) 't';
    break;
  case 'V':
    verbose_flag = true;
    break;
  case 'v':
    fprintf (stderr, "Glammar (c) %s\n", VERSION);
  case 'U':
    meta_uniq_flag = true;
    break;
  case 'M':
    AM_flag = false;            /* No advance memoizing */
  case 'm':
    memo_flag = true;
    sscanf (args + 1, "%d", &runtime_input_size);
    if (runtime_input_size < 1)
      runtime_input_size = 2;
    runtime_input_size <<= 10;
    break;
  default:
    break;
  }
}


code_includes ()
{
  if (separate_comp_flag && !MARKED (root, docompile))
    fprintf (output, "#include \"%s%sglammar3.h\"\n", glm_incdir (), dirsep);
  else
    fprintf (output, "#include \"%s%sglammar2.h\"\n", glm_incdir (), dirsep);
}

/* file : separate compilation
           generate code and compile for each module  with
                      different sum then in the one in the partlist
*/


compile_parts ()
{
  int cc = root, rule = root, up_to_date = true;
  for (cc = root; cc > laststdpred; cc = newcc (cc))
  {
    sum = 0;
    for (rule = root; rule != laststdpred; rule = BROTHER (rule))
      if (PART (cc) == PART (rule))
      {
        SET (rule, docompile);
        sum += SUM (rule);
      }
      else
        UNSET (rule, docompile);
    NEWSUM (cc) = sum;
    if (sum != OLDSUM (cc) || no_object (PART (cc)))
    {
      char *y = (char *) PART (cc), *x = outputfile, *x0;

      if (hide_flag)
      {
        (void) strcpy (outputfile, ".glammar/");
        x += 9;
      }
      for (x0 = x; *y != '\0'; y++)
        if (*y == DIRSEP)
          x = x0;
        else
          *x++ = *y;

      *x++ = '.';
      *x++ = 'c';
      *x = '\0';
      code ();
      up_to_date = false;
      c_compiler ();
      *commandbuffer = '\0';
    }
  }
  if (up_to_date && have_prog ())
  {
    fprintf (stderr, "glammar: `%s' is up to date.\n", ex_filename);
    exit (0);
  }
  write_part_file ();
  link_flag = true;
}

extern char *part_file;
c_compiler ()
{

  /* do C-assembler pass */
  if (output == stdout)
    return;
  if (verbose_flag)
    fprintf (stderr, "C-compiler phase\n");
#ifdef CC
  (void) strcat (commandbuffer, CC);
#else
  (void) strcat (commandbuffer, "cc ");
#endif
#ifdef CCFLAGS
  (void) strcat (commandbuffer, CCFLAGS);
#endif
  if (separate_comp_flag && !link_flag)
  {
    (void) strcat (commandbuffer, "-c ");
    if (hide_flag)
    {
      char ofile[256], *p;
      (void) strcpy (ofile, outputfile);
      for (p = ofile; *p != '\0'; p++);
      *(p - 1) = 'o';
      *p++ = ' ';
      *p = '\0';
#ifndef IBM6000
      (void) strcat (commandbuffer, "-o ");
      (void) strcat (commandbuffer, ofile);
#endif
    }

  }
  else
  {
    (void) strcat (commandbuffer, "-o ");
    (void) strcat (commandbuffer, ex_filename);
  }
  (void) strcat (commandbuffer, c_flags);
  if (!link_flag)
  {
    (void) strcat (commandbuffer, outputfile);
    (void) strcat (commandbuffer, " ");
  }
  else
    (void) strcat (commandbuffer, parts);

  if (verbose_flag)
    fprintf (stderr, "%s\n", commandbuffer);
  if (system (commandbuffer) > 0)
  {
    fprintf (stderr, "Glammar fatal msg: (C) assembler phase failed.\n");
    exit (1);
  }
#ifndef VMS
  if (!syntax_flag)
    (void) sprintf (commandbuffer, "/bin/rm -f %s", outputfile);
#else
  {
    char ofile[256], *p;
    (void) strcpy (ofile, outputfile);
    for (p = ofile; *p != '\0'; p++);
    *(p - 1) = 'o';
    *p++ = ' ';
    (void) sprintf (commandbuffer, "link /library=glammar:glam /executable=%s %s", ex_filename, ofile);
  }
#endif
  system (commandbuffer);

  *commandbuffer = '\0';
#ifdef IBM6000
  if (separate_comp_flag && !link_flag && hide_flag)
  {
    char ofile[256], *p;
    (void) strcpy (ofile, outputfile);
    for (p = ofile; *p != '\0'; p++);
    *(p - 1) = 'o';
    *p++ = ' ';
    *p = '\0';
    (void) sprintf (commandbuffer, "/bin/mv  %s %s", ofile + 9, ofile);
    system (commandbuffer);
    *commandbuffer = '\0';
  }
#endif /* IBM6000 */
}

int have_prog ()
{
  FILE *f;
  f = fopen (ex_filename, "r");
  if (f == NULL)
    return false;
  fclose (f);
  return true;
}

no_object (repr)
char *repr;
{
  char obj[255];
  FILE *f;
  if (hide_flag)
    sprintf (obj, ".glammar/%s.o", repr);
  else
    sprintf (obj, "%s.o", repr);
  f = fopen (obj, "r");
  if (f == NULL)
    return true;
  fclose (f);
  return false;
}

int newcc (cc)
int cc;
{
  int rule;
  char *part_cc = PART (cc);
  for (rule = cc; rule != init_one_star; rule = BROTHER (rule))
    if (PART (rule) != part_cc)
      return rule;
  return 0;
}

write_part_file ()
{
  int rule, prev = root;
  fclose (partlist);
  partlist = fopen (part_file, "w");
  if (!partlist)
    return;
  fprintf (partlist, "%s %ld\n", PART (root), NEWSUM (root));
  for (rule = root; rule != init_one_star; prev = rule, rule = BROTHER (rule))
    if (PART (rule) != PART (prev))
      fprintf (partlist, "%s %ld\n", PART (rule), NEWSUM (rule));
  fclose (partlist);
}

get_stddefs ()
{
  (void) sprintf (stddefs_g, "%s%sglammar.g", glm_libdir (), dirsep);
}

print_usage ()
{
  fprintf (stderr, "Usage: glammar [ options | -h  ]  script | - \n");
  fprintf (stderr, "       (compiles file script.g)\n");
  fprintf (stderr, "Most used options:\n\
		'E': generate a program that will report error msg about which\n\
                     rule failed  last.\n\
		'd': generate a deterministic program; In particular\n\
                     don't backtrack \"add to\"\n\
		'i': generate a program that will report statistcs for \n\
                     failed depths.\n\
		'p': separate compilation flag: The input file is \n\
                     taken the be a part list of .g files that are \n\
                     automatically recompiled if out of date\n\
		'n': Start rule is a predicate such that the program \n\
                     without input file given will skip reading from stdin \n\
                     (which is otherwise the default)\n\
                        break;\n\
		't': generate a program that will print the name of each rule\n\
                     it enters (==>) and exits \n\
                     (indicating failure -, or success +)\n\
		'm': Generate a program that will use a dynamic programming \n\
                     technique called memorizing.\n");
}
