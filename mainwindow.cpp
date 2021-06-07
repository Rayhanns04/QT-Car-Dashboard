#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QPixmap>
#include <QPalette>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QFontDatabase>
#include <QFont>
#include <QFile>
#include <QTime>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QFontDatabase::addApplicationFont(":/assets/fonts/Courier Prime Code.ttf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Digital Dismay.ttf.otf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Sary Soft ExtraLight.otf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Sary Soft Light.otf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Sary Soft SemiBold.otf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Sary Soft Thin.otf");
    QFontDatabase::addApplicationFont(":/assets/fonts/Sary Soft.otf");

    //    QFont Digital("Digital Dismay", 10, QFont::Normal);
    //    ui->speed_label->setFont(Digital);
    //    QFile font(":/assets/fonts/Digital Dismay.ttf.otf");
    //    qDebug() << font.exists();

    ui->setupUi(this);
    this->setSizeIncrement(800, 480);
    loadBackgroud();

    //    QPixmap pix(":/assets/battery/svg/Asset 67.svg");
    //    int w = ui->label_pic->width();
    //    int h = ui->label_pic->height();
    //    ui->label_pic->setPixmap(pix.scaled(w,h, Qt::KeepAspectRatio));

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


void MainWindow::loadBackgroud()
{
    QPixmap pix;
    QPalette pal;
    pix.load("://assets/background/bg.svg");
    pal.setBrush(QPalette::Background, pix);
    this->setPalette(pal);
}


// ON READ DATA FROM SERIAL
void MainWindow::onReadData()
{
    while (serial->canReadLine()) {
        QByteArray data = serial->readLine();
        QStringList dataArray = QString(data).trimmed().split(",");
        QString currentTime = QTime::currentTime().toString("hh:mm");

        qDebug() << data;
        qDebug() << dataArray;

        // validasi data banyak data yang diterima
        if(dataArray.length() < 9) break;

        updateTachoMeter(dataArray[5], 7);
        updateStatusInformation(dataArray[1], dataArray[2]);
        updateBatteryIcon(dataArray[3], ui->mainbattery_icon);
        updateBatteryIcon(dataArray[4], ui->secondbattery_icon);

        ui->mainbatterytemp_label->setVisible(true);

        ui->time_label->setText(currentTime);
        ui->speed_label->setText(dataArray[0]);
        ui->motortemp_label->setText(dataArray[1].append("°C"));
        ui->controller_label->setText(dataArray[2].append("°C"));
        ui->mainbattery_label->setText(dataArray[3].append("%"));
        ui->secondbattery_label->setText(dataArray[4].append("%"));
        ui->odometer_label->setText(dataArray[6]);
        ui->mainbatterytemp_label->setText(dataArray[7].append("°C"));
        ui->secondbatterytemp_label->setText(dataArray[8].append("°C"));
    }
}


// UPDATE BATTERY ICON
void MainWindow::updateBatteryIcon(QString percentage, QLabel *label) {
    QPixmap pixmap;
    uint batteryLevel = percentage.toUInt();

    if(batteryLevel == 0) pixmap.load(":/assets/battery/svg/Asset 71.svg");
    else if(batteryLevel <= 25) pixmap.load(":/assets/battery/svg/Asset 70.svg");
    else if(batteryLevel <= 50) pixmap.load(":/assets/battery/svg/Asset 69.svg");
    else if(batteryLevel <= 75) pixmap.load(":/assets/battery/svg/Asset 68.svg");
    else pixmap.load(":/assets/battery/svg/Asset 67.svg");

    label->setPixmap(pixmap);
}


// UPDATE STATUS INFORMATION
void MainWindow::updateStatusInformation(QString motorTemp, QString controllerTemp) {
    QPixmap pixmap;
    uint motorTemperature = motorTemp.toUInt();
    uint controllerTemperature = controllerTemp.toUInt();

       qDebug() << motorTemperature;

    if(motorTemperature > 80 || controllerTemperature > 80) {
        pixmap.load(":/assets/icon/svg/Asset 5.svg");
        ui->status_icon->setPixmap(pixmap);
        ui->status_label->setText("Engine is Overheating");
    } else {
        pixmap.load(":/assets/icon/svg/Asset 4.svg");
        ui->status_icon->setPixmap(pixmap);
        ui->status_label->setText("Everthing Looks Good");
    }
}


// UPDATE TIME
void MainWindow::updateTime(QLabel *label) {

}


// UPDATE TACHO METER
void MainWindow::updateTachoMeter(QString value, int max) {
    QLabel *widget = ui->tachometer_label;
    int x = widget->geometry().x();
    int y = widget->geometry().y();
    int width = value.toInt() * 30;
    int height = widget->geometry().height();

    qDebug() << width;
    widget->setGeometry(x, y, width, height);
}


// SERIAL CONNECTION
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




