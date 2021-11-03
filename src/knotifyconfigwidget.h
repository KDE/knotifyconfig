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

class KNotifyConfigElement;
class KNotifyConfigWidgetPrivate;
/**
 * @class KNotifyConfigWidget knotifyconfigwidget.h <KNotifyConfigWidget>
 *
 * Configure the notification for a given application / context
 *
 * You probably will want to use the static function configure
 *
 * If you create the widget yourself, you must call setApplication before showing it
 *
 * @author Olivier Goffart <ogoffart @ kde.org>
 */
class KNOTIFYCONFIG_EXPORT KNotifyConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KNotifyConfigWidget(QWidget *parent);
    ~KNotifyConfigWidget() override;

    /**
     * Show a dialog with the widget.
     * @param parent the parent widget of the dialog
     * @param appname the application name,  if null, it is autodetected
     * @return the widget itself    the topLevelWidget of it is probably a KDialog
     */
    static KNotifyConfigWidget *configure(QWidget *parent = nullptr, const QString &appname = QString());

    /**
     * Change the application and the context
     *
     * @param appname name of the application.   if null QCoreApplication::instance()->applicationName() is used
     * @param context_name the name of the context, if null , avery context are considered
     * @param context_value the context value
     */
    void setApplication(const QString &appname = QString(), const QString &context_name = QString(), const QString &context_value = QString());

    /**
     * Select a given notification in the current list
     *
     * @param id The id of the notification
     * @since 5.18
     */
    void selectEvent(const QString &eventId);

public Q_SLOTS:
    /**
     * save to the config file
     */
    void save();

    /*
     * Reset the UI to display the default values
     * @see KCModule::defaults
     * @since 5.15
     */
    void revertToDefaults();

    /*
     * Disable all sounds for the current application
     * @since 5.23
     */
    void disableAllSounds();

Q_SIGNALS:
    /**
     * Indicate that the state of the modules contents has changed.
     * This signal is emitted whenever the state of the configuration changes.
     * @see KCModule::changed
     */
    void changed(bool state);

private:
    KNotifyConfigWidgetPrivate *const d;
private Q_SLOTS:
    void slotEventSelected(KNotifyConfigElement *e);
    void slotActionChanged();
};

#endif
