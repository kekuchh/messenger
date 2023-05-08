#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWidget; }
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_showPassword_stateChanged(int arg1);

    void on_enterButton_clicked();

private:
    Ui::MainWidget *ui;
};
#endif // MAINWIDGET_H
