/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Target dependent code generation switches for x86.
*
****************************************************************************/


I_MATH_INLINE           = 0x00000001L,     /* Inline math functions */
EZ_OMF                  = 0x00000002L,     /* Generate EZ-OMF objects */
BIG_DATA                = 0x00000004L,     /* Data pointers are far */
BIG_CODE                = 0x00000008L,     /* Code pointers are far */
CHEAP_POINTER           = 0x00000010L,     /* Model isn't huge */
FLAT_MODEL              = 0x00000020L,     /* Flat memory model */
FLOATING_FS             = 0x00000040L,     /* FS selector is floating */
FLOATING_GS             = 0x00000080L,     /* GS selector is floating */
FLOATING_ES             = 0x00000100L,     /* ES selector is floating */
FLOATING_SS             = 0x00000200L,     /* SS selector is floating */
FLOATING_DS             = 0x00000400L,     /* DS selector is floating */
USE_32                  = 0x00000800L,     /* Generate 32-bit segments */
INDEXED_GLOBALS         = 0x00001000L,     /* Position Independent Code (faulty!) */
WINDOWS                 = 0x00002000L,     /* Generate Win16 prologs */
CHEAP_WINDOWS           = 0x00004000L,     /* Cheap Win16 prologs */
NO_CALL_RET_TRANSFORM   = 0x00008000L,     /* Don't turn calls into jumps */
CONST_IN_CODE           = 0x00010000L,     /* FP consts in code segment */
NEED_STACK_FRAME        = 0x00020000L,     /* Always generate stack frame */
LOAD_DS_DIRECTLY        = 0x00040000L,     /* No runtime call to load DS */
SMART_WINDOWS           = 0x00100000L,     /* Smart Win16 prolog (DS==SS) */
P5_PROFILING            = 0x00200000L,     /* Pentium RDTSC profiling (-et) */
P5_DIVIDE_CHECK         = 0x00400000L,     /* Check for bad Pentium FDIV */
GENERIC_TLS             = 0x00800000L,     /* TLS code not NT specific (unused?) */
NEW_P5_PROFILING        = 0x01000000L,     /* "New" profiling (-etp) */
STATEMENT_COUNTING      = 0x02000000L,     /* Statement counting (-esp) */
NULL_SELECTOR_BAD       = 0x04000000L,     /* Avoid null selectors on i86 */
P5_PROFILING_CTR0       = 0x08000000L,     /* Use RDPMC instead of RDTSC */
GEN_FWAIT_386           = 0x10000000L,     /* Generate FWAITs on 386 and up */

