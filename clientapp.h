#ifndef CLIENTAPP_H
#define CLIENTAPP_H

#include <QWidget>
#include "hal/env/envmonitor.h"

namespace Ui {
class ClientApp;
}

class NewsText {

};

class ClientApp : public QWidget
{
    Q_OBJECT

public:
    explicit ClientApp(QWidget *parent = nullptr);
    ~ClientApp();

public slots:
    void onNotification(QString msg);
    void onEnvDataReceived(EnvData data);
    void onNewsChanged(QString news);
    void onNewsChanged();

private:
    Ui::ClientApp *ui;
    QList<QString> newsList;
    QString serializeNewsList();

signals:
    void showNotification(QString msg);
};

#endif // CLIENTAPP_H
