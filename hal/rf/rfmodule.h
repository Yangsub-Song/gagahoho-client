#ifndef RFMODULE_H
#define RFMODULE_H

#include <QObject>
#include <QSerialPort>

#include <stdint.h>

#include "rfpacket.h"

enum PacketHandShakes {
    RF_PACKET_HANDSHAKE_NONE,
    RF_PACKET_HANDSHAKE_AUDIO,
    RF_PACKET_HANDSHAKE_TIME,
    RF_PACKET_HANDSHAKE_TEXT,
    RF_PACKET_HANDSHAKE_ALERT,
    RF_PACKET_INTERNAL_PACKET,
    RF_PACKET_TYPE_HANDSHAKE,
    RF_PACKET_TYPE_DATA,
    RF_PACKET_TYPE_DATA_END,
    RF_PACKET_TYPE_REQUEST_TIMESYNC
};

Q_DECLARE_METATYPE(PacketHandShakes)

class RFModule : public QObject
{
    Q_OBJECT
public:
    explicit RFModule(QObject *parent = nullptr);
    explicit RFModule(QString path, int channel, QObject *parent = nullptr);
    explicit RFModule(int grp, QObject *parent = nullptr);

private:
    QSerialPort *port = nullptr;
    QByteArray stagingBuffer;
    QByteArray receiveBuffer;

    int currentChannel = -1;
    int currentGroup = 1;
    bool rfSettingMode = false;
    bool isGroupSet = false;
    bool handShaked = false;
    bool parsePacket(QByteArray &bytes, RFPacket *packet);

    bool handShake(uint8_t grp, PacketHandShakes type);
    bool data(QByteArray &arr, uint8_t grp);
    bool dataEnd(uint16_t arrSize, uint8_t grp);
    QString generateFilePath();
    bool rfSend(QByteArray *arr);
    void broadcast(QByteArray &arr, uint8_t grpId, PacketHandShakes type);
    void syncTime(int grp);
    void setRFChannel(int channel);
    PacketHandShakes handShakedType = RF_PACKET_HANDSHAKE_NONE;

public slots:
    void onBroadcast(QString path, uint8_t grpId, PacketHandShakes type);
    void onBroadcast(QByteArray arr, uint8_t grpId, PacketHandShakes type);
    void onRequestSyncTime();
    void onSetGroupId(int id);
    void onAlertAlarm(QString individual, QString kind);

signals:
    void audioReceived(QString path);
    void textReceived(QString text);
    void timeReceived(QString time);
    void setGpio(int gpio, int v);
    void rfModuleInitialized();

private slots:
    void onReadyRead();

};

#endif // RFMODULE_H
