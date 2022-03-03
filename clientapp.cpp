#include "clientapp.h"
#include "ui_clientapp.h"

#include <QDebug>
#include <QTimer>

#define CLIENTAPP_STACKED_TIME_VIEW 0
// 10 minutes
#define CLIENTAPP_DEFAULT_NEWS_TIME 1000 * 60 * 10

ClientApp::ClientApp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientApp)
{
    ui->setupUi(this);

    connect(this, SIGNAL(showNotification(QString)),
            ui->ViewControl->widget(CLIENTAPP_STACKED_TIME_VIEW), SLOT(createNotification(QString)));
}

ClientApp::~ClientApp()
{
    delete ui;
}

QString ClientApp::serializeNewsList() {
    QString newsSerialized;

    Q_FOREACH (const QString& news, newsList) {
        newsSerialized += news;
        newsSerialized += "     ";
    }

    return newsSerialized;
}

void ClientApp::onNewsChanged() {
    ui->LabelNews->setText(serializeNewsList());
}

void ClientApp::onNewsChanged(QString news) {
    newsList.append(news);

    int listIndex = newsList.indexOf(news);

    QTimer *deleteTimer = new QTimer(this);
    deleteTimer->setSingleShot(true);
    connect(deleteTimer,&QTimer::timeout, [this, &news]() {
        if (newsList.size() > 0) {
            newsList.removeAt(0);
            onNewsChanged();
        }
    });
    deleteTimer->start(CLIENTAPP_DEFAULT_NEWS_TIME);

    onNewsChanged();
}

void ClientApp::onEnvDataReceived(EnvData data) {
    qDebug() << "onEnvDataReceived =====";
    ui->labelTemp->setText(data.temp);
    ui->labelHumidity->setText(data.humidity);
    ui->labelGas->setText(data.gas);
    ui->labelCo->setText(data.co);
}

void ClientApp::onNotification(QString msg) {
    qDebug() << "onNotification : " << msg;
    emit showNotification(msg);
}
