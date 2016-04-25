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
/* file constdef.h : constant definitions and macros */

  
/*     !! IN CASE OF A SIMPLE MAKEFILE SPECIFY DIRECTORIES HERE !! */

#define true    1
#define false   0
#define nil    -2
#define affixnt 0
#define superaffix 1 
#define metaffix 2
#define adp_affix 4
#define factor 8
#define affixtm 16
#define supernt 32 
#define meta_alt 64 
#define mint 128
#define derived 0
#define inherited 1
#define lattice_affix 2

/* NODENAME(mem) */
#define nset         32*4   
#define longset      64 *4
#define plusset      128 *4
#define starset     256*4
#define complset    512*4
#define tnode       1024 *4 
#define cnode       2048 *4 
#define lookahead   1  
#define domemo   0  

#define ntnode           0

#define rnode            1

/* NODENAME(rule) */
#define docompile                0X000002
#define processing               0X000004
#define notemptyrule             0X000008
#define emptyrule                0X000010
#define deterministic            0X000020
#define nondeterministic         0X000040
#define leftrec                  0X000080
#define notleftrec               0X000100
#define recursive                0X000200
#define notrecursive             0X000400
#define meta_prod_rule           0X000800
#define external                 0X001000
#define rule_used                0X002000
#define built_aliased            0X004000
#define isvolatile               0X008000
#define nonvolatile              0X010000
#define freestacks_f             0X020000
#define tail_recursion_opt_f     0X040000
#define no_tail_recursion_opt_f  0X080000
#define is_predicate             0X100000
#define not_a_predicate          0X200000
#define redirected_input         0X400000
#define generated_rule           0X800000



#define meta_One  4
#define meta_OneStar  8
#define meta_Mult  16
#define meta_Super  16
#define meta_Mint  32
#define One 4
#define OneStar 8
#define Mult 16
#define Super 32
#define Lattice  64
#define Mint  164

#define affix_directed_parsing  2
#define no_affix_directed_parsing  1

#define maxnt     16384   /* absolute maximum */ 
#define nearest_prime_maxnt     16381   
#define MAX_CHARS  1000000  
#define MAX_ITEMS  30000  

/* macros */

#define NODENAME(node)      ast[node].n
#define BROTHER(node)       ast[node].b
#define SON(node)           ast[node].s
#define REPR(node)          ast[node].r
#define FREPR(node)         full_repr(REPR(node))
#define DEF(node)           ast[node].d
#define LEFTDEF(node)       ast[node].d
#define LINE(node)           ast[node].l
#define FLAGS(node)          ast[node].x

#define MARKED(rule,value)   ((NODENAME (rule) & (value) ) != 0)
#define SET(rule,value)       (NODENAME(rule) ) |= value
#define UNSET(rule,value)     (NODENAME(rule) ) &=  ~ (value )

#define FLAG_MARKED(node,value)   ((FLAGS (node) & (value) ) != 0)
#define FLAG_SET(node,value)       (FLAGS(node) ) |= value
#define FLAG_UNSET(node,value)     (FLAGS(node) ) &=  ~ (value )

#define MARKED_ADP(rule,value)   ((DEF(rule) & (value) ) != 0)
#define SET_ADP(rule,value)       (DEF(rule) ) |= value
#define UNSET_ADP(rule,value)     (DEF(rule) ) &=  ~ (value )

#define LATTICE_DEF(node)       ast[node].s
#define INHERITED(node)      (ast[node].n == inherited)
#define DERIVED(node)      (ast[node].n == derived)
#define LATTICE(node)      (ast[node].n == lattice_affix)
#define MEMOIZE(node)     (ast[node].d >= domemo )
#define LKH(node)  MARKED(node,lookahead)
#define NONTERMINAL(member) ((NODENAME (member) ) < 128)
#define TERMINAL(member)    ((NODENAME (member)) >=  128)
#define STRING(member)    ((NODENAME (member) & tnode) == tnode )
#define COMPLEMENT(member)    ((NODENAME (member) & complset) == complset )
#define EXCLAMATIONSTARCHOICE(member)  ((NODENAME(member) & starset) == starset)
#define EXCLAMATIONPLUSCHOICE(member)  ((NODENAME(member) & plusset) == plusset)


#define AFFIXTREE(node)       SON(node)
#define AFFIXDEF(node)       NODENAME(node)
#define OR +
#define LASTALT(alt)         (BROTHER(alt) == nil)

#define IS_LEFTDEF(node)       (ast[node].d != -1)
#define NOT_IS_LEFTDEF(node)   (ast[node].d == -1)

#define META(node)    (ast[node].n == metaffix )
#define FREE_AFFIX(node)    (ast[node].n >= metaffix )
#define APPLY_BOUND_AFFIX(node)    (ast[node].n <  metaffix)
#define DONTCARE(node)  (ast[node].d = 9999999) 
#define IS_DONTCARE(node)  (ast[node].d == 9999999)
#define ISNT_DONTCARE(node)  (ast[node].d != 9999999)

#define PART(node)           ast[node].p
#define SUM(node)           ast[node+1].n
#define OLDSUM(node)           ast[node+1].b
#define NEWSUM(node)           ast[node+1].s
#define COUNT(node)           ast[node+1].d

#define SUM_ALT  sum +=   3000001
#define SUM_AFX  sum +=    100022    
#define SUM_MEM  sum +=     91333 
#define SUM_TERM sum +=     15077
#define SUM_DC   sum +=      1125
#define SUM_NOLD sum +=       156
#define SUM_LD   sum +=        19


#define _GNU_SOURCE

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 typedef
struct {
   long n, b, s, d, l, x;
   char *r,*p;}
AST;

/* environment variables */
#define ENV_LIBDIR "LIBGLAMMAR"
#define ENV_INCDIR "INCGLAMMAR"
 
/* Version */
#ifndef VERSION
#define VERSION "1.19 Jan 22 2001"
#endif

#ifndef LIBDIR
#define NOMP
#define NOSTRIP
#define YESLINK
#define DIRSEP '/'
#define LIBDIR "glammar"
#define INCLUDEDIR "glammar"
#else 
#define DIRSEP '/'
#endif

#ifndef CC
#define CC "CC"
#define CCFLAGS " /NOWARN " 
#endif

