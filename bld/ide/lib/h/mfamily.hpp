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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef mfamily_class
#define mfamily_class

#include "idecfg.h"
#include "wobject.hpp"
#include "wstring.hpp"
#include "wpicklst.hpp"
#include "wtokfile.hpp"

WCLASS MSwitch;
WCLASS MFamily : public WObject
{
    Declare( MFamily )
    public:
        MFamily( const char* name=NULL ) : _name( name ) {}
        MFamily( WTokenFile& fil, WString& tok );
        ~MFamily();
        void name( WString& name ) { name = _name; }
        WPickList& switches() { return( _switches ); }
        bool WEXPORT hasSwitches( bool setable );
        MSwitch* WEXPORT findSwitch( MTool *tool, WString& switchtag, long fixed_version=0 );
        void WEXPORT addSwitches( WVList& list, const char* mask, bool setable );
        WString *WEXPORT displayText( MSwitch *sw, WString& text );
#if IDE_CFG_VERSION_MAJOR > 4
        WString* findSwitchByText( WString& id, WString& text, int kludge=0 );
        WString *WEXPORT translateID( MSwitch *sw, WString& text );
#endif
    private:
        WString         _name;
        WPickList       _switches;      //<MSwitch>
#if IDE_CFG_VERSION_MAJOR > 4
        WStringMap      _switchesTexts; //<WString>
        WStringMap      _switchesIds;   //<WString>
#endif
};

#endif
