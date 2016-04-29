%
Description of builtin rules in Glammar

This section contains the description of  the current set of builtin rules.  
Some builtin rules may not be redefined.
The comment statement \verb+#plain+ indicates this for hyper rules.
None of the meta builtin rules may be redefined.
There are also some builtin rules that you may not even use.
In that case the name ends with an underscore. 
Other properties of a builtin rule are implicit in the form of its definition.
These properties are:

   -deterministic builtin rules have only one production:
    example:
      
       builtin: x.

   - nondeterministic builtin rules that can be forced
             to behave deterministic (-d option) have two productions:

       builtin: x;
       builtin: x.

    - nondeterministic builtin rules have three productions:

       builtin: x;
       builtin: x;
       builtin: x.

    - builtin rules that always succeed have no members in the right hand side:

       builtin: .

    - builtin rules that do not produce empty have a terminal ("a" by default) in the right hand side:

       builtin:  "a".

    - builtin rules that use an inherited affix for affix directed parsing have a single affix terminal on this position:

       builtin(>"x"):  "a".

    - builtin rules that do not use an inherited affix for affix directed parsing
             have a single affix variable on this position:

       builtin(>x):  "a".
%

empty ::.
nlcr :: "\n".
quote :: "\"".
back slash :: "\\".
unary base::"1".
RESET::"\033[0m".
BLACK::"\033[30m".
RED::"\033[31m".
GREEN::"\033[32m".
YELLOW::"\033[33m".
BLUE::"\033[34m".
MAGENTA::"\033[35m".
CYAN::"\033[36m".
WHITE::"\033[37m".
BOLD BLACK::"\033[1m\033[30m".
BOLD RED::"\033[1m\033[31m".
BOLD GREEN::"\033[1m\033[32m".
BOLD YELLOW::"\033[1m\033[33m".
BOLD BLUE::"\033[1m\033[34m".
BOLD MAGENTA::"\033[1m\033[35m".
BOLD CYAN::"\033[1m\033[36m".
BOLD WHITE::"\033[1m\033[37m".




%
The time and date the grammar was generated.
This meta rule is set when you compile the grammar.  
The definitions below show their form only.
%
generation time :: "Fri Sep 22 11:53:37 1989".
generation date :: "Sep 22, 1989".
script name::.
%
 All special symbols can be denoted by giving the 
octal ascii representation escaped by a back slash.
To remain compatible with earlier implementations some special symbols
are predefined. 
%
#plain
end of sentence= ?"\000".
%
Tests for reached the end of the input.
%
nlcr= "\n".
quote = "\"".
back slash = "\\".
%
Recognize a newline, quote or back slash respectively.
%

%
This is a list of predicates which have been implemented in very early
versions also.
%


