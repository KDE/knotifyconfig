/* This file is part of the KDE libraries
   Copyright (C) 2005-2007 Olivier Goffart <ogoffart at kde.org>

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
#include "knotifyconfigactionswidget.h"
#include "knotifyconfigelement.h"

#include <QStandardPaths>

#include "knotify-config.h"
#if HAVE_PHONON
#include <phonon/mediaobject.h>
#endif

KNotifyConfigActionsWidget::KNotifyConfigActionsWidget(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);

    //Show sounds directory by default
    QStringList soundDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("sounds"), QStandardPaths::LocateDirectory);
    if (!soundDirs.isEmpty()) {
        m_ui.Sound_select->setStartDir(QUrl::fromLocalFile(soundDirs.last()));
    }
    m_ui.Sound_select->setMimeTypeFilters({QStringLiteral("audio/x-vorbis+ogg"),
                                           QStringLiteral("audio/x-wav")});

    m_ui.Sound_play->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    m_ui.Sound_check->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    m_ui.Popup_check->setIcon(QIcon::fromTheme(QStringLiteral("dialog-information")));
    m_ui.Logfile_check->setIcon(QIcon::fromTheme(QStringLiteral("text-x-generic")));
    m_ui.Execute_check->setIcon(QIcon::fromTheme(QStringLiteral("system-run")));
    m_ui.Taskbar_check->setIcon(QIcon::fromTheme(QStringLiteral("services")));
    m_ui.TTS_check->setIcon(QIcon::fromTheme(QStringLiteral("text-speak")));

    connect(m_ui.Execute_check, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(m_ui.Sound_check, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(m_ui.Popup_check, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(m_ui.Logfile_check, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(m_ui.Taskbar_check, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(m_ui.TTS_check, SIGNAL(toggled(bool)), this, SLOT(slotTTSComboChanged()));
    connect(m_ui.Execute_select, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));
    connect(m_ui.Sound_select, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));
    connect(m_ui.Logfile_select, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));
    connect(m_ui.Sound_play, SIGNAL(clicked()), this, SLOT(slotPlay()));
    connect(m_ui.TTS_combo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotTTSComboChanged()));
    m_ui.TTS_combo->setEnabled(false);
    if (!KNotifyConfigElement::have_tts()) {
        m_ui.TTS_check->setVisible(false);
        m_ui.TTS_select->setVisible(false);
        m_ui.TTS_combo->setVisible(false);
    }

}

void KNotifyConfigActionsWidget::setConfigElement(KNotifyConfigElement *config)
{
    bool blocked = blockSignals(true); //to block the changed() signal
    QString prstring = config->readEntry(QStringLiteral("Action"));
    QStringList actions = prstring.split(QLatin1Char('|'));

    m_ui.Sound_check->setChecked(actions.contains(QStringLiteral("Sound")));
    m_ui.Popup_check->setChecked(actions.contains(QStringLiteral("Popup")));
    m_ui.Logfile_check->setChecked(actions.contains(QStringLiteral("Logfile")));
    m_ui.Execute_check->setChecked(actions.contains(QStringLiteral("Execute")));
    m_ui.Taskbar_check->setChecked(actions.contains(QStringLiteral("Taskbar")));
    m_ui.TTS_check->setChecked(actions.contains(QStringLiteral("TTS")));

    m_ui.Sound_select->setUrl(QUrl(config->readEntry(QStringLiteral("Sound"), true)));
    m_ui.Logfile_select->setUrl(QUrl(config->readEntry(QStringLiteral("Logfile"), true)));
    m_ui.Execute_select->setUrl(QUrl::fromLocalFile(config->readEntry(QStringLiteral("Execute"))));
    m_ui.TTS_select->setText(config->readEntry(QStringLiteral("TTS")));
    if (m_ui.TTS_select->text() == QLatin1String("%e")) {
        m_ui.TTS_combo->setCurrentIndex(1);
    } else if (m_ui.TTS_select->text() == QLatin1String("%m") || m_ui.TTS_select->text() == QLatin1String("%s")) {
        m_ui.TTS_combo->setCurrentIndex(0);
    } else {
        m_ui.TTS_combo->setCurrentIndex(2);
    }
    blockSignals(blocked);
}

void KNotifyConfigActionsWidget::save(KNotifyConfigElement *config)
{
    QStringList actions;
    if (m_ui.Sound_check->isChecked()) {
        actions << QStringLiteral("Sound");
    }
    if (m_ui.Popup_check->isChecked()) {
        actions << QStringLiteral("Popup");
    }
    if (m_ui.Logfile_check->isChecked()) {
        actions << QStringLiteral("Logfile");
    }
    if (m_ui.Execute_check->isChecked()) {
        actions << QStringLiteral("Execute");
    }
    if (m_ui.Taskbar_check->isChecked()) {
        actions << QStringLiteral("Taskbar");
    }
    if (m_ui.TTS_check->isChecked()) {
        actions << QStringLiteral("TTS");
    }

    config->writeEntry(QStringLiteral("Action"), actions.join(QLatin1Char('|')));

    config->writeEntry(QStringLiteral("Sound"), m_ui.Sound_select->text());  // don't use .url() here, .notifyrc files have predefined "static" entries with no path
    config->writeEntry(QStringLiteral("Logfile"), m_ui.Logfile_select->url().toString());
    config->writeEntry(QStringLiteral("Execute"), m_ui.Execute_select->url().toLocalFile());
    switch (m_ui.TTS_combo->currentIndex()) {
    case 0:
        config->writeEntry(QStringLiteral("TTS"), QStringLiteral("%s"));
        break;
    case 1:
        config->writeEntry(QStringLiteral("TTS"), QStringLiteral("%e"));
        break;
    case 2:
    default:
        config->writeEntry(QStringLiteral("TTS"), m_ui.TTS_select->text());
    }
}

void KNotifyConfigActionsWidget::slotPlay()
{
    const QString soundFilename = m_ui.Sound_select->text();
    QUrl soundURL;
    const auto dataLocations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    foreach (const QString &dataLocation, dataLocations) {
        soundURL = QUrl::fromUserInput(soundFilename,
                                       dataLocation + QStringLiteral("/sounds"),
                                       QUrl::AssumeLocalFile);
        if (soundURL.isLocalFile() && QFile::exists(soundURL.toLocalFile())) {
            break;
        } else if (!soundURL.isLocalFile() && soundURL.isValid()) {
            break;
        }
        soundURL.clear();
    }
#if HAVE_PHONON
    Phonon::MediaObject *media = Phonon::createPlayer(Phonon::NotificationCategory, soundURL);
    media->play();
    connect(media, SIGNAL(finished()), media, SLOT(deleteLater()));
#endif
}

void KNotifyConfigActionsWidget::slotTTSComboChanged()
{
    m_ui.TTS_select->setEnabled(m_ui.TTS_check->isChecked() &&  m_ui.TTS_combo->currentIndex() == 2);
    emit changed();
}
