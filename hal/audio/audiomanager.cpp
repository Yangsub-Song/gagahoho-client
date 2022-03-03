#include "hal/audio/audiomanager.h"

#include <QSound>
#include <QDebug>

AudioManager::AudioManager(int volume, QObject *parent) : QObject(parent)
{
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/mpeg");
    audioSettings.setQuality(QMultimedia::HighQuality);

    player = new QMediaPlayer(this);
    player->setVolume(volume);

    recorder = new QAudioRecorder(this);
    recorder->setEncodingSettings(audioSettings);

    connect(player, &QMediaPlayer::stateChanged, [this](QMediaPlayer::State state) {

        if (state != QMediaPlayer::PlayingState) {
            emit audioPlayEnd();
        }
    });
}

void AudioManager::play(QString path) {
    this->player->setMedia(QUrl::fromLocalFile(path));
    this->player->play();
}

void AudioManager::stopPlay() {
    this->player->stop();
}

bool AudioManager::isAudioRecording() {
    return audioRecording;
}

void AudioManager::recordStart(QString path) {
    if (audioRecording) {
        qInfo() << "Audio is Recording now";
        return;
    }

    recorder->setOutputLocation(QUrl::fromLocalFile(path));
    recorder->record();
    audioRecording = true;
}

void AudioManager::recordEnd() {
    if (audioRecording) {
        recorder->stop();
        audioRecording = false;
    }
}

void AudioManager::onVolumeChanged(int volume) {
    this->player->setVolume(volume);
}
