#ifndef INBROADCASTDISPLAY_H
#define INBROADCASTDISPLAY_H

#include <QWidget>

namespace Ui {
class BroadCastDisplay;
}

class BroadCastDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit BroadCastDisplay(QWidget *parent = nullptr);
    ~BroadCastDisplay();

    void setBroadcastMessage(QString msg);

private:
    Ui::BroadCastDisplay *ui;
};

#endif // INBROADCASTDISPLAY_H
