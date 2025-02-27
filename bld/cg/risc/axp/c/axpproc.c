/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Alpha AXP procedure prolog/epilog generation.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "zoiks.h"
#include "data.h"
#include "rtrtn.h"
#include "objout.h"
#include "dbsyms.h"
#include "rscconst.h"
#include "object.h"
#include "targetin.h"
#include "targetdb.h"
#include "opttell.h"
#include "encode.h"
#include "rgtbl.h"
#include "rscobj.h"
#include "utils.h"
#include "axpenc.h"
#include "feprotos.h"


#define STORE_QUADWORD  0x2d
#define STORE_DOUBLE    0x27
#define LOAD_QUADWORD   0x29
#define LOAD_DOUBLE     0x23
#define LEA_OPCODE      0x08
#define LEAH_OPCODE     0x09


static  void    calcUsedRegs( void )
/**********************************/
{
    block       *blk;
    instruction *ins;
    name        *result;
    hw_reg_set  used;

    HW_CAsgn( used, HW_EMPTY );
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( _IsBlkAttr( blk, BLK_CALL_LABEL ) ) {
            HW_TurnOn( used, ReturnAddrReg() );
        }
        for( ins = blk->ins.head.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            result = ins->result;
            if( result != NULL && result->n.class == N_REGISTER ) {
                HW_TurnOn( used, result->r.reg );
            }
            /* placeholder for big label doesn't really zap anything */
            if( ins->head.opcode != OP_NOP ) {
                HW_TurnOn( used, ins->zap->reg );
            }
        }
    }
    if( FEAttr( AskForLblSym( CurrProc->label ) ) & FE_VARARGS ) {
        HW_TurnOn( used, VarargsHomePtr() );
    }
    HW_TurnOn( CurrProc->state.used, used );
}


void    AddCacheRegs( void )
/**************************/
{
}

static  void    initParmCache( stack_record *pc, type_length *offset )
/********************************************************************/
{
    pc->start = *offset;
    pc->size = MaxStack;
    if( MaxStack > 0 ) {
        *offset += MaxStack;
    }
}


static  void    emitParmCacheProlog( stack_record *pc )
/*****************************************************/
{
    /* unused parameters */ (void)pc;
}


static  void    emitParmCacheEpilog( stack_record *pc )
/*****************************************************/
{
    /* unused parameters */ (void)pc;
}


static  void    initLocals( stack_record *locals, type_length *offset )
/*********************************************************************/
{
    locals->start = *offset;
    locals->size = CurrProc->locals.size;
    *offset += locals->size;
}


static  void    emitLocalProlog( stack_record *locals )
/*****************************************************/
{
    /* unused parameters */ (void)locals;
}


static  void    emitLocalEpilog( stack_record *locals )
/*****************************************************/
{
    /* unused parameters */ (void)locals;
}


static  uint_32 registerMask( hw_reg_set rs, hw_reg_set *rl )
/***********************************************************/
{
    hw_reg_set          *curr;
    uint_32             result;

    result = 0;
    for( curr = rl; !HW_CEqual( *curr, HW_EMPTY ); curr++ ) {
        if( HW_Ovlap( rs, *curr ) ) {
            result |= 1 << RegTrans( *curr );
        }
    }
    return( result );
}


static  void    initSavedRegs( stack_record *saved_regs, type_length *offset )
/****************************************************************************/
{
    unsigned            num_regs;
    hw_reg_set          saved;

    calcUsedRegs();
    saved = SaveRegs();
    if( FEAttr( AskForLblSym( CurrProc->label ) ) & FE_VARARGS ) {
        HW_TurnOn( saved, VarargsHomePtr() );
    }
    CurrProc->targ.gpr_mask = registerMask( saved, GPRegs() );
    CurrProc->targ.fpr_mask = registerMask( saved, FPRegs() );
    num_regs  = CountBits( CurrProc->targ.gpr_mask );
    num_regs += CountBits( CurrProc->targ.fpr_mask );
    saved_regs->size = num_regs * REG_SIZE;
    saved_regs->start = *offset;
    *offset += saved_regs->size;
}

static  void    genMove( uint_32 src, uint_32 dst )
/*************************************************/
{
    GenOPINS( 0x11, 0x20, ZERO_REG_IDX, src, dst );
}


