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
#define is_predicate            (long)  0X020000
#define not_a_predicate         (long)  0X040000
#define generated_rule          (long)  0X100000

/* FLAGS ast.x */
#define metatermopt             (long)  0X01
#define freestacks_f           (long)   0X02
#define tail_recursion_opt_f    (long)  0X04
#define no_tail_recursion_opt_f (long)  0X08
#define redirected_input        (long)  0X10
#define wasmeta                 (long)  0X20
/* lattice affix of transform_lattice */
#define transform_lattice_arg   (long)  0X40
#define TRC_LATTICE          (long)  0X80
#define TRC_ALIST            (long)  0X100


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

#define maxnt     (long) 262144   /* absolute maximum */ 
#define nearest_prime_maxnt    (long)  262051   
#define MAX_CHARS  (long) 10000000  
#define MAX_ITEMS  (long) 300000  

/* These are fixed values, based on size of long in bits */
/* Do not change */
#define MAX_LATTICES_PER_GROUP  55
#define MAX_LATTICE_GROUPS  254  /* max val of unsigned char -1 */
#define MAX_LATTICE_GROUPS_DIGITS 4
#define LATTICE_GROUP(node) (ast[node].g & 0xff)
#define LATTICE_VAL(node) (ast[node].g >> (sizeof(long)*7))
/* macros */

#define NODENAME(node)      ast[node].n
#define BROTHER(node)       ast[node].b
#define SON(node)           ast[node].s
#define REPR(node)          ast[node].r
#define FREPR(node)         full_repr(REPR(node))
#define DEF(node)           ast[node].d
#define LEFTDEF(node)       ast[node].d
#define LINE(node)           ast[node].l
#define GROUP(node)           ast[node].g
#define LINEX(n,m)           LINE(n)?LINE(n):LINE(m)
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
#define PARTX(n,m)           PART(n)?PART(n):PART(m)
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
   long n, b, s, d, l, x, g;
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

long addname ();
void choice_switch_statement_coder ();
long close_symbol ();
long consistentaffixtype (long def, long app);
long conv_table ();
long getnextchar ();
long lattice_top (long afx);
long lift_element (long mem);
long lookahead_in_alt (long alt);
long mystrcmp ( char *s1, char *s2);
long name ();
long name_display_mix ();
long open_symbol ();
long set_meta_affixes ();
long set_volatile ();
long skiplayout ();
void skiptopoint_symbol ();
long valid_overloaded_affixtype (long def, long app, long parent_app);
void adp_walk ();
void affixerrmsg (long original, long bad_copy);
void affixes ();
void affix_intermediate_defs (long mem);
void affixuse ();
void alloc_chartable ();
void altdetnestarset (long alt);
void better_index ();
void check_metagrammar ();
void code ();
void code_includes ();
void compile ();
void compile_parts ();
void compute_predicates ();
void determ ();
void det_rule_alts (long alt);
void det_rule_head ();
void det_rule_tail ();
void dump_hash ();
void eag ();
void empty ();
void errmsg (char *msg);
void final_nestarset_removal ();
void get_affixes (long affix, long count);
void getfirstchar ();
void get_stddefs ();
void glm_options ();
void hint_on_non_used_hyperrules (long local);
void id_tree ();
void ie ();
void init_builtins ();
void initnametable ();
void lattice_defined (long rule, long alt, long trm);
void lattice_used (long term, long alt);
void left_rec ();
void link_lattice ();
void linktodefs ();
void list ();
void make_member (char *name);
void memopt ();
void mems_intermediate_defs (long mem);
void meta2init ();
void move_terms_up ();
void newdefnode (long name, long brothers, long sons, long def, char *repr);
void newnode (long name, long brothers, long sons, char *repr);
void newrulenode (long name, long brothers, long sons, long line, char *part, char *repr);
void nondet_code ();
void parse_tree ();
void print_cfg_tree ();
void printtree ();
void push_affixes (long affix, long count);
void recursive_ ();
void result_intermediate_defs (long alt);
void rules (long ruletype);
void statistic_table ();
void tag_index ();
void tail_rec_opt ();
void trace ();
void trace_code (long built_als,long affix);
void tr_lattice ();
void wheres ();
void optimize_detnestarset (long alt);
int same_lattice_not_already_encountered(long alt, long mem, long affix);
