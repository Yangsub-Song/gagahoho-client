#include "timedisplay.h"
#include "ui_timedisplay.h"

#include <QDateTime>
#include "util/dateutil.h"
#include <QDebug>

#define LABEL_STYLESHEET_SATURDAY \
    "background-color :#160C4B; color : blue; border-style: solid; border-width: 0px; border-color: #000000;"
#define LABEL_STYLESHEET_SUNDAY \
    "background-color :#160C4B; color : red;  border-style: solid; border-width: 0px; border-color:  #000000;"
#define LABEL_STYLESHEET_NORMAL \
    "background-color :#160C4B; color : white;  border-style: solid; border-width: 0px; border-color:  #000000;"

TimeDisplay::TimeDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeDisplay)
{
    ui->setupUi(this);

    tickTimer = new QTimer(this);
    tickTimer->setInterval(1000);
    connect(tickTimer, SIGNAL(timeout()), this, SLOT(onTickTimer()));
    tickTimer->start();

    notificationTimer = new QTimer(this);
    notificationTimer->setSingleShot(true);
    connect(notificationTimer, SIGNAL(timeout()), this, SLOT(onHideNotification()));
}


void TimeDisplay::onTickTimer() {
    QDateTime currentTime = QDateTime::currentDateTime();

    int dayOfWeek = currentTime.date().dayOfWeek();

    ui->LabelAMPM->setText(currentTime.toString("AP"));
    ui->LabelYear->setText(currentTime.toString("yyyy년"));
    ui->LabelDate->setText(currentTime.toString("M월 dd일"));

    int hour = currentTime.toString("hh").toInt();

    if (hour > 12) {
        // AM, PM
        hour -= 12;
    }

    ui->LabelHour->setText(QString::number(hour) + currentTime.toString(":mm"));
    ui->LabelSec->setText(currentTime.toString(":ss"));

    ui->LabelDayofWeek->setText(currentTime.toString("ddd요일"));

    QDate lunarDate = DateUtil::solarToLunar(currentTime.date());

    ui->LabelLunarDate->setText(QString("%1월 ").arg(lunarDate.month(), 2, 10, QChar(' ')) +
                                QString("%1일").arg(lunarDate.day(), 2, 10, QChar(' ')));

    ui->LabelSpecialDay->setText(DateUtil::Check_TodaySpecial());


    if (dayOfWeek == 6) {
        ui->LabelDayofWeek->setStyleSheet(LABEL_STYLESHEET_SATURDAY);
    } else if (dayOfWeek == 7) {
        ui->LabelDayofWeek->setStyleSheet(LABEL_STYLESHEET_SUNDAY);
    } else {
        ui->LabelDayofWeek->setStyleSheet(LABEL_STYLESHEET_NORMAL);
    }
}

void TimeDisplay::createNotification(QString msg) {
    ui->LabelNotification->setText(msg);
    notificationTimer->start(3000);

}

void TimeDisplay::onHideNotification() {
    ui->LabelNotification->setText("");
}

TimeDisplay::~TimeDisplay()
{
    delete ui;
}
