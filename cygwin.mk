#
# Makefile to build the glammar version of the EAG compiler
# 

GLAMMAR = glammar
PROGRAM = $(GLAMMAR)
VERSION=`cat .version`
#Generator sources
GG=     gg01.c gg02.c gg03.c gg04.c gg05.c gg06.c gg07.c gg08.c\
        gg09.c gg10.c gg11.c gg12.c gg13.c

#Runtime Environment sources
EV=     ge01.c ge02.c ge03.c ge04.c ge05.c ge06.c ge07.c ge08.c\
        ge09.c ge10.c ge11.c ge12.c ge13.c ge14.c ge15.c ge16.c\
        ge20.c ge21.c ge22.c ge23.c ge30.c 

IH1 = $(GLAMMAR)2.h 
IH2 = $(GLAMMAR)3.h
GGH = gg1.h gg2.h 
EVH = ge1.h $(IH1) $(IH2)

OBJS = $(GG:.c=.o)
MODULE=$(GLAMMAR).a
SKELFILES = make.local
MAKEFILE= Makefile
DISTFILES= $(GG) $(EV) ge40.c $(EVH) $(GGH) glammar-pp.g goodout\
 $(MAKEFILE) .version doc examples $(GLAMMAR).g $(GLAMMAR).1 \
 $(GLAMMAR).sty COPYING VERSION INSTALL glammar.vim cygwin.mk
JUNK=glammar-pp  builtin-rules.tex
# Installation dependent make variables.

# This line is used for automatic installation
xxx: all
	@echo
#++ make module local start, do NOT touch this line. ++
# full path of the environment
INSTALLDIR = /usr/local

#  BDIR     : bin directory
BDIR = $(INSTALLDIR)/bin

#  IDIR     : include directory
IDIR = $(INSTALLDIR)/include

#  LDIR     : library directory
LDIR = $(INSTALLDIR)/lib

# Where to put latex style file (/dev/null??)
TEXDIR = $(LDIR)/tex/macros

NFULLDS = $(INSTALLDIR)/ds/nfullgl.ds
NKERNDS = $(INSTALLDIR)/ds/nkerngl.ds

#  HOSTNAME : name of installation machine.
HOSTNAME=dlsun130

# SYS     : name of operating system or machine type
#     BSD	For generic BSD systems
#     AVIION	For generic DG_UX systems
#     HPUX	For all HP-UX systems
#     SUN	For all SUN systems
#     APOLLO	For all Apollo systems
SYS = CYGWIN

# CLIBLOC : required C libraries
CLIBLOC =

LINTLIBLOC =

PASLIBLOC =

# CBADADR : address that will cause a core-dump if read or written.
CBADADR = 0

SYSLINTFLAGS = -hpac
SYSCFLAGS = -Usun -Xa -xs -xstrconst -mr -xcg92 
SYSCPROFFLAGS =
SYSCDBUGFLAGS =
SYSPFLAGS =
#CC=/opt/SUNWspro/bin/cc -Usun -Xa -xs -xstrconst -mr -xcg92
#CC=/opt/SUNWspro/bin/cc -Usun -Xa -xs -xstrconst -mr -xcg92
CC=gcc 
# Collection of host dependent make rules.

### libinstall ###
# MODULE	- module to install
libinstall:
	cp $(MODULE) $(LDIR)/$(MODULE)

### includeinstall ###
# MODULE	- module to install
includeinstall:
	cp $(MODULE) $(IDIR)/$(MODULE)

### maninstall ###
# MAN		- manual page to install
maninstall:
	cp $(MAN) /usr/man/manl

### texinstall ###
# TEX		- latex sty files
texinstall:
	cp $(STY) $(TEXDIR)

### bininstall ###
# MODULE	- module to install
bininstall:
	cp $(MODULE) $(BDIR)/$(MODULE)
	true $(BDIR)/$(MODULE)

### arlibinstall ###
# MODULE	- module to install
arlibinstall:
	cp $(MODULE) $(LDIR)/$(MODULE)
