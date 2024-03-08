#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "result.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

private slots:
    void on_buttonBox_accepted();

    void on_tempButton_clicked();

    void on_highlight_result_stateChanged(int arg1);

signals:
    void search_signal(QString str="");

    void search_signal2(QString pattern="",bool highlight=false);

    void highlight_signal();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