#plain
equals empty(>"x")=.
equal (>"x",>"x")= .
#plain
not equal (>"x",>"y")= .
%
Recognizes the empty string (`succeeds') if the values of x} and y are
(not) equal.
%

to utc(>time, utc>)=.
%
convert to utc 
time must be in format as in 2013/08/07 14:34:17
%

find(>"target")= "a".
%
Find skips input until target is found.
It fails if target is not in the current input stream.  
%
was 1000(last 1000 chars read>)=. 
was 100(last 100 chars read>)=. 
was 10(last 10 read>)=. 
was (last>)=. 
%
last is assigned the value of the previously read character. 
If at the beginning of the input last} is set to empty. 
The returned affix appears to be reversed from the original input, 
thus the rule rule: "WAS" , last10 ("SAW">).  can be succesfully evaluated. 
%
reverse (>string, gnirts>)=.
%
reverses string} and assigns the result to gnirts.
%
compare (>x,>y,tail x>, tail y>)= .

%
Let K be the longest common prefix of (the values of)  x} and  y.
Then   tail x} and  tail y 
will assigned values such that   x}  = K +  tail x} and  y = K +  {\tt tail y.
%

charnum (pos>)= .
settabstop (>tabstop)= .
linenum (line>)= .
length (>in,length>)= .
%
The affix pos} in charnum is given the number of characters, in decimal, already recognized in the input.
The affix tabstop} in settabstop specifies the value (in decimal) of the variable tabstop.
This value is used in charnum.
The default value for tabstop} is 8.
The affix line} in linenum is given the value (in decimal) of the current line number.
The affix length} in length is given the value (in decimal) of the number of characters 
of affix in.
%

some name (name>)= .
%
The affix name is given a value that is composed of 32 upper and 
lower case letters.
The number of characters of this name is the minimum that is necessary 
to yield a unique name.
Each name can occur only once.
%

%
GLAMMAR allows the manipulation of certain ordered sequences of pairs of values 
for historic reasons called {\em lists} although {\em tables} would be better instead.
%
add to (>table, >key,>value)= ;
add to (>table, >key,>value)= .
%
Each table is recorded under a name which is an arbitrary affix value 
(it may be empty, contain newlines, be obtained from some name etc.)
However it may not be composite
(i.e:  add to (>x*y,k, >v) is not allowed).
Initially every possible affix value is the name of an empty table.
The builtin add to puts the pair (K,P) in front of the table
named L, where L,K and P are the values of table, key} and value
respectively.
Upon backtracking the pair is 
deleted from the table, unless it is forced to behave deterministic.
%

value of (>"table", >"key",value>)= .
%
If (K,P) is the first pair, if any, in table L such that L
and K are the values of table} and key respectively
(which most be both known), then the value of value is P otherwise it
is the empty string.
%

select (>"table", key>,value>)=  ;
select (>"table", key>,value>)=  ;
select (>"table", key>,value>)=  .
%
The values of key and value are K and P respectively, where (K,P)
is any pair in the table L, where L is the value of table (which must be known).
Note that select succeeds once for each pair.
%
delete (>"table", key>,value>)=  ;
delete (>"table", key>,value>)=  ;
delete (>"table", key>,value>)=  .
%
Deletes all pairs in the table, one at a time.
When a pair is deleted,  upon backtracking the  deleted item is 
not restored. 
Instead one more element is deleted,
until the table is empty.
%


contents of (>"table",>pair pair sep, >key val sep, contents>)=.
%
contents of finds the contents of a table in one go, provided
the table is not empty.
The list of pairs are separated with pair pair sep and
the two elements of a pair are separated by key val sep.
%

size (>"table", size>)=  .
%
Affix size is given the number of pairs of table, in decimal notation.
%
lookup (>"table", >"key",value>)= .
%
lookup finds the first pair in the table with matching key.
It fails if there is no such pair.
%

includes (>"table", >"key")=  "a".
excludes (>"table", >"key")=  "a".
%
Above predicates succeed if there is a (no) pair (K,P) in table L where
L and K  are the values of table and key respectively.
%

pair (cell>, >key,>value)=  .
unpair (>"cell", key>,value>)= .
%
In addition to the table (tables) processing builtin rules there are three builtin rules
which work on named tuples.
Each tuple has a unique name.
You can always work avoid using the predicates pair and unpair.
Both are  generated by eliminating  the composition operator.
For example the following two rules are equal:
    a(>x*y*z):.
    
 and 
   a (>x paired with y paired with z):
      unpair(>x paired with y paired with z, x>, y paired with z>),
      unpair(>y paired with z>, y>, z>).

If the left position is applying instead of defining unpair} must be replaced with  pair.


The * operator can be used to implement lists, by using the convention of 
always adding an empty element as the tail element. Elements themselves can also 
be empty.

To traverse a list one can write:

list traverse(>empty):.
list traverse(>el*rest list):
     traverse (>rest list).

Because * is rigth associative concatenation of two list is somewhat cumbersom.
I.e concat(>l1, >l2, l1*l2>):. is not a list. It is a pair of two lists.
Nor is concat(>l1, >l2, l1+l2>):. Though it valid to concatenate two pairs it does
not result in the concatenation of two lists.


For a proper concatenate 
One has to construct a new list by duplicating the first list and replacing
the empty element of the first with the second list.

concat(>a*r, >l2, a*rr>): 
    concat(>r, >l2, rr>),->;
concat(>empty, >l2, l2>):. 


A non associative way of creating lists is by concatenating pairs.
If we use a concatenation of pairs as a list then the concatenation of 
two list is jus that.
Note that (a+b) as a defining expression is undeterministic, there are n+2 different 
assignments to a and b if the length of the affix is n. For (a+b+c) this would be n+2*(n+1)*(n)*(n-1)
.. = 1/2(n^2).
However Glammar has a special feature to let you traverse such list deterministicly.
On a defining positions (a*b+c) is  deterministic  and hence efficient.
If c again consist of a concatenation of pairs then traversing c is deterministic to.
Thus: 

traverse pair concat list(>empty): ->;
traverse pair concat list(>a*b+r): 
   traverse pair concat list(>r).
     
and the concat of two pair concat lists is 

  pair concat list concat (>l1, >l2, l1+l2>):.
 
The following example shows non-deterministic usage of paired concat lists.

test paired concat list ("Selected "+y+nlcr>):
 select list(>PAIRED LIST, y>).

select list(>x+e*_+y, e>):.

A::"a".
B::"b".
C::"c".
D::"d".
E::"e".
F::"f".
G::"g".
H::"h".

PAIRED LIST::  A*empty+B*empty+C*empty+D*empty+E*empty+F*empty+G*empty+H*empty.

running this results in:

Selected a
Selected b
Selected c
Selected d
Selected e
Selected f
Selected g
Selected h

%

repair (>"cell", >key,>value)= ;
repair (>"cell", >key,>value)= .
% 
repair} updates the earlier created cell (with pair) with new values.
Upon backtracking this is undone unless deterministic behavior is forced.
repair} fails if there is no tuple (key,value) with name cell.
%

#plain
where (>x,x>)=.
%
Because the composition operator is right associative, parenthesis would be needed
to write certain paring directly. 
Glammar does not allow you to use parenthesis in affix expressions.
For example  >a*(b1*b2)*c is not allowed.
You can however make such structures using where.
For example :
   t (>a*b*c):
     where(>b, b1*b2>):.

where(>x, x>):.

After the composition operator is rewritten to the pair/unpair predicates this becomes:
   t (>L_1):
     unpair(>L1, a>, L_2>),
     unpair(>L_2, b>,c>),
     where(>b, L_3>),
     unpair(>L_3, b1>, b2>).

     
It is obvious that my own where has become superfluous if we substitute
b for L_3.
This is the reason why there is a predefined built-in where.
The compiler tries to remove where if it is possible by renaming affixes.
The result after this transformation is:

   t (>L_1):
     unpair(>L1, a>, L_2>),
     unpair(>L_2, b>,c>),
     unpair(>b, b1>, b2>).
%

lexical less (>"x",>"y")= .
lexical more (>"x",>"y")= .
lexical less equal (>"x",>"y")= .
lexical more equal (>"x",>"y")= .
%
These builtin rules compare affixes using their lexical ordering.
%

compute (>expr, result>)= .
%
If  expr} is a string representing a simple expression (like  2*(3+1))
 result} will assigned the value of this expression in decimal ( 8).
The expression may only contain the one-character operators $! < > = + - * /$ and
signed integer notation.
The maximum integer value is inherited from the C value for this.  
Booleans are implemented as 0,X,  where 0 denotes false, anything 
else (preferably 1) is taken to be true. 
Anything not an operator or integral number (spaces included) terminates 
the expression.
A badly formed expression however gives an error message. 
For arbitrary precision arithmetic you can set up a connection to the unix
program dc.
%
realtoint (>number, result>)= .
% 
Converts a decimal real in scientific notation to the nearest decimal integer not greater than real.
%
decimal to unary (>number, result>)= .
% 
Converts a decimal integer value to unary with base notation 1.
%

identifier (x+y>)= !AB..Za..z! (x>), !A..Za..z1..0'_! * (y>).
letter digit sequence (x+y>)= !AB..Za..z! (x>), !A..Za..z1..0! * (y>).
% 
These scan  an identifier as specified but 
slightly more efficiently.
%

upper case (>str, result>)= .
% 
upper case transforms a value with mixed cased letters to a upper cased value.
%

lower case (>str, result>)= .
% 
lower case transforms a value with mixed cased letters to a lower cased value.
%

keyword (>"upper case word")=  "a".
% 
Matches an upper case word to lower or mixed case input.
So keyword(>"IF")+ scans if}, IF},  If, and  iF.
%


white space(w>) = !\n\t\f\r !*(w>). 
layout(l>) = !\n\t\f !*. 
% 
You can use this rule to skip spaces, tabs,  carriage returns, newlines, vertical tabs, and form feeds.
%

type out (>affix)=.
% 
Write the value of affix to stderr.
%

type in (affix>)=.
%
Read affix from stdin.
%

report (>affix)=.
% 
Write the value of affix to the default output.
%

fail = not equal (>empty,>empty).
%
fail just always fails. 
%

#plain
cut= .
%
This predicate forces the next productions --- if any --- to fail. 
This member may also be written as ->, and can be removed
by adding complementary members to all other productions.
%

eval affix (>a,evaluated a>)= .
%
This predicate reduces the evaluation count at 
run time (see -r option) for deterministic grammars.
Nondeterministic grammars overwrite the result of an affix evaluation 
and restore the original value upon backtracking.
Since deterministic grammars do not need to backtrack the original value 
cannot be restored that easily. 
Instead, the result of the affix evaluation is lost which means 
that the same affix most be evaluated over and over again.
If you wish to make your compiler as efficient as possible
and you know that some affixes are tested frequently you can first 
evaluate these affixes and use the results in the rest of the computation.
%

set line file (>line,>file)= ;
set line file (>line,>file)= .
get line file (line>,file>)= ;
get line file (line>,file>)= .
%
Line and file control.
In order to produce correct messages when the input was
the output of a preprocessor there is a predicate which
sets the current file name and line number.
%

get next option (option>)=.
%
This predicate gets the next option (the first one will be the command name) 
from the command line that invoked the program. 
It fails if all option are processed.
%

get env (>"name", val>)=.
%
get env retrieves the value of the environment variable "name". 
It fails if the environment variable is not defined. 
%

error message (val>)=.
%
error message generates on error message based upon the 
most advanced input pointer.
It causes to suppress output generation,
and the exit status will be 1.
It can also be used to implement error recovery.
%
set exit code=.
%
This predicates sets the exit code to 2,
but output generation is not suppressed. 
%

assign (>x,>y,old x>)=;
assign (>x,>y,old x>)=.
%
assign is a very tricky predicate and therefore its use is not recommended.
Basically it makes it possible to avoid adding inherited affix positions if
these affixes change in rare cases only.
If this is the case you can let a meta notion take another value,
although this sometimes give surprising results.
For example in many cases it is necessary to copy the meta notion
(when putting it in tables etc.),
because of the fact that on an affix expression
containing  the  meta notion,
an assignation of that meta notion
has effect on previous evaluations of that affix expression
(consistent substitution is not guaranteed.)
%


base64 encode( out>)=.
base64 add encode(>in)=.
base64 int encode(>size,>int)=.
base64 decode(>in, out>)=.
%
These function implement base64 encoding/decoding
base 64 encode encodes the buffer as fillled with add encode and int decode. The latter size argument 
is the number of bytes added to the encode buffer after converting int to the appropriate type as indicated
by size.
Note that it is not supported to decode something that is binary.
%
get from unix (>cmd, cmd stdout>)=.
write file (>file, >file contents)=.
system(>system)=.
% 
The most simple and reliable way to get some information from unix
is to give a command,  wait for the command to exit and store
the result it produced.
Examples of such unix utility programs are date, cat, ls etc.
None of them need any input.
%

set up connection to unix (>cmd,>channel)=.
talk to unix (>channel,>to the programs stdin,from the programs stdout>)=.
%
These two builtin rules are only available on an unix-system.
They can be used to set up and maintain a connection 
to unix.
Only 32 connections can be made at a time.
When this limit is exceeded setup connection to unix fails.

Useful programs to connect with are for instance dc for
arbitrary precision arithmetic, or perhaps a program 
written by yourself.
The lexicon interface also is implemented with connections.
 
As an example we will maintain a connection to dc.
First we set up the connection.
{\small
\begin{glammar}
\L{\LB{ \Proc{1}\K{s}\index{s1}:}}
\L{\LB{ set up connection with unix \D{}(\>\S{}\"dc \"\SE{},\>DC CON)\DE{}.}}
\end{glammar}}
Here DC CON is a meta defined affix specifying the channel number.
This channel is assigned to the meta rule by the set up builtin;
initially it may be defined as empty (eg : DC CON::.).
 
Next we can give input to this program's stdin and get the answer from
the program's stdout. For example   
{\small
\begin{glammar}
\L{\LB{  \Proc{2}\K{x}\index{x2}:}}
\L{\LB{    talk to unix \D{}(\>DC CON , \>\S{}\"1 2 +ps.\!n\"\SE{},\S{}\"3\"\SE{}\>)\DE{}.}}
\end{glammar}}
will succeed.
Because the builtin  talk to unix first supplies input to the \
unix-program and then waits for the answer communication problems can occur.
To solve this problem a very simple (not very reliable)  strategy is used:
As soon as a control character is read  the answer reading is stopped
and the result so far is  made the total result. Caveat:
the next time the builtin is called there might be some output left from previous
 calls.  Also one has to take care that there always will be an answer. 
For instance in the above example the newline in the input given to 
talk to unix} is essential because dc waits for it  before generating
any output, so both processes would have been locked forever ("deadlock")
if this new line was omitted.
%


% 
The following builtin rules are for internal usage and should therefore not be
used in your own grammar.
It is not possible to redefine these builtin rules and when its name is followed 
by an underscore it is also not possible to use the builtin  yourself.  
%

reset inputptr_ (>saved ip)= .
set inputptr to_ (>x,saved ip>)= .
getlist_ (a>)= .
detprefix (>a, b>, >"m")=.
detprefix2_ (>saved ip, b>, >"m")=.
detnestarset (a>, >b)= "a";
detnestarset (a>, >b)= .
nestarset (a>)= "a";
nestarset (a>)= ;
nestarset (a>)= .
nestaralt_ (a>,>saved ip)= .
metaterminal (>"m")=  "a".
metaterminal2_ (>"m", >saved ip)=  "a".
intersect_ (>a,>b)= "a".
explintersect_ (>a,>b)= "a".
transform lattice_ ( >a,>b,c>)= .
tl traditional_ ( >a,>b,c>)= ;
tl traditional_ ( >a,>b,c>)= ;
tl traditional_ ( >a,>b,c>)= .
transform latticeterm_ ( >a,>b,c>)= .
tl traditionalterm_ ( >a,>b,c>)= ;
tl traditionalterm_ ( >a,>b,c>)= ;
tl traditionalterm_ ( >a,>b,c>)= .
init mint_ ( >a,>b)= .
initmeta ( >a,>b)= .
evalmeta ( >a)= .
get ip_ ( ip>)= .
restore ip_ ( >ip)= .
false ip_ ( >ip)= .
skip_= ;
skip_= ;
skip_= .



%
The following builtin rules implement multiple precision integer
arithmetic.
These are implemented using the unix standard package mp.
Some implementations of unix do however not support 
this package.
More information can be found in the manual page for mp.
%
equal to ::"0".
less than ::"-1".
more than ::"1".
METATAB:: "@!META!@".
%
%
int (>a) plus int (>b) is int (c>)=.
int (>a) sub int (>b) is int (c>)=.
int (>a) times int (>b) is int (c>)=.
int (>a) divided by int (>b) is int (q>) with remainder int (r>)=.
int (>a) is (c>) int (>b)=.
int (>a) raised by int (>b) modulo int (>d) is int (r>)=.
int (c>) is the gcd of int (>a) and int (>b)=.
int (s>) is the square root of int (>a) with remainder int (r>)=.
int (>a) is dec (dec a>)=.
dec (>a) is int (int a>)=.
free int (>d)=.
decrement small int (>d)=.
increment small int (>d)=.
small dec (>d) is int (i>)=.
int (>a) is negative=.
negate int (>a) =.
int (>a) is positive=.
int (>a) is zero=.

