#ifndef DB_CTRL_H
#define DB_CTRL_H

#include <QDateTime>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtSql>

#include "config.h"

class DB_Ctrl
{
  public:
    DB_Ctrl();

    // Open&Close DB
    bool open_db();
    void close_db();

    // Command Table
    bool CreateCmdTable();
    bool DeleteCmdTable();
    bool InsertCommand(const int &row, const QString &name, const bool &hex, const QString &cmd);
    bool UpdateCommand(const int &row, const QString &name, const bool &hex, const QString &cmd);
    bool ReadCommand(const int &row, QString &name, bool &hex, QString &cmd);
    bool DeleteCommand(const int &row);
    int GetCommandNum();

    // Setting Table
    bool DeleteSettingTable();
    bool CreateSettingTable();
    bool InsertSetting(const QString &parameter, const QString &value);
    bool UpdateSetting(const QString &parameter, const QString &value);
    bool ReadSetting(const QString &parameter, QString &value);
    bool DeleteSetting(const QString &parameter);
    void InsertDefaultSetting();

  private:
    QSqlDatabase db;
};

#endif // DB_CTRL_H
