#ifndef SMSRECEIVER_H
#define SMSRECEIVER_H

#include <QObject>
#include <QDBusObjectPath>

class SMSReceiver : public QObject
{
    Q_OBJECT
public:
    explicit SMSReceiver(QObject *parent = nullptr);

private:
    QString modemPath;
    QString getAvailableModemPath();
    bool registerMessageAdded(QString path);
    void deleteAllMessages(QString path);
    void deleteMessage(QString modemPath, QString messagePath);
    QString getMessageText(QString messagePath);

private slots:
    void onSMSNotified(QDBusObjectPath path, bool received);
    void onStartReceiver();

signals:
    void smsReceived(QString text);
};

#endif // SMSRECEIVER_H
