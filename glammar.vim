" Vim syntax file
" Language:	GLAMMAR 
" Maintainer:	Eriv Voss
" Last change:	1997 April 20

" Remove any old syntax stuff hanging around
syn clear


" The SQL reserved words, defined as keywords.


" Strings and characters:
syn region glammarString	start=+{+hs=s+1  skip=+\\}+  end=+}+he=e-1
syn region glammarString	start=+!+hs=s+1  skip=+\\!+  end=+!+he=e-1
syn region glammarString	start=+"+hs=s+1  skip=+\\.+  end=+"+he=e-1
syn match  glammarD             "\([*+ ]\|\i\)\+>\()\|,\)"he=e-1
syn match  glammarI             ">[*+A-Za-z0-9 ]\+"hs=s+1
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
  hi link glammarType	Type
  hi link glammarI	Special
  hi link glammarD	Macro
"endif

let b:current_syntax = "glammar"
set tags=glammar.vim

" vim: ts=8
