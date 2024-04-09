#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QDockWidget>

namespace Ui {
class DockWidget;
}

class DockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockWidget(QWidget *parent = nullptr);
    ~DockWidget();

    bool is_option_str();
    bool is_option_wstr();

private:
    Ui::DockWidget *ui;
};

#endif // DOCKWIDGET_H
