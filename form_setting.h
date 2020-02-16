#ifndef FORM_SETTING_H
#define FORM_SETTING_H

#include "db_ctrl.h"
#include <QWidget>

namespace Ui
{
class Form_Setting;
}

class Form_Setting : public QWidget
{
    Q_OBJECT

  public:
    explicit Form_Setting(QWidget *parent = nullptr);
    ~Form_Setting();

    void LoadSetting();

  private slots:

    void on_comboBox_TxEndStr_currentIndexChanged(int index);

    void on_comboBox_RxEndStr_currentIndexChanged(int index);

  private:
    Ui::Form_Setting *ui;

    DB_Ctrl *m_dbCtrl;

    QString tx_end_str;
    QString rx_end_str;
};

#endif // FORM_SETTING_H
