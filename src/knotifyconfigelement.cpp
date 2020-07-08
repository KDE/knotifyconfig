/*  This file is part of the KDE project
   Copyright (C) 2005-2006 by Olivier Goffart <ogoffart at kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.

 */

#include "knotifyconfigelement.h"

#include <KConfigGroup>
#include <KConfig>

KNotifyConfigElement::KNotifyConfigElement(const QString &eventid, KConfig *config)
    : m_config(new KConfigGroup(config, QStringLiteral("Event/") + eventid))
    , m_eventId(eventid)
{
}

KNotifyConfigElement::~KNotifyConfigElement()
{
    delete m_config;
}

QString KNotifyConfigElement::readEntry(const QString &entry, bool path)
{
    if (m_cache.contains(entry)) {
        return m_cache[entry];
    }
    return path ?  m_config->readPathEntry(entry, QString()) :
           m_config->readEntry(entry, QString());
}

void KNotifyConfigElement::writeEntry(const QString &entry, const QString &data)
{
    m_cache[entry] = data;
}

QString KNotifyConfigElement::eventId() const
{
    return m_eventId;
}

void KNotifyConfigElement::save()
{
    QMap<QString, QString>::const_iterator it = m_cache.constBegin();
    for (; it != m_cache.constEnd(); ++it) {
        m_config->writeEntry(it.key(), it.value());
    }
}

bool KNotifyConfigElement::have_tts() //[static]
{
    // TODO: Add a runtime check to see if the status is not BackendError
#ifdef HAVE_SPEECH
    return true;
#else
    return false;
#endif
}
