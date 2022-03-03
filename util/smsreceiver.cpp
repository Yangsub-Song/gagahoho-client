#include "smsreceiver.h"

#include <QDebug>

#include <QMap>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusArgument>

#define DBUS_BUS_NAME QString("org.freedesktop.ModemManager1")
#define DBUS_OBJECT_PATH QString("/org/freedesktop/ModemManager1")
#define MODEM_MANAGER_OBJECT_MANAGER QString("org.freedesktop.DBus.ObjectManager")
#define MODEM_MANAGER_MESSAGING QString("org.freedesktop.ModemManager1.Modem.Messaging")
#define MODEM_MANAGER_MESSAGE_PROPERTIES QString("org.freedesktop.ModemManager1.Sms")

SMSReceiver::SMSReceiver(QObject *parent) : QObject(parent)
{



}

QString SMSReceiver::getAvailableModemPath() {
    QDBusInterface intf(DBUS_BUS_NAME,
                               DBUS_OBJECT_PATH,
                               MODEM_MANAGER_OBJECT_MANAGER,
                               QDBusConnection::systemBus(), this);

    QDBusMessage modemObjects = intf.call("GetManagedObjects");
    QMap<QString, QMap<QString, QVariant>> objectMap;
    QDBusArgument args = modemObjects.arguments().at(0).value<QDBusArgument>();
    args >> objectMap;
    auto keys = objectMap.keys();

    if (keys.size() > 0) {
        return keys.at(0);
    }

    return "";
}

bool SMSReceiver::registerMessageAdded(QString path) {
    return QDBusConnection::systemBus().connect(DBUS_BUS_NAME,
                                                path,
                                                MODEM_MANAGER_MESSAGING,
                                                "Added",
                                                this,
                                                SLOT(onSMSNotified(QDBusObjectPath, bool)));
}

void SMSReceiver::deleteMessage(QString modemPath, QString messagePath) {
    QDBusInterface intf(DBUS_BUS_NAME,
                               modemPath,
                               MODEM_MANAGER_MESSAGING,
                               QDBusConnection::systemBus(), this);
    intf.call("Delete", QVariant::fromValue(QDBusObjectPath(messagePath)));
}

void SMSReceiver::deleteAllMessages(QString path) {
    QDBusInterface intf(DBUS_BUS_NAME,
                               path,
                               MODEM_MANAGER_MESSAGING,
                               QDBusConnection::systemBus(), this);

    QDBusMessage messageObjects = intf.call("List");
    QDBusArgument args = messageObjects.arguments().at(0).value<QDBusArgument>();
    QList<QDBusObjectPath> messageList;

    args >> messageList;

    if (messageList.length() > 0) {
        Q_FOREACH (const QDBusObjectPath& p, messageList) {
            intf.call("Delete", QVariant::fromValue(p));
        }
    }
}


void SMSReceiver::onStartReceiver() {
    QString modem = getAvailableModemPath();

    if (modem.length() > 0) {
        deleteAllMessages(modem);
        registerMessageAdded(modem);
    }
}

QString SMSReceiver::getMessageText(QString messagePath) {
    QDBusInterface intf(DBUS_BUS_NAME,
                               messagePath,
                               MODEM_MANAGER_MESSAGE_PROPERTIES,
                               QDBusConnection::systemBus(), this);

    QVariant t = intf.property("Text");

    return t.toString();
}

void SMSReceiver::onSMSNotified(QDBusObjectPath path, bool received) {
    qDebug() << "sms notified : " << path.path();
    QString t = getMessageText(path.path());
    emit smsReceived(t);
}
