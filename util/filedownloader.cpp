#include "filedownloader.h"

#include <QFile>

FileDownloader::FileDownloader(QString url, QString path, QObject *parent) : QObject(parent)
{
    this->url = url;
    this->path = path;
    connect(&network, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));
}

void FileDownloader::download() {
    QNetworkRequest request(url);
    network.get(request);
}

void FileDownloader::fileDownloaded(QNetworkReply *pReply) {
    QFile f(path);
    QByteArray data = pReply->readAll();

    f.open(QFile::WriteOnly);
    f.write(data);
    f.close();

    pReply->deleteLater();
    emit downloaded(path);
}
