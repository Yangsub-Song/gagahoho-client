#ifndef TIMEDISPLAY_H
#define TIMEDISPLAY_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class TimeDisplay;
}

class TimeDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit TimeDisplay(QWidget *parent = nullptr);
    ~TimeDisplay();

private:
    Ui::TimeDisplay *ui;
    QTimer *tickTimer;
    QTimer *notificationTimer;

public slots:
    void createNotification(QString msg);

private slots:
    void onTickTimer();
    void onHideNotification();
};

#endif // TIMEDISPLAY_H