#++ make module local end, do NOT touch this line. ++

# Additional system dependent compilation flags:
SYSPFLAGS =
SYSLDFLAGS =

CFLAGS = 
SYSCFLAGS = -DLIBDIR=\"$(LDIR)\" -DINCLUDEDIR=\"$(IDIR)\"\
             $(RMOBJECT) -DCC=\"'$(CC)'\" \
             -DCCFLAGS=\"'$(WARN) $(COPT) $(LONGJ)'\" $(NOSTRIP) $(NOMP) \
             $(ADDRESSING)

LDFLAGS = $(SYSLDFLAGS)

help :
	@echo " Possible make targets:"
	@echo "sun		Sun Os "
	@echo "sung	        Sun Os with gnu c-compiler"	
	@echo "next	        Next running Mach"
	@echo "aviion		Data Generals DG-UX" 
	@echo "orion 		Orion BSD"
	@echo "dec		Digitals Ultrix"
	@echo "dec_mips		Digitals Ultrix"
	@echo "dec_alpha	"OSF"
	@echo "hpux		HP"
	@echo "clean		Free disk space."
	@echo "distfiles	List distribution files."
	@echo "install		Install relevant files."
	@echo "setup		Adapt sources to local situation."
	@echo "test		Run tests."


# Add rules for the programs themselves here.

glammar-pp: glammar-pp.g
	$(GLAMMAR) -S -v -d glammar-pp

test : glammar-pp
	./glammar-pp glammar.g builtin-rules.tex
	cmp builtin-rules.tex  goodout


clean:
	rm -f $(GLAMMAR).a $(GLAMMAR)  $(OBJS)  $(JUNK)


distfiles:
	@echo $(DISTFILES) | tr ' ' '\012'

tar:
	tar -cf $(PROGRAM)$(VERSION).tar $(DISTFILES) 
gtar: 
	gtar -Icf $(PROGRAM)$(VERSION).tgb $(DISTFILES)

setup:
	Config/modmake local Config/make.local $(MAKEFILE)


# Rules to make $(GLAMMAR) #

COMPILE.c= $(CC) -c $(CFLAGS) -D$(SYS)
LINK.c= $(PURIFY) $(CC)

$(GLAMMAR): $(OBJS) 
	$(LINK.c) $(LDFLAGS) -o $@ $(OBJS) 

install: glammar  
	make 'MODULE=$(GLAMMAR).a' arlibinstall
	make 'MODULE=$(GLAMMAR).exe' bininstall
	make 'MODULE=$(IH1)' includeinstall
	make 'MODULE=$(IH2)' includeinstall
	make 'MODULE=$(GLAMMAR).g' libinstall
	@echo "enter 'make install-glammar-pp'"

install-glammar-pp: glammar-pp
	make 'MODULE=$(GLAMMAR)-pp.exe' bininstall
	make 'STY=glammar.sty' texinstall


all: $(SYS)

allsystems: $(GLAMMAR) $(GLAMMAR).a
	@echo "enter 'make install'"

#++++ Used for orion
first: $(GLAMMAR)

second:$(EV) $(MP)
	make -f Makefile.orion "CC=$(CC)" $(?:.c=)
#----

# Meaning of FLAGS:
#
#   
#      WARN
#		 -DWARN  :- pass -w flag to the c-compiler
#			    This is meant to catch warning of non used 
#			    meta-rules when such meta-rule is only used
#			    within other meta-rules.
#      NOMP 
#		 -DNOMP  :- This multiple precision integer arithmetic 
#                            package is not available for your system 
#                            Builtins using this package cannnot be called
#
#      NOSTRIP 
#		 -DNOSTRIP  :- do not strip executable using "strip"
#
#      RMOBJECT 
#		 -DRMOBJECT  :- remove object file when the c-compiler 
#    			        fails to perform this action 		
#
#      LONGJ 
#		 -DLONGJ  :- long jump flag (-J) for cc 
#



