#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QPalette>

#include <QSerialPortInfo>
#include <QSerialPort>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setSizeIncrement(800, 480);

    //    QPixmap pix(":/assets/battery/svg/Asset 67.svg");
    //    int w = ui->label_pic->width();
    //    int h = ui->label_pic->height();
    //    ui->label_pic->setPixmap(pix.scaled(w,h, Qt::KeepAspectRatio));
    load_backgroud();
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::onReadData);
    openDefault();

}

MainWindow::~MainWindow()
{
    delete ui;
}


//void MainWindow::on_pushButton_clicked()
//{
//    QString username = ui->lineEdit_username->text();
//    QString password = ui->lineEdit_password->text();

//    if(username == "test" && password == "password") {
//        QMessageBox::information(this, "Login", "Username and Password is correct");
//        hide(); //untuk menutup ui sebelumnya
//        secDialog = new SecDialog(this);
//        secDialog->show();

//    } else {
//        QMessageBox::warning(this, "Login", "Username and Password is not correct");
//    }
//}

void MainWindow::load_backgroud()
{
    QPixmap pix;
    QPalette pal;
    pix.load("://assets/background/bg.svg");
    pal.setBrush(QPalette::Background, pix);
    this->setPalette(pal);
}

void MainWindow::onReadData()
{
    while (serial->canReadLine()) {
        QByteArray data = serial->readLine();
        QStringList dataArray = QString::fromStdString(data.toStdString()).split(",");


//        qDebug()<<QString(data).trimmed();
        qDebug()<<dataArray;
        ui->speed_label->setText(dataArray[0]);
    }
}

void MainWindow::openDefault() {
    qDebug()<<"Counting number of avaible devices...";
    if(QSerialPortInfo::availablePorts().count() > 0) {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            if(info.hasVendorIdentifier() && info.hasProductIdentifier() && !serial->isOpen()){
                quint16 prodId = info.productIdentifier();
                quint16 vendId = info.vendorIdentifier();

                qDebug() << QString("Product id %1, vendor id %2, port name : %3").arg(prodId).arg(vendId).arg(info.portName());

                if(prodId == arduino_uno_product_id && vendId == arduino_uno_vendor_id){
                    qDebug() << "Device found!";
                    qDebug() << QString("Port name : %1").arg(info.portName());

                    serial->setPortName(info.portName());
                    serial->setBaudRate(QSerialPort::Baud9600);
                    serial->setDataBits(QSerialPort::Data8);
                    serial->setParity(QSerialPort::NoParity);
                    serial->setStopBits(QSerialPort::OneStop);
                    serial->setFlowControl(QSerialPort::NoFlowControl);

                    if(serial->open(QIODevice::ReadOnly)){
                        qDebug() << "Connected to " << serial->portName();
                    } else{
                        qCritical() << "Serial Port error: " << serial->errorString();
                    };
                }
            }
        }
    }
}




