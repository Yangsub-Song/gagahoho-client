#include <QThread>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calendar/calendarview.h"

#include "util/database.h"

#include "hal/rf/rfmodule.h"
#include "hal/audio/audiomanager.h"

#include "clientapp.h"
#include "hal/env/envmonitor.h"

#define GPIO_PIN_KEY_LEFT 20
#define GPIO_PIN_KEY_RIGHT 21
#define GPIO_PIN_KEY_REPLAY 16
#define GPIO_PIN_KEY_CALENDAR 12
#define GPIO_PIN_AMP 23
#define GPIO_PIN_RF_OPTION 1
#define GPIO_PIN_POWER_STATUS 22
#define GPIO_PIN_CHANNEL_ONE 26
#define GPIO_PIN_CHANNEL_TWO 19
#define GPIO_PIN_CHANNEL_THREE 13
#define GPIO_PIN_CHANNEL_FOUR 6
#define GPIO_PIN_CHANNEL_FIVE 5

static const int GPIO_PIN_CHANNEL_ARR[5] = {
    GPIO_PIN_CHANNEL_ONE,
    GPIO_PIN_CHANNEL_TWO,
    GPIO_PIN_CHANNEL_THREE,
    GPIO_PIN_CHANNEL_FOUR,
    GPIO_PIN_CHANNEL_FIVE
};

#define STACKEDVIEW_APP 0
#define STACKEDVIEW_CALENDAR 1

#define KEYMIX_TIME 100

#define CALENDAR_MAX_APPEAR_TIME 20000

void MainWindow::systemInit() {
    os = new Linux(this);
    connect(this, SIGNAL(timeSync(long,long)), os, SLOT(onTimeSync(long,long)));
    connect(this, SIGNAL(powerOff()), os, SLOT(onSystemOff()));

    gpio = new Gpio(this);
    connect(gpio, SIGNAL(gpioInterrupted(int)), this, SLOT(onGpioInterrupted(int)));

    gpio->pinInit(GPIO_PIN_AMP, 1); 
    gpio->pinInit(GPIO_PIN_RF_OPTION, 1);

    gpio->pinInit(10,0);  // Audio Check
    gpio->pinInit(GPIO_PIN_KEY_CALENDAR,0);
    gpio->pinInit(GPIO_PIN_KEY_REPLAY,0);
    gpio->pinInit(GPIO_PIN_KEY_LEFT,0);
    gpio->pinInit(GPIO_PIN_KEY_RIGHT,0);
    gpio->pinInit(GPIO_PIN_CHANNEL_ONE,0);
    gpio->pinInit(GPIO_PIN_CHANNEL_TWO,0);
    gpio->pinInit(GPIO_PIN_CHANNEL_THREE,0);
    gpio->pinInit(GPIO_PIN_CHANNEL_FOUR,0);
    gpio->pinInit(GPIO_PIN_CHANNEL_FIVE,0);

    gpio->set(GPIO_PIN_AMP, 0);
    gpio->set(GPIO_PIN_RF_OPTION, 0);

    gpio->addInterrupt(GPIO_PIN_KEY_LEFT, false);
    gpio->addInterrupt(GPIO_PIN_KEY_RIGHT, false);
    gpio->addInterrupt(GPIO_PIN_KEY_CALENDAR, false);
    gpio->addInterrupt(GPIO_PIN_KEY_REPLAY, false);
    gpio->addInterrupt(GPIO_PIN_POWER_STATUS, false);

    // Power check manually
    int power = gpio->get(GPIO_PIN_POWER_STATUS);

    if (power == 0) {
        emit powerOff();
    }

    int channels[5];

    for(int i = 0; i < 5; i++) {
        channels[i] = gpio->get(GPIO_PIN_CHANNEL_ARR[i]);
        qDebug() << "channels[" + QString::number(i) + "] : " << QString::number(channels[i]);
    }


    currentChannel = (channels[0] << 4 |
                    channels[1] << 3 |
                    channels[2] << 2 |
                    channels[3] << 1 | channels[4]);

    qDebug() << "currnetChannel : " << QString::number(currentChannel);
}

void MainWindow::databaseInit() {
    QThread *dbThread = new QThread(this);
    db = new DataBase();
    db->moveToThread(dbThread);
    dbThread->start();
}