sun SUN:
	@echo SYS=$(SYS)
	make allsystems \
	MPE="ge40.c "\
	NOSTRIP=-DNOSTRIP \
	VERSION="`cat .version` `date`"
	


#Cygwin
CYGWIN:
	@echo SYS=$(SYS)
	make allsystems NOMP=-DNOMP\
		VERSION="`cat .version` `date`"


vers= `cat .version`

indx:
	ctags -v $(GHDRS) $(GSRCS)
tgrind:
	tgrind  $(GHDRS) $(GSRCS)
version: 
	@echo $(VERSION) 
newversion:
	@(cat .version; echo " 0.01 + p") | dc > version
	mv version .version

gg01.o: gg01.c .version
	$(COMPILE.c)  $(SYSCFLAGS)  -DVERSION=\"'$(VERSION)'\"  gg01.c
#	$(COMPILE.c)  $(SYSCFLAGS)    gg01.c


$(GLAMMAR).a: $(EV) $(MPE)
	make "CC=$(CC)" $(?:.c=)

#:.,$s/\(ge...\)c -o \(.*\)/\1c ; mv \1o \2/
ge01: ge01.c
	 $(COMPILE.c) -DUSKIP ge01.c ; mv ge01.o t2.o
	 $(COMPILE.c) -DLKH ge01.c ; mv ge01.o t3.o
	 $(COMPILE.c) -DUCUT ge01.c ; mv ge01.o t4.o
	 $(COMPILE.c) -DUIDENTIFIER ge01.c ; mv ge01.o t5.o
	 $(COMPILE.c) -DDIDENTIFIER ge01.c ; mv ge01.o t6.o
	 $(COMPILE.c) -DULETTERDIGITSEQUENCE ge01.c ; mv ge01.o t7.o
	 $(COMPILE.c) -DDLETTERDIGITSEQUENCE ge01.c ; mv ge01.o t8.o
	 $(COMPILE.c) -DUNLCR ge01.c ; mv ge01.o t9.o
	 $(COMPILE.c) -DDNLCR ge01.c ; mv ge01.o t10.o
	 $(COMPILE.c) -DUBSLASH ge01.c ; mv ge01.o t11.o
	 $(COMPILE.c) -DDBSLASH ge01.c ; mv ge01.o t12.o
	 $(COMPILE.c) -DUQUOTE ge01.c ; mv ge01.o t13.o
	 $(COMPILE.c) -DDQUOTE ge01.c ; mv ge01.o t14.o
	 $(COMPILE.c) -DUENDOFSENTENCE ge01.c ; mv ge01.o t15.o
	 $(COMPILE.c) -DDENDOFSENTENCE ge01.c ; mv ge01.o t16.o
	ar rv $(GLAMMAR).a  t16.o t15.o t14.o t13.o t12.o t11.o t10.o t9.o t8.o t7.o t6.o t5.o t4.o t3.o t2.o
	rm -f  t16.o t15.o t14.o t13.o t12.o t11.o t10.o t9.o t8.o t7.o t6.o t5.o t4.o t3.o t2.o
ge02: ge02.c
	 $(COMPILE.c) -DDDEQUAL ge02.c ; mv ge02.o t17.o
	 $(COMPILE.c) -DDEQUAL ge02.c ; mv ge02.o t18.o
	 $(COMPILE.c) -DDNOTEQUAL ge02.c ; mv ge02.o t19.o
	 $(COMPILE.c) -DDDFQUAL ge02.c ; mv ge02.o t20.o
	 $(COMPILE.c) -DDFQUAL ge02.c ; mv ge02.o t21.o
	 $(COMPILE.c) -DDFOTEQUAL ge02.c ; mv ge02.o t22.o
	 $(COMPILE.c) -DDCOMPARE ge02.c ; mv ge02.o t23.o
	 $(COMPILE.c) -DULINENUM ge02.c ; mv ge02.o t24.o
	 $(COMPILE.c) -DDLINENUM ge02.c ; mv ge02.o t25.o
	ar rv $(GLAMMAR).a  t25.o t24.o t23.o t22.o t21.o t20.o t19.o t18.o t17.o
	rm -f  t25.o t24.o t23.o t22.o t21.o t20.o t19.o t18.o t17.o
