#-d
#{{{1 glammar source  4 alts 
a glammar source ( STUFF>):  #{{{2
    "?", !\n 	!,->;
a glammar source ( header+layout+options+rules+trailor+"\n">): #{{{2
    options (options>),
    plain glammar text (header>, trailor>),
    layout (layout>),
    rules (rules>).

#{{{1
rules ("\Proc{"+line+"}"+rule +rules>):
    line num (line>),
    rule (rule>), ->,
    rules (rules>);
rules (empty>):    .

#{{{1
rule ("\K{"+metaname+"}" +meta rule>):
    affix identifier (metaname>),
    rest meta rule (meta rule>), ->;
rule (hyperrule>):
    hyperrule (hyperrule>).

hyperrule (lhs+alternative+more hyperrules>):
    bold lhs (lhs>, lhs name>),
    alternative (>lhs name, alternative>),
    more hyperrules (>lhs name, more hyperrules>).

more hyperrules (>lhs name, ";"+layout+lhs+alternative+more hyperrules>):
    ";", ->,
    layout (layout>),
    lhs option (lhs>),
    alternative (>lhs name, alternative>),
    more hyperrules (>lhs name, more hyperrules>);
more hyperrules (>lhs name, "."+layout>):
    ".",
    layout (layout>).

lhs option (lhs>):
  lhs (lhs>), ->;
lhs option(empty>):.

lhs (ndm+def+no pb+layout>):
  name display mix+ (ndm>, _>), !:=! (def>),
     no good page break (no pb>), layout (layout>).

name display mix (r>, empty>):
   affix display (r>),->;
name display mix (esc id>, esc id + " ">):
   ?glammar index identifier (index>),
   glammar identifier (id>), [ id ] escape underscore (esc id>).


name display mix* (ndm+ ndm star>, i + ii>):
   name display mix (ndm>, i>) ,
   name display mix* (ndm star>, ii>) , ->;
name display mix* (empty>, empty>):
   .

name display mix+ (ndm+ ndm star>, i+ii>):
   name display mix (ndm>, i>) ,
   name display mix* (ndm star>, ii>).

bold lhs (full index+ndm+def+layout>, index>):
  linenum (line>),
  bold name display mix+ (ndm>, index>), !:=! (def>), layout (layout>),
  where (>"\index{"+index+ " "+line +"}", full index>).

escape underscore (eu>):
       ^!_! *  (id>),
       escape underscore tail (>id, eu>).

escape underscore tail (>id, id + "\_"+x>):
    "_", ^!! * (x>),->;
escape underscore tail (>id, id >):.

glammar index identifier (id>):
  !abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+*\t !+(id>). 

bold name display mix (r>, empty>):
   affix display (r>),->;
#bold name display mix ("\K{"+INDEX+"}\index{"+INDEX+ line +"}">):
bold name display mix ("\K{"+escaped identifier+"}">, index>):
   ?glammar index identifier (index>),
   glammar identifier (id>), 
   [ id ] escape underscore (escaped identifier>).

bold name display mix* (ndm+ ndm star>, l+ll>):
   bold name display mix (ndm>,l>) ,
   bold name display mix* (ndm star>,ll>) , ->;
bold name display mix* (empty>,empty>):
   .

bold name display mix+ (ndm+ ndm star>,l+ll>):
   bold name display mix (ndm>,l>) ,
   bold name display mix* (ndm star>, ll>).

alternative (>lhs name, members>):        members (>lhs name, members>),->;
alternative (>lhs name, empty>):    .

members (>lhs name, redirection + member+more members>):
    redirection (redirection>), member (>lhs name, member>),
    more members (>lhs name, more members>).

more members (>lhs name, ","+no pb+layout+members>):
    ",", no good page break (no pb>),
    layout (layout>),
    members (>lhs name, members>),->;
more members (>lhs name, empty>):    .

redirection("["+ l1 + affix expression + "]" + l2>):
    "[", layout (l1>), 
      affix es( affix expression>), 
     "]" , layout (l2>), ->;
redirection (empty>):.

member (>lhs name, ndm +"\index{"+in+ "!"+lhs name +"}">):
    name display mix+ (ndm>, in>), ->;
member (>_,optional not symbol +set+optional sequence symbol +affix1 display option>):
    optional not symbol (optional not symbol>),
    set (set>),
    optional sequence symbol (optional sequence symbol>),
    affix1 display option (affix1 display option>),->;
member (>_,"|\>"+layout>):
    "->",
    layout (layout>),->;
member (>lhs name, "?"+layout+member>):
    "?", ->,
    layout (layout>),
    member (>lhs name, member>);
member (>_,"\T{}"+quote+string+quote+"\TE{}"+layout>):
    quote,
    any escaped but quote (string>),
    quote,
    layout (layout>).

affix display ("\D{}("+layout+affix es+")\DE{}"+layout2>):
    "(",
    layout (layout>),
    affix es (affix es>),
    ")",
    layout (layout2>).

