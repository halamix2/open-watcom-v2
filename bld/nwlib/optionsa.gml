:cmt.*****************************************************************************
:cmt.*
:cmt.*                            Open Watcom Project
:cmt.*
:cmt.* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
:cmt.*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
:cmt.*
:cmt.*  ========================================================================
:cmt.*
:cmt.*    This file contains Original Code and/or Modifications of Original
:cmt.*    Code as defined in and that are subject to the Sybase Open Watcom
:cmt.*    Public License version 1.0 (the 'License'). You may not use this file
:cmt.*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
:cmt.*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
:cmt.*    provided with the Original Code and Modifications, and is also
:cmt.*    available at www.sybase.com/developer/opensource.
:cmt.*
:cmt.*    The Original Code and all software distributed under the License are
:cmt.*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
:cmt.*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
:cmt.*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
:cmt.*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
:cmt.*    NON-INFRINGEMENT. Please see the License for the specific language
:cmt.*    governing rights and limitations under the License.
:cmt.*
:cmt.*  ========================================================================
:cmt.*
:cmt.* Description:  librarian ar command line options.
:cmt.*
:cmt.*****************************************************************************
:cmt.
:cmt. Source file uses UTF-8 encoding, ¥
:cmt.
:cmt. Definition of command line options to use by optencod utility to generate
:cmt.  	appropriate command line parser and usage text.
:cmt.
:cmt.
:cmt. GML Macros used:
:cmt.
:cmt.	:chain. <option> <option> ...               options that start with <option>
:cmt.                                                   can be chained together i.e.,
:cmt.                                                   -oa -ox -ot => -oaxt
:cmt.   :target. <targ1> <targ2> ...                valid for these targets (default is 'any')
:cmt.   :ntarget. <targ1> <targ2> ...               not valid for these targets
:cmt.   :usagechain. <option> <usage text>          group of options that start with <option>
:cmt.                                                   are chained together in usage text
:cmt.   :usagegroup. <num> <usage text>             group of options that have group <num>
:cmt.                                                   are chained together in usage text
:cmt.   :title. <text>                              English title usage text
:cmt.   :jtitle. <text>                             Japanese title usage text
:cmt.   :titleu. <text>                             English title usage text for QNX resource file
:cmt.   :jtitleu. <text>                            Japanese title usage text for QNX resource file
:cmt.
:cmt.   :option. <option> <synonym> ...             define an option
:cmt.   :immediate. <fn> [<usage argid>]            <fn> is called when option parsed
:cmt.   :code. <source-code>                        <source-code> is executed when option parsed
:cmt.   :enumerate. <name> [<option>]               option is one value in <name> enumeration
:cmt.   :number. [<fn>] [<default>] [<usage argid>] =<num> allowed; call <fn> to check
:cmt.   :id. [<fn>] [<usage argid>]                 =<id> req'd; call <fn> to check
:cmt.   :char. [<fn>] [<usage argid>]               =<char> req'd; call <fn> to check
:cmt.   :file. [<usage argid>]                      =<file> req'd
:cmt.   :path. [<usage argid>]                      =<path> req'd
:cmt.   :special. <fn> [<usage argid>]              call <fn> to parse option
:cmt.   :usage. <text>                              English usage text
:cmt.   :jusage. <text>                             Japanese usage text
:cmt.
:cmt.   :optional.                                  value is optional
:cmt.   :internal.                                  option is undocumented
:cmt.   :prefix.                                    prefix of a :special. option
:cmt.   :nochain.                                   option isn't chained with other options
:cmt.                                                   in parser code
:cmt.   :usagenochain.                              option isn't chained with other options
:cmt.                                                   in usage text
:cmt.   :timestamp.                                 kludge to record "when" an option
:cmt.                                                   is set so that dependencies
:cmt.                                                   between options can be simulated
:cmt.   :negate.                                    negate option value
:cmt.   :group. <num> [<chain>]                     group <num> to which option is included
:cmt.                                                   optionaly <chain> can be specified
:cmt.
:cmt. Global macros
:cmt.
:cmt.   :noequal.                                   args can't have option '='
:cmt.   :argequal. <char>                           args use <char> instead of '='
:cmt.
:cmt. where <targ>:
:cmt.   default - any, dbg, unused
:cmt.   architecture - i86, 386, x64, axp, ppc, mps, sparc
:cmt.   host OS - bsd, dos, linux, nov, nt, os2, osx, pls, qnx, rsi, haiku, rdos, win
:cmt.   extra - targ1, targ2
:cmt.
:cmt. The :jtitle. or :jusage. tag is required if no text is associated with the tag.
:cmt. Otherwise, English text defined with :title. or :use. tag will be used instead.
:cmt.

:cmt. dmpqrtx options must use '-' option character (latest POSIX standard)
:cmt. for backward compatibility they can be used without '-' option character
:cmt. it is control by targ1 target, if targ1 is defined then it uses full set
:cmt. of options with '-' option character otherwise only dmpqrtx options are selected 
:cmt. to use it without '-' option character

:title.  Usage: %s <options> <archive> [modules]
:jtitle. 使用法： %s <options> <archive> [modules]
:titleu.  Usage: %C <options> <archive> [modules]
:jtitleu. 使用法： %C <options> <archive> [modules]

:title.

:title.  Options:
:jtitle. オプション：

:option. ? h
:target. targ1
:usage.  .     display this screen
:jusage. .     この画面を表示します

:option. c
:target. targ1
:usage.  .     suppress create archive message
:jusage. .     アーカイブ作成メッセージを抑制する

:option. d
:usage.  .     delete modules from archive
:jusage. .     アーカイブからモジュールを削除する

:option. m
:target. unused
:internal.
:usage.  option m

:option. p
:target. unused
:internal.
:usage.  option p

:option. q
:target. unused
:internal.
:usage.  option q

:option. r
:usage.  .     insert or replace modules
:jusage. .     モジュールの挿入または交換

:option. t
:usage.  .     display modules or content
:jusage. .     モジュールまたはコンテンツを表示する

:option. u
:target. targ1
:usage.  .     update (only with -r command)
:jusage. .     更新（-rコマンドのみ）

:option. v
:target. targ1
:usage.  .     verbose output
:jusage. .     詳細な出力

:option. x
:usage.  .     extract modules
:jusage. .     モジュールを抽出する
