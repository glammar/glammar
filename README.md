# glammar
Glammar is a compiler compiler useful in rapid prototyping of syntax directed transducers.
A glammar source is also named a grammar as very basic glammar source is also a grammar.
Not all programs need to parse or process input.  For example a glammar source is often the easiest way
to solve a puzzle programmatically. 

Below is an example of a deterministic puzzle solving grammer: 

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

     -- `gen' is the only direct non-deterministic rule.
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

    solve robbers with a bag river crossing puzzle("Found one: \n"+crossings>):
    # start condition is an empty boat meets all robbers and bags on bank A
    (>"000", >"000") cross to bank (>"A") meeting (>"111", >"111", crossings>).

    # end condition is boat and all robbers and bags  are on bank B
    (>one robber,>one bag) cross to bank (>"B") meeting ( >two robbers,  >two bags, empty>):
        two (>two robbers),
        two (>two bags),
        one(>one robber),
        one(>one bag),->;
    (>one robber,>"000") cross to bank (>"B") meeting ( >two robbers,  >THREE, empty>):
        two (>two robbers),
        one(>one robber),->;
    (>any new robbers, >any new bags) cross to bank (>b) meeting ( >any old robbers,  >any old bags, crossing+crossings>):
    # check for safe arrival
     (>any new robbers, >any new bags ) can meet (>any old robbers, > any old bags) provided  (stay put>) stays,
     no new bags if (>stay put, >any new bags, arriving bags>),
    # check for safe departure
    arriving(>any new robbers, >arriving bags, >stay put) on bank (>b) meeting ( >any old robbers,  >any old bags)
                       departing (d r>, d b>) towards (t r>, t b>),
      invert(>b, ib>),
      pretty print this crossing(>"robber",>d r, m>),
      pretty print this crossing(>"and bag   ",>d b, mm>),
      where(>"  "+m+mm+"row to "+ib+ " meeting "+t r + " "+ t b+nlcr, crossing>),
     #    type out(>crossing),
    (>d r, >d b) cross to bank (>ib) meeting (>t r, >t b, crossings>).


     arriving(>any new robbers, >any newbags, >stay put bag) on bank (>b)
                       meeting ( >any old robbers,  >any old bags)
                       departing (departing robbers>, departing bags>)
                       towards (robbers on other side>, bags on other side>):
       combine(>any  new bags, >any old bags, new and old bags>),
       combine(>any  new robbers, >any old robbers, robbers>),
       choose robbers and or bag (>robbers, >new and old bags,>stay put bag,  departing robbers>, departing bags>, 
                             stay behind robbers>,stay behind bags>),
       not equal (>any old robbers+any old bags, >stay behind robbers+stay behind bags),
       robber (>stay behind robbers) with bags (>stay behind bags) doesn't run,
       combine(>new and old bags, >stay put bag, bags>),
       where  (>robbers+any old bags+stay put bag+b, occurs at most once>),
       excludes (>occurs at most once, >empty),
       add to  (>occurs at most once, >empty, >empty),
       invert (>bags, bags on other side>),
       invert (>robbers, robbers on other side>).

     # choose robbers and or bag for departure
     choose robbers and or bag (>robbers, >stay behind bags,>bag left in boat,  departing robber>,  bag left in boat>,
        stay behind robbers>, stay behind bags>):
        one (>bag left in boat),->,
       [ robbers ]  choose any (departing robber>, stay behind robbers>), one (>departing robber);
    choose robbers and or bag (>robbers, >bags,>no bag in boat,  departing robbers>,  departing bag>,
        stay behind robbers>, stay behind bags>):
     [ robbers ] choose any (departing robbers>, stay behind robbers>), one or two (>departing robbers),
     [ bags    ] choose any (departing bag>, stay behind bags>), zero or one (>departing bag).

    two (>"110"):->;
    two (>"101"):->;
    two (>"011"):.

     one(>"100"):->;
     one(>"010"):->;
     one(>"001"):.
     
     one or two(>one): one (>one),->;
     one or two(>two): two (>two).
     
     zero or one (>"000"):->;
     zero or one (>one): one (>one).
     
     THREE::"111".
     
     combine(>empty, >empty, empty>):->;
     combine(>"0"+x, >"0"+y, "0"+z>):
          combine(>x,>y, z>),->;
     combine(>"0"+x, >"1"+y, "1"+z>):
          combine(>x,>y, z>),->;
     combine(>"1"+x, >"0"+y, "1"+z>):
          combine(>x,>y, z>),->;
     combine(>x, >y, x>):
         type out (>"combine of "+x+","+y+" failed\n"),
          fail.
     
     
     choose any(zz>, zz+empty>):
        !0!*(zz>),
        end of sentence,->;
     choose any ("0"+c>, "0"+nc>):
        "0", ->,
        choose any(c>, nc>);
     choose any ("0"+c>, "1"+nc>):
        "1",
        choose any(c>, nc>);
     choose any ("1"+c>, "0"+nc>):
        "1",
        choose any(c>, nc>).
     
     
     invert(>empty, empty>):->;
     invert(>"0", "1">):->;
     invert(>"A", "B">):->;
     invert(>"B", "A">):->;
     invert(>"1", "0">):->;
     invert(>"0"+x, "1"+z>):
         invert(>x, z>),->;
     invert(>"1"+x, "0"+z>):
         invert(>x, z>).
     
     robber (>robbers) with bags (>bags) doesn't run:
        more robber (>robbers) with bags (>bags) doesn't run,->;
     robber (>robbers) with bags (>bags) doesn't run:
     #   type out (>"robbers "+robbers+" with bags "+bags+" run"+nlcr),
           fail.
     
     # checking what is left behind on departure is safe
     more robber(>robbers)with bags (>"000") doesn't run:->;
     more robber(>"001") with bags (>b) doesn't run:->, equal (>b, >"001");
     more robber(>"010") with bags (>"1"+x) doesn't run:->, fail;
     more robber(>"010") with bags (>"011") doesn't run:->, fail;
     more robber(>"100") with bags (>"100") doesn't run:->;
     more robber(>"100") with bags (>"1"+x) doesn't run:->, fail;
     more robber(>robbers) with bags (>bags) doesn't run:.
     
     
     # checking safe arrival
     # (arriving robbers,arriving bags) can meet (robbers on shore, bags on shore) provided (bag in boat or no bag) stays
     (>"000", >"000") can meet (>"111", >"111") provided ("000">) stays:->;
     (>one robber, >without a bag) can meet (>any old robbers, >bags) provided ("000">) stays:
           one (>one robber),
           equal (>without a bag, >"000"),
           combine (>one robber, >any old robbers, robbers>),
           robber (>robbers) with bags (>bags) doesn't run,->;
     (>robbers, >_) can meet (>_+"1"+_, >_) provided ("000">) stays :->;
     (>two robbers,>"000") can meet (>any robbers,>bags)  provided ("000">) stays :
           two (>two robbers),->;
       #      choose one (>two robbers, one robber>, other robber>),
       #      robber (>one robber) with bags (>bags) doesn't run;
     (>robber,>with a bag) can meet (>"000",>meeting bags) provided ("000">) stays :
           one (>robber),
           combine (>with a bag , >meeting bags, bags>),
           robber (>robber) with bags (>bags) doesn't run,->;
     (>robber,>with a bag) can meet (>"000",>bags) provided (with a bag>) stays :
           one (>robber),
           robber (>robber) with bags (>bags) doesn't run,->;
     (>robbers,>bags) can meet (>any old robbers,>any old bags) provided ("000">) stays:
        #    type out (>"robbers "+robbers+" with bags "+bags+" cannot meet "+ any old robbers+ " "+any old bags+nlcr),
         fail.
     
     no new bags if (>"000", >new bags, new bags>): ->;
     no new bags if (>stay put bag, >stay put bag, "000">):.
     
     
     pretty print this crossing(>w, >"100", w+"8k ">):->;
     pretty print this crossing(>w, >"010", w+"5k ">):->;
     pretty print this crossing(>w, >"001", w+"3k ">):->;
     pretty print this crossing(>w, >"000", empty>):->;
     pretty print this crossing(>w, >"110", w+"8k and "+w+"5k ">):->;
     pretty print this crossing(>w, >"101", w+"8k and "+w+"3k ">):->;
     pretty print this crossing(>w, >"011", w+"5k and "+w+"3k ">):.
     

