#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 串口句柄
    m_serial = new QSerialPort();

    db_ctrl = new DB_Ctrl();

    // 注册热插拔检测事件
    RegHandler();

    // 刷新串口
    RefreshComList();

    // 清空计数器
    MainWindow::on_pushButton_SendClear_clicked();
    MainWindow::on_pushButton_ReceiveClear_clicked();

    // 默认设置
    ui->comboBox_BaundRate->setCurrentText("115200");

    // 初始化StatusBar
    comStatus = new QLabel("");
    ui->statusBar->addWidget(comStatus);

    // 初始化命令Table
    InitCmdTable();

    ui->actionStart->setDisabled(false);
    ui->actionStop->setDisabled(true);

    // 计数器清零
    receive_cnt = 0;
    send_cnt = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::LogPrint(const QString msg)
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss.zzz ") + msg;
    ui->textEdit_Log->append(str);
}

char MainWindow::ConvertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return ch - 0x30;
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else
        return ch - ch; //不在0-f范围内的会发送成0
}

void MainWindow::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();

    if (len % 2 == 1) //如果发送的数据个数为奇数的，则在前面最后落单的字符前添加一个字符0
    {
        str = str.insert(len - 1, '0'); // insert(int position, const QString & str)
        len = len + 1;
    }

    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;) {
        // char lstr,
        hstr = str[i].toLatin1();
        if (hstr == ' ') {
            i++;
            continue;
        }
        i++;
        if (i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

void MainWindow::RefreshComList()
{
    QSerialPortInfo com_info;

    ui->comboBox_Port->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        qDebug() << "Name : " << info.portName();
        //        qDebug() << "Description : " << info.description();
        //        qDebug() << "Manufacturer: " << info.manufacturer();
        //        qDebug() << "Serial Number: " << info.serialNumber();
        //        qDebug() << "System Location: " << info.systemLocation();
        ui->comboBox_Port->addItem(info.portName());
    }
}

void MainWindow::InitCmdTable()
{
    // Set Header
    ui->tableWidget_SendCmd->setColumnCount(4);
    QStringList header;
    header << "Name"
           << "HEX"
           << "Command"
           << "Send";
    ui->tableWidget_SendCmd->setHorizontalHeaderLabels(header);

    RefreshCmdTable();
}

void MainWindow::AddEmptyRowInCommandTable(const int &row, const bool &hex)
{
    // add checkbox
    QWidget *pWidget_checkbox = new QWidget();
    QCheckBox *checkbox = new QCheckBox();
    QHBoxLayout *pLayout_checkbox = new QHBoxLayout(pWidget_checkbox);
    pLayout_checkbox->addWidget(checkbox);
    pLayout_checkbox->setAlignment(Qt::AlignCenter);
    pLayout_checkbox->setContentsMargins(0, 0, 0, 0);
    pWidget_checkbox->setLayout(pLayout_checkbox);
    ui->tableWidget_SendCmd->setCellWidget(row, 1, pWidget_checkbox);

    checkbox->setChecked(hex);

    // add pushbutton
    QWidget *pWidget = new QWidget();
    QPushButton *btn = new QPushButton("Send");
    QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
    pLayout->addWidget(btn);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(0, 0, 0, 0);
    pWidget->setLayout(pLayout);
    ui->tableWidget_SendCmd->setCellWidget(row, 3, pWidget);

    // Lamba
    connect(btn, &QPushButton::clicked, this, [=] { SendTableCommand(row); });
}

void MainWindow::RefreshCmdTable()
{
    // Open DB
    db_ctrl->open_db();

    // Set Table Cnt
    int row_cnt = db_ctrl->get_row_cnt();
    ui->tableWidget_SendCmd->setRowCount(row_cnt);

    for (int i = 0; i < row_cnt; i++) {
        // Read a command from db
        QString name;
        bool hex;
        QString cmd;
        if (db_ctrl->read_command(i, name, hex, cmd)) {
            qDebug() << "Read Command:" << i << name << hex << cmd;

            // Add a Blank Row
            AddEmptyRowInCommandTable(i, hex);
            qDebug() << "Add Empty Row.";

            // Fit
            ui->tableWidget_SendCmd->setItem(i, 0, new QTableWidgetItem(name));
            ui->tableWidget_SendCmd->setItem(i, 2, new QTableWidgetItem(cmd));

            qDebug() << "Fit OK.";
        }
    }

    // Close DB
    db_ctrl->close_db();
}

void MainWindow::SendTableCommand(const int &row)
{
    qDebug() << "Row:" << row;
    const QString cmd = ui->tableWidget_SendCmd->item(row, 2)->text();
    qDebug() << "Command:" << cmd;
    QWidget *pWidget = ui->tableWidget_SendCmd->cellWidget(row, 1);
    QCheckBox *checkbox = pWidget->findChild<QCheckBox *>();
    bool hex;
    if (checkbox && checkbox->isChecked()) {
        hex = true;
    } else {
        hex = false;
    }
    qDebug() << "HEX:" << hex;

    // Send Cmd
    if (m_serial->isOpen()) {
        QByteArray send_buf;
        if (hex) {
            StringToHex(cmd, send_buf);
        } else {
            send_buf = cmd.toLatin1();
        }
        m_serial->write(send_buf);

        // 计数
        send_cnt += send_buf.length();
        ui->label_SendCnt->setText(QString::number(send_cnt));

        // 日志
        QString log = "[Send] " + send_buf;
        LogPrint(log);
    } else {
        qDebug() << "Serial Port is not opened.";
        comStatus->setStyleSheet(("color:red"));
        comStatus->setText("Port NOT Opened!");
    }
}

void MainWindow::ReceiveData()
{
    QByteArray msg = m_serial->readAll();

    if (ui->checkBox_ReceiveAsHex->isChecked()) {
        qDebug() << "Rec hex : " << msg.toHex();
        ui->textEdit_Receive->append(msg.toHex());
    } else {
        qDebug() << "Rec str:" << msg;
        ui->textEdit_Receive->append(msg);
    }

    if (ui->checkBox_ReceiveAutoNewLine->isChecked()) {
        ui->textEdit_Receive->append("\n");
    }

    // 计数
    receive_cnt += msg.length();
    ui->label_ReceiveCnt->setText(QString::number(receive_cnt));
}

void MainWindow::RegHandler()
{
    //注册插拔事件
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler, sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    for (int i = 0; i < sizeof(GUID_DEVINTERFACE_LIST) / sizeof(GUID); i++) {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i]; // GetCurrentUSBGUID();//m_usb->GetDriverGUID();
        hDevNotify = RegisterDeviceNotification((HANDLE)this->winId(), &NotifacationFiler, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!hDevNotify) {
            int Err = GetLastError();
            qDebug() << "Failed to Reg" << Err;
        }
    }
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG *msg = reinterpret_cast<MSG *>(message);
    int msgType = msg->message;
    if (msgType == WM_DEVICECHANGE) {
        // qDebug() << "Event DEVICECHANGE Happend" << endl;
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
        PDEV_BROADCAST_DEVICEINTERFACE b;
        switch (msg->wParam) {
        case DBT_DEVICEARRIVAL:
            b = (PDEV_BROADCAST_DEVICEINTERFACE)msg->lParam;
            if (b->dbcc_classguid == GUID_DEVINTERFACE_LIST[0]) {
                //检测到注册的GUID dosomething
                qDebug() << "CSC Insert";
            }
            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if (lpdbv->dbcv_flags == 0) {
                    //插入u盘
                    QString USBDisk = QString(this->FirstDriveFromMask(lpdbv->dbcv_unitmask));
                    qDebug() << "USB_Arrived and The USBDisk is: " << USBDisk;
                }
            }
            if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;

                QString strname = QString::fromWCharArray(pDevInf->dbcc_name, pDevInf->dbcc_size);
                qDebug() << "Device Insert";
                RefreshComList();
                // ui->textBrowser->append("插入设备 ：" + strname);
            }
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            b = (PDEV_BROADCAST_DEVICEINTERFACE)msg->lParam;
            if (b->dbcc_classguid == GUID_DEVINTERFACE_LIST[0]) {
                //检测到注册的GUID dosomething
                qDebug() << "CSC Removed";
            }
            if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if (lpdbv->dbcv_flags == 0) {
                }
            }
            if (lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;

                qDebug() << "Device Removed";
                RefreshComList();

                QString strname = QString::fromWCharArray(pDevInf->dbcc_name, pDevInf->dbcc_size);
            }
            break;
        }
    }
    return false;
}

