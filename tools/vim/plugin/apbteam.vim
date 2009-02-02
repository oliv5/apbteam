" Name:		apbteam
" Author:	Nicolas Schodet <nico at ni.fr.eu.org> 
" Last Update:	2008-01-28
" Description:	Vim setup for APBTeam.
" License:	Public domain.
" Installation:	Put it in &runtimepath/plugin.

if exists('g:apbteam_plugin') | finish | endif
let g:apbteam_plugin = 1

" APBTeam setup.
function! APBTeam()
   set nocompatible
   set backspace=2
   " Formatting.
   set autoindent
   set textwidth=78
   set formatoptions=tcqnl
   set nocindent
   set smartindent
   set smarttab
   set nojoinspaces
   " Quickfix.
   set errorformat=
	    \%*[^\"]\"%f\"%*\\D%l:\ %m,
	    \\"%f\"%*\\D%l:\ %m,
	    \%-G%f:%l:\ error:\ (Each\ undeclared\ identifier%.%#,
	    \%-G%f:%l:\ error:\ for\ each\ function\ it\ appears\ in.),
	    \%f:%l:%m,
	    \\"%f\"\\,
	    \\ line\ %l%*\\D%c%*[^\ ]\ %m,
	    \%D%*\\a:\ Entering\ directory\ `%f',
	    \%X%*\\a:\ Leaving\ directory\ `%f',
	    \%DMaking\ %*\\a\ in\ %f
   " .h are for C.
   let g:c_syntax_for_h = 1
   " Format options.
   set cinoptions={.5s:.5sg.5sh.5st0(0=.5s
   " Syntax highlighting & file types.
   filetype plugin indent on
   syn on
   " Plugins.
   let g:template_variant = "apbteam"
   let g:ghph_GrabComments = 1
   let g:ghph_Reformat = 1
   let g:ghph_PutAfter = 1
   let g:ghph_SplitReturn = 1
endfunction

function! APBTeamProg()
   " Programming options.
   setlocal formatoptions+=rt
   setlocal shortmess-=T
   setlocal shiftwidth=4
   setlocal cindent
   setlocal fo-=o fo-=r
   setlocal com-=:// com+=:///,://
   " Call GHPH
   nmap <buffer> <Leader>g :GHPH g<CR>
   nmap <buffer> <Leader>h :GHPH p<CR>
endfunction

function! APBTeamPython()
   " Programming options.
   setlocal tabstop=4
   setlocal shiftwidth=4
   setlocal smarttab
   setlocal expandtab
   setlocal softtabstop=4
endfunction


" If you do not want automatic execution.
if exists('g:no_apbteam') | finish | endif

call APBTeam()

au FileType c call APBTeamProg()
au FileType cpp call APBTeamProg()
au FileType python call APBTeamPython()
au BufNewFile README Template README
au BufNewFile *.c,*.cc,*.tcc,*.icc,*.h,*.hh,*.hpp,*.C,*.cxx TemplateHeader
