/*
    Copyright (C) 2007 Murad Tagirov <tmurad@gmail.com>
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "GeoDataDocument.h"

#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

#include <QtCore/QDebug>

#include "GeoDataDocument_p.h"

namespace Marble
{

GeoDataDocument::GeoDataDocument( GeoDataObject *parent )
    : GeoDocument()
    , GeoDataContainer( new GeoDataDocumentPrivate() )
{
}

GeoDataDocument::GeoDataDocument( const GeoDataDocument& other )
: GeoDataContainer( other )
{
}

GeoDataDocument::~GeoDataDocument()
{
}

GeoDataDocumentPrivate* GeoDataDocument::p() const
{
    return static_cast<GeoDataDocumentPrivate*>(d);
}

QString GeoDataDocument::fileName() const
{
    return p()->m_filename;
}

void GeoDataDocument::setFileName( const QString &value )
{
    p()->m_filename = value;
}

void GeoDataDocument::addStyle( GeoDataStyle* style )
{
    Q_ASSERT( style );
    qDebug( "GeoDataDocument: Add new style" );
    p()->m_styleHash.insert( style->styleId(), style );
}

void GeoDataDocument::removeStyle( GeoDataStyle* style )
{
    Q_ASSERT( style );
    qDebug( "GeoDataDocument: remove style" );
    p()->m_styleHash.remove( style->styleId() );
}

GeoDataStyle* GeoDataDocument::style( const QString& styleId ) const
{
    /*
     * FIXME: m_styleHash always should contain at least default
     *        GeoDataStyle element
     */
    return p()->m_styleHash.value( styleId );
}

QList<GeoDataStyle*> GeoDataDocument::styles() const
{
    return p()->m_styleHash.values();
}

void GeoDataDocument::addStyleMap( GeoDataStyleMap* map )
{
    Q_ASSERT( map );
    qDebug( "GeoDataDocument: Add new styleMap" );
    p()->m_styleMapHash.insert( map->styleId(), map );
}

void GeoDataDocument::removeStyleMap( GeoDataStyleMap* map )
{
    Q_ASSERT( map );
    qDebug( "GeoDataDocument: remove styleMap" );
    p()->m_styleMapHash.remove( map->styleId() );
}

GeoDataStyleMap* GeoDataDocument::styleMap( const QString& styleId ) const
{
    return p()->m_styleMapHash.value( styleId );
}

QList<GeoDataStyleMap*> GeoDataDocument::styleMaps() const
{
    return p()->m_styleMapHash.values();
}

void GeoDataDocument::pack( QDataStream& stream ) const
{
    GeoDataContainer::pack( stream );

    stream << p()->m_styleHash.size();
    
    
    for( QHash<QString, GeoDataStyle*>::const_iterator iterator 
          = p()->m_styleHash.constBegin(); 
        iterator != p()->m_styleHash.constEnd(); 
        ++iterator ) {
        iterator.value()->pack( stream );
    }
}


void GeoDataDocument::unpack( QDataStream& stream )
{
    GeoDataContainer::unpack( stream );

    int size = 0;

    stream >> size;
    for( int i = 0; i < size; i++ ) {
        GeoDataStyle* style = new GeoDataStyle();
        style->unpack( stream );
        p()->m_styleHash.insert( style->styleId(), style );
    }
}

}