static  void    genLea( uint_32 src, int_16 disp, uint_32 dst )
/*************************************************************/
{
    GenMEMINS( LEA_OPCODE, dst, src, disp );
}


static  uint_32 addressableRegion( stack_record *region, type_length *offset )
/****************************************************************************/
{
    if( region->start > AXP_MAX_OFFSET ) {
        *offset = 0;
        GenLOADS32( region->start, AT_REG_IDX );
        // add sp, r28 -> r28
        GenOPINS( 0x10, 0x00, SP_REG_IDX, AT_REG_IDX, AT_REG_IDX );
        return( AT_REG_IDX );
    } else {
        *offset = region->start;
        return( SP_REG_IDX );
    }
}


static  void    saveReg( uint_32 reg, uint_32 index, type_length offset, bool fp )
/********************************************************************************/
{
    uint_8              opcode;

    opcode = STORE_QUADWORD;
    if( fp ) {
        opcode = STORE_DOUBLE;
    }
    GenMEMINS( opcode, index, reg, offset );
}


static  void    loadReg( uint_32 reg, uint_32 index, type_length offset, bool fp )
/********************************************************************************/
{
    uint_8              opcode;

    opcode = LOAD_QUADWORD;
    if( fp ) {
        opcode = LOAD_DOUBLE;
    }
    GenMEMINS( opcode, index, reg, offset );
}


static  void    saveRegSet( uint_32 index_reg,
                        uint_32 reg_set, type_length offset, bool fp )
/********************************************************************/
{
    uint_32     index;
    uint_32     high_bit;

    index = sizeof( reg_set ) * 8 - 1;
    high_bit = 1 << index;
    for( ; reg_set != 0; reg_set <<= 1 ) {
        if( reg_set & high_bit ) {
            offset -= 8;
            saveReg( index_reg, index, offset, fp );
        }
        index -= 1;
    }
}


static  void    loadRegSet( uint_32 index_reg,
                        uint_32 reg_set, type_length offset, bool fp )
/********************************************************************/
{
    uint_32     index;

    index = 0;
    for( ; reg_set != 0; reg_set >>= 1 ) {
        if( reg_set & 1 ) {
            loadReg( index_reg, index, offset, fp );
            offset += 8;
        }
        index++;
    }
}


static  void    emitSavedRegsProlog( stack_record *saved_regs )
/*************************************************************/
{
    type_length         offset;
    uint_32             index_reg;

    index_reg = addressableRegion( saved_regs, &offset );
    offset += saved_regs->size;
    saveRegSet( index_reg, CurrProc->targ.gpr_mask, offset, false );
    offset -= CountBits( CurrProc->targ.gpr_mask ) * REG_SIZE;
    saveRegSet( index_reg, CurrProc->targ.fpr_mask, offset, true );
}


static  void    emitSavedRegsEpilog( stack_record *saved_regs )
/*************************************************************/
{
    type_length         offset;
    uint_32             index_reg;

    index_reg = addressableRegion( saved_regs, &offset );
    loadRegSet( index_reg, CurrProc->targ.fpr_mask, offset, true );
    offset += CountBits( CurrProc->targ.fpr_mask ) * REG_SIZE;
    loadRegSet( index_reg, CurrProc->targ.gpr_mask, offset, false );
}


static  void    initVarargs( stack_record *varargs, type_length *offset )
/***********************************************************************/
{
    cg_sym_handle       sym;
    fe_attr             attr;

    varargs->start = *offset;
    varargs->size = 0;
    sym = AskForLblSym( CurrProc->label );
    attr = FEAttr( sym );
    if( attr & FE_VARARGS ) {
        varargs->size = 12 * REG_SIZE;
        *offset += varargs->size;
    }
}


static  void    emitVarargsProlog( stack_record *varargs )
/********************************************************/
{
    type_length         offset;
    uint_32             index_reg;

    if( varargs->size != 0 ) {
        index_reg = addressableRegion( varargs, &offset );
        offset += varargs->size;
        saveRegSet( index_reg, 0x3f << 16, offset, false );
        offset -= 6 * REG_SIZE;
        saveRegSet( index_reg, 0x3f << 16, offset, true );
    }
}


static  void    emitVarargsEpilog( stack_record *varargs )
/********************************************************/
{
    // NB see FrameSaveEpilog below
    /* unused parameters */ (void)varargs;
}


