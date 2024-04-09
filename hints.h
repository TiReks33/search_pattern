#ifndef HINTS_H
#define HINTS_H

#include <QStackedWidget>

namespace Ui {
class Hints;
}

class Hints : public QStackedWidget
{
    Q_OBJECT

public:
    explicit Hints(QWidget *parent = nullptr);
    ~Hints();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::Hints *ui;
};

#endif // HINTS_H
