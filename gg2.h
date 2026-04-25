/*

    This file is a part of the GLAMMAR source distribution 
    and therefore subjected to the copy notice below. 
    
    Copyright (C) 1989,2012  Eric Voss, eric337@yahoo.com 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation version 1

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* file gg1.h : external definitions */

extern AST     *ast;
extern long      astindex,
                brother,
                root,
                startnode,
                lastnode,
                laststdpred,
                lastmetarule,
                laststdmetarule,
                first_lattice;
extern char     thispart[256],
                outputfile[256],
                ex_filename[256],
                inputfilename[256],
                tagindexfilename[256],
                stddefs_g[256],
                sep_comp_part[256],
                thischar,
               *string,
               *ntname,
              **nametable,
               *chartable,
                hack_dir[256];
extern long      charindex,
                hashindex,
                prevcharindex,
                usefullerrmsg,
                underscore_allowed,
                syntaxerrors,
                line;
extern long      NtCount,
                RuleCount,
                nrterms,
                nrntmems,
                lastaffixtree,
                defaffixtree;
extern char    *rulename,
               *startname,
               *redirect,
               *dont_care;
extern FILE    *output,
               *input,
               *partlist,
               *indexfile,
               *tagindexfile;
extern long      ulhs,
                reccount,
                lastsettype,
                PartNr,
                RuleNr,
                qCount,
                aCount;
extern long      AM_flag,
                memo_flag,
                index_flag,
                tagindex_flag,
                hide_flag,
                input_from_partlist,
                input_from_stdin;
extern long     runtime_stksize,
                runtime_input_size;
extern long      det_flag,
                lift_flag,
                ambiguous_flag,
                noerrmsg_flag,
                no_recursion_elm_flag,
                tree_flag,
                hack_flag;
extern long      verbose_flag,
                backtrace_mark,
                affix_trace,
                name_clashes;
extern long      nestarset,
                metaterminal,
                metaterminal2;
extern long      memopt_flag,
                eag_flag,
                syntax_flag,
                errormsg_flag,
                cfg_flag;
extern long      stat_flag,
                fullstat_flag,
                trace_flag,
                fulltrace_flag;
extern long     max_item,
                maxchars;
extern long
                tltraditionalterm,
                transformlatticeterm,
                tltraditional,
                transformlattice,
                getip,
                restoreip,
                falseip,
                shadow,
                unshadow,
                firstshadow,
                nextshadow,
                intersect,
                explintersect,
                nlcr,
                init_one_star,
                skip,
                cut,
                equal,
                pair,
                unpair,
                repair,
                add_to,
                add_to_used,
                assign,
                notequal,
                where,
                initmeta,
                getlist_,
                getlastlist_,
                initmint,
                evalmeta;
extern long      separate_comp_flag,
                link_flag,
                meta_uniq_flag,
                lat_trad_flag;
extern char     parts[],
               *partptr;
extern long      max_group,
                nr_names,
                init_sum,
                nr_of_memo_alts,
                symbol_table_size,
                sum;
extern long      parse_tree_flag,
                id_tree_flag,
                all_external_flag,
                start_predicate_flag,
                afx_opt_flag;
extern long      endofsentence,
                nestaralt,
                detnestarset,
                detprefix,
                detprefix2,
                equalsempty,
                meta_empty,
                setinputptrto,
                resetinputptr;
extern char     dirsep[];


extern char repr_group[MAX_LATTICE_GROUPS][MAX_LATTICE_GROUPS_DIGITS];
char *full_repr(char *s);
