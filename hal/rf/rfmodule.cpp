#include <sys/time.h>
#include <QFile>
#include <QDebug>
#include <QThread>
#include <QtMath>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QRandomGenerator>

#include "rfmodule.h"
#include "rfpacket.h"

#ifndef RF_MODEM_DEFAULT_PATH
#ifdef DESKTOP_DEBUG
#define RF_MODEM_DEFAULT_PATH "/dev/ttyUSB0"
#else
#define RF_MODEM_DEFAULT_PATH "/dev/serial0"
#endif
#endif

#define RF_MODEM_SAVED_FILE_LENGTH 12

#ifndef RF_MODEM_DEFAULT_SAVE_PATH
#define RF_MODEM_DEFAULT_SAVE_PATH "/var/lib/gagahoho/audios/"
#endif

RFModule::RFModule(QObject *parent) : RFModule(RF_MODEM_DEFAULT_PATH, 1, parent)
{
}


RFModule::RFModule(QString path, int channel, QObject *parent)
    : QObject(parent),
      handShakedType(RF_PACKET_HANDSHAKE_NONE),
      rfSettingMode(false) {

    currentChannel = channel;

    qRegisterMetaType<PacketHandShakes>("PacketHandShakes");

    port = new QSerialPort(this);
    port->setPortName(path);
    port->setBaudRate(QSerialPort::Baud19200);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setParity(QSerialPort::NoParity);
    assert(port->open(QSerialPort::OpenModeFlag::ReadWrite));
    connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    qDebug() << "RFModule: Set Channel to " << QString::number(currentChannel);
    setRFChannel(currentChannel);
}

RFModule::RFModule(int grp, QObject *parent)
    : RFModule(RF_MODEM_DEFAULT_PATH, grp, parent)
{

}

void RFModule::setRFChannel(int channel) {
    QByteArray packet;

    rfSettingMode = true;
    packet.append((char)0x40);
    packet.append((char)0x43);
    packet.append((char)0x48);
    packet.append((char)0x00);
    packet.append((char)channel);
    packet.append((char)0x40);

    port->write(packet);
    port->waitForBytesWritten();
    port->flush();
}

bool RFModule::handShake(uint8_t grp, PacketHandShakes type) {
    QByteArray packet;

    packet.append((uint8_t)RF_PACKET_SOP);
    packet.append((uint8_t)RF_PACKET_TYPE_HANDSHAKE);
    packet.append((uint8_t)grp);
    packet.append((char)1);
    packet.append((char)type);
    return rfSend(&packet);
}


bool RFModule::parsePacket(QByteArray &bytes, RFPacket *packet) {
    uint16_t len = 0;
    int sop_index = -1;
    int real_size = 0;

    if (bytes.size() > 0) {
        sop_index = stagingBuffer.indexOf(RF_PACKET_SOP);

        if (sop_index >= 0) {
            real_size = bytes.size() - sop_index;

            if (real_size >= 4) {
                len = bytes.at(3 + sop_index);

                if (real_size >= 4 + len) {
                    // All Data is Found

                    packet->type = bytes.at(1 + sop_index);
                    packet->grp = bytes.at(2 + sop_index);
                    packet->len = len;

                    for(int i = 0; i < packet->len; i++) {
                        packet->data[i] = bytes.at(4 + sop_index + i);
                    }

                    bytes.remove(0, 4 + packet->len + sop_index);
                    return true;
                }
            }
        } else {
            sop_index = stagingBuffer.indexOf(0x2a);
            if (sop_index >= 0) {
                int enp_index = stagingBuffer.indexOf(0x2a, sop_index + 1);


                if (enp_index >= 0) {
                    packet->type = RF_PACKET_INTERNAL_PACKET;

                    for(int i = sop_index; i <= enp_index; i++) {
                        packet->data[i] = bytes.at(i);
                    }

                    bytes.remove(sop_index, enp_index - sop_index + 1);
                    return true;
                }

            }
        }

    }

    return false;
}

void RFModule::onSetGroupId(int id) {
    currentGroup = id;
}

RFPacket packet;

void RFModule::onReadyRead() {
    stagingBuffer.append(port->readAll());
    port->flush();

    if (stagingBuffer.size() > 0) {
        while (parsePacket(stagingBuffer, &packet)) {
            qDebug() << "Packet Found : " << packet.type;

            if (packet.type == RF_PACKET_INTERNAL_PACKET) {
                qDebug() << "RF Setting ECHO";
                emit rfModuleInitialized();
            } else if (packet.type == RF_PACKET_TYPE_REQUEST_TIMESYNC) {
                syncTime(packet.grp);
            } else if (packet.grp == currentGroup || packet.grp == 0xFF) {
                if (packet.type == RF_PACKET_TYPE_HANDSHAKE) {
                    handShaked = true;
                    handShakedType = static_cast<PacketHandShakes>(packet.data[0]);
                    stagingBuffer.clear();
                    receiveBuffer.clear();
                } else if (packet.type == RF_PACKET_TYPE_DATA) {
                    if (handShaked) {
                        receiveBuffer.append((const char*)packet.data, packet.len);
                    } else {
                        stagingBuffer.clear();
                    }
                } else if (packet.type == RF_PACKET_TYPE_DATA_END) {
                    qDebug() << "Data End : " << handShakedType;


                    if (handShaked) {

                        switch(handShakedType) {
                        case RF_PACKET_HANDSHAKE_AUDIO:
                        {
                            QString messageFilePath = generateFilePath();
                            QFile messageFile(messageFilePath);
                            qDebug() << "Emit Signal to parent : " << handShakedType;
                            if (messageFile.open(QFile::OpenModeFlag::ReadWrite)) {
                                messageFile.write(receiveBuffer);
                                messageFile.close();
                                emit audioReceived(messageFilePath);
                            }
                        }
                            break;

                        case RF_PACKET_HANDSHAKE_TIME:
                            emit timeReceived(QString(receiveBuffer));
                            break;

                        case RF_PACKET_HANDSHAKE_TEXT:
                            emit textReceived(QString(receiveBuffer));
                            break;
                        }


                        receiveBuffer.clear();
                        stagingBuffer.clear();
                        handShaked = false;
                        handShakedType = RF_PACKET_HANDSHAKE_NONE;
                    } else {
                        stagingBuffer.clear();
                    }
                }
            }
        }
    }
}

