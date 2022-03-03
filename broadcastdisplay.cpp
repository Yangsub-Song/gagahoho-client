#include "broadcastdisplay.h"
#include "ui_broadcastdisplay.h"

BroadCastDisplay::BroadCastDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BroadCastDisplay)
{
    ui->setupUi(this);
}

BroadCastDisplay::~BroadCastDisplay()
{
    delete ui;
}

void BroadCastDisplay::setBroadcastMessage(QString msg) {
    ui->labelBroadcastMsg->setText(msg);
}
