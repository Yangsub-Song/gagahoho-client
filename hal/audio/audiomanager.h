#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QAudioRecorder>

class AudioManager : public QObject
{
    Q_OBJECT
public:
    explicit AudioManager(int volume, QObject *parent = nullptr);
    bool isAudioRecording();

private:
    QMediaPlayer *player;
    QAudioRecorder *recorder;

    bool audioRecording;

private slots:
    void play(QString path);
    void stopPlay();

    void onVolumeChanged(int volume);

    void recordStart(QString path);
    void recordEnd();

signals:
    void audioPlayEnd();
};

#endif // AUDIOPLAYER_H
