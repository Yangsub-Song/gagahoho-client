#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "hal/linux/linux.h"
#include "hal/gpio/gpio.h"
#include "util/database.h"
#include "util/smsreceiver.h"

class RFModule;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum CurrentUIMode {
    UI_ON_APP,
    UI_ON_CALENDAR,
    UI_ON_REPLAY,
};

enum KeyInput {
    KEYINPUT_CALENDAR,
    KEYINPUT_REPLAY,
    KEYINPUT_STOP_REPLAY,
    KEYINPUT_PREV,
    KEYINPUT_NEXT,
};

void onKeyInput(KeyInput i);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    Ui::MainWindow *ui;
    CurrentUIMode uiMode;

    Gpio *gpio;
    DataBase *db;
    Linux *os;

    QTimer *keyMixTimer;
    QList<int> keyMixes;
    QList<QString> replayList;

    QTimer *calendarTimer;

    RFModule *rfModule;

    int currentVolume;
    int replayCursor;
    int currentChannel;

    void systemInit();
    void databaseInit();
    void audioInit();
    void rfModuleInit();

    void replayStart();
    void replayStop();
    void replayPlayNext();
    void onKeyInput(KeyInput i);

private slots:
    void onGpioInterrupted(int pin);
    void onKeyMixer();

signals:
    void createNotification(QString message);
    void volumeChanged(int v);
    void playMessage(QString path);
    void stopMessage();
    void changeNews(QString txt);
    void requestSyncTime();
    void calendarNext();
    void calendarPrev();
    void timeSync(long sec, long usec);
    void powerOff();

};
#endif // MAINWINDOW_H