void MainWindow::on_actionStart_triggered()
{
    QString portName = ui->comboBox_Port->currentText();
    int bps = ui->comboBox_BaundRate->currentText().toInt();

    m_serial->setPortName(portName);
    if (!m_serial->open(QIODevice::ReadWrite)) {
        qDebug() << portName << "Failed to Open";
        return;
    } else {
        if (bps == 115200) {
            m_serial->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections); //设置波特率和读写方向
        } else if (bps == 38400) {
            m_serial->setBaudRate(QSerialPort::Baud38400, QSerialPort::AllDirections); //设置波特率和读写方向
        } else if (bps == 57600) {
            m_serial->setBaudRate(QSerialPort::Baud57600, QSerialPort::AllDirections); //设置波特率和读写方向
        } else if (bps == 19200) {
            m_serial->setBaudRate(QSerialPort::Baud19200, QSerialPort::AllDirections); //设置波特率和读写方向
        } else if (bps == 9600) {
            m_serial->setBaudRate(QSerialPort::Baud9600, QSerialPort::AllDirections); //设置波特率和读写方向
        } else if (bps == 4800) {
            m_serial->setBaudRate(QSerialPort::Baud9600, QSerialPort::AllDirections); //设置波特率和读写方向
        } else {
            m_serial->setBaudRate(QSerialPort::Baud115200, QSerialPort::AllDirections); //设置波特率和读写方向
        }

        m_serial->setDataBits(QSerialPort::Data8);            //数据位为8位
        m_serial->setFlowControl(QSerialPort::NoFlowControl); //无流控制
        m_serial->setParity(QSerialPort::NoParity);           //无校验位
        m_serial->setStopBits(QSerialPort::OneStop);          //一位停止位

        connect(m_serial, SIGNAL(readyRead()), this, SLOT(ReceiveData()));

        //        ui->actionStart->setDisabled(true);
        QString status = "Opened : " + portName + ", " + QString::number(bps) + ", N, 8, 1";
        comStatus->setStyleSheet(("color:green"));
        comStatus->setText(status);

        ui->actionStart->setDisabled(true);
        ui->actionStop->setDisabled(false);
    }
}

