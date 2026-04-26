#
#Solve the jealous husbands problem, in which three married couples must cross a river 
#using a boat which can hold at most two people, subject to the constraint that no woman 
#can be in the presence of another man unless her husband is also present. 
#
#How to:
#
# compile to make jh
# glammar jh
#
# echo iiiiiiiiiii >i 
# ./jh -a1 i 
#
# -a1 ==> stop at 1 solution found 
# ./jh  i
# finds all 480 different solutions for a max trips of 11
# There seems no solution requiring less then 11 trips  

solve xxx and yyy river crossing puzzle("Found a solution for no more then max "+max nr trips+" trips:\n"+crossings>):
   !i!*(max trips>), layout,
    length(>max trips, max nr trips>),
# start condition is an empty boat meets all xxx and yyy on bamk A
    (>max trips, >"000", >"000") cross to bank (>"A") meeting (>"111", >"111", crossings>).

# end condition is boat and all xxx and yyy  are on bank B
#(>trips left, >x,>y) cross to bank (>"B") meeting ( >xx,  >yy, nr trips left + " nr trips left\n">): 
(>trips left, >x,>y) cross to bank (>"B") meeting ( >xx,  >yy, empty>): 
    combine(>y, >yy, THREE>),
    combine(>x, >xx, THREE>),
    length(>trips left, nr trips left>),->;
(>"i"+trips left, >arriving x, >arriving y) cross to bank (>b) meeting ( >at bank x, >at bank y, crossing+crossings>): 
# check for safe arrival
   (>arriving x, >arriving y) can meet (>at bank x, >at bank y) provided (stay in boat x>, stay in boat y>) stays, 

# check for safe departure
    arriving(>arriving x, >arriving y, >stay in boat x, >stay in boat y) 
        on bank (>b) meeting ( >at bank x,  >at bank y) 
            departing (dx>, dy>) towards (ib>, tx>, ty>) as crossing( crossing>),
#    type out(>crossing), 
    (>trips left,  >dx, >dy) cross to bank (>ib) meeting (>tx, >ty, crossings>). 
                                           
arriving(>arriving x, >arriving y, >must stay in boat x, >must stay in boat y) on bank (>b) 
                       meeting ( >at the bank x,  >at the bank y) 
                       departing (departing x>, departing y>) 
                       towards (ib>, x on other side>, y on other side>)
                       as crossing ( crossing>):
     combine(>arriving y, >at the bank y, near this bank y>),
     combine(>arriving x, >at the bank x, near this bank x>),
     choose x and or y (>near this bank x, >near this bank y, departing x>, departing y>, stay behind x>,stay behind y>),
     not equal (>arriving x+arriving y, >departing x+departing y),
     test departing (>departing x) must include (>must stay in boat x),
     test departing (>departing y) must include (>must stay in boat y),
     (>stay behind x) with (>stay behind y) are safe,
     where  (>at the bank x+at the bank y+departing x+departing y, occurs at most once>),
     excludes (>occurs at most once, >empty),
     add to  (>occurs at most once, >empty, >empty),
     invert (>near this bank y, y on other side>),
     invert (>near this bank x, x on other side>),
     invert(>b, ib>),
     pretty print(>ib, >stay behind x,>stay behind y, >departing x, >departing y, >x on other side, >y on other side,
        crossing>).

# choose x and or y for departure
choose x and or y (>x, >y, departing x>,departing y>, stay behind x>, stay behind y>):
    [ x ]  choose any (departing x>, stay behind x>), 
    [ y ]  choose any (departing y>, stay behind y>), 
    not equal(>departing x+departing y, >"000000"),
    zero one or two xy(>"ii", >departing x+departing y).
      
one(>"100"):->;
one(>"010"):->;
one(>"001"):.

zero one or two x y (>_, >empty): ->;
zero one or two x y (>i, >"0"+x): 
   zero one or two x y (>i, >x), ->;
zero one or two x y (>"i"+i, >"1"+x): 
   zero one or two x y (>i, >x).

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

test departing (>x) must include (>"000"): ->;
test departing (>x) must include (>y):
  departing (>x) must include (>y),
    type out (>"departing must include"+x+","+y+" succeeded\n"), ->;
test departing (>x) must include (>y):
    type out (>"departing must include"+x+","+y+" failed\n"),
     fail.
    
departing (>empty) must include (>empty):->;
departing (>"1"+d) must include (>"1"+i):
   departing (>d) must include (>i) ,->;
departing (>"0"+d) must include (>"0"+i):
   departing (>d) must include (>i) ,->;
departing (>"0"+d) must include (>"1"+i):
   departing (>d) must include (>i).

choose any(zz>, zz>):
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

(>xxx) with (>yyy) are safe:
   more (>xxx) with (>yyy) are safe,
