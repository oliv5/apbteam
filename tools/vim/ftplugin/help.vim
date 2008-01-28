" Vim filetype plugin file
" Language:	help files
" Maintainer:	Nicolas Schodet <nico at ni.fr.eu.org>
" Last Change:	Monday 27 January 2003

" Only do this when not done yet for this buffer
if exists("b:did_ftplugin")
  finish
endif

" Don't load another plugin for this buffer
let b:did_ftplugin = 1

nnoremap <buffer> <CR> <C-]>
nnoremap <buffer> <BS> <C-T>
nnoremap <buffer> + /\(['\|]\)[^'\|]\{2,\}\1/b+<CR>
nnoremap <buffer> - ?\(['\|]\)[^'\|]\{2,\}\1?b+<CR>
