#include <QDebug>
#include <QFile>
#include "database.h"

#define DATABASE_NAME_GAGAHOHO "/var/lib/gagahoho/gagahoho.db"

#define DATABASE_TABLE_EXIST_QUERY \
    "SELECT name FROM sqlite_master WHERE type='table'" \
    "AND name='AUDIOS';"

#define DATABASE_INSERT_NEW_FILE \
    "INSERT INTO AUDIOS (PATH) VALUES(?);"

#define DATABASE_CREATE_TABLE_QUERY \
    "CREATE TABLE AUDIOS (" \
    "PATH TEXT NOT NULL," \
    "ID INTEGER PRIMARY KEY AUTOINCREMENT" \
    ");"

#define DATABASE_GET_LAST_FILES \
    "SELECT PATH FROM AUDIOS ORDER BY ROWID ASC LIMIT 10;"

#define DATABASE_SQUASH_HISTORY \
    "DELETE FROM AUDIOS WHERE NOT ID in (SELECT ID FROM AUDIOS ORDER BY ROWID DESC LIMIT 10);"

#define DATABASE_GET_OLD_PATHS \
    "SELECT PATH FROM AUDIOS WHERE NOT ID in (SELECT ID FROM AUDIOS ORDER BY ROWID DESC LIMIT 10);"

DataBase::DataBase(QObject *parent) : QObject(parent)
{
    int rc = sqlite3_open(DATABASE_NAME_GAGAHOHO, &db);
    assert(rc == SQLITE_OK);

    if (!isTableExist()) {
        qDebug() << "Create Tables";
        initTables();
    }
}

bool DataBase::executeQuery(QString query) {
    sqlite3_stmt *stmt;
    bool result = true;

    sqlite3_prepare(db, query.toStdString().c_str(), -1, &stmt, NULL);

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        qDebug() << "Failed to execute query : " << QString(sqlite3_errmsg(this->db));
        result = false;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return result;
}

int DataBase::initTables() {
    sqlite3_stmt * stmt;
    bool result = true;

    sqlite3_prepare(db, DATABASE_CREATE_TABLE_QUERY, -1, &stmt, NULL);

    executeQuery("BEGIN TRANSACTION;");
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        qDebug() << "Failed to execute query : " << QString(sqlite3_errmsg(this->db));
        result = false;
    }  // commit
    executeQuery("END TRANSACTION;");

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return result;
}

int DataBase::isTableExist() {
    sqlite3_stmt *stmt;
    bool result = true;

    sqlite3_prepare(db, DATABASE_TABLE_EXIST_QUERY, -1, &stmt, NULL);

    if(sqlite3_step(stmt) != SQLITE_ROW) {
        qDebug() << "Failed to execute query : " << QString(sqlite3_errmsg(this->db));
        result = false;
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return result;
}

QList<QString> DataBase::getFiles() {
    sqlite3_stmt *stmt;
    QList<QString> files;

    sqlite3_prepare(db, DATABASE_GET_LAST_FILES, -1, &stmt, NULL);
    while (sqlite3_step(stmt) == SQLITE_ROW) {

        int type = sqlite3_column_type(stmt, 0);
        if (type == SQLITE_TEXT) {
            files.append(QString(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
        }
    }

    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return files;
}

int DataBase::newFile(QString path) {
    bool result = true;
    sqlite3_stmt *stmt;

    // Squash History
    executeQuery(DATABASE_SQUASH_HISTORY);

    sqlite3_prepare(db, DATABASE_INSERT_NEW_FILE, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, path.toUtf8().constData(), -1, SQLITE_STATIC);

    executeQuery("BEGIN TRANSACTION;");
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        qDebug() << "Failed to Insert New File : " << QString(sqlite3_errmsg(this->db));
        result = false;
    }
    executeQuery("END TRANSACTION;");
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
    return result;
}

void DataBase::squashHistory() {
    sqlite3_stmt *stmt;

    executeQuery("BEGIN TRANSACTION;");

    sqlite3_prepare(db, DATABASE_GET_OLD_PATHS, -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int type = sqlite3_column_type(stmt, 0);
        if (type == SQLITE_TEXT) {
            QString filePath(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            QFile oldFile(filePath);
            oldFile.remove();
        }
    }
    executeQuery(DATABASE_SQUASH_HISTORY);
    executeQuery("END TRANSACTION;");
    sqlite3_reset(stmt);
    sqlite3_finalize(stmt);
}
