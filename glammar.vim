" Vim syntax file
" Language:	GLAMMAR 
" Maintainer:	Eriv Voss
" Last change:	1997 April 20

" Remove any old syntax stuff hanging around
syn clear


" The SQL reserved words, defined as keywords.


" Strings and characters:
syn region glammarSet	start=+{+hs=s+1  skip=+\\}+  end=+}+he=e-1
syn region glammarSet	start=+!+hs=s+1  skip=+\\!+  end=+!+he=e-1
syn region glammarString	start=+"+hs=s+1  skip=+\\.+  end=+"+he=e-1
syn region glammarDisplay start=+(+hs=s+1 end=+)+he=e-1 contains=glammarI,glammarString
"syn region glammarDisplay start=+(+hs=s+1 end=+)+ contains=glammarI,glammarD
"syn region glammarDisplay start=+(+hs=s+1 end=+)+ contains=glammarI,glammarD
"syn region glammarI	start=+>\s*\("\|\i\)+hs=s+1   end=+,\|)+he=e-1 contained contains=glammarString
"syn region glammarD	start=+\s*\(\i\|"\)+   end=+>+he=e-1 contained contains=glammarString
"syn match  glammarD             "\([*+ ]\|\i\)\+>\()\|,\)"he=e-1
syn match  glammarI             ">[*+A-Za-z0-9 ]\+"
syn region glammarI		start=+\[+hs=s+1   end=+]+he=e-1


" Numbers:
syn match glammarNumber		"-\=\<[0-9]*\.\=[0-9_]\>"

" Comments:
syn region glammarComment    start="^%"hs=e+1  end="%"he=s-1
syn match glammarComment 	"#.*"hs=s+1

syn sync ccomment glammarComment

"if !exists("did_glammar_syntax_inits")
" let did_glammar_syntax_inits = 1
  " The default methods for highlighting. Can be overridden later.
  hi link glammarComment	Comment
  hi link glammarKeyword	glammarSpecial
  hi link glammarNumber	Number
  hi link glammarOperator	glammarStatement
  hi link glammarSpecial	Special
  hi link glammarStatement	Statement
  hi link glammarString	String
  hi link glammarSet	String
  hi link glammarDisplay	Type
  hi link glammarI	Macro
  hi link glammarD	Macro
"endif

let b:current_syntax = "glammar"
set tags=glammar.vim

" vim: ts=8
