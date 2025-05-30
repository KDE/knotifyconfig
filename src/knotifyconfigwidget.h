/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2005 Olivier Goffart <ogoffart at kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KNOTIFYCONFIGWIDGET_H
#define KNOTIFYCONFIGWIDGET_H

#include <QString>
#include <QWidget>
#include <knotifyconfig_export.h>

#include <memory>

class KNotifyConfigElement;
class KNotifyConfigWidgetPrivate;
/*!
 * \class KNotifyConfigWidget
 * \inmodule KNotifyConfig
 *
 * \brief Configure the notification for a given application.
 *
 * You probably will want to use the static function configure
 *
 * If you create the widget yourself, you must call setApplication before showing it.
 */
class KNOTIFYCONFIG_EXPORT KNotifyConfigWidget : public QWidget
{
    Q_OBJECT
public:
    /*!
     *
     */
    explicit KNotifyConfigWidget(QWidget *parent);
    ~KNotifyConfigWidget() override;

    /*!
     * Show a dialog with the widget.
     *
     * \a parent the parent widget of the dialog
     *
     * \a appname the application name, if null, it is autodetected
     *
     * Returns the widget itself    the topLevelWidget of it is probably a KDialog
     */
    static KNotifyConfigWidget *configure(QWidget *parent = nullptr, const QString &appname = QString());

    /*!
     * Change the application
     *
     * \a appname name of the application. If null QCoreApplication::instance()->applicationName() is used
     */
    void setApplication(const QString &appname = QString());

    /*!
     * Select a given notification in the current list
     *
     * \a id The id of the notification
     * \since 5.18
     */
    void selectEvent(const QString &eventId);

public Q_SLOTS:
    /*!
     * save to the config file
     */
    void save();

    /*!
     * Reset the UI to display the default values
     * \sa KCModule::defaults
     * \since 5.15
     */
    void revertToDefaults();

    /*!
     * Disable all sounds for the current application
     * \since 5.23
     */
    void disableAllSounds();

Q_SIGNALS:
    /*!
     * Indicate that the state of the modules contents has changed.
     * This signal is emitted whenever the state of the configuration changes.
     * \sa KCModule::changed
     */
    void changed(bool state);

private Q_SLOTS:
    KNOTIFYCONFIG_NO_EXPORT void slotEventSelected(KNotifyConfigElement *e);
    KNOTIFYCONFIG_NO_EXPORT void slotActionChanged();

private:
    std::unique_ptr<KNotifyConfigWidgetPrivate> const d;
};

#endif
