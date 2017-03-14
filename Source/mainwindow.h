#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QDateTime>
#include <unistd.h>
#include <QWidget>
#include <QLineEdit>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void init();
    void askV();
    void answer();
    void startM();
    void stopM();
    void test();
    void check(QByteArray);
    void info();
    void comChanged();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