#   type out (>"xxx "+xxx+" with yyy "+yyy+" are safe"+nlcr),
   ->;
(>xxx) with (>yyy) are safe:
#   type out (>"xxx "+xxx+" with yyy "+yyy+" run"+nlcr),
      fail.
# checking what is left behind on departure is safe
more (>empty) with (>empty ) are safe:   ->;
more (>"000") with (>y ) are safe:       ->;
more (>"1"+x) with (>"1"+y ) are safe:   ->, more (>x) with (>y) are safe;
more (>"1"+x) with (>"0"+y ) are safe:   ->, more (>x) with (>y) are safe;
more (>"0"+x) with (>"0"+y ) are safe:       more (>x) with (>y) are safe.

# checking safe arrival
# (arriving xxx,arriving yyy) can meet (xxx on shore, yyy on shore) provided (x , y) stays ib boat  
(>"000", >"000") can meet (>"111", >"111") provided ("000">, "000">) stays:->;
(>a x, >a y ) can meet (>b x, >b y) provided ("000">,"000">) stays:
      combine (>a x, >b x,   c x>),
      combine (>a y, >b y,   c y>),
      (>c x) with (>c y)  are safe, ->;
(>a x,  >a y ) can meet (>b x, >b y) provided ("000">, a y>) stays:
      combine (>a y, >b y,   c y>),
      [ a x] choose any (c x>, r x>), one (>r x),
      combine (>r x, >b x,   c x>),
      (>c x) with (>c y)  are safe,->;
(>a x,  >a y ) can meet (>b x, >b y) provided (a x>, "000">) stays:
      combine (>a x, >b x,   c x>),
      [a y] choose any (c y>, r y>), one (>c y),
      combine (>r y, >b y,   c y>),
      (>c x) with (>c y)  are safe.

pretty print(>"A", >sx,>sy, >dx, >dy, >tx, >ty, crossing>):
    help pretty print (>empty,>"x",>dx, pdx>),
    help pretty print (>empty,>"y",>dy, pdy>),
    help pretty print (>".",>"x",>tx, ptx>),
    help pretty print (>".", >"y",>ty, pty>),
    help pretty print (>".",>"x",>sx, psx>),
    help pretty print (>".", >"y",>sy, psy>),
    extra space(>"  ",>dx+dy,es>),
    where(>"  "+ptx+" "+pty+ "  <-- "+pdx+pdy+es+" <-- "+psx + " "+ psy+nlcr, crossing>),->;
pretty print(>"B", >sx,>sy, >dx, >dy, >tx, >ty, crossing>):
    help pretty print (>empty,>"x",>dx, pdx>),
    help pretty print (>empty,>"y",>dy, pdy>),
    help pretty print (>".",>"x",>tx, ptx>),
    help pretty print (>".", >"y",>ty, pty>),
    help pretty print (>".",>"x",>sx, psx>),
    help pretty print (>".", >"y",>sy, psy>),
    extra space(>"  ",>dx+dy,es>),
    where(>"  "+psx+" "+psy+ "  --> "+pdx+pdy+es+" --> "+ptx + " "+ pty+nlcr, crossing>).

help pretty print (>n, >w, >"000", BOLD MAGENTA+n+RESET+BOLD GREEN+n+RESET+BOLD YELLOW+n+RESET>):->;
help pretty print (>n, >w, >"001", BOLD MAGENTA+n+RESET+BOLD GREEN+n+RESET+BOLD YELLOW+w+RESET>):->;
help pretty print (>n, >w, >"010", BOLD MAGENTA+n+RESET+BOLD GREEN+w+RESET+BOLD YELLOW+n+RESET>):->;
help pretty print (>n, >w, >"011", BOLD MAGENTA+n+RESET+BOLD GREEN+w+RESET+BOLD YELLOW+w+RESET>):->;
help pretty print (>n, >w, >"100", BOLD MAGENTA+w+RESET+BOLD GREEN+n+RESET+BOLD YELLOW+n+RESET>):->;
help pretty print (>n, >w, >"101", BOLD MAGENTA+w+RESET+BOLD GREEN+n+RESET+BOLD YELLOW+w+RESET>):->;
help pretty print (>n, >w, >"110", BOLD MAGENTA+w+RESET+BOLD GREEN+w+RESET+BOLD YELLOW+n+RESET>):->;
help pretty print (>n, >w, >"111", BOLD MAGENTA+w+RESET+BOLD GREEN+w+RESET+BOLD YELLOW+w+RESET>):.

extra space(>empty, >_, empty>):->;
extra space(>es, >empty, es>):->;
extra space(>" "+s, >"1"+d, es>):
   extra space(>s, >d, es>),->;
extra space(>s, >"0"+d, es>):
   extra space(>s, >d, es>).
