combine words (generate+nlcr>):
     word (w>), " ",
     word (d>),
     generate combined word (>w,>d,generate>).

word (x>) :!abcdefghijklmnopqrstuvwxyz! + (x>).

generate combined word (>w, >d, c +generate>):
     split one alpha (>w, >d, c>, g1>, g2>),
     generate combined word (>g1, >g2, generate>), ->;  # cut
generate combined word (>w,>d,empty>):.
 
split one alpha (>ALPHA 1 + x,>ALPHA 2 +y, choose>,x>,y>): 
     choose alpha (>ALPHA 1, >ALPHA 2, choose>).

choose alpha (>alpha,>alpha,alpha>): ->;     # filters doubles
choose alpha (>alpha 1,>alpha 2, alpha 1>):;
choose alpha (>alpha 1,>alpha 2, alpha 2>):.

ALPHA 1:: @ one char.
ALPHA 2:: @ one char.

one char (c>): ^!! (c>).
