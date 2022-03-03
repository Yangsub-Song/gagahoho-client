#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QList>
#include "sqlite3.h"

class DataBase : public QObject
{
    Q_OBJECT
public:
    explicit DataBase(QObject *parent = nullptr);
    int newFile(QString path);
    QList<QString> getFiles();
    void squashHistory();

private:
    sqlite3 *db;
    bool executeQuery(QString query);
    int initTables();
    int isTableExist();

signals:

};

#endif // DATABASE_H