#affix display option (afx>):
#    affix display (afx>),->;
#affix display option (empty>):    .

affix es (affix +rest affix es>):
    affix (affix>),
    rest affix es (rest affix es>).

rest affix es (","+layout+affix es>):
    ",",
    layout (layout>),
    affix es (affix es>),->;
rest affix es (empty>):    .

affix ("\Flow{}"+layout+terms>):
    ">",
    layout (layout>),
    terms (terms>),->;
affix (terms+flow>):
    terms (terms>),
    flow option (flow>).

flow option ("\Flow{}"+layout>):
    ">",
    layout (layout>),->;
flow option (empty>):.

affix1 display option ("\D{}("+layout+term+"\>"+layout2+")\DE{}"+layout3>):
    "(",
    layout (layout>),
    term (term>),
    ">",
    layout (layout2>),
    ")",
    layout (layout3>),->;
affix1 display option (empty>):    .

rest metarule ("::"+layout+terms+"."+layout2>):
    "::",
    layout (layout>),
    terms (terms>),
    ".",
    layout (layout2>),->;
rest metarule ("::"+layout+"."+layout2>):
    "::",
    layout (layout>),
    ".",
    layout (layout2>),->;
rest metarule ("::"+layout+number+"."+layout2>):
    "::",
    layout (layout>),
    !-+0123456789! + (number>),  layout (_>),
    ".",
    layout (layout2>),->;
rest metarule ("::"+layout+"@@"+layout2+glammar identifier +
                   affix1 display option +"."+layout3>):
    "::",
    layout (layout>),
    "@",
    layout (layout2>),
    glammar identifier (glammar identifier>),
    affix1 display option (affix1 display option>),
    ".",
    layout (layout3>),->;
rest metarule ("::"+layout+"\{\}"+layout2+affix identifier list+ "."+layout3>):
    "::",
    layout (layout>),
    "{}",
    layout (layout2>),
    affix identifier list (affix identifier list>),
    ".",
    layout (layout3>).

terms (term+rest terms>):
    term (term>),
    rest terms (rest terms>).

rest terms ("\Pl{}"+layout+terms>):
    "+",
    layout (layout>),
    terms (terms>),->;
rest terms ("\Ti{}"+layout+terms>):
    "*",
    layout (layout>),
    terms (terms>),->;
rest terms (empty>):    .

term (affix identifier>):  affix identifier (affix identifier>),->;
term ("\_"+layout>):  "_", layout (layout>),->;
term ("\S{}\\"+quote+string+"\\"+quote+"\SE{}"+layout>):
    quote ,
    any escaped but quote (string>),
    quote ,
    layout (layout>).

optional not symbol ("\^"+layout>):
    "^",
    layout (layout>),->;
optional not symbol (empty>):    .

optional sequence symbol ("+"+layout>):
    "+",
    layout (layout>),->;
optional sequence symbol ("*"+layout>):
    "*",
    layout (layout>),->;
optional sequence symbol (empty>):    .

affix identifier list (affix identifier +";"+layout
                           +affix identifier list>):
   affix identifier (affix identifier>),
   ";", layout (layout>),
   affix identifier list (affix identifier list>),->;
affix identifier list (affix identifier>):
   affix identifier (affix identifier>).

affix identifier (identifier + layout + rest identifier>):
    identifier (identifier>),
    layout (layout>),
    rest affix identifier (rest identifier>).

rest affix identifier (numbers+rest>):
    !0123456789\n\t ! + (numbers>),
    rest identifier (rest>),->;
rest affix identifier (affix identifier>):
   affix identifier (affix identifier>), ->;
rest affix identifier (empty>):    .


glammar identifier (identifier + r+layout + rest identifier>):
    identifier (identifier>), ! *+?! *(r>),
    layout (layout>),
    rest identifier (rest identifier>).

rest identifier (numbers+rest>):
    !0123456789\n\t ! + (numbers>),
    rest identifier (rest>),->;
rest identifier (glammar identifier>):
   glammar identifier (glammar identifier>), ->;
rest identifier (empty>):    .

set ("!\T{}"+set1+"\TE{}!"+layout>):
    "!",
    any escaped but exclamation sign (set1>),
    "!",
    layout (layout>).

layout (x+rest layout>):
     ! ! * (x>),
    rest layout (rest layout>).

rest layout ("}}\n\L{\LB{"+l+layout>):
    "\n",->,line number (l>), layout (layout>),->;
rest layout ("}\Tab{"+n+"}{"+layout>):
    !\t!+ (t>), charnum (n>),->,layout (layout>),->;
rest layout ("\C{}\#"+x+"\CE{}"+layout>):
    "#", any escaped but new line (x>),
    rest layout (layout>),->;
rest layout ("}}\n\end{glammar}}\n"+x+"{\small\n\\begin{glammar}\n\L{\LB{"+l+layout>):
    "%TEX", ^!%!*(x>),
    "%",
    line number (l>),
    layout (layout>),->;
