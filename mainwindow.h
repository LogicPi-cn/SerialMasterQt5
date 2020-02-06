#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractNativeEventFilter>
#include <QCheckBox>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

#include <locale>

#include <Windows.h>

#include <Dbt.h>

#include <devguid.h>

#include <SetupAPI.h>

#include <InitGuid.h>

#include "config.h"
#include "db_ctrl.h"
#include "form_about.h"

static const GUID GUID_DEVINTERFACE_LIST[] = {
    // CSC板卡
    {0x4e4f17d3, 0xf4c1, 0x468d, {0x9d, 0xc2, 0x74, 0xdb, 0x5b, 0x56, 0x97, 0x59}},
    //   GUID_DEVINTERFACE_USB_DEVICE
    {0xA5DCBF10, 0x6530, 0x11D2, {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}},
    // GUID_DEVINTERFACE_DISK
    {0x53f56307, 0xb6bf, 0x11d0, {0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b}},
    // GUID_DEVINTERFACE_HID,
    {0x4D1E55B2, 0xF16F, 0x11CF, {0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30}},
    // GUID_NDIS_LAN_CLASS
    {0xad498944, 0x762f, 0x11d0, {0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c}}
    //// GUID_DEVINTERFACE_COMPORT
    //{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },
    //// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
    //{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },
    //// GUID_DEVINTERFACE_PARALLEL
    //{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },
    //// GUID_DEVINTERFACE_PARCLASS
    //{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } }
};

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    char ConvertHexChar(char ch);
    void StringToHex(QString str, QByteArray &senddata);

    // 日志打印
    void LogPrint(const QString msg);

    // 刷新串口列表
    void RefreshComList();

    // 初始化命令Table
    void InitCmdTable();
    void SendTableCommand(const int &row);
    void AddEmptyRowInCommandTable(const int &row, const bool &hex);
    void RefreshCmdTable();

    char FirstDriveFromMask(ULONG unitmask)
    {
        char i;
        for (i = 0; i < 26; ++i) {
            if (unitmask & 0x1)
                break;
            unitmask = unitmask >> 1;
        }
        return (i + 'A');
    }

    // for Update
    QNetworkAccessManager *manager;    // network requester
    int parse_UpdateJSON(QString str); // json parser
    void CheckUpdate();

    // InitLayout
    void InitLayout();

  protected:
    // 注册热插拔
    void RegHandler();
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

  private slots:

    void replyFinished(QNetworkReply *reply); // network reply

    // 接收数据
    void ReceiveData();

    void on_actionStart_triggered();

    void on_actionStop_triggered();

    void on_pushButton_ReceiveClear_clicked();

    void on_pushButton_SendClear_clicked();

    void on_pushButton_Send_clicked();

    void on_pushButton_AddCmd_clicked();

    void on_pushButton_RemoveCmd_clicked();

    void on_pushButton_SaveAllCmd_clicked();

    void on_pushButton_LoadCmd_clicked();

    void on_pushButton_SaveAs_clicked();

    void on_pushButton_ClearLog_clicked();

    void on_actionAbout_triggered();

    void on_actionUpdate_triggered();

    void on_actionExit_triggered();

  private:
    Ui::MainWindow *ui;

    QSerialPort *m_serial;
    DB_Ctrl *db_ctrl;

    QLabel *comStatus;
    QLabel *RX_Label;
    QLabel *TX_Label;

    int receive_cnt;
    int send_cnt;
};

#endif // MAINWINDOW_H