static  void    initFrameSave( stack_record *fs, type_length *offset )
/********************************************************************/
{
    fs->start = *offset;
    fs->size = 0;
    if( CurrProc->targ.base_is_fp ) {
        fs->size = REG_SIZE;
        *offset += fs->size;
    }
}


static  void    emitFrameSaveProlog( stack_record *fs )
/*****************************************************/
{
    uint_32     index_reg;
    type_length offset;

    if( fs->size != 0 ) {
        index_reg = addressableRegion( fs, &offset );
        saveReg( index_reg, FP_REG_IDX, offset, false );
    }
}


static  void    emitFrameSaveEpilog( stack_record *fs )
/*****************************************************/
{
    uint_32     index_reg;
    type_length offset;

    // NB This instruction must immediately precede the
    // stack restoration instruction - which means that the
    // varargs epilog above must be empty
    if( fs->size != 0 ) {
        index_reg = addressableRegion( fs, &offset );
        loadReg( index_reg, FP_REG_IDX, offset, false );
    }
}


static  void    initSlop( stack_record *slop, type_length *offset )
/*****************************************************************/
{
    type_length         off;

    off = *offset;
    slop->start = off;
    slop->size = 0;
    if( off & (STACK_ALIGNMENT - 1) ) {
        slop->size = STACK_ALIGNMENT - (off & (STACK_ALIGNMENT - 1));
        *offset += slop->size;
    }
}


static  void    emitSlopProlog( stack_record *fs )
/************************************************/
{
    /* unused parameters */ (void)fs;
}


static  void    emitSlopEpilog( stack_record *fs )
/************************************************/
{
    /* unused parameters */ (void)fs;
}


static int_32   frameSize( stack_map *map )
/*****************************************/
{
    int_32      size;

    size = map->slop.size + map->varargs.size + map->frame_save.size + map->saved_regs.size +
                map->locals.size + map->parm_cache.size;
    assert( (size & (STACK_ALIGNMENT - 1)) == 0 );
    return( size );
}


static  void    initStackLayout( stack_map *map )
/***********************************************/
{
    type_length         offset;

    offset = 0;
    initParmCache( &map->parm_cache, &offset );
    initLocals( &map->locals, &offset );
    initSavedRegs( &map->saved_regs, &offset );
    initFrameSave( &map->frame_save, &offset );
    initSlop( &map->slop, &offset );
    initVarargs( &map->varargs, &offset );
}


static  void    SetupVarargsReg( stack_map *map )
/***********************************************/
{
    if( map->varargs.size != 0 ) {
        type_length     offset;

        offset = map->varargs.start + 6 * REG_SIZE;
        // Skip hidden parameter in first register
        if( CurrProc->targ.return_points != NULL ) {
            offset += REG_SIZE;
        }
        if( offset > AXP_MAX_OFFSET ) {
            GenLOADS32( offset, RT_VARARGS_REG_IDX );
            GenOPINS( 0x10, 0x00, SP_REG_IDX, RT_VARARGS_REG_IDX, RT_VARARGS_REG_IDX );
        } else {
            genLea( SP_REG_IDX, offset, RT_VARARGS_REG_IDX );
        }
    }
}


static  void    emitProlog( stack_map *map )
/******************************************/
{
    type_length         frame_size;

    frame_size = frameSize( map );
    if( frame_size != 0 ) {
        if( frame_size <= AXP_MAX_OFFSET ) {
            genLea( SP_REG_IDX, -frame_size, SP_REG_IDX );
        } else {
            GenLOADS32( frame_size, AT_REG_IDX );
            // sub sp,r28 -> sp
            GenOPINS( 0x10, 0x09, SP_REG_IDX, AT_REG_IDX, SP_REG_IDX );
        }
        if( frame_size >= _TARGET_PAGE_SIZE ) {
            if( frame_size <= AXP_MAX_OFFSET ) {
                genLea( ZERO_REG_IDX, frame_size, RT_PARM1_REG_IDX );
            } else {
                genMove( AT_REG_IDX, RT_PARM1_REG_IDX );
            }
            GenCallLabelReg( RTLabel( RT_STK_CRAWL_SIZE ), RT_RET_REG_IDX );
        }
    }
    if( map->locals.size != 0 || map->parm_cache.size != 0 ) {
        if( _IsTargetModel( CGSW_RISC_STACK_INIT ) ) {
            type_length         size;
            size = map->locals.size + map->parm_cache.size;
            if( size > AXP_MAX_OFFSET ) {
                GenLOADS32( size, RT_PARM1_REG_IDX );
            } else {
                genLea( ZERO_REG_IDX, map->locals.size + map->parm_cache.size, RT_PARM1_REG_IDX );
            }
            GenCallLabelReg( RTLabel( RT_STK_STOMP ), RT_RET_REG_IDX );
        }
    }
    emitVarargsProlog( &map->varargs );
    emitSlopProlog( &map->slop );
    emitFrameSaveProlog( &map->frame_save );
    emitSavedRegsProlog( &map->saved_regs );
    emitLocalProlog( &map->locals );
    emitParmCacheProlog( &map->parm_cache );
    if( map->frame_save.size != 0 ) {
        genMove( SP_REG_IDX, FP_REG_IDX );
    }
}


