#ifndef FORM_DRAW_CURVE_H
#define FORM_DRAW_CURVE_H

#include <QWidget>

namespace Ui
{
class Form_Draw_Curve;
}

class Form_Draw_Curve : public QWidget
{
    Q_OBJECT

  public:
    explicit Form_Draw_Curve(QWidget *parent = nullptr);
    ~Form_Draw_Curve();

  private:
    Ui::Form_Draw_Curve *ui;
};

#endif // FORM_DRAW_CURVE_H
