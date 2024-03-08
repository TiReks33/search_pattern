#include "result.h"
#include "ui_result.h"

Result::Result(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Result)
{
    ui->setupUi(this);
    ui->head_label->setText("Pattern exist in next literal positions(from zero):\n");
}

Result::~Result()
{
    delete ui;
}

void Result::search_results(QString text)
{
    ui->result_text->setHtml(text);
}
