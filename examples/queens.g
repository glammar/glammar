
eight queens on a chess board ("Found one: "+nlcr+y+nlcr>):  layout,
     permute(>Eight Queens,x>),
     safe (>x),
     walk(>x,y>).

permute(>a*list,out>) :  
     permute(>list,another list>),
     gen (>a, >another list, out>);
permute(>empty,empty>) : .

# `gen' is the only direct non-deterministic rule.
gen (>a,>list, a*list>):;
gen (>a, >b*list, b*out>): 
     gen (>a, >list, out>).

safe (>a*b):  safe (>b),no diag (>a,>One,>b),->;
safe (>empty):.


no diag (>a ,> m, >c*d): 
     where (>a , a minus c + c>), 
     not equal (>a minus c ,>m), 
     no diag (>a,>m+One,>d),->;
no diag (>a ,> m, >c*d): 
     where (>c, c minus a + a>),
     not equal (>c minus a ,>m), 
     no diag (>a,>m+One,>d),->;
no diag (>a,>n, >empty): .
   
walk (>a*b,out+nlcr+wb>):
     out (>a, out>),
     walk (>b, wb>),->;
walk (>empty,empty>):.

out  (>One,  " q  .  .  .  .  .  .  .">):->;
out  (>Two,  " .  q  .  .  .  .  .  .">):->;
out  (>Three," .  .  q  .  .  .  .  .">):->;
out  (>Four, " .  .  .  q  .  .  .  .">):->;
out  (>Five, " .  .  .  .  q  .  .  .">):->;
out  (>Six,  " .  .  .  .  .  q  .  .">):->;
out  (>Seven," .  .  .  .  .  .  q  .">):->;
out  (>Eight," .  .  .  .  .  .  .  q">):.

Eight Queens :: One*Two*Three*Four*Five*Six*Seven*Eight*empty.

One   :: "i".
Two   :: "i" +One. 
Three :: "i" +Two. 
Four  :: "i" +Three. 
Five  :: "i" +Four. 
Six   :: "i" +Five. 
Seven :: "i" +Six. 
Eight :: "i" +Seven. 

#
#       The following hyperrules are builtins but could be defined as:
#
# layout : !\n\t !* .
# where (>x,x>):.
# empty ::.
# nlcr :: "\n".
