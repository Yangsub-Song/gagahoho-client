#include <QThread>


#define DEFAULT_AUDIO_FILE "/tmp/new_message.mp3"

MediaManager::MediaManager(int volume, QObject *parent) : QObject(parent)
{
    qRegisterMetaType<uint16_t>("uint16_t");

#if (defined SERVER) && (SERVER == 1)
    smsReceiver = new SMSReceiver();
    QThread *smsThread = new QThread(this);
    smsReceiver->moveToThread(smsThread);
    smsThread->start();

    connect(smsReceiver, SIGNAL(smsReceived(QString)), this, SLOT(onSMSReceived(QString)));
    connect(this, SIGNAL(startSMSReceiver()), smsReceiver, SLOT(onStartReceiver()));
#endif

    QThread *rfThread = new QThread(this);

    rf = new RFModule(rfThread);
    //rf->moveToThread(rfThread);
    rfThread->start();

    connect(this, SIGNAL(broadcastToRF(QString, uint8_t)), rf, SLOT(onBroadcast(QString, uint8_t)));

    audioPlayer = new AudioManager(volume, this);


    connect(this, SIGNAL(volumeChange(int)), audioPlayer, SLOT(onVolumeChanged(int)));
    connect(this, SIGNAL(requestAudioPlay(QString)), audioPlayer, SLOT(play(QString)));

#if !(defined SERVER) || (SERVER == 0)
    connect(rf, SIGNAL(dataReceived(QString)), this, SLOT(onMessageReceived(QString)));
#endif


    emit startSMSReceiver();
}

void MediaManager::onVolumeChanged(int v) {
    qDebug() << "MediaManager::onVolumeChanged : " << v;
    emit volumeChange(v);
}

#if !(defined SERVER) || (SERVER == 0)
void MediaManager::onMessageReceived(QString path) {
    qDebug() << "onMessageReceived : " << path;
    emit requestAudioPlay(path);
}
#endif

#if (defined SERVER) && (SERVER == 1)
void MediaManager::onSMSReceived(QString text) {
    qDebug() << "SMS Received : " << text;

    fileDownloader = new FileDownloader(text, DEFAULT_AUDIO_FILE, this);
    connect(fileDownloader, SIGNAL(downloaded(QString)), this, SLOT(onFileDownloaded(QString)));
    fileDownloader->download();
}

void MediaManager::onFileDownloaded(QString path) {
    qDebug() << "Downloaded : " << path;

    fileDownloader->deleteLater();

    emit broadcastToRF(path, 0xFFFF);
}
#endif
