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
    bool openDB();
    void closeDB();

    // Command Table
    bool createCmdTable();
    bool deleteCmdTable();
    bool insertCommand(const int &row, const QString &name, const bool &hex, const QString &cmd);
    bool updateCommand(const int &row, const QString &name, const bool &hex, const QString &cmd);
    bool getCommand(const int &row, QString &name, bool &hex, QString &cmd);
    bool deleteCommand(const int &row);
    int getCommandNum();

    // Setting Table
    bool deleteSettingTable();
    bool createSettingTable();
    bool insertSetting(const QString &parameter, const QString &value);
    bool updateSetting(const QString &parameter, const QString &value);
    bool getSetting(const QString &parameter, QString &value);
    bool deleteSetting(const QString &parameter);
    void insertDefaultSetting();

  private:
    QSqlDatabase db;
};

#endif // DB_CTRL_H
