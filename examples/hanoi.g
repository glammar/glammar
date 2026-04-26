#-d 

hanoi( moves>) :  
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
