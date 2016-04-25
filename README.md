# glammar
Glammar is a compiler compiler useful in rapid prototyping of syntax directed transducers.
A glammar source is also named a grammar as very basic glammar source is also a grammar.
Not all programs need to parse or process input.  For example a glammar source is often the easiest way
to solve a puzzle programmatically. 

Below is an example of a deterministic puzzle solving grammer:
#-d
towers of hanoi( moves>) :
    number (n>),
    (moves>) move (>n) disks from (>S) to (>D) using (>A).

("Move disc 1 from " +s + " to " +d + nlcr>) move (>I) disks from (>s) to (>d) using (>_): ->;
(x+"Move disc "+dec n+ " from " + s+ " to "+ d+ nlcr+y>) move (>I+m) disks from (>s) to (>d) using (>a):
    length(>I+m, dec n>),
    (x>) move (>m) disks from (>s) to (>a) using (>d),
    (y>) move (>m) disks from (>a) to (>d) using (>s).

number (u>):
    layout, !0123456789! + (n>),layout,
    (>n) decimal to unary ( u>).

I::"1".
S::"1".
D::"2".
A::"3".

And below is an example of a non-deterministic puzzle solving grammar:

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

safe (>a*b):  safe (>b),no diag (>a,>One,>b);
safe (>empty):.


no diag (>a ,> m, >c*d):
     where (>a , a minus c + c>),
     not equal (>a minus c ,>m),
     no diag (>a,>m+One,>d);
no diag (>a ,> m, >c*d):
     where (>c, c minus a + a>),
     not equal (>c minus a ,>m),
     no diag (>a,>m+One,>d);
no diag (>a,>n, >empty): .


walk (>a*b,out+nlcr+wb>):
     out (>a, out>),
     walk (>b, wb>);
walk (>empty,empty>):.


out  (>One,  " q  .  .  .  .  .  .  .">):;
out  (>Two,  " .  q  .  .  .  .  .  .">):;
out  (>Three," .  .  q  .  .  .  .  .">):;
out  (>Four, " .  .  .  q  .  .  .  .">):;
out  (>Five, " .  .  .  .  q  .  .  .">):;
out  (>Six,  " .  .  .  .  .  q  .  .">):;
out  (>Seven," .  .  .  .  .  .  q  .">):;
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

And while these are fairly standard examples of puzzles for which a solition is provided in many languages
here is one that is harder to google:

# grammar to solve the robbers crossing puzzle

rrx ("Found one "+m+nlcr>):
    moves (>"1",>"111 111", m>).

moves(>"0",>"000 000", empty>): ->;
moves(>"0", >x, m+" row from B to A\n"+mm>):
   move down (>x, z>, m>),
   excludes(>z, >"0"),
   add to (>z, >"0", >m),
   type out(>m+" row from B to A\n"),
   moves(>"1",>z, mm>),->;
moves(>"1",>x, m+" row from A to B\n"+mm>):
   move up (>x, z>, m>),
   excludes(>z, >"1"),
   add to (>z, >"1", >m),
   type out(>m+" row from B to A\n"),
   moves(>"0",>z, mm>).


#2 transitions 1r and 1b (3r,3b) at A
move up(>"111 111","011 011">, "r8 and b8">):;
move up(>"111 111","011 101">, "r8 and b5">):;
move up(>"111 111","011 110">, "r8 and b3">):;
move up(>"111 111","101 101">, "r5 and b5">):;
move up(>"111 111","101 110">, "r5 and b3">):;
move up(>"111 111","110 110">, "r3 and b3">):;

#2 transitions 1r and 1b (3r,2b at A ) ->(2r, 1b at A)
move up(>"111 110","011 100">, "r8 and b5">):;
move up(>"111 101","011 100">, "r8 and b3">):;

#2 transition 1r and 1b (2r,3b) at A -> (1r, 2b) at A
move up(>"101 111","100 011">, "r3 and b8">):;


#2 transition 1r and 1b (2r,2b) at A -> (1r, 1b) at A
move up(>"110 110","010 010">, "r8 and b8">):;
move up(>"110 110","100 010">, "r5 and b5">):;
move up(>"110 110","100 100">, "r5 and b8">):;
move up(>"101 110","100 100">, "r3 and b5">):;
move up(>"011 110","010 010">, "r3 and b3">):;


#2 transition 1r and 1b (1r,2b at A)-> (0r, 1b at A)

move up(>"100 011","000 010">, "r8 and b3">):;
move up(>"100 011","000 001">, "r8 and b5">):;

#2 transition 1r and 1b (2r,1b at A) -> (1r, 0b at A)

