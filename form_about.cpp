#include "form_about.h"
#include "ui_form_about.h"

Form_About::Form_About(QWidget *parent) : QWidget(parent), ui(new Ui::Form_About)
{
    ui->setupUi(this);

    setFixedSize(320, 240);
    setWindowTitle("About");
}

Form_About::~Form_About()
{
    delete ui;
}
