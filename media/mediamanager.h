#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <QObject>

#include "audiomanager.h"

#include "util/smsreceiver.h"
#include "util/filedownloader.h"
#include "util/audiomanager.h"
#include "util/rfmodule.h"

class MediaManager : public QObject
{
    Q_OBJECT
public:
    explicit MediaManager(int volume, QObject *parent = nullptr);

private:
    AudioManager *audioPlayer;
    RFModule *rf;

#if (defined SERVER) && (SERVER == 1)
    SMSReceiver *smsReceiver;
#endif

    FileDownloader *fileDownloader;

private slots:
#if (defined SERVER) && (SERVER == 1)
    void onSMSReceived(QString text);
    void onFileDownloaded(QString path);
#endif

#if !(defined SERVER) || (SERVER == 0)
    void onMessageReceived(QString path);
#endif
    void onVolumeChanged(int v);


signals:
    void volumeChange(int v);
    void startSMSReceiver();
    void requestAudioPlay(QString path);
    void broadcastToRF(QString path, uint8_t grp);
};

#endif // MEDIAMANAGER_H
