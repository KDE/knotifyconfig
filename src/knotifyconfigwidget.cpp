/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2005 Olivier Goffart <ogoffart at kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "knotifyconfigwidget.h"
#include "knotifyconfigactionswidget.h"
#include "knotifyconfigelement.h"
#include "knotifyeventlist.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#ifdef HAVE_DBUS
#include <QDBusConnectionInterface>
#endif

#include <KLocalizedString>

class KNotifyConfigWidgetPrivate
{
public:
    KNotifyEventList *eventList;
    KNotifyConfigActionsWidget *actionsconfig;
    KNotifyConfigElement *currentElement;
    QString application;
};

KNotifyConfigWidget::KNotifyConfigWidget(QWidget *parent)
    : QWidget(parent)
    , d(new KNotifyConfigWidgetPrivate)
{
    d->currentElement = nullptr;
    d->eventList = new KNotifyEventList(this);
    d->eventList->setFocus();
    d->actionsconfig = new KNotifyConfigActionsWidget(this);
    d->actionsconfig->setEnabled(false);
    connect(d->eventList, SIGNAL(eventSelected(KNotifyConfigElement *)), this, SLOT(slotEventSelected(KNotifyConfigElement *)));
    connect(d->actionsconfig, SIGNAL(changed()), this, SLOT(slotActionChanged()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->eventList, 1);
    layout->addWidget(d->actionsconfig);
}

KNotifyConfigWidget::~KNotifyConfigWidget() = default;

void KNotifyConfigWidget::setApplication(const QString &app)
{
    d->currentElement = nullptr;
    d->application = app.isEmpty() ? QCoreApplication::instance()->applicationName() : app;
    d->eventList->fill(d->application);
}

void KNotifyConfigWidget::selectEvent(const QString &eventId)
{
    d->eventList->selectEvent(eventId);
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
    Q_EMIT changed(false);

#ifdef HAVE_DBUS
    // ask KNotification objects to reload their config
    QDBusMessage message =
        QDBusMessage::createSignal(QStringLiteral("/Config"), QStringLiteral("org.kde.knotification"), QStringLiteral("reparseConfiguration"));
    message.setArguments(QVariantList() << d->application);
    QDBusConnection::sessionBus().send(message);
#endif
}

void KNotifyConfigWidget::revertToDefaults()
{
    d->eventList->fill(d->application, true);
    Q_EMIT changed(true);
}

void KNotifyConfigWidget::disableAllSounds()
{
    if (d->eventList->disableAllSounds()) {
        if (d->currentElement) {
            d->actionsconfig->setConfigElement(d->currentElement);
        }
        d->eventList->updateAllItems();
        Q_EMIT changed(true);
    }
}

KNotifyConfigWidget *KNotifyConfigWidget::configure(QWidget *parent, const QString &appname)
{
    QDialog *dialog = createDialog(parent, appname);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
    return dialog->findChild<KNotifyConfigWidget *>();
}

QDialog *KNotifyConfigWidget::createDialog(QWidget *parent, const QString &appname)
{
    QDialog *dialog = new QDialog(parent);
    dialog->setWindowTitle(i18n("Configure Notifications"));

    KNotifyConfigWidget *w = new KNotifyConfigWidget(dialog);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(dialog);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(w);
    layout->addWidget(buttonBox);

    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, w, &KNotifyConfigWidget::save);
    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, w, &KNotifyConfigWidget::save);
    connect(w, &KNotifyConfigWidget::changed, buttonBox->button(QDialogButtonBox::Apply), &QPushButton::setEnabled);

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    w->setApplication(appname);
    return dialog;
}

void KNotifyConfigWidget::slotActionChanged()
{
    Q_EMIT changed(true); // TODO
    if (d->currentElement) {
        d->actionsconfig->save(d->currentElement);
        d->eventList->updateCurrentItem();
    }
}

#include "moc_knotifyconfigwidget.cpp"
