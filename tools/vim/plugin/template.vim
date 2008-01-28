" Name:		ni-template
" Author:	Nicolas Schodet <nico at ni.fr.eu.org>
" Last Update:	2007-03-29
" Version:	0.84
" Description:	Chargeur de templates.
" License:	Domaine public.
" Installation:	Mettre le script dans &runtimepath/plugin, et les templates
" 		dans &runtimepath/templates.
"
"		Appeler la commande :Template [filetype] pour charger un
"		template. filetype est optionnel, il est detecté sinon.
"
"		La commande :TemplateHeader essaye de détecter si le fichier
"		est un en-tête (h au lieu de c, hpp au lieu de cpp).
"
"		Exemple : Mettre le script dans ~/.vim/plugin, et les
"		templates dans ~/.vim/templates. Mettre une autocommande :
"		au BufNewFile * TemplateHeader
"
" Usage:	Les fichiers de templates peuvent contenir des expressions
"		remplacées par le script :
"		 @DATE@ : Tue Jan 28 00:42:47 2003
"		 @ISODATE@ : 2003-01-28
"		 @YEAR@ : 2003
"		 @FILE@ : template.vim
"		 @FILEBASE@ : template
"		 @FILECAP@ : TEMPLATE_VIM
"		 @FILEDEF@ : template_vim
"		 @PATH@ : path/to/template.vim
"		 @PATHBASE@ : path/to/template
"		 @PATHCAP@ : PATH_TO_TEMPLATE_VIM
"		 @PATHDEF@ : path_to_template_vim
"		 @AUTHOR@ : valeur de g:fullname
"		 @EMAIL@ : valeur de g:email
"		 @NICK@ : valeur de g:nick
"		 @WEB@ : valeur de g:web
"		 @COPYRIGHT@ : valeur de g:copyright
"		 @%...@ : utilise expand pour construire un nom de fichier
"		 @!...@ : evalue une expression VIM
"		 @=...@ : evalue une expression VIM et remplace par le
"		 	  résultat.

if exists('g:ni_template_plugin') | finish | endif
let g:ni_template_plugin = 1

function! s:Eval(expr)
   exe 'return' a:expr
endf

function! s:Exec(expr)
   exe a:expr
   return ''
endf

" Find a template file for a given filetype.
function! s:Template_find_file(ft)
   if exists ('b:template_variant')
      let template_file = globpath (&runtimepath, 'templates/' . b:template_variant . '.' . a:ft)
      if filereadable (template_file)
	 return template_file
      endif
   endif
   if exists ('g:template_variant')
      let template_file = globpath (&runtimepath, 'templates/' . g:template_variant . '.' . a:ft)
      if filereadable (template_file)
	 return template_file
      endif
   endif
   let template_file = globpath (&runtimepath, 'templates/default.' . a:ft)
   return template_file
endf

" Insert template.
function! s:Template(...)
   let dellast = line ('$') == 1 && getline ('$') == ''
   let ft = (a:0) ? (a:1) : (strlen (&ft) ? &ft : 'default')
   let template_file = s:Template_find_file (ft)
   if filereadable (template_file)
      silent exe '0r ' . template_file
      keepjumps silent %s/@=\([^@]*\)@/\=s:Eval(submatch(1))/ge
      keepjumps silent %s/@!\([^@]*\)@/\=s:Exec(submatch(1))/ge
      keepjumps silent %s/@\(%[^@]*\)@/\=expand(submatch(1))/ge
      silent exe 'keepjumps %s/@DATE@/' . strftime ('%c') . '/ge'
      silent exe 'keepjumps %s/@ISODATE@/' . strftime ('%Y-%m-%d') . '/ge'
      silent exe 'keepjumps %s/@YEAR@/' . strftime ('%Y') . '/ge'
      let file = expand ('%:t')
      silent exe 'keepjumps %s/@FILE@/' . file . '/ge'
      silent exe 'keepjumps %s/@FILEBASE@/' . expand ('%:t:r') . '/ge'
      silent exe 'keepjumps %s/@FILECAP@/' . toupper (substitute (file, '\.', '_', 'g')) . '/ge'
      silent exe 'keepjumps %s/@FILEDEF@/' . substitute (file, '\.', '_', 'g') . '/ge'
      let path = expand ('%')
      silent exe 'keepjumps %s:@PATH@:' . path . ':ge'
      silent exe 'keepjumps %s:@PATHBASE@:' . expand ('%:r') . ':ge'
      silent exe 'keepjumps %s:@PATHCAP@:' . toupper (substitute (path, '[/\.]', '_', 'g')) . ':ge'
      silent exe 'keepjumps %s:@PATHDEF@:' . substitute (path, '[/\.]', '_', 'g') . ':ge'
      if exists ('g:fullname')
	 silent exe 'keepjumps %s/@AUTHOR@/' . g:fullname . '/ge'
      endif
      if exists ('g:email')
	 silent exe 'keepjumps %s/@EMAIL@/' . g:email . '/ge'
      endif
      if exists ('g:nick')
	 silent exe 'keepjumps %s/@NICK@/' . g:nick . '/ge'
      endif
      if exists ('g:web')
	 silent exe 'keepjumps %s/@WEB@/' . g:web . '/ge'
      endif
      if exists ('g:copyright')
	 silent exe 'keepjumps %s/@COPYRIGHT@/' . g:copyright . '/ge'
      endif
      if dellast
	 keepjumps $d _
      endif
      keepjumps 0
   endif
endf

" Detect headers files, then call Template().
function! s:TemplateHeader()
   if &filetype == 'c' && expand ('%') =~ '\.h$'
      call s:Template ('h')
   elseif &filetype == 'cpp' && expand ('%') =~ '\.\(h\|H\|hh\|hxx\|hpp\|tcc\|inl\)$'
      call s:Template ('hpp')
   else
      call s:Template ()
   endif
endfunction

if !exists(':Template')
   command -nargs=? Template :call s:Template (<f-args>)
endif

if !exists(':TemplateHeader')
   command -nargs=0 TemplateHeader :call s:TemplateHeader ()
endif

