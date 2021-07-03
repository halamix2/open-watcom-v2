/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "guiwind.h"
#include "guiscale.h"
#include "guixutil.h"
#include "guiscrol.h"

static void SetRange( gui_window *wnd, int bar, unsigned int range, bool text )
{
    int new_range;
    int screen_size;

    screen_size = GUIGetScrollScreenSize( wnd, bar );
    if( bar == SB_HORZ ) {
        if( !GUI_HSCROLL_ON( wnd ) ) {
            return;
        }
        wnd->hscroll_range = range;
        if( text ) {
            range = GUIFromTextX( range, wnd );
        }
        wnd->flags |= HRANGE_SET;
    } else {
        if( !GUI_VSCROLL_ON( wnd ) ) {
            return;
        }
        wnd->vscroll_range = range;
        if( text ) {
            range = GUIFromTextY( range, wnd );
        }
        wnd->flags |= VRANGE_SET;
    }
    new_range = range - screen_size;
    if( new_range < 0 ) {
        new_range = 0;
    }
    GUISetScrollRange( wnd, bar, 0, new_range, true );
}

/*
 * GUISetHScrollRangeCols
 */

void GUISetHScrollRangeCols( gui_window *wnd, gui_ord range )
{
    wnd->flags |= HRANGE_COL;
    SetRange( wnd, SB_HORZ, range, true );
}

/*
 * GUISetVScrollRangeRows
 */

void GUISetVScrollRangeRows( gui_window *wnd, gui_ord range )
{
    wnd->flags |= VRANGE_ROW;
    SetRange( wnd, SB_VERT, range, true );
}

/*
 * GUISetHScrollRange
 */

void GUISetHScrollRange( gui_window *wnd, gui_ord range )
{
    wnd->flags &= ~HRANGE_COL;
    SetRange( wnd, SB_HORZ, GUIScaleToScreenH( range ), false );
}

/*
 * GUISetVScrollRange
 */

void GUISetVScrollRange( gui_window *wnd, gui_ord range )
{
    wnd->flags &= ~VRANGE_ROW;
    SetRange( wnd, SB_VERT, GUIScaleToScreenV( range ), false );
}

/*
 * GUIGetHScrollRange
 */

gui_ord GUIGetHScrollRange( gui_window *wnd )
{
    return( GUIScreenToScaleH( GUIGetScrollRange( wnd, SB_HORZ ) ) );
}

/*
 * GUIGetVScrollRange
 */

gui_ord GUIGetVScrollRange( gui_window *wnd )
{
    return( GUIScreenToScaleV( GUIGetScrollRange( wnd, SB_VERT ) ) );
}

/*
 * GUIGetVScrollRangeRows
 */

gui_ord GUIGetVScrollRangeRows( gui_window *wnd )
{
    gui_ord     range;

    range = GUIGetScrollRange( wnd, SB_VERT );
    if( range == GUI_NO_ROW ) {
        return( range );
    } else {
        return( GUIToTextY( range, wnd ) );
    }
}

/*
 * GUIGetHScrollRangeCols
 */

gui_ord GUIGetHScrollRangeCols( gui_window *wnd )
{
    gui_ord     range;

    range = GUIGetScrollRange( wnd, SB_HORZ );
    if( range == GUI_NO_COLUMN ) {
        return( range );
    } else {
        return( GUIToTextX( range, wnd ) );
    }
}