void MainWindow::audioInit() {

    QThread *audioThread = new QThread(this);
    AudioManager *audio = new AudioManager(currentVolume);
    audio->moveToThread(audioThread);
    audioThread->start();
    connect(this, SIGNAL(volumeChanged(int)), audio, SLOT(onVolumeChanged(int)));
    connect(this, SIGNAL(playMessage(QString)), audio, SLOT(play(QString)));
    connect(this, SIGNAL(stopMessage()), audio, SLOT(stopPlay()));
    connect(audio, &AudioManager::audioPlayEnd, [this]() {
        qDebug() << "Play End";

        if (uiMode == UI_ON_REPLAY) {
            replayPlayNext();
        } else if (uiMode != UI_ON_REPLAY) {
            gpio->set(GPIO_PIN_AMP, 0);
        }
    });
}

void MainWindow::rfModuleInit() {

    QThread *rfThread = new QThread(this);
    rfModule = new RFModule(currentChannel);

    connect(rfModule, &RFModule::rfModuleInitialized, [this]() {
        gpio->set(GPIO_PIN_RF_OPTION, 1);
        QThread::msleep(100);
        emit requestSyncTime();

    });

    connect(rfModule, &RFModule::audioReceived, [this](QString path) {
        db->squashHistory();
        db->newFile(path);
        qDebug() << "path : " << path;
        if (uiMode == UI_ON_APP) {
            gpio->set(GPIO_PIN_AMP, 1);
            emit playMessage(path);
        }
    });

    connect(rfModule, &RFModule::textReceived, [this](QString text) {
        // New Text
        emit changeNews(text);
    });

    connect(rfModule, &RFModule::timeReceived, [this](QString time) {
        qDebug() << "Time Received";
        qDebug() << time;

        QStringList times = time.split(",");

        if (times.size() >= 2) {
            emit timeSync(times[0].toLong(), times[1].toLong());
        }
    });

    connect(this, SIGNAL(requestSyncTime()), rfModule, SLOT(onRequestSyncTime()));

    rfModule->moveToThread(rfThread);
    rfThread->start();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , uiMode(UI_ON_APP)
    , currentVolume(50)
    , replayCursor(0)
    , currentChannel(0)
{
    ui->setupUi(this);
    systemInit();
    databaseInit();
    audioInit();
    rfModuleInit();

    keyMixTimer = new QTimer(this);
    keyMixTimer->setSingleShot(true);
    connect(keyMixTimer, SIGNAL(timeout()), this, SLOT(onKeyMixer()));

    ClientApp *clientApp = new ClientApp();
    ui->ViewControl->addWidget(clientApp);
    connect(this, SIGNAL(createNotification(QString)), clientApp, SLOT(onNotification(QString)));
    connect(this, SIGNAL(changeNews(QString)), clientApp, SLOT(onNewsChanged(QString)));

    QThread *envThread = new QThread(this);
    EnvMonitor *envMonitor = new EnvMonitor();
    envMonitor->moveToThread(envThread);
    connect(envMonitor, SIGNAL(onDataFromEnvMonitor(EnvData)), clientApp, SLOT(onEnvDataReceived(EnvData)));
    connect(envMonitor, &EnvMonitor::groupSelected, [this](int grp) {
        grp += 1;
        rfModule->onSetGroupId(grp);
    });
    connect(envMonitor, SIGNAL(alertAlarm(QString,QString)), rfModule, SLOT(onAlertAlarm(QString, QString)));
    envThread->start();

    CalendarView *calendarView = new CalendarView();
    ui->ViewControl->addWidget(calendarView);
    connect(this, SIGNAL(calendarNext()), calendarView, SLOT(onNextButton()));
    connect(this, SIGNAL(calendarPrev()), calendarView, SLOT(onBackButton()));

    calendarTimer = new QTimer(this);
    connect(calendarTimer, &QTimer::timeout, [this]() {
        if (uiMode == UI_ON_CALENDAR) {
            ui->ViewControl->widget(STACKEDVIEW_APP)->show();
            ui->ViewControl->widget(STACKEDVIEW_CALENDAR)->hide();
            uiMode == UI_ON_APP;
        }
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onKeyMixer() {

    if (keyMixes.contains(GPIO_PIN_KEY_REPLAY) &&
            keyMixes.contains(GPIO_PIN_KEY_CALENDAR)) {
        onKeyInput(KEYINPUT_STOP_REPLAY);
    } else {
        int v = keyMixes.at(0);
        switch(v) {

        case GPIO_PIN_KEY_REPLAY:
            onKeyInput(KEYINPUT_REPLAY);
            break;

        case GPIO_PIN_KEY_CALENDAR:
            onKeyInput(KEYINPUT_CALENDAR);
            break;

        case GPIO_PIN_KEY_LEFT:
            onKeyInput(KEYINPUT_PREV);
            break;

        case GPIO_PIN_KEY_RIGHT:
            onKeyInput(KEYINPUT_NEXT);
            break;
        }

    }

    keyMixes.clear();
}

void MainWindow::replayStart() {
    gpio->set(GPIO_PIN_AMP, 1);

    replayList = db->getFiles();
    replayCursor = 0;

    if (replayList.size() > 0) {
        qDebug() << "replayStart : " << replayList.at(replayCursor);
        emit createNotification(QString("%1 번 파일 재생중").arg(replayCursor+1));
        emit playMessage(replayList.at(replayCursor));
    }
}

void MainWindow::replayStop() {
    gpio->set(GPIO_PIN_AMP, 0);
    replayCursor = 0;
    emit stopMessage();
}

void MainWindow::replayPlayNext() {
    emit stopMessage();

    if(++replayCursor >= replayList.size())
        replayCursor = 0;
    
    emit createNotification(QString("%1 번 파일 재생중").arg(replayCursor+1));
    gpio->set(GPIO_PIN_AMP, 1);
    emit playMessage(replayList.at(replayCursor));
}

void MainWindow::onKeyInput(KeyInput i) {
    qDebug() << "KeyInput : " << i;

    switch(i) {
    case KEYINPUT_CALENDAR:
        if (uiMode == UI_ON_APP) {
            uiMode = UI_ON_CALENDAR;
            ui->ViewControl->widget(STACKEDVIEW_APP)->hide();
            ui->ViewControl->widget(STACKEDVIEW_CALENDAR)->show();
            calendarTimer->start(CALENDAR_MAX_APPEAR_TIME);
        } else if (uiMode == UI_ON_CALENDAR) {
            uiMode = UI_ON_APP;
            ui->ViewControl->widget(STACKEDVIEW_APP)->show();
            ui->ViewControl->widget(STACKEDVIEW_CALENDAR)->hide();
            calendarTimer->stop();
        }
        break;

    case KEYINPUT_REPLAY:
        if (uiMode == UI_ON_APP) {
            uiMode = UI_ON_REPLAY;
            replayStart();
        } else if (uiMode == UI_ON_REPLAY) {
            replayPlayNext();
        }
        break;

    case KEYINPUT_STOP_REPLAY:
        if (uiMode == UI_ON_REPLAY){
            uiMode = UI_ON_APP;
            replayStop();
        }

        break;

    case KEYINPUT_PREV:
        if (uiMode == UI_ON_APP || uiMode == UI_ON_REPLAY) {
            if(--currentVolume < 0)
                currentVolume = 0;
            emit createNotification(QString("음량 : %1\%  ").arg(currentVolume, 3, 10, QChar(' ')));
            emit volumeChanged(currentVolume);
        } else if (uiMode == UI_ON_CALENDAR) {
            emit calendarPrev();
        }
        break;

    case KEYINPUT_NEXT:
        if (uiMode == UI_ON_APP || uiMode == UI_ON_REPLAY) {
            if(++currentVolume > 100)
                currentVolume = 0;
            emit createNotification(QString("음량 : %1\%  ").arg(currentVolume, 3, 10, QChar(' ')));
            emit volumeChanged(currentVolume);
        } else  if (uiMode == UI_ON_CALENDAR) {
            emit calendarNext();

            // play Next
        }
        break;
    }
}

void MainWindow::onGpioInterrupted(int pin) {
    qDebug() << "onGpioInterrupted : " << pin;

    switch(pin) {

    case GPIO_PIN_KEY_REPLAY:
        keyMixes.append(GPIO_PIN_KEY_REPLAY);
        break;

    case GPIO_PIN_KEY_CALENDAR:
        keyMixes.append(GPIO_PIN_KEY_CALENDAR);
        break;

    case GPIO_PIN_KEY_LEFT:
        keyMixes.append(GPIO_PIN_KEY_LEFT);
        break;

    case GPIO_PIN_KEY_RIGHT:
        keyMixes.append(GPIO_PIN_KEY_RIGHT);
        break;

    case GPIO_PIN_POWER_STATUS:
        emit powerOff();
        return;
    }


    keyMixTimer->start(KEYMIX_TIME);
}
