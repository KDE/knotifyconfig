/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2005-2007 Olivier Goffart <ogoffart at kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "knotifyconfigactionswidget.h"

#include "knotifyconfigelement.h"
#include <knotifyconfig_debug.h>

#include <QFile>
#include <QStandardPaths>
#include <QUrl>

#if HAVE_CANBERRA
#include <canberra.h>
#elif HAVE_PHONON
#include <phonon/mediaobject.h>
#endif

KNotifyConfigActionsWidget::KNotifyConfigActionsWidget(QWidget *parent)
    : QWidget(parent)
{
    m_ui.setupUi(this);

    // Show sounds directory by default
    QStringList soundDirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("sounds"), QStandardPaths::LocateDirectory);
    if (!soundDirs.isEmpty()) {
        m_ui.Sound_select->setStartDir(QUrl::fromLocalFile(soundDirs.last()));
    }
    m_ui.Sound_select->setMimeTypeFilters({QStringLiteral("audio/x-vorbis+ogg"), QStringLiteral("audio/x-wav")});

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

KNotifyConfigActionsWidget::~KNotifyConfigActionsWidget()
{
#if HAVE_CANBERRA
    if (m_context) {
        ca_context_destroy(m_context);
    }
    m_context = nullptr;
#endif
}

void KNotifyConfigActionsWidget::setConfigElement(KNotifyConfigElement *config)
{
    bool blocked = blockSignals(true); // to block the changed() signal
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

    config->writeEntry(QStringLiteral("Sound"),
                       m_ui.Sound_select->text()); // don't use .url() here, .notifyrc files have predefined "static" entries with no path
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
    for (const QString &dataLocation : dataLocations) {
        soundURL = QUrl::fromUserInput(soundFilename, dataLocation + QStringLiteral("/sounds"), QUrl::AssumeLocalFile);
        if (soundURL.isLocalFile() && QFile::exists(soundURL.toLocalFile())) {
            break;
        } else if (!soundURL.isLocalFile() && soundURL.isValid()) {
            break;
        }
        soundURL.clear();
    }

#if HAVE_CANBERRA
    if (!m_context) {
        int ret = ca_context_create(&m_context);
        if (ret != CA_SUCCESS) {
            qCWarning(KNOTIFYCONFIG_LOG) << "Failed to initialize canberra context for audio notification:" << ca_strerror(ret);
            m_context = nullptr;
            return;
        }

        QString desktopFileName = QGuiApplication::desktopFileName();
        // handle apps which set the desktopFileName property with filename suffix,
        // due to unclear API dox (https://bugreports.qt.io/browse/QTBUG-75521)
        if (desktopFileName.endsWith(QLatin1String(".desktop"))) {
            desktopFileName.chop(8);
        }
        ret = ca_context_change_props(m_context,
                                      CA_PROP_APPLICATION_NAME,
                                      qUtf8Printable(qApp->applicationDisplayName()),
                                      CA_PROP_APPLICATION_ID,
                                      qUtf8Printable(desktopFileName),
                                      CA_PROP_APPLICATION_ICON_NAME,
                                      qUtf8Printable(qApp->windowIcon().name()),
                                      nullptr);
        if (ret != CA_SUCCESS) {
            qCWarning(KNOTIFYCONFIG_LOG) << "Failed to set application properties on canberra context for audio notification:" << ca_strerror(ret);
        }
    }

    ca_proplist *props = nullptr;
    ca_proplist_create(&props);

    // We'll also want this cached for a time. volatile makes sure the cache is
    // dropped after some time or when the cache is under pressure.
    ca_proplist_sets(props, CA_PROP_MEDIA_FILENAME, QFile::encodeName(soundURL.toLocalFile()).constData());
    ca_proplist_sets(props, CA_PROP_CANBERRA_CACHE_CONTROL, "volatile");

    int ret = ca_context_play_full(m_context, 0, props, nullptr, nullptr);

    ca_proplist_destroy(props);

    if (ret != CA_SUCCESS) {
        qCWarning(KNOTIFYCONFIG_LOG) << "Failed to play sound with canberra:" << ca_strerror(ret);
        return;
    }
#elif HAVE_PHONON
    Phonon::MediaObject *media = Phonon::createPlayer(Phonon::NotificationCategory, soundURL);
    media->play();
    connect(media, SIGNAL(finished()), media, SLOT(deleteLater()));
#endif
}

void KNotifyConfigActionsWidget::slotTTSComboChanged()
{
    m_ui.TTS_select->setEnabled(m_ui.TTS_check->isChecked() && m_ui.TTS_combo->currentIndex() == 2);
    Q_EMIT changed();
}
