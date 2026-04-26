# Generate makefile entries
# Usage : grep "ifdef" ge??.c | gen
# Delete first two lines ; append two at the end

lines ( l + ll>) : line (l>) ,  lines(ll>), ->;
lines ( empty>):.

#Orion patch
#line(header + "\t $(COMPILE.c) -O -D"+target+" "+ fn+ "; mv "+bn+".o t"+d+".o"+nlcr>):
line (header + "\t $(COMPILE.c) -O -D"+target+" "+ fn+ " -o t"+d+".o"+nlcr>):
   filename (header>,fn>, bn>),
   ":#ifdef", layout,
   name (target>),
   increment smallint (>N),
   int (>N) is dec (d>),
   assign (>FILES, > " t"+d+".o"+o,o>).

filename (h>,fn>, BASENAME>):
   !gemp0123456789.c! * (fn>),
   where (>fn,BASENAME>),
   more finename (>fn,>BASENAME, h>).

more finename (>FN, >bn, empty>): ->;
more finename (>fn, >bn, "\tar rv $(GLAMMAR).a " + ofiles+  "\n\trm -f "+ 
                              ofiles+ "\n"+bn+": "+fn +nlcr>):
 assign (>FN,>fn,o>),
 assign (>FILES,>empty,ofiles>).

name (id>): !ABCDEFGHIJKLMNOPQRSTUVWXYZ_! +(id>), layout.

FN::.
BN::.
FILES::.
N::1.

BASENAME::@basename.

basename(basename>):
  ^!.!*(basename>), ^!!*.
