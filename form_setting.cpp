#include "form_setting.h"
#include "ui_form_setting.h"

Form_Setting::Form_Setting(QWidget *parent) : QWidget(parent), ui(new Ui::Form_Setting)
{
    ui->setupUi(this);

    m_dbCtrl = new DB_Ctrl();
    LoadSetting();
}

Form_Setting::~Form_Setting()
{
    delete ui;
}

void Form_Setting::LoadSetting()
{
    m_dbCtrl->OpenDB();
    if (!m_dbCtrl->ReadSetting("tx_end_str", tx_end_str)) {
        tx_end_str = "\n";
    }
    if (!m_dbCtrl->ReadSetting("rx_end_str", rx_end_str)) {
        rx_end_str = "\n";
    }
    m_dbCtrl->CloseDB();

    if (tx_end_str == "") {
        ui->comboBox_TxEndStr->setCurrentIndex(0);
    } else if (tx_end_str == "\r") {
        ui->comboBox_TxEndStr->setCurrentIndex(1);
    } else if (tx_end_str == "\n") {
        ui->comboBox_TxEndStr->setCurrentIndex(2);
    } else if (tx_end_str == "\r\n") {
        ui->comboBox_TxEndStr->setCurrentIndex(3);
    } else if (tx_end_str == "\n\r") {
        ui->comboBox_TxEndStr->setCurrentIndex(4);
    }

    if (rx_end_str == "") {
        ui->comboBox_RxEndStr->setCurrentIndex(0);
    } else if (tx_end_str == "\r") {
        ui->comboBox_RxEndStr->setCurrentIndex(1);
    } else if (tx_end_str == "\n") {
        ui->comboBox_RxEndStr->setCurrentIndex(2);
    } else if (tx_end_str == "\r\n") {
        ui->comboBox_RxEndStr->setCurrentIndex(3);
    } else if (tx_end_str == "\n\r") {
        ui->comboBox_RxEndStr->setCurrentIndex(4);
    }
}

void Form_Setting::on_comboBox_TxEndStr_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        tx_end_str = "";
        break;
    case 1:
        tx_end_str = "\r";
        break;
    case 2:
        tx_end_str = "\n";
        break;
    case 3:
        tx_end_str = "\r\n";
        break;
    case 4:
        tx_end_str = "\n\r";
        break;
    default:
        tx_end_str = "\n";
        break;
    }

    m_dbCtrl->OpenDB();
    m_dbCtrl->UpdateSetting("tx_end_str", tx_end_str);
    m_dbCtrl->CloseDB();
}

void Form_Setting::on_comboBox_RxEndStr_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        rx_end_str = "";
        break;
    case 1:
        rx_end_str = "\r";
        break;
    case 2:
        rx_end_str = "\n";
        break;
    case 3:
        rx_end_str = "\r\n";
        break;
    case 4:
        rx_end_str = "\n\r";
        break;
    default:
        rx_end_str = "\n";
        break;
    }

    m_dbCtrl->OpenDB();
    m_dbCtrl->UpdateSetting("rx_end_str", rx_end_str);
    m_dbCtrl->CloseDB();
}
