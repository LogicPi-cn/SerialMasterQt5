#ifndef DB_CTRL_H
#define DB_CTRL_H

#include <QDateTime>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QtSql>

// Crypted or NOT
//#define CRYPTED

class DB_Ctrl
{
  public:
    DB_Ctrl();

    // Open&Close DB
    bool open_db();
    void close_db();

    // 创建 Table
    bool create_cmd_table();

    // 插入一条命令
    bool add_command(const int &row, const QString &name, const bool &hex, const QString &cmd);

    // 更新命令
    bool update_command(const int &row, const QString &name, const bool &hex, const QString &cmd);

    // 读取一条命令
    bool read_command(const int &row, QString &name, bool &hex, QString &cmd);

    // 删除一条命令
    bool delete_command(const int &row);

    // 获取行数
    int get_row_cnt();

    // Clear Table
    bool clear_table();

    // insert a setting
    bool insert_setting(const QString &parameter, const QString &value);

    // update setting
    bool update_setting(const QString &parameter, const QString &value);
    // read setting
    QString get_setting(const QString &parameter);
    // delete setting
    bool delete_setting(const QString &parameter);

    const QString dbName = "command.db";

  private:
    QSqlDatabase db;
};

#endif // DB_CTRL_H
