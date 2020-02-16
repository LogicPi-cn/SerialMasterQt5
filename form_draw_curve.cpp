#include "form_draw_curve.h"
#include "ui_form_draw_curve.h"

Form_Draw_Curve::Form_Draw_Curve(QWidget *parent) : QWidget(parent), ui(new Ui::Form_Draw_Curve)
{
    ui->setupUi(this);
}

Form_Draw_Curve::~Form_Draw_Curve()
{
    delete ui;
}
