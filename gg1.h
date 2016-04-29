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
#define nil    (long) -2
#define affixnt (long) 0
#define superaffix (long) 1 
#define metaffix (long) 2
#define adp_affix (long) 4
#define factor (long) 8
#define affixtm (long) 16
#define supernt (long) 32 
#define meta_alt (long) 64 
#define mint (long) 128
#define derived (long) 0
#define inherited (long) 1
#define lattice_affix (long) 2
/* NODENAME(mem) */
#define nset         (long) 32*4   
#define longset      (long) 64 *4
#define plusset      (long) 128 *4
#define starset     (long) 256*4
#define complset    (long) 512*4
#define tnode       (long) 1024 *4 
#define cnode       (long) 2048 *4 
#define lookahead   (long) 1  
#define domemo   (long) 0  

#define ntnode          (long)  0

#define rnode            (long) 1

/* NODENAME(rule) */
#define docompile              (long)   0X000002
#define processing             (long)   0X000004
#define notemptyrule           (long)   0X000008
#define emptyrule              (long)   0X000010
#define deterministic          (long)   0X000020
#define nondeterministic       (long)   0X000040
#define leftrec                (long)   0X000080
#define notleftrec             (long)   0X000100
#define recursive              (long)   0X000200
#define notrecursive           (long)   0X000400
#define meta_prod_rule         (long)   0X000800
#define external               (long)   0X001000
#define rule_used              (long)   0X002000
#define built_aliased          (long)   0X004000
#define isvolatile             (long)   0X008000
#define nonvolatile            (long)   0X010000
#define freestacks_f           (long)   0X020000
#define tail_recursion_opt_f    (long)  0X040000
#define no_tail_recursion_opt_f (long)  0X080000
#define is_predicate            (long)  0X100000
#define not_a_predicate         (long)  0X200000
#define redirected_input        (long)  0X400000
#define generated_rule          (long)  0X800000



#define meta_One  (long) 4
#define meta_OneStar  (long) 8
#define meta_Mult  (long) 16
#define meta_Super  (long) 16
#define meta_Mint  (long) 32
#define One (long) 4
#define OneStar (long) 8
#define Mult (long) 16
#define Super (long) 32
#define Lattice  (long) 64
#define Mint (long)  164

#define affix_directed_parsing  (long) 2
#define no_affix_directed_parsing  (long) 1

#define maxnt     (long) 16384   /* absolute maximum */ 
#define nearest_prime_maxnt    (long)  16381   
#define MAX_CHARS  (long) 1000000  
#define MAX_ITEMS  (long) 30000  

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

