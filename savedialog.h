#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>

namespace Ui {
class saveDialog;
}

class saveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit saveDialog(QWidget *parent = nullptr);
    ~saveDialog();

public slots:

    bool & is_cancel_button_clicked();

private slots:

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

//    void on_buttonBox_rejected();

    void on_pushButton_3_clicked();

signals:

    void save_signal();

private:
    Ui::saveDialog *ui;
    bool abort_;
};

#endif // SAVEDIALOG_H