static  void    emitEpilog( stack_map *map )
/******************************************/
{
    type_length         frame_size;

    if( map->frame_save.size != 0 ) {
        // NB should just use FP_REG_IDX instead of SP_REG_IDX in restore
        // code and not bother emitting this instruction
        genMove( FP_REG_IDX, SP_REG_IDX );
    }
    emitParmCacheEpilog( &map->parm_cache );
    emitLocalEpilog( &map->locals );
    emitSavedRegsEpilog( &map->saved_regs );
    emitFrameSaveEpilog( &map->frame_save );
    emitSlopEpilog( &map->slop );
    emitVarargsEpilog( &map->varargs );
    frame_size = frameSize( map );
    if( frame_size != 0 ) {
        if( frame_size <= AXP_MAX_OFFSET ) {
            genLea( SP_REG_IDX, frame_size, SP_REG_IDX );
        } else {
            GenLOADS32( frame_size, AT_REG_IDX );
            GenOPINS( 0x10, 0x00, SP_REG_IDX, AT_REG_IDX, SP_REG_IDX );
        }
    }
}


void    GenProlog( void )
/***********************/
{
    segment_id          old_segid;
    label_handle        label;

    old_segid = SetOP( AskCodeSeg() );
    label = CurrProc->label;
    if( _IsModel( CGSW_GEN_DBG_NUMBERS ) ) {
        OutFileStart( HeadBlock->ins.head.line_num );
    }
    TellKeepLabel( label );
    TellProcLabel( label );
    CodeLabelLinenum( label, DepthAlign( PROC_ALIGN ), HeadBlock->ins.head.line_num );
    if( _IsModel( CGSW_GEN_DBG_LOCALS ) ) {  // d1+ or d2
        // DbgRtnBeg( CurrProc->targ.debug, lc );
        EmitRtnBeg( /*label, HeadBlock->ins.head.line_num*/ );
    }
    // keep stack aligned
    CurrProc->locals.size = _RoundUp( CurrProc->locals.size, REG_SIZE );
    CurrProc->parms.base = 0;
    CurrProc->parms.size = CurrProc->state.parm.offset;
    initStackLayout( &CurrProc->targ.stack_map );
    emitProlog( &CurrProc->targ.stack_map );
    EmitProEnd();
    SetupVarargsReg( &CurrProc->targ.stack_map );
    CurrProc->targ.frame_size = frameSize( &CurrProc->targ.stack_map );
    SetOP( old_segid );
}


void    GenEpilog( void )
/***********************/
{
    segment_id      old_segid;

    old_segid = SetOP( AskCodeSeg() );
    EmitEpiBeg();
    emitEpilog( &CurrProc->targ.stack_map );
    GenReturn();
    CurrProc->prolog_state |= PST_EPILOG_GENERATED;
    EmitRtnEnd();
    SetOP( old_segid );
}


int     AskDisplaySize( level_depth level )
/*****************************************/
{
    /* unused parameters */ (void)level;

    return( 0 );
}


void    InitStackDepth( block *blk )
/**********************************/
{
    /* unused parameters */ (void)blk;
}


type_length     PushSize( type_length len )
/*****************************************/
{
    if( len < REG_SIZE )
        return( REG_SIZE );
    return( len );
}


type_length     NewBase( name *op )
/*********************************/
{
    return( TempLocation( op ) );
}


int     ParmsAtPrologue( void )
/*****************************/
{
    return( 0 );
}
