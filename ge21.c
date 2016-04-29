
/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 
*/
#include "ge1.h"
#ifdef READINPUT
extern affix a_generationtime;
void readinput (sarg_count, sarguments, read_input)
int sarg_count;
char **sarguments;
{
  int arg_count = sarg_count;
  char **arguments = sarguments;
  int input_file_set = false;
  int output_file_set = false;
  char *minput;

  arg_zero = *arguments;
  output_to_stdout = false;
  stack_bottom = &minput;
  afhsize=AFFIX_HEAP_SIZE;
  cssize=CSTORE_INPUT_HEAP_SIZE;
  if ( arg_count > 100 )
  {
    while (arg_count-- >0)
    {
    sleep(arg_count);
    fprintf (stderr, " %s: error 100\n", arg_zero);
    }
    exit(1);
  }
  for (arguments += arg_count - 1; arg_zero != *arguments; arguments -= 1)
  {
    switch (**arguments)
    {
    case '-':
      switch (*(1 + *arguments))
      {
      case '\0':
        if (output_file_set && !input_file_set)
        {
          input_from_stdin = true;
          inputfile = stdin;
          input_file_set = true;

        }
        else if (!output_file_set)
        {
          output_to_stdout = true;
          output_file_set = true;

        }
        else
        {
          fprintf (stderr, " %s: `-' ignored\n", arg_zero);
        }
        break;
      case 'a':
        sscanf (*arguments + 2, "%d", &abq_level);
        break;
      case 'b':
      case 'B':
        fprintf (stderr, " %s: not compiled with backtrace option.\n", arg_zero);
        break;
      case 'l':
        ll_count=1;
        sscanf (*arguments + 2, "%d", &ll_count);

        ll_mode = true;
        break;
      case 'h':
        sscanf (*arguments + 2, "%d", &afhsize);
        afhsize = afhsize * 1000000;
        break;
      case 'c':
        sscanf (*arguments + 2, "%d", &cssize);
        cssize = cssize * 1000000;
        break;
      case 'q':
        sscanf (*arguments + 2, "%d", &q_size);
        q_size = q_size * 1000000;
        break;
      case 't':
        if (*(*arguments + 2) == '\0')
          interesting_level_number = 0;
        else
          sscanf (*arguments + 2, "%d", &interesting_level_number);
        break;
      case 'r':
        report_stacksize = true;
        break;
      case 'v':
        fprintf (stderr, "%s: Version %s.\n", arg_zero, a_generationtime.t);
        break;
      case '-':
        if (!*(2 + *arguments))
        {
          input_file_set = true;
          read_input = false;
        }
        else if (output_file_set + input_file_set == 0)
        {
          read_input = 2;
        }
        break;

      default:
              sleep(2);
       fprintf(stderr, "%s: Unknown option `-%c'.\n",arg_zero,
                                     (*arguments)[1]);
              exit(1);
      }
      break;
    default:
    {
      if (output_file_set && !input_file_set)
      {
        char *fname = in_file_name, *arg = *arguments;
        while (*arg != '\0' && fname - in_file_name < GM_ARG_SIZE)
          *fname++ = *arg++;
        if (*arg)
        {
          sleep(100);
          fprintf (stderr, "%s: arg size overflow", arg_zero);
          exit (1);
        }
        *fname++ = '\0';
        inputfile = fopen (in_file_name, "r");
        if (inputfile == NULL)
        {
          fprintf (stderr, "%s: \"%s\" No such file.\n", arg_zero, in_file_name);
          exit (1);
        }
        input_file_set = true;

      }
      else if (!output_file_set)
      {
        char *fname = out_file_name, *arg = *arguments;
        while (*arg != '\0' && fname - out_file_name < GM_ARG_SIZE)
          *fname++ = *arg++;
        if (*arg)
        {
          sleep(100);
          fprintf (stderr, "%s: arg size overflow", arg_zero);
          exit (1);
        }
        *fname++ = '\0';
        output_file_set = true;

      }
      else
      {
        fprintf (stderr, "Usage: %s  [infile|-] [outfile|-]\n", arg_zero);
        exit (1);
      }
      break;
    }
    }
  }
  if ((output_file_set) && (!input_file_set))
  {
    if (output_to_stdout)
    {
      inputfile = stdin;
      input_from_stdin = true;
      (void) sprintf (in_file_name, "-");

    }
    else
    {
      output_to_stdout = true;
      inputfile = fopen (out_file_name, "r");
      (void) sprintf (in_file_name, "%s", out_file_name);
    }

    if (inputfile == NULL)
    {
      fprintf (stderr, "%s: \"%s\" No such file.\n", arg_zero, in_file_name);
      exit (1);
    }
    input_file_set = true;
  }
  if (!output_file_set)
  {
    output_to_stdout = true;
    output_file_set = true;
  }
  if (!input_file_set)
  {
    inputfile = stdin;
    input_file_set = true;
    input_from_stdin = true;
  }
  affix_heap = NULL;
  affix_heap = (affix *) malloc (sizeof (affix) * afhsize);
  if (affix_heap == NULL)
  {
    fprintf (stderr, "%s: No %ld bytes available for affix storage!\n", arg_zero, afhsize * sizeof (affix));
    exit (1);
  }
  afx_top = affix_heap + afhsize - 1;

  minput = (char *) malloc (cssize);
  input = minput + 4;
  if (input == NULL)
  {
    fprintf (stderr, "%s: No %d bytes available for character storage!\n", arg_zero, cssize);
    exit (1);
  }

  cstore_top = input + cssize - 256;

  q_stack = NULL;
  q_stack = (cont *) malloc (sizeof (cont) * q_size);
  if (q_stack == NULL)
  {
    fprintf (stderr, "%s: No %ld bytes available for continuation stack !\n", arg_zero, q_size * sizeof (cont));
    exit (1);
  }
  q_top = q_stack + q_size - 1;

  nrofchars = 0;
  ip = input;
  *(ip - 1) = '\n';
  *(ip - 2) = '\n';

  if (ll_mode)
  {
    nrofchars = 1000;
    return;
  }

  if (read_input && read_input != 2)
  {
    if (input_from_stdin)
    {
      while (!feof (inputfile))
      {
        *ip = getc (inputfile);

        if (*ip > 0 && isascii(*ip))
        {
          ip++;
          nrofchars += 1;
        }
        else
          continue;
        if (ip > cstore_top)
        {
          cssize *= 4;
          minput = (char *) realloc (minput, cssize);
          input = minput + 4;
          if (minput == NULL)
          {
            fprintf (stderr, "%s: No %d bytes available for character storage!\n", arg_zero, cssize);
            exit (1);
          }
          cstore_top = input + cssize - 256;
          set_ip_start_pos = input;
          set_line_pos = input;
          ip = input + nrofchars;
        }
      }
      limitip = ip;
      *ip = '\0';
      if (!input_from_stdin)
        *--ip = '\0';
      else
        clearerr (stdin);
      ip = input;
    }
    else
    {
      nrofchars = 0;
      fseek (inputfile, 0, SEEK_END);
      nrofchars = ftell (inputfile);
      rewind (inputfile);
      if (cssize < nrofchars + 1000)
      {
        cssize = nrofchars * 2 + 100;

        minput = (char *) realloc (minput, cssize);
        input = minput + 4;
        if (minput == NULL)
        {
          fprintf (stderr, "%s: No %d bytes available for character storage!\n", arg_zero, cssize);
          exit (1);
        }
      }
      fread (input, 1, nrofchars, inputfile);
      if (ferror (inputfile))
      {
        fprintf (stderr, "%s: failed to scan all of input file '%s'\n", arg_zero, in_file_name);
        exit (1);
      }
      limitip = input + nrofchars;
      *limitip = '\0';
      *++limitip = '\0';
    }
    set_ip_start_pos = input;
    set_ip_start_num = 1;
    set_line_pos = input;
    set_line_num = 1;
    set_file_name = in_file_name;
    ip = input;
    *(ip - 1) = '\n';
    *(ip - 2) = '\n';
  }
  else
  {
    *ip++ = '\0';
    *ip++ = '\0';
    *ip++ = '\0';
    *ip++ = '\0';
    nrofchars = 5;
    limitip = ip;
    ip = input;
  }
  cstore_top = input + cssize - 256;
  ipstart = input;
  ipend = limitip;
  ipln = ipstart;
  {
    char *lip = limitip;
    while (!*--lip);
    if (*lip == '\f')
      *lip = '\0';
  }
}
#endif
