/* This file is part of the KDE libraries
   Copyright (C) 2005 Olivier Goffart <ogoffart at kde.org>

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

#include "knotifyconfigwidget.h"
#include "knotifyconfigactionswidget.h"
#include "knotifyeventlist.h"
#include "knotifyconfigelement.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QDBusInterface>
#include <QDBusConnectionInterface>

#include <klocalizedstring.h>

struct KNotifyConfigWidgetPrivate {
    KNotifyEventList *eventList;
    KNotifyConfigActionsWidget *actionsconfig;
    KNotifyConfigElement *currentElement;
    QString application;
    QString contextName;
    QString contextValue;
};

KNotifyConfigWidget::KNotifyConfigWidget(QWidget *parent)
    : QWidget(parent), d(new KNotifyConfigWidgetPrivate)
{
    d->currentElement = 0l;
    d->eventList = new KNotifyEventList(this);
    d->eventList->setFocus();
    d->actionsconfig = new KNotifyConfigActionsWidget(this);
    d->actionsconfig->setEnabled(false);
    connect(d->eventList, SIGNAL(eventSelected(KNotifyConfigElement*)),
            this, SLOT(slotEventSelected(KNotifyConfigElement*)));
    connect(d->actionsconfig, SIGNAL(changed()), this, SLOT(slotActionChanged()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d->eventList, 1);
    layout->addWidget(d->actionsconfig);
}

KNotifyConfigWidget::~KNotifyConfigWidget()
{
    delete d;
}

void KNotifyConfigWidget::setApplication(const QString &app, const QString &context_name, const QString &context_value)
{
    d->currentElement = 0l;
    d->application = app.isEmpty() ? QCoreApplication::instance()->applicationName()  : app;
    d->contextName = context_name;
    d->contextValue = context_value;
    d->eventList->fill(d->application, d->contextName, d->contextValue);
}

void KNotifyConfigWidget::slotEventSelected(KNotifyConfigElement *e)
{
    if (d->currentElement) {
        d->actionsconfig->save(d->currentElement);
    }
    d->currentElement = e;
    if (e) {
        d->actionsconfig->setConfigElement(e);
        d->actionsconfig->setEnabled(true);
    } else {
        d->actionsconfig->setEnabled(false);
    }

}

void KNotifyConfigWidget::save()
{
    if (d->currentElement) {
        d->actionsconfig->save(d->currentElement);
    }

    d->eventList->save();
    emit changed(false);

    //ask KNotification objects to reload their config
    QDBusMessage message = QDBusMessage::createSignal("/Config", "org.kde.knotification", "reparseConfiguration");
    message.setArguments(QVariantList() << d->application);
    QDBusConnection::sessionBus().send(message);
}

void KNotifyConfigWidget::revertToDefaults()
{
    d->eventList->fill(d->application, d->contextName, d->contextValue, true);
    emit changed(true);
}

KNotifyConfigWidget *KNotifyConfigWidget::configure(QWidget *parent, const QString &appname)
{
    QDialog *dialog = new QDialog(parent);
    dialog->setWindowTitle(i18n("Configure Notifications"));

    KNotifyConfigWidget *w = new KNotifyConfigWidget(dialog);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(w);
    layout->addWidget(buttonBox);
    dialog->setLayout(layout);

    connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), w, SLOT(save()));
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), w, SLOT(save()));
    connect(w, SIGNAL(changed(bool)), buttonBox->button(QDialogButtonBox::Apply), SLOT(setEnabled(bool)));

    connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    w->setApplication(appname);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    return w;
}

void KNotifyConfigWidget::slotActionChanged()
{
    emit changed(true);   //TODO
    if (d->currentElement) {
        d->actionsconfig->save(d->currentElement);
        d->eventList->updateCurrentItem();
    }
}

