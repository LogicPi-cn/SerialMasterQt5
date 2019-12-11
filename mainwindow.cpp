#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 串口句柄
    m_serial = new QSerialPort();

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

char MainWindow::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return ch-ch;//不在0-f范围内的会发送成0
}

void MainWindow::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();

    if(len%2 == 1)   //如果发送的数据个数为奇数的，则在前面最后落单的字符前添加一个字符0
    {
        str = str.insert(len-1,'0'); //insert(int position, const QString & str)
        len = len +1;
    }

    senddata.resize(len/2);
    char lstr,hstr;

    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
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

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name : " << info.portName();
//        qDebug() << "Description : " << info.description();
//        qDebug() << "Manufacturer: " << info.manufacturer();
//        qDebug() << "Serial Number: " << info.serialNumber();
//        qDebug() << "System Location: " << info.systemLocation();
        ui->comboBox_Port->addItem(info.portName());
    }
}

void MainWindow::ReceiveData()
{
    QByteArray msg = m_serial->readAll();

    if (ui->checkBox_ReceiveAsHex->isChecked()) {
        qDebug() << "Rec hex : " << msg.toHex();
        ui->textEdit_Receive->append(msg.toHex());
    }else{
        qDebug() << "Rec str:" << msg;
        ui->textEdit_Receive->append(msg);
    }

    if (ui->checkBox_ReceiveAutoNewLine->isChecked()) {
        ui->textEdit_Receive->append("\n");
    }

}

void MainWindow::RegHandler()
{
    //注册插拔事件
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler,sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    for(int i=0;i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID);i++)
    {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];//GetCurrentUSBGUID();//m_usb->GetDriverGUID();
        hDevNotify = RegisterDeviceNotification((HANDLE)this->winId(),&NotifacationFiler,DEVICE_NOTIFY_WINDOW_HANDLE);
        if(!hDevNotify)
        {
            int Err = GetLastError();
            qDebug() << "注册失败:" << Err;
        }
    }

}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    int msgType = msg->message;
    if(msgType==WM_DEVICECHANGE)
    {
        //qDebug() << "Event DEVICECHANGE Happend" << endl;
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
         PDEV_BROADCAST_DEVICEINTERFACE b;
        switch (msg->wParam) {
        case DBT_DEVICEARRIVAL:
            b = (PDEV_BROADCAST_DEVICEINTERFACE)msg->lParam;
            if(b->dbcc_classguid == GUID_DEVINTERFACE_LIST[0])
            {
                 //检测到注册的GUID dosomething
                qDebug()<<"CSC板卡插入";
            }
            if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if(lpdbv->dbcv_flags ==0)
                {
                    //插入u盘
                    QString USBDisk = QString(this->FirstDriveFromMask(lpdbv ->dbcv_unitmask));
                    qDebug() << "USB_Arrived and The USBDisk is: "<<USBDisk ;

                }
            }
            if(lpdb->dbch_devicetype = DBT_DEVTYP_DEVICEINTERFACE)
            {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;

                QString strname = QString::fromWCharArray(pDevInf->dbcc_name,pDevInf->dbcc_size);
                qDebug() << "设备插入";
                RefreshComList();
               // ui->textBrowser->append("插入设备 ：" + strname);
            }
            break;
        case DBT_DEVICEREMOVECOMPLETE:
            b = (PDEV_BROADCAST_DEVICEINTERFACE)msg->lParam;
            if(b->dbcc_classguid == GUID_DEVINTERFACE_LIST[0])
            {
                 //检测到注册的GUID dosomething
                qDebug()<<"CSC板卡拔出";
            }
            if(lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME)
            {
                PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
                if(lpdbv->dbcv_flags == 0)
                {

                }
            }
            if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;

//                qDebug() << "移除设备(name)：" << pDevInf->dbcc_name;
//                qDebug()<< "移除设备(guid)：" << pDevInf->dbcc_classguid;
//                qDebug() << "移除设备(size)：" << pDevInf->dbcc_size;

                qDebug() << "移除设备";
                RefreshComList();

                QString strname = QString::fromWCharArray(pDevInf->dbcc_name,pDevInf->dbcc_size);
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
    if (!m_serial->open(QIODevice::ReadWrite)){
        qDebug()<<portName<<"打开失败";
        return;
    }else{

        if (bps == 115200) {
            m_serial->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
        }else if (bps == 38400) {
            m_serial->setBaudRate(QSerialPort::Baud38400,QSerialPort::AllDirections);//设置波特率和读写方向
        }else if (bps == 57600) {
            m_serial->setBaudRate(QSerialPort::Baud57600,QSerialPort::AllDirections);//设置波特率和读写方向
        }else if (bps == 19200) {
            m_serial->setBaudRate(QSerialPort::Baud19200,QSerialPort::AllDirections);//设置波特率和读写方向
        }else if (bps == 9600) {
            m_serial->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections);//设置波特率和读写方向
        }else if (bps == 4800) {
            m_serial->setBaudRate(QSerialPort::Baud9600,QSerialPort::AllDirections);//设置波特率和读写方向
        }else{
            m_serial->setBaudRate(QSerialPort::Baud115200,QSerialPort::AllDirections);//设置波特率和读写方向
        }

        m_serial->setDataBits(QSerialPort::Data8);		//数据位为8位
        m_serial->setFlowControl(QSerialPort::NoFlowControl);//无流控制
        m_serial->setParity(QSerialPort::NoParity);	//无校验位
        m_serial->setStopBits(QSerialPort::OneStop); //一位停止位

        connect(m_serial,SIGNAL(readyRead()),this,SLOT(ReceiveData()));

//        ui->actionStart->setDisabled(true);
        QString status = "Opened : " + portName + ", " + QString::number(bps) + ", N, 8, 1";
        comStatus->setStyleSheet(("color:green"));
        comStatus->setText(status);

    }
}

void MainWindow::on_actionStop_triggered()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
    }
    comStatus->setStyleSheet(("color:red"));
    QString status = "Closed : " + ui->comboBox_Port->currentText();
    comStatus->setText(status);
}

void MainWindow::on_pushButton_ReceiveClear_clicked()
{
    ui->textEdit_Receive->clear();
    ui->label_ReceiveCnt->setText("0");
}

void MainWindow::on_pushButton_SendClear_clicked()
{
    ui->textEdit_Send->clear();
    ui->label_SendCnt->setText("0");
}

void MainWindow::on_pushButton_Send_clicked()
{
    if (m_serial->isOpen()) {

        QString msg = ui->textEdit_Send->toPlainText();
        QByteArray send_buf;

        if (ui->checkBox_SendAsHex->isChecked()) {
            StringToHex(msg, send_buf);
        }else{
            send_buf = msg.toLatin1();
        }
        m_serial->write(send_buf);
    }else{
        qDebug() << "Serial Port is not opened.";
        comStatus->setStyleSheet(("color:red"));
        comStatus->setText("Port NOT Opened!");
    }
}