void MainWindow::on_actionStop_triggered()
{
    if (m_serial->isOpen()) {
        m_serial->close();

        comStatus->setStyleSheet(("color:red"));
        QString status = "Closed : " + ui->comboBox_Port->currentText();
        comStatus->setText(status);

        ui->actionStart->setDisabled(false);
        ui->actionStop->setDisabled(true);
    }
}

void MainWindow::on_pushButton_ReceiveClear_clicked()
{
    ui->textEdit_Receive->clear();
    ui->label_ReceiveCnt->setText("0");
    receive_cnt = 0;
}

void MainWindow::on_pushButton_SendClear_clicked()
{
    ui->label_SendCnt->setText("0");
    send_cnt = 0;
}

void MainWindow::on_pushButton_Send_clicked()
{
    if (m_serial->isOpen()) {
        QString msg = ui->textEdit_Send->toPlainText();
        QByteArray send_buf;

        if (ui->checkBox_SendAsHex->isChecked()) {
            StringToHex(msg, send_buf);
        } else {
            send_buf = msg.toLatin1();
        }
        m_serial->write(send_buf);

        // 计数
        send_cnt += send_buf.length();
        ui->label_SendCnt->setText(QString::number(send_cnt));

        // 日志
        QString log = "[Send] " + msg;
        LogPrint(log);
    } else {
        qDebug() << "Serial Port is not opened.";
        comStatus->setStyleSheet(("color:red"));
        comStatus->setText("Port NOT Opened!");
    }
}

void MainWindow::on_pushButton_AddCmd_clicked()
{
    int row_cnt = ui->tableWidget_SendCmd->rowCount();
    row_cnt += 1;
    ui->tableWidget_SendCmd->setRowCount(row_cnt);

    AddEmptyRowInCommandTable(row_cnt - 1, false);
}

