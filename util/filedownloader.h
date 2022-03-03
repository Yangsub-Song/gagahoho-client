#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QString url, QString path, QObject *parent = nullptr);
    void download();


private:
    QNetworkAccessManager network;
    QString url;
    QString path;

private slots:
    void fileDownloaded(QNetworkReply *pReply);

signals:
    void downloaded(QString path);

};

#endif // FILEDOWNLOADER_H
