#include "db_ctrl.h"

DB_Ctrl::DB_Ctrl() {}

bool DB_Ctrl::openDB()
{
    if (QSqlDatabase::contains(DB_NAME)) {
        db = QSqlDatabase::database(DB_NAME);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", DB_NAME);
        db.setDatabaseName(DB_NAME);
        if (db.open()) {
            if (createCmdTable()) {
                qDebug() << "Create Command Table Success";
            } else {
                qDebug() << "Create Command Table Failed or NO Need to Create";
            }
            if (createSettingTable()) {
                insertDefaultSetting();
                qDebug() << "Create Setting Table Success";
            } else {
                qDebug() << "Create Setting Table Failed or NO Need to Create";
            }
        }
    }

    db.setDatabaseName(DB_NAME);

    if (!db.open()) {
        qDebug() << "Database Error!";
        return false;
    } else {
        qDebug() << "Database Open Success";
        return true;
    }
}

void DB_Ctrl::closeDB()
{
    db.close();
}

bool DB_Ctrl::createCmdTable()
{
    QSqlQuery query(db);
    QString create_sql = "create table command ( \
                        id INTEGER primary key NOT NULL, \
                        name varchar(255), \
                        hex bool, \
                        cmd varchar(255))";
    // Create Table
    query.prepare(create_sql);
    if (!query.exec()) {
        qDebug() << QObject::tr("Table Create failed");
        qDebug() << query.lastError();
        return false;
    } else {
        return true;
    }
}

bool DB_Ctrl::deleteCmdTable()
{
    QSqlQuery query(db);
    QString sql_exec = "drop table command";

    // Drop a table
    query.prepare(sql_exec);

    if (!query.exec()) {
        qDebug() << QObject::tr("Table command drop failed");
        qDebug() << query.lastError();
        return false;
    } else {
        qDebug() << QObject::tr("Table command drop success.");
        return true;
    }
}

bool DB_Ctrl::insertCommand(const int &row, const QString &name, const bool &hex, const QString &cmd)
{
    QSqlQuery query(db);
    QString insert_sql = "INSERT INTO command \
                         (id, name, hex, cmd) \
                         VALUES \
                         (:id, :name, :hex, :cmd)";

    query.prepare(insert_sql);
    query.bindValue(":id", row);
    query.bindValue(":name", name);
    query.bindValue(":hex", hex);
    query.bindValue(":cmd", cmd);

    if (!query.exec()) {
        qDebug() << "Insert Failed : " << query.lastError();
        return false;
    } else {
        qDebug() << "Insert Success : " << row << name << hex << cmd;
        return true;
    }
}

bool DB_Ctrl::updateCommand(const int &row, const QString &name, const bool &hex, const QString &cmd)
{
    QSqlQuery query(db);
    QString sql = "UPDATE command SET name=:name,hex=:hex,cmd=:cmd WHERE id=:id";

    query.prepare(sql);
    query.bindValue(":id", row);
    query.bindValue(":name", name);
    query.bindValue(":hex", hex);
    query.bindValue(":cmd", cmd);

    if (!query.exec()) {
        qDebug() << "Update Failed :" << query.lastError();
        return false;
    } else {
        qDebug() << "Update Success : " << row << name << hex << cmd;
        return true;
    }
}

bool DB_Ctrl::getCommand(const int &row, QString &name, bool &hex, QString &cmd)
{
    QSqlQuery query(db);
    QString sql = "SELECT * FROM command WHERE id=:id";
    QString result = "";

    query.prepare(sql);
    query.bindValue(":id", row);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return false;
    } else {
        while (query.next()) {
            name = query.value(1).toString();
            hex = query.value(2).toBool();
            cmd = query.value(3).toString();
            qDebug() << "Query Value:" << row << name << hex << cmd;
        }
        return true;
    }
}

bool DB_Ctrl::deleteCommand(const int &row)
{
    QSqlQuery query(db);
    QString sql = "DELETE FROM command WHERE id=:id";

    query.prepare(sql);
    query.bindValue(":id", row);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return false;
    } else {
        qDebug() << "Delete Success, row:" << row;
        return true;
    }
}

