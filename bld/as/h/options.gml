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
:cmt.* Description:  RISC assembler command line options.
:cmt.*
:cmt.*     UTF-8 encoding, ¥
:cmt.*
:cmt.*****************************************************************************
:cmt.
:cmt.
:cmt. GML Macros used:
:cmt.
:cmt.	:chain. <option> <option> ...               options that start with <option>
:cmt.						    	can be chained together i.e.,
:cmt.						    	-oa -ox -ot => -oaxt
:cmt.	:target. <targ1> <targ2> ...                valid for these targets
:cmt.	:ntarget. <targ1> <targ2> ...               not valid for these targets
:cmt.	:usagechain. <option> <usage text>          group of options that start with <option>
:cmt.                                                	are chained together in usage
:cmt.	:usagegroup. <num> <usage text>             group of options that have group <num>
:cmt.                                                	are chained together in usage
:cmt.	:title. <text>                              English title usage text
:cmt.	:jtitle. <text>                             Japanese title usage text
:cmt.	:titleu. <text>                             English title usage text for QNX resource file
:cmt.	:jtitleu. <text>                            Japanese title usage text for QNX resource file
:cmt.
:cmt.	:option. <option> <synonym> ...             define an option
:cmt.	:immediate. <fn> [<usage argid>]            <fn> is called when option parsed
:cmt.	:code. <source-code>                        <source-code> is executed when option parsed
:cmt.	:enumerate. <name> [<option>]               option is one value in <name> enumeration
:cmt.	:number. [<fn>] [<default>] [<usage argid>] =<num> allowed; call <fn> to check
:cmt.	:id. [<fn>] [<usage argid>]		    =<id> req'd; call <fn> to check
:cmt.	:char. [<fn>] [<usage argid>]		    =<char> req'd; call <fn> to check
:cmt.	:file. [<usage argid>]			    =<file> req'd
:cmt.	:path. [<usage argid>]			    =<path> req'd
:cmt.	:special. <fn> [<usage argid>]		    call <fn> to parse option
:cmt.	:usage. <text>                              English usage text
:cmt.	:jusage. <text>                             Japanese usage text
:cmt.
:cmt.	:optional.                                  value is optional
:cmt.	:internal.                                  option is undocumented
:cmt.	:prefix.                                    prefix of a :special. option
:cmt.	:nochain.                                   option isn't chained with other options
:cmt.	:timestamp.                                 kludge to record "when" an option
:cmt.                                                	is set so that dependencies
:cmt.                                                	between options can be simulated
:cmt.	:negate.                                    negate option value
:cmt.	:group. <num>                               group <num> to which option is included
:cmt.
:cmt. Global macros
:cmt.
:cmt.	:noequal.                                   args can't have option '='
:cmt.	:argequal. <char>                           args use <char> instead of '='
:cmt.
:cmt. where <targ>:
:cmt.		    default - any, dbg
:cmt.		    architecture - i86, 386, x64, axp, ppc, mps, sparc
:cmt.		    host OS - bsd, dos, linux, nt, os2, osx, qnx, haiku, rdos, win
:cmt.		    extra - targ1, targ2
:cmt.
:cmt.	Translations are required for the :jtitle. and :jusage. tags
:cmt.	if there is no text associated with the tag.


:chain. o v

:title. Usage: wasaxp {options} {asm_files}
:jtitle.
:target. axp

:title. Usage: wasppc {options} {asm_files}
:jtitle.
:target. ppc

:title. Usage: wasmips {options} {asm_files}
:jtitle.
:target. mps

:title. Options:
:jtitle.
:target. any
:title.  .         ( /option is also accepted )
:jtitle. .         ( /optionも使用できます )
:target. any
:ntarget. bsd linux osx qnx haiku

:option. d
:target. any
:nochain.
:special. scanDefine <name>[=text]
:usage. define text macro
:jusage.

:option. e
:target. any
:number.
:usage. set error limit number
:jusage.

:option. fo
:target. any
:file.
:usage. set output filename (applies to the first asm_file)
:jusage.

:option. h ?
:target. any
:nochain.
:usage. print this message
:jusage.

:option. i
:target. any
:path.
:usage. set include path
:jusage.

:usagechain. o Output object file format
:jusage.

:option. oc
:target. any
:path.
:usage. COFF object file format
:jusage.

:option. oe
:target. any
:path.
:usage. ELF object file format
:jusage.

:option. q zq
:target. any
:usage. operate quietly
:jusage.

:option. we
:target. any
:usage. treat all warnings as errors
:jusage.

:usagechain. v Debug verbose output
:jusage.

:option. vi
:target. any
:internal.
:usage. view instruction
:jusage.

:option. vl
:target. any
:internal.
:usage. view lex buffer
:jusage.

:option. vp
:target. any
:internal.
:usage. view parse
:jusage.

:option. vs
:target. any
:internal.
:usage. view symbols
:jusage.

:option. vt
:target. any
:internal.
:usage. view ins table
:jusage.
