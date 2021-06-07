#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "secdialog.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QDebug>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow;  }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadBackgroud();
    void updateTime(QLabel *label);
    void updateTachoMeter(QString value, int max);
    void updateBatteryIcon(QString percentage, QLabel *label);
    void updateStatusInformation(QString motorTemp, QString controllerTemp);

public slots:
    void onReadData();

private:
    Ui::MainWindow *ui;
    SecDialog *secDialog;
    QSerialPort *serial;
    static const quint16 arduino_uno_vendor_id = 9025;
    static const quint16 arduino_uno_product_id = 67;
    QString serialBuffer;

    void openDefault();
};
#endif // MAINWINDOW_H