QString RFModule::generateFilePath()
{
    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    QString randomString;

    randomString.append(RF_MODEM_DEFAULT_SAVE_PATH);
    for(int i = 0; i< RF_MODEM_SAVED_FILE_LENGTH; i++)
    {
        int index = QRandomGenerator::global()->generate() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }
    randomString.append(".mp3");

    return QString(randomString);
}

bool RFModule::data(QByteArray &arr, uint8_t grp) {
    QByteArray packet;

    packet.append((uint8_t)RF_PACKET_SOP);
    packet.append((uint8_t)RF_PACKET_TYPE_DATA);
    packet.append((uint8_t)grp);
    packet.append((char)arr.size());
    packet.append(arr);

    return rfSend(&packet);
}

bool RFModule::dataEnd(uint16_t arrSize, uint8_t grp) {
    QByteArray packet;

    packet.append((uint8_t)RF_PACKET_SOP);
    packet.append((uint8_t)RF_PACKET_TYPE_DATA_END);
    packet.append((uint8_t)grp);
    packet.append((char)2);
    packet.append((char)((arrSize & 0xFF00) >> 8));
    packet.append((char)(arrSize & 0x00FF));
    return rfSend(&packet);
}


void RFModule::broadcast(QByteArray &arr, uint8_t grpId, PacketHandShakes type) {
    int dataCursor = 0;
    qDebug() << "onBroadcast() : Data Size " << arr.size();
    handShake(grpId, type);

    while (dataCursor < arr.size()) {
        QByteArray chunk = arr.mid(dataCursor, RF_PACKET_DATA_SIZE);
        data(chunk, grpId);
        dataCursor += chunk.size();
    }

    dataEnd(arr.size(), grpId);
}

void RFModule::onRequestSyncTime() {
    QByteArray packet;

    packet.append((uint8_t)RF_PACKET_SOP);
    packet.append((uint8_t)RF_PACKET_TYPE_REQUEST_TIMESYNC);
    packet.append((uint8_t)0x00);
    packet.append((uint8_t)0x00);
    rfSend(&packet);
}


void RFModule::syncTime(int grp) {
    int dataCursor = 0;
    struct timeval v;
    QString dataStr;
    QByteArray dataArr;

    handShake(grp, RF_PACKET_HANDSHAKE_TIME);

    gettimeofday(&v, NULL);
    dataStr = QString::number(v.tv_sec) + "," + QString::number(v.tv_usec);
    dataArr = dataStr.toUtf8();

    while (dataCursor < dataArr.size()) {
        QByteArray chunk = dataArr.mid(dataCursor, RF_PACKET_DATA_SIZE);
        data(chunk, grp);
        dataCursor += chunk.size();
    }

    dataEnd(dataArr.size(), grp);
}


void RFModule::onBroadcast(QByteArray arr, uint8_t grpId, PacketHandShakes type) {
    broadcast(arr, grpId, type);
}

void RFModule::onBroadcast(QString path, uint8_t grpId, PacketHandShakes type) {
    QFile broadcastFile(path);

    qDebug() << "onBroadCast() : " << path << ", Id : " << grpId;

    if (broadcastFile.open(QIODevice::ReadOnly)) {
        QByteArray fileData = broadcastFile.readAll();
        broadcast(fileData, grpId, type);
    }
}

void RFModule::onAlertAlarm(QString individual, QString kind) {
    //
    //
    QByteArray arr;
    arr.append(individual);
    arr.append("/");
    arr.append(kind);

    broadcast(arr, currentGroup, RF_PACKET_HANDSHAKE_ALERT);
}

// Board -> RF Module (19200Bps) -> 19.2
// RF Module -> RF Module (4800Bps) -> 4.8
bool RFModule::rfSend(QByteArray *arr) {
    bool rc = false;
    double sendDelay = 0;

    if (arr->length() > 0) {
        sendDelay = qCeil(((double)(arr->length() * 10) / 19.2) + 5.0 + ((double)((arr->length() + 10) * 8) / 4.8));

        // Add 100 ms process time for clients
        sendDelay += 100;

        rc = port->write(arr->data(), arr->size()) > 0 ? true : false;
        port->waitForBytesWritten();
        port->flush();

        if (rc) {
            QThread::msleep(sendDelay);
        }
    }

    return rc;
}