int DB_Ctrl::getCommandNum()
{
    QSqlQuery query(db);
    QString sql = "SELECT COUNT(*) FROM command";
    int result = 0;

    query.prepare(sql);

    if (!query.exec()) {
        qDebug() << query.lastError();
    } else {
        while (query.next()) {
            result = query.value(0).toInt();
            qDebug() << "Row Cnt:" << result;
        }
    }

    return result;
}

bool DB_Ctrl::deleteSettingTable()
{
    QSqlQuery query(db);
    QString sql_exec = "drop table user_setting";

    // Drop a table
    query.prepare(sql_exec);

    if (!query.exec()) {
        qDebug() << QObject::tr("Table user_setting drop failed");
        qDebug() << query.lastError();
        return false;
    } else {
        qDebug() << QObject::tr("Table user_setting drop success.");
        return true;
    }
}

bool DB_Ctrl::createSettingTable()
{
    QSqlQuery query(db);
    QString create_sql = "create table user_setting ( \
                        id INTEGER primary key AUTOINCREMENT NOT NULL, \
                        parameter varchar(30), \
                        value varchar(255), \
                        update_time varchar(30))";
    // Create Table
    query.prepare(create_sql);
    if (!query.exec()) {
        qDebug() << QObject::tr("Table Create failed");
        qDebug() << query.lastError();
        return false;
    } else {
        return true;
    }
}

bool DB_Ctrl::insertSetting(const QString &parameter, const QString &value)
{
    QSqlQuery query(db);
    QString insert_sql = "INSERT INTO user_setting \
                         (parameter, value, update_time) \
                         VALUES \
                         (:parameter, :value, :update_time)";

    QDateTime local(QDateTime::currentDateTime());
    QString localTime = local.toString("yyyy-MM-dd hh:mm:ss");

    query.prepare(insert_sql);
    query.bindValue(":parameter", parameter);
    query.bindValue(":value", value);
    query.bindValue(":update_time", localTime);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return false;
    } else {
        qDebug() << "Insert Parameter Success : " << parameter << " --> " << value;
        return true;
    }
}

bool DB_Ctrl::updateSetting(const QString &parameter, const QString &value)
{
    QSqlQuery query(db);
    QString sql = "UPDATE user_setting SET value=:value WHERE parameter=:parameter";

    query.prepare(sql);
    query.bindValue(":parameter", parameter);
    query.bindValue(":value", value);

    if (!query.exec()) {
        qDebug() << query.lastError();
        // if database has not this parameter, try to insert it.
        return insertSetting(parameter, value);
    } else {
        qDebug() << "Update Parameter Succes : " << parameter << " --> " << value;
        return true;
    }
}

bool DB_Ctrl::getSetting(const QString &parameter, QString &value)
{
    QSqlQuery query(db);
    QString sql = "SELECT * FROM user_setting WHERE parameter=:parameter";
    QString result = "";

    query.prepare(sql);
    query.bindValue(":parameter", parameter);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return false;
    } else {
        while (query.next()) {
            QString param = query.value(1).toString();
            QString val = query.value(2).toString();
            if (param == parameter) {
                result = val;
            }
        }
        qDebug() << "Queray Parameter : " << parameter << " <-- " << result;
        value = result;
        return true;
    }
}

void DB_Ctrl::insertDefaultSetting()
{
    // Default Mode
    insertSetting("mode", "0"); // 0-> Lite, 1->Full;

    // Plotting->LineColor
    insertSetting("line_color_r", "0");
    insertSetting("line_color_g", "0");
    insertSetting("line_color_b", "255");

    insertSetting("tx_end_str", "\n"); // End String
    insertSetting("rx_end_str", "\n"); // End String

    insertSetting("rx_as_hex", "0");
    insertSetting("rx_new_line", "0");
    insertSetting("rx_show_time", "0");

    insertSetting("tx_as_hex", "0");
    insertSetting("tx_new_line", "0");

    insertSetting("baundrate", "115200");
}
