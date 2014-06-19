//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackFlyToItem.h"

#include "GeoDataTypes.h"
#include "GeoDataLookAt.h"
#include "GeoDataCamera.h"
#include "Quaternion.h"

#include <QTimer>

namespace Marble
{
PlaybackFlyToItem::PlaybackFlyToItem( const GeoDataFlyTo* flyTo ):
        m_flyTo( flyTo ),
        m_before( 0 ),
        m_next( 0 ),
        m_duration( flyTo->duration() ),
        m_isPlaying( false )
{
    //do nothing
}

const GeoDataFlyTo* PlaybackFlyToItem::flyTo() const
{
    return m_flyTo;
}

double PlaybackFlyToItem::duration() const
{
    return m_flyTo->duration();
}

void PlaybackFlyToItem::play()
{
    if( m_isPlaying ){
        return;
    } else {
        m_isPlaying = true;
        if ( !( m_start.isValid() ) ){
            m_start = QDateTime::currentDateTime();
            Q_ASSERT( m_start.isValid() );
        } else {
            m_start = m_start.addMSecs( m_pause.msecsTo( QDateTime::currentDateTime() ) );
        }
        playNext();
    }
}

void PlaybackFlyToItem::playNext()
{
    if( !m_start.isValid() ){
        return;
    }
    double const progress = m_start.msecsTo( QDateTime::currentDateTime() ) / 1000.0;
    Q_ASSERT( progress >= 0.0 );
    double const t = progress / m_duration;
    if( t <= 1 ){
        if( m_isPlaying ){
            center( t );
            emit progressChanged( progress );
            QTimer::singleShot( 5, this, SLOT( playNext() ) );
        }
    } else {
        emit finished();
        stop();
    }
}

void PlaybackFlyToItem::pause()
{
    m_isPlaying = false;
    m_pause = QDateTime::currentDateTime();
}

void PlaybackFlyToItem::seek( double t )
{
    m_start = QDateTime::currentDateTime().addMSecs( -t * m_duration * 1000 );
    m_pause = QDateTime::currentDateTime();
    center( t );
}

void PlaybackFlyToItem::stop()
{
    m_isPlaying = false;
    m_start = QDateTime();
    m_pause = QDateTime();
}

void PlaybackFlyToItem::center( double t )
{
    Q_ASSERT( m_before );
    if ( m_flyTo->flyToMode() == GeoDataFlyTo::Bounce ) {
        emit centerOn( m_before->m_flyTo->view()->coordinates().interpolate( m_flyTo->view()->coordinates(), t ) );
    } else {
        Q_ASSERT( m_flyTo->flyToMode() == GeoDataFlyTo::Smooth );
        GeoDataCoordinates const a = m_before->m_before ? m_before->m_before->m_flyTo->view()->coordinates() : m_before->m_flyTo->view()->coordinates();
        GeoDataCoordinates const b = m_before->m_before ? m_before->m_flyTo->view()->coordinates() : m_before->m_flyTo->view()->coordinates().interpolate( m_flyTo->view()->coordinates(), 0.1 );
        GeoDataCoordinates const c = m_next ? m_flyTo->view()->coordinates() : b.interpolate( m_flyTo->view()->coordinates(), 0.9 );
        GeoDataCoordinates const d = m_next ? m_next->m_flyTo->view()->coordinates() : m_flyTo->view()->coordinates();
        emit centerOn( b.interpolate( a, c, d, t ) );
    }
}

void PlaybackFlyToItem::setBefore( PlaybackFlyToItem *before )
{
    m_before = before;
}

void PlaybackFlyToItem::setNext( PlaybackFlyToItem *next )
{
    m_next = next;
}

}

#include "PlaybackFlyToItem.moc"