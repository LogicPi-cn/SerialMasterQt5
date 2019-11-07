#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 注册热插拔检测事件
    RegHandler();

    // 刷新串口
    RefreshComList();
}

MainWindow::~MainWindow()
{
    delete ui;
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

}