void MainWindow::on_pushButton_RemoveCmd_clicked()
{
    int cur_row = ui->tableWidget_SendCmd->currentRow();

    // No Row Selected
    if (cur_row < 0) {
        cur_row = ui->tableWidget_SendCmd->rowCount();
    }

    ui->tableWidget_SendCmd->removeRow(cur_row);

    //
    int row_cnt = ui->tableWidget_SendCmd->rowCount();
    for (int i = 0; i < row_cnt; i++) {
        qDebug() << "Data :" << i << ui->tableWidget_SendCmd->item(i, 0)->text()
                 << ui->tableWidget_SendCmd->item(i, 2)->text();
    }
}

void MainWindow::on_pushButton_SaveAllCmd_clicked()
{
    int row_cnt = ui->tableWidget_SendCmd->rowCount();
    qDebug() << "Table Row Cnt:" << row_cnt;

    // Save
    db_ctrl->open_db();
    for (int i = 0; i < row_cnt; i++) {
        // check if Blank
        QString name = "";
        if (ui->tableWidget_SendCmd->item(i, 0) == nullptr ||
            (ui->tableWidget_SendCmd->item(i, 0) && ui->tableWidget_SendCmd->item(i, 0)->text() == tr(""))) {
        } else {
            name = ui->tableWidget_SendCmd->item(i, 0)->text();
        }

        QString cmd = "";
        if (ui->tableWidget_SendCmd->item(i, 2) == nullptr ||
            (ui->tableWidget_SendCmd->item(i, 2) && ui->tableWidget_SendCmd->item(i, 2)->text() == tr(""))) {
        } else {
            cmd = ui->tableWidget_SendCmd->item(i, 2)->text();
        }

        QWidget *pWidget = ui->tableWidget_SendCmd->cellWidget(i, 1);
        QCheckBox *checkbox = pWidget->findChild<QCheckBox *>();
        bool hex;
        if (checkbox && checkbox->isChecked()) {
            hex = true;
        } else {
            hex = false;
        }

        qDebug() << "Row Data: " << i << name << hex << cmd;

        bool success = db_ctrl->add_command(i, name, hex, cmd);
        if (!success) {
            qDebug() << "Insert failed, trying to update...";
            success = db_ctrl->update_command(i, name, hex, cmd);
            qDebug() << success;
        }
    }
    db_ctrl->close_db();

    // delete rows
    db_ctrl->open_db();
    int db_row_cnt = db_ctrl->get_row_cnt();
    qDebug() << "DB Row Cnt:" << db_row_cnt;
    if (row_cnt < db_row_cnt) {
        for (int i = row_cnt; i < db_row_cnt; i++) {
            db_ctrl->delete_command(i);
        }
    }
    db_ctrl->close_db();

    QMessageBox::information(nullptr, "Saved!", "Saved success.");

    // Refresh
    RefreshCmdTable();
}

void MainWindow::on_pushButton_LoadCmd_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, ("Open Setting"), "", tr("DB Files (*.db)"), nullptr);
    if (!fileName.isNull()) {
        QString dst = QCoreApplication::applicationDirPath() + "/" + db_ctrl->dbName;
        qDebug() << "Dst :" << dst;
        if (!QFile::copy(fileName, dst)) {
            QMessageBox::warning(nullptr, "Failed!", "Load Failed!");
        } else {
            QMessageBox::information(nullptr, "Success!", "Load success.");
        }
    }
}

void MainWindow::on_pushButton_SaveAs_clicked()
{
    QDateTime time = QDateTime::currentDateTime();
    QString fileName_str = time.toString("yyyy-MM-dd----hh-mm-ss");
    QByteArray filename_ba = fileName_str.toLatin1();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Setting"), filename_ba, tr("DB Files (*.db)"));

    if (!fileName.isNull()) {
        if (!QFile::copy(db_ctrl->dbName, fileName)) {
            QMessageBox::warning(nullptr, "Failed!", "Save Failed!");
        } else {
            QMessageBox::information(nullptr, "Success!", "Save success.");
        }
    }
}

void MainWindow::on_pushButton_ClearLog_clicked()
{
    ui->textEdit_Log->setText("");
}
