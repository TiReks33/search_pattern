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

void Result::search_results(QString& text)
{
    ui->result_text->setPlainText(text);
}

void Result::search_results(QString& text, char /*color*/)
{
    ui->result_text->setHtml("<div><font color=\"red\">"+text+"</font></div>");
}

void Result::total_occur_slot(size_t occurrences)
{
    ui->total_label->setText(QString::number(occurrences));
}
