#include "savedialog.h"
#include "ui_savedialog.h"
#include <QAbstractButton>

saveDialog::saveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saveDialog)
  , abort_(false)
{
    ui->setupUi(this);
}

saveDialog::~saveDialog()
{
    delete ui;
}

bool & saveDialog::is_cancel_button_clicked()
{
    return abort_;
}


void saveDialog::on_pushButton_clicked()
{
    //abort_=false;
    emit save_signal();
    this->close();

}

void saveDialog::on_pushButton_2_clicked()
{
    //this->~saveDialog();
        //abort_=true;
    is_cancel_button_clicked()=true;
    this->close();
}


void saveDialog::on_pushButton_3_clicked()
{
    //abort_=false;
    this->close();
}