ge03: ge03.c
	 $(COMPILE.c) -DUSETLINEFILE ge03.c ; mv ge03.o t26.o
	 $(COMPILE.c) -DUGETLINEFILE ge03.c ; mv ge03.o t27.o
	 $(COMPILE.c) -DDSETLINEFILE ge03.c ; mv ge03.o t28.o
	 $(COMPILE.c) -DDGETLINEFILE ge03.c ; mv ge03.o t29.o
	 $(COMPILE.c) -DDCHARNUM ge03.c ; mv ge03.o t30.o
	 $(COMPILE.c) -DSETTABSTOP ge03.c ; mv ge03.o t31.o
	 $(COMPILE.c) -DDSOMENAME ge03.c ; mv ge03.o t32.o
	ar rv $(GLAMMAR).a  t32.o t31.o t30.o t29.o t28.o t27.o t26.o
	rm -f  t32.o t31.o t30.o t29.o t28.o t27.o t26.o
ge04: ge04.c
	 $(COMPILE.c) -DUADDTO ge04.c ; mv ge04.o t33.o
	 $(COMPILE.c) -DDADDTO ge04.c ; mv ge04.o t34.o
	 $(COMPILE.c) -DDVALUEOF ge04.c ; mv ge04.o t35.o
	 $(COMPILE.c) -DDINCLUDES ge04.c ; mv ge04.o t36.o
	 $(COMPILE.c) -DDEXCLUDES ge04.c ; mv ge04.o t37.o
	 $(COMPILE.c) -DDLOOKUP ge04.c ; mv ge04.o t38.o
	 $(COMPILE.c) -DDCONTENTSOF ge04.c ; mv ge04.o t39.o
	 $(COMPILE.c) -DDSIZE ge04.c ; mv ge04.o t40.o
	 $(COMPILE.c) -DUDELETE ge04.c ; mv ge04.o t41.o
	 $(COMPILE.c) -DUSELECT ge04.c ; mv ge04.o t42.o
	ar rv $(GLAMMAR).a  t42.o t41.o t40.o t39.o t38.o t37.o t36.o t35.o t34.o t33.o
	rm -f  t42.o t41.o t40.o t39.o t38.o t37.o t36.o t35.o t34.o t33.o
ge05: ge05.c
	 $(COMPILE.c) -DGETNEXTOPTION ge05.c ; mv ge05.o t43.o
	 $(COMPILE.c) -DUGETNEXTOPTION ge05.c ; mv ge05.o t44.o
	 $(COMPILE.c) -DLAYOUT ge05.c ; mv ge05.o t45.o
	 $(COMPILE.c) -DULAYOUT ge05.c ; mv ge05.o t46.o
	 $(COMPILE.c) -DWHITESPACE ge05.c ; mv ge05.o t45a.o
	 $(COMPILE.c) -DUWHITESPACE ge05.c ; mv ge05.o t46a.o
	 $(COMPILE.c) -DLENGTH ge05.c ; mv ge05.o t47.o
	 $(COMPILE.c) -DULENGTH ge05.c ; mv ge05.o t48.o
	ar rv $(GLAMMAR).a  t48.o t47.o t46.o t46a.o t45a.o t45.o t44.o t43.o
	rm -f  t48.o t47.o t46a.o t46.o t45.o t45a.o t44.o t43.o
