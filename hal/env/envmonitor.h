#ifndef ENVMONITOR_H
#define ENVMONITOR_H

#include <QObject>
#include <QSerialPort>

struct EnvData
{
    QString temp;
    QString humidity;
    QString gas;
    QString co;
};
Q_DECLARE_METATYPE(EnvData)

class EnvMonitor : public QObject
{
    Q_OBJECT
public:
    explicit EnvMonitor(QObject *parent = nullptr);

private:
    QSerialPort *port;
    QString readBuffer;

private slots:
    void onSendEnvMonitor(QByteArray data);
    void onReadyRead();

signals:
    void onDataFromEnvMonitor(EnvData data);
    void groupSelected(int grp);
    void alertAlarm(QString inv, QString kind);
};

#endif // ENVMONITOR_H