move up(>"011 100","010 000">, "r8 and b3">):;
move up(>"011 100","001 000">, "r8 and b3">):;

#2 transition 1r and 1b (1r,1b at A) -> (0r, 0b at A)
move up(>"100 100","000 000">, "r8 and b8">):;
move up(>"100 010","000 000">, "r8 and b5">):;
move up(>"100 001","000 000">, "r8 and b3">):;
move up(>"010 001","000 000">, "r5 and b3">):;
move up(>"010 010","000 000">, "r5 and b5">):;
move up(>"001 001","000 000">, "r3 and b3">):;


#2 transition 2r  (3r,3b at A) -> (1r, 3b at A) NA
#2 transition 2r  (2r,3b at A) -> (0r, 2b at A)
move up(>"110 111","000 111">, "r8 and r5">):;
move up(>"101 111","000 111">, "r8 and r3">):;
move up(>"011 111","000 111">, "r5 and r3">):;

#2 transition 2r  (2r,2b at A) -> (0r, 2b at A)
move up(>"110 110","000 110">, "r8 and r5">):;
move up(>"101 110","000 110">, "r8 and r3">):;
move up(>"011 110","000 110">, "r5 and r3">):;

#1 transition 1r   (3r,3b at A ) ->(2r, 3b at A)
move up(>"111 111","011 111">, "r8">):;
move up(>"111 111","101 111">, "r5">):;
move up(>"111 111","110 111">, "r8">):;

#1 transition 1r   (3r,2b at A ) ->(2r, 2b at A)
move up(>"111 110","011 110">, "r8">):;
move up(>"111 101","011 101">, "r8">):;
move up(>"111 011","011 011">, "r8">):;
move up(>"111 101","101 101">, "r5">):;
move up(>"111 110","101 110">, "r5">):;
move up(>"111 110","110 110">, "r3">):;

#1 transition 1r   (3r,1b at A ) ->(2r, 1b at A)
move up(>"111 100","011 100">, "r8">):.

move down ( >"011 100", "111 100">, "r8">):;
move down ( >"110 110", "111 110">, "r3">):;
move down ( >"101 110", "111 110">, "r5">):;
move down ( >"101 101", "111 101">, "r5">):;
move down ( >"011 011", "111 011">, "r8">):;
move down ( >"011 101", "111 101">, "r8">):;
move down ( >"011 110", "111 110">, "r8">):;
move down ( >"110 111", "111 111">, "r8">):;
move down ( >"101 111", "111 111">, "r5">):;
move down ( >"011 111", "111 111">, "r8">):;
move down ( >"000 110", "011 110">, "r5 and r3">):;
move down ( >"000 110", "101 110">, "r8 and r3">):;
move down ( >"000 110", "110 110">, "r8 and r5">):;
move down ( >"000 111", "011 111">, "r5 and r3">):;
move down ( >"000 111", "101 111">, "r8 and r3">):;
move down ( >"000 111", "110 111">, "r8 and r5">):;
move down ( >"000 000", "001 001">, "r3 and b3">):;
move down ( >"000 000", "010 010">, "r5 and b5">):;
move down ( >"000 000", "010 001">, "r5 and b3">):;
move down ( >"000 000", "100 001">, "r8 and b3">):;
move down ( >"000 000", "100 010">, "r8 and b5">):;
move down ( >"000 000", "100 100">, "r8 and b8">):;
move down ( >"001 000", "011 100">, "r8 and b3">):;
move down ( >"010 000", "011 100">, "r8 and b3">):;
move down ( >"000 001", "100 011">, "r8 and b5">):;
move down ( >"000 010", "100 011">, "r8 and b3">):;
move down ( >"010 010", "011 110">, "r3 and b3">):;
move down ( >"100 100", "101 110">, "r3 and b5">):;
move down ( >"100 100", "110 110">, "r5 and b8">):;
move down ( >"100 010", "110 110">, "r5 and b5">):;
move down ( >"010 010", "110 110">, "r8 and b8">):;
move down ( >"100 011", "101 111">, "r3 and b8">):;
move down ( >"011 100", "111 101">, "r8 and b3">):;
move down ( >"011 100", "111 110">, "r8 and b5">):;
move down ( >"110 110", "111 111">, "r3 and b3">):;
move down ( >"101 110", "111 111">, "r5 and b3">):;
move down ( >"101 101", "111 111">, "r5 and b5">):;
move down ( >"011 110", "111 111">, "r8 and b3">):;
move down ( >"011 101", "111 111">, "r8 and b5">):;
move down ( >"011 011", "111 111">, "r8 and b8">):.