ge06: ge06.c
	 $(COMPILE.c) -DDTYPEIN ge06.c ; mv ge06.o t49.o
	 $(COMPILE.c) -DDTYPEOUT ge06.c ; mv ge06.o t50.o
	 $(COMPILE.c) -DDREALTOINT ge06.c ; mv ge06.o t51.o
	 $(COMPILE.c) -DDDECIMALTOUN ge06.c ; mv ge06.o t52.o
	 $(COMPILE.c) -DDKEYWORD ge06.c ; mv ge06.o t53.o
	 $(COMPILE.c) -DDREVERSE ge06.c ; mv ge06.o t54a.o
	 $(COMPILE.c) -DDUPPERCASE ge06.c ; mv ge06.o t54.o
	 $(COMPILE.c) -DDLOWERCASE ge06.c ; mv ge06.o t55.o
	ar rv $(GLAMMAR).a  t55.o t54.o t53.o t52.o t51.o t50.o t49.o t54a.o
	rm -f  t55.o t54.o t53.o t52.o t51.o t50.o t49.o t54.a t54a.o
ge07: ge07.c
	 $(COMPILE.c) -O -D$(SYS) -DDCOMPUTE ge07.c ; mv ge07.o t56.o
	ar rv $(GLAMMAR).a  t56.o
	rm -f  t56.o
ge08: ge08.c
	 $(COMPILE.c) -DUSETINPUTPTRTO ge08.c ; mv ge08.o t57.o
	 $(COMPILE.c) -DURESETINPUTPTR ge08.c ; mv ge08.o t58.o
	 $(COMPILE.c) -DUNESTARALT ge08.c ; mv ge08.o t59.o
	 $(COMPILE.c) -DUNESTARSET ge08.c ; mv ge08.o t60.o
	 $(COMPILE.c) -DUMETATERMINAL ge08.c ; mv ge08.o t61.o
	 $(COMPILE.c) -DUWAS ge08.c ; mv ge08.o t61a.o
	 $(COMPILE.c) -DUFIND ge08.c ; mv ge08.o t61b.o
	 $(COMPILE.c) -DDSETINPUTPTRTO ge08.c ; mv ge08.o t62.o
	 $(COMPILE.c) -DDRESETINPUTPTR ge08.c ; mv ge08.o t63.o
	 $(COMPILE.c) -DDNESTARALT ge08.c ; mv ge08.o t64.o
	 $(COMPILE.c) -DDMETATERMINAL ge08.c ; mv ge08.o t65.o
	 $(COMPILE.c) -DDWAS ge08.c ; mv ge08.o t65a.o
	 $(COMPILE.c) -DDFIND ge08.c ; mv ge08.o t65b.o
	ar rv $(GLAMMAR).a  t65.o t64.o t63.o t62.o t61.o \
                       t60.o t59.o t58.o t57.o t65a.o t61a.o t65b.o t61b.o 
	rm -f  t65b.o t61b.o t65a.o t65.o t64.o t63.o t62.o t61.o t60.o \
                t59.o t58.o t57.o t61a.o
ge09: ge09.c
	 $(COMPILE.c) $(ADDRESSING) -DDPAIR ge09.c ; mv ge09.o t66.o
	 $(COMPILE.c) $(ADDRESSING) -DDUNPAIR ge09.c ; mv ge09.o t67.o
	 $(COMPILE.c) $(ADDRESSING) -DDREPAIR ge09.c ; mv ge09.o t68.o
	 $(COMPILE.c) $(ADDRESSING) -DUREPAIR ge09.c ; mv ge09.o t69.o
	 $(COMPILE.c) -DDWHERE ge09.c ; mv ge09.o t70.o
	 $(COMPILE.c) -DDEVALMETA ge09.c ; mv ge09.o t71.o
	 $(COMPILE.c) -DUEVALMETA ge09.c ; mv ge09.o t72.o
	 $(COMPILE.c) -DDINITMETA ge09.c ; mv ge09.o t73.o
	 $(COMPILE.c) -DUINITMETA ge09.c ; mv ge09.o t74.o
	 $(COMPILE.c) -DDASSIGN ge09.c ; mv ge09.o t75.o
	 $(COMPILE.c) -DUASSIGN ge09.c ; mv ge09.o t76.o
	ar rv $(GLAMMAR).a  t76.o t75.o t74.o t73.o t72.o t71.o t70.o t69.o t68.o t67.o t66.o
	rm -f  t76.o t75.o t74.o t73.o t72.o t71.o t70.o t69.o t68.o t67.o t66.o
