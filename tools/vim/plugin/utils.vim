" Name:		ni-utils
" Author:	Nicolas Schodet <nico at ni.fr.eu.org>
" Last Update:	2008-01-28
" Description:	Misc utils.
" License:	Public domain.
" Installation:	Put it in &runtimepath/plugin.

if exists('g:ni_utils_plugin') | finish | endif
let g:ni_utils_plugin = 1

" Look up README file in '.' and '..' to find a program description.
function! GetReadme()
   if filereadable ('README')
      let readme = 'README'
   elseif filereadable ('../README')
      let readme = '../README'
   endif
   if exists ('readme')
      let progname = substitute (system ('head -1 ' . readme), "\n", '', '')
   else
      let progname = ''
   endif
   return progname
endfunction

