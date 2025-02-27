/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#include "wlib.h"
#include "ar.h"
#include "convert.h"

#include "clibext.h"


static void GetARValue( const char *element, ar_len len, char delimiter, char *buffer );

static unsigned long GetARNumeric( const char *str, int max )
/***********************************************************/
// get a numeric value from an ar_header
{
    char                buf[20];
    unsigned long       value;

    memcpy( buf, str, max );
    buf[max] = '\0';
    value = strtoul( buf, NULL, 10 );
    return( value );
}

static unsigned long GetARNumericMode( const char *str, int max )
/***************************************************************/
// get a numeric value from an ar_header
{
    char                buf[20];
    unsigned long       value;

    memcpy( buf, str, max );
    buf[max] = '\0';
    value = strtoul( buf, NULL, 8 );
    return( value );
}

static size_t   ARstrlen( const char * str )
/******************************************/
// find the length of a NULL or /\n terminated string.
{
    const char  *c;

    for( c = str; *c != '\0'; c++ ) {
        if( ( c[0] == '/' ) && ( c[1] == '\n' ) ) {
            break;
        }
    }
    return( c - str );
}

char *GetARName( libfile io, ar_header *header, arch_header *arch )
/*****************************************************************/
{
    char        buffer[AR_NAME_LEN + 1];
    char        *buf;
    char        *name;
    size_t      len;

    if( header->name[0] == '/' ) {
        len = GetARNumeric( header->name + 1, AR_NAME_LEN - 1 );
        buf = arch->fnametab + len;
    } else if( header->name[0] == '#' && header->name[1] == '1' && header->name[2] == '/') {
        len = GetARNumeric( header->name + 3, AR_NAME_LEN - 3 );
        name = MemAlloc( len + 1 );
        LibRead( io, name, len );
        name[len] = '\0';
        return( name );
    } else {
        GetARValue( header->name, AR_NAME_LEN, AR_NAME_END_CHAR, buffer );
        buf = buffer;
    }
    len = ARstrlen( buf );
    name = MemAlloc( len + 1 );
    memcpy( name, buf, len );
    name[len] = '\0';
    return( name );
}

char *GetFFName( arch_header *arch )
/**********************************/
{
    char        *name;

    name = NULL;
    if( arch->ffnametab != NULL && arch->nextffname != NULL ) {
        name = DupStr( arch->nextffname );
        arch->nextffname += strlen( name ) + 1;
        if( arch->nextffname >= arch->lastffname || ( arch->nextffname[0] == '\n'
                && arch->nextffname + 1 >= arch->lastffname ) ) {
            arch->nextffname = NULL;
        }
    }
    return( name );
}

static void GetARValue( const char *element, ar_len len, char delimiter, char *buffer )
/*************************************************************************************/
// function to copy a value from an ar_header into a buffer so
// that it is null-terminated rather than blank-padded
{
    for( ; len > 0; --len ) {
        if( element[len - 1] != ' ' && element[len - 1] != delimiter ) {
            break;
        }
    }
    if( len > 0 ) {
        strncpy( buffer, element, len );
    }
    buffer[len] = '\0';
}

void GetARHeaderValues( ar_header *header, arch_header * arch )
{
    arch->date = GetARNumeric( header->date, AR_DATE_LEN );
    arch->uid = GetARNumeric( header->uid, AR_UID_LEN );
    arch->gid = GetARNumeric( header->gid, AR_GID_LEN );
    arch->mode = GetARNumericMode( header->mode, AR_MODE_LEN );
    arch->size = GetARNumeric( header->size, AR_SIZE_LEN );
}

static void PutARPadding( char * element, ar_len current_len, ar_len desired_len )
{
    ar_len      loop;

    for( loop = current_len; loop < desired_len; loop++ ) {
        element[loop] = AR_VALUE_END_CHAR;
    }
}

static void PutARName( char *ar_name, const char *arch_name )
{
    ar_len      name_len;


    name_len = strlen( arch_name );
    strncpy( ar_name, arch_name, name_len );
    if( name_len < AR_NAME_LEN ) {
        PutARPadding( ar_name, name_len, AR_NAME_LEN );
    }
}

static void PutARValue( char *element, uint_32 value, ar_len desired_len )
{
    ar_len      value_len;

    sprintf( element, "%lu", (unsigned long)value );
    value_len = strlen( element );
    if( value_len < desired_len ) {
        PutARPadding( element, value_len, desired_len );
    }
}

static void PutARValueMode( char *element, uint_32 value, ar_len desired_len )
{
    ar_len      value_len;
    char        buffer[14];

    sprintf( buffer, "%lo", (unsigned long)value );
    value_len = strlen( buffer );
    if( value_len < desired_len ) {
        strcpy( element, buffer );
        PutARPadding( element, value_len, desired_len );
    } else {
        strncpy( element, buffer + (value_len - desired_len), desired_len );
    }
}

void CreateARHeader( ar_header *ar, arch_header * arch )
{
    PutARName( ar->name, arch->name );
    PutARValue( ar->date, arch->date, AR_DATE_LEN );
    PutARValue( ar->uid, arch->uid, AR_UID_LEN );
    PutARValue( ar->gid, arch->gid, AR_GID_LEN );
    PutARValueMode( ar->mode, arch->mode, AR_MODE_LEN );
    PutARValue( ar->size, arch->size, AR_SIZE_LEN );
    memcpy( ar->header_ident, AR_HEADER_IDENT, AR_HEADER_IDENT_LEN );
}