ge10: ge10.c
	 $(COMPILE.c) -DDUNIXCON ge10.c ; mv ge10.o t77.o
	 $(COMPILE.c) -DUUNIXCON ge10.c ; mv ge10.o t78.o
	ar rv $(GLAMMAR).a  t78.o t77.o
	rm -f  t78.o t77.o
ge11: ge11.c
	 $(COMPILE.c) -DUEQUAL ge11.c ; mv ge11.o t79.o
	 $(COMPILE.c) -DUNOTEQUAL ge11.c ; mv ge11.o t80.o
	 $(COMPILE.c) -DUFQUAL ge11.c ; mv ge11.o t81.o
	 $(COMPILE.c) -DUFOTEQUAL ge11.c ; mv ge11.o t82.o
	 $(COMPILE.c) -DUCOMPARE ge11.c ; mv ge11.o t83.o
	 $(COMPILE.c) -DUCONTENTSOF ge11.c ; mv ge11.o t84.o
	 $(COMPILE.c) -DUCHARNUM ge11.c ; mv ge11.o t85.o
	 $(COMPILE.c) -DUSETTABSTOP ge11.c ; mv ge11.o t86.o
	 $(COMPILE.c) -DUSOMENAME ge11.c ; mv ge11.o t87.o
	ar rv $(GLAMMAR).a  t87.o t86.o t85.o t84.o t83.o t82.o t81.o t80.o t79.o
	rm -f  t87.o t86.o t85.o t84.o t83.o t82.o t81.o t80.o t79.o
ge12: ge12.c
	 $(COMPILE.c) -DUVALUEOF ge12.c ; mv ge12.o t88.o
	 $(COMPILE.c) -DULOOKUP ge12.c ; mv ge12.o t89.o
	 $(COMPILE.c) -DUINCLUDES ge12.c ; mv ge12.o t90.o
	 $(COMPILE.c) -DUEXCLUDES ge12.c ; mv ge12.o t91.o
	 $(COMPILE.c) -DUPAIR ge12.c ; mv ge12.o t92.o
	 $(COMPILE.c) -DUUNPAIR ge12.c ; mv ge12.o t93.o
	 $(COMPILE.c) -DUWHERE ge12.c ; mv ge12.o t94.o
	 $(COMPILE.c) -DUCOMPUTE ge12.c ; mv ge12.o t95.o
	 $(COMPILE.c) -DUSIZE ge12.c ; mv ge12.o t96.o
	ar rv $(GLAMMAR).a  t96.o t95.o t94.o t93.o t92.o t91.o t90.o t89.o t88.o
	rm -f  t96.o t95.o t94.o t93.o t92.o t91.o t90.o t89.o t88.o
ge13: ge13.c
	 $(COMPILE.c) -DUREALTOINT ge13.c ; mv ge13.o t97.o
	 $(COMPILE.c) -DUDECIMALTOUN ge13.c ; mv ge13.o t98.o
	 $(COMPILE.c) -DUREVERSE ge13.c ; mv ge13.o t99a.o
	 $(COMPILE.c) -DUUPPERCASE ge13.c ; mv ge13.o t99.o
	 $(COMPILE.c) -DULOWERCASE ge13.c ; mv ge13.o t100.o
	 $(COMPILE.c) -DUKEYWORD ge13.c ; mv ge13.o t101.o
	 $(COMPILE.c) -DUTYPEOUT ge13.c ; mv ge13.o t102.o
	 $(COMPILE.c) -DUTYPEIN ge13.c ; mv ge13.o t103.o
	ar rv $(GLAMMAR).a t103.o t102.o t101.o t100.o t99.o t98.o t97.o t99a.o
	rm -f  t103.o t102.o t101.o t100.o t99.o t98.o t97.o t99a.o