rest layout (x+"{\small\n\\begin{glammar}\n\L{\LB{"+layout>):
    "%^TEX", ^!%!*(x>),
    "%",
    line number (l>),
    layout (layout>),->;
rest layout ("}}\n\end{glammar}}"+x>):
    "%$TEX", ^!%!*(x>),
    "%",
    layout (layout>),->;
rest layout ("\C{}\%"+x+"\%\CE{}"+layout>):
    "%", any escaped but percent sign (x>),
    "%",
    layout (layout>),->;
rest layout (empty>):    .

any escaped but percent sign (any+rest>):
   ^!%|#"<>_$^~-/'&\{}\n! *(any>), more any escaped but percent sign (rest>).

more any escaped but percent sign (empty>): ?"%",->;
more any escaped but percent sign ("}}\n\L{\LB{"+l+rest>):
   "\n" ,  line number (l>),any escaped but percent sign (rest>),->;
    
more any escaped but percent sign ("\!"+rest>):
   "\\" ,  any escaped but percent sign (rest>),->;
more any escaped but percent sign ("\\"+any+rest>):
   !#"|<>_$~-'^&/{}! (any>),  any escaped but percent sign (rest>).

any escaped but quote (any+rest>):
   ^!%|#"<>~-'/_$^&{\}! *(any>), more any escaped but quote (rest>).

more any escaped but quote ("\!\""+rest>):
     "\\\"", any escaped but quote (rest>),->;
more any escaped but quote ("\!\!"+rest>):
   "\\\\" ,  any escaped but quote (rest>),->;
more any escaped but quote ("\!"+rest>):
   "\\" ,  any escaped but quote (rest>),->;
more any escaped but quote (empty>): ?"\"",->;
more any escaped but quote ("\\"+any+rest>):
   !%|#<>_~-'/$^&{}! (any>),  any escaped but quote (rest>).

any escaped but exclamation sign (any+rest>):
   ^!%#|\!<>~-'/"_$^&{\}! *(any>), more any escaped but exclamation sign (rest>).

more any escaped but exclamation sign ("\!!"+rest>):
     "\!", any escaped but exclamation sign (rest>),->;
more any escaped but exclamation sign ("\!"+rest>):
   "\\" ,  any escaped but exclamation sign (rest>),->;
more any escaped but exclamation sign (empty>): ?"!",->;
more any escaped but exclamation sign ("\\"+any+rest>):
   !%#|<>"~-'/_$^&{}! (any>),  any escaped but exclamation sign (rest>).

any escaped but new line (any+rest>):
   ^!%|<>#"\!~-'/_$^&{\}\n! *(any>), more any escaped but new line (rest>).

more any escaped but new line (empty>): ? "\n",->;
more any escaped but new line ("\!"+rest>):
   "\\" ,  any escaped but new line (rest>),->;
more any escaped but new line ("\\"+any+rest>):
   !%#|~-'/<>"\!_$^&{}! (any>),  any escaped but new line (rest>).

no good page break ("}}\n\\nopagebreak\n\L{\LB{"+l>):
    "\n",line number (l>),->;
no good page break (empty>):.

options (o+oo>):
   option (o>),
   options (oo>),->;
options (empty>):.  

option ("\C{}\#"+x+"\CE{}}}\n\L{\LB{"+l>): 
         "#", any escaped but new line (x>), nlcr,line number (l>).

plain glammar text( "{\small\n\\begin{glammar}\n\L{\LB{">, "}}\n\end{glammar}}\n">):
    try to find  option plain, ->;
plain glammar text (empty>, empty>):.

try to find  option plain: 
    get next option (opt>), more try find (>opt).

more try find (>"-plain"): ->;
more try find (>opt):
    try to find  option plain.
   
line number("\Ln{"+l+s+"}">):
   get line file (l>, file>), length(>l,len>), 
   compute (>"9-"+len,r>), 
   decimal to unary (>r, ur>), 
   space out (>ur, s>).

space out (>"1"+x, " "+s>): space out (>x, s>), ->;
space out (>empty, empty>):.

STUFF:: "%^TEX\n\
\documentstyle[glammar]{article}\n\
\\topmargin      0 mm\n\
\headheight     0 mm\n\
\headsep        0 mm\n\
\\textheight     240 mm\n\
\\footskip       7 mm\n\
\\footheight     11 mm\n\
\n\
\oddsidemargin  0 mm\n\
\evensidemargin 0 mm\n\
\\textwidth      159.2 mm\n\
\\author{Eric Voss}\n\
\\title{glammar to \LaTeX transducer}\n\
\makeindex\n\
\\begin{document}\n\
\maketitle\n\
%\n\
\n\
%$TEX\n\
\input{\jobname.ind}\n\
\end{document}\n\
%\n".

%$TEX
\input{\jobname.ind}
\end{document}
%

