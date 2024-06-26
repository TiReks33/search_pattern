#ifndef RESULT_H
#define RESULT_H

#include <QDialog>

namespace Ui {
class Result;
}

class Result : public QDialog
{
    Q_OBJECT

public:
    explicit Result(QWidget *parent = nullptr);
    ~Result();

    void search_results(QString&);

    void search_results(QString&,char);

public slots:

    void total_occur_slot(size_t);

private:
    Ui::Result *ui;
};

#endif // RESULT_H