ge14: ge14.c
	 $(COMPILE.c) -DDLEXICALLESS ge14.c ; mv ge14.o t104.o
	 $(COMPILE.c) -DDLEXICALMORE ge14.c ; mv ge14.o t105.o
	 $(COMPILE.c) -DDLEXICALLESSEQUAL ge14.c ; mv ge14.o t106.o
	 $(COMPILE.c) -DDLEXICALMOREEQUAL ge14.c ; mv ge14.o t107.o
	 $(COMPILE.c) -DULEXICALLESS ge14.c ; mv ge14.o t108.o
	 $(COMPILE.c) -DULEXICALMORE ge14.c ; mv ge14.o t109.o
	 $(COMPILE.c) -DULEXICALLESSEQUAL ge14.c ; mv ge14.o t110.o
	 $(COMPILE.c) -DULEXICALMOREEQUAL ge14.c ; mv ge14.o t111.o
	ar rv $(GLAMMAR).a  t111.o t110.o t109.o t108.o t107.o t106.o t105.o t104.o
	rm -f  t111.o t110.o t109.o t108.o t107.o t106.o t105.o t104.o
ge15: ge15.c
	 $(COMPILE.c) -DDEVALAFFIX ge15.c ; mv ge15.o t112.o
	 $(COMPILE.c) -DUEVALAFFIX ge15.c ; mv ge15.o t113.o
	 $(COMPILE.c) -DUFAIL ge15.c ; mv ge15.o t114.o
	 $(COMPILE.c) -DDFAIL ge15.c ; mv ge15.o t115.o
	 $(COMPILE.c) -DUSETEXITCODE ge15.c ; mv ge15.o t116.o
	 $(COMPILE.c) -DDSETEXITCODE ge15.c ; mv ge15.o t117.o
	 $(COMPILE.c) -DDGETENV ge15.c ; mv ge15.o t118.o
	 $(COMPILE.c) -DUGETENV ge15.c ; mv ge15.o t119.o
	 $(COMPILE.c) -DDERRORMESSAGE ge15.c ; mv ge15.o t118a.o
	 $(COMPILE.c) -DUERRORMESSAGE ge15.c ; mv ge15.o t119a.o
	ar rv $(GLAMMAR).a  t119a.o t119.o t118a.o t118.o t117.o t116.o t115.o t114.o t113.o t112.o
	rm -f  t119a.o t118a.o t119.o t118.o t117.o t116.o t115.o t114.o t113.o t112.o
ge16: ge16.c
	 $(COMPILE.c) -DDTRANSFORMLATTICE ge16.c ; mv ge16.o t120.o
	 $(COMPILE.c) -DDTRANSFORMLATTICETERM ge16.c ; mv ge16.o t121.o
	 $(COMPILE.c) -DDEXPLINTERSECT ge16.c ; mv ge16.o t122.o
	 $(COMPILE.c) -DUEXPLINTERSECT ge16.c ; mv ge16.o t123.o
	 $(COMPILE.c) -DDINTERSECT ge16.c ; mv ge16.o t124.o
	 $(COMPILE.c) -DUINTERSECT ge16.c ; mv ge16.o t125.o
	 $(COMPILE.c) -DUTRANSFORMLATTICE ge16.c ; mv ge16.o t126.o
	 $(COMPILE.c) -DUTRANSFORMLATTICETERM ge16.c ; mv ge16.o t127.o
	 $(COMPILE.c) -DUTLTRADITIONAL ge16.c ; mv ge16.o t128.o
	 $(COMPILE.c) -DUTLTRADITIONALTERM ge16.c ; mv ge16.o t129.o
	ar rv $(GLAMMAR).a  t129.o t128.o t127.o t126.o t125.o t124.o t123.o t122.o t121.o t120.o
	rm -f  t129.o t128.o t127.o t126.o t125.o t124.o t123.o t122.o t121.o t120.o
ge20: ge20.c
	 $(COMPILE.c) -DSET_AND_TEST_LIMITS ge20.c ; mv ge20.o t130.o
	 $(COMPILE.c) $(ADDRESSING) -DTRACE ge20.c ; mv ge20.o t131.o
	 $(COMPILE.c) -DFOUND ge20.c ; mv ge20.o t132.o
	ar rv $(GLAMMAR).a  t132.o t131.o t130.o
	rm -f  t132.o t131.o t130.o
ge21: ge21.c
	 $(COMPILE.c) -DREADINPUT ge21.c ; mv ge21.o t133.o
	ar rv $(GLAMMAR).a  t133.o
	rm -f  t133.o
ge22: ge22.c
	 $(COMPILE.c) -DLOOP ge22.c ; mv ge22.o t134.o
	 $(COMPILE.c) -DERMSG ge22.c ; mv ge22.o t135.o
	 $(COMPILE.c) -DPRINTA ge22.c ; mv ge22.o t136.o
	 $(COMPILE.c) -DDREPORT ge22.c ; mv ge22.o t136a.o
	 $(COMPILE.c) -DUREPORT ge22.c ; mv ge22.o t136b.o
	ar rv $(GLAMMAR).a  t136a.o t136b.o t136.o t135.o t134.o
	rm -f  t136a.o t136b.o t136.o t135.o t134.o
ge23: ge23.c
	 $(COMPILE.c) -DSET_ERRMSG ge23.c ; mv ge23.o t137.o
	 $(COMPILE.c) -DAFXCMP ge23.c ; mv ge23.o t138.o
	ar rv $(GLAMMAR).a  t138.o t137.o
	rm -f  t138.o t137.o
ge30: ge30.c
	 $(COMPILE.c) -DDTERMINAL ge30.c ; mv ge30.o t139.o
	 $(COMPILE.c) -DDCHOICE ge30.c ; mv ge30.o t140.o
	 $(COMPILE.c) -DDEX_CHOICE ge30.c ; mv ge30.o t141.o
	 $(COMPILE.c) -DDX_STAR_CHOICE ge30.c ; mv ge30.o t142.o
	 $(COMPILE.c) -DDX_STAR_EX_CHOICE ge30.c ; mv ge30.o t143.o
	 $(COMPILE.c) -DDX_PLUS_CHOICE ge30.c ; mv ge30.o t144.o
	 $(COMPILE.c) -DDX_PLUS_EX_CHOICE ge30.c ; mv ge30.o t145.o
	 $(COMPILE.c) -DUTERMINAL ge30.c ; mv ge30.o t146.o
	 $(COMPILE.c) -DUCHOICE ge30.c ; mv ge30.o t147.o
	 $(COMPILE.c) -DUEX_CHOICE ge30.c ; mv ge30.o t148.o
	 $(COMPILE.c) -DUX_STAR_EX_CHOICE ge30.c ; mv ge30.o t149.o
	 $(COMPILE.c) -DUX_STAR_CHOICE ge30.c ; mv ge30.o t150.o
	 $(COMPILE.c) -DUX_PLUS_EX_CHOICE ge30.c ; mv ge30.o t151.o
	 $(COMPILE.c) -DUX_PLUS_CHOICE ge30.c ; mv ge30.o t152.o
	ar rv $(GLAMMAR).a  t152.o t151.o t150.o t149.o t148.o t147.o t146.o t145.o t144.o t143.o t142.o t141.o t140.o t139.o
	rm -f  t152.o t151.o t150.o t149.o t148.o t147.o t146.o t145.o t144.o t143.o t142.o t141.o t140.o t139.o
ge40: ge40.c
	 $(COMPILE.c) -DDMP ge40.c ; mv ge40.o t153.o
	 $(COMPILE.c) -DUMP ge40.c ; mv ge40.o t154.o
	ar rv $(GLAMMAR).a  t154.o t153.o
	rm -f  t154.o t153.o
