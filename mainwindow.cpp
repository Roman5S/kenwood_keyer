#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QFileInfo>
#include <QSerialPortInfo>
#include <QSettings>
#include <QTextStream>
#include <qmessagebox.h>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        ui->vPortCBox->addItem(serialPortInfo.portName());
        ui->pPortCBox->addItem(serialPortInfo.portName());
    }
    connect(ui->setupBtn, &QPushButton::clicked, this, [=] () {
        readCurrSettings();
    });
    connect(ui->saveSettingsBtn, &QPushButton::clicked, this, &MainWindow::saveSettings);
    connect(ui->startBtn, &QPushButton::clicked, this, &MainWindow::start);
    openPath(QCoreApplication::applicationDirPath() + "/settings.ini");

    vTRX = new serialTRX();
    pTRX = new serialTRX();
    pinCheckTmr = new QTimer();
    cts = false;
    dsr = false;

    connect(vTRX, &serialTRX::received, this, [=] () {
        if (!vTRX->message.isEmpty() && vTRX->message.mid(vTRX->message.size()-1, 1)==";")
        {
            pTRX->send(vTRX->message);
            vTRX->message.clear();
        }
    });
    connect(pTRX, &serialTRX::received, this, [=] () {
            vTRX->send(pTRX->message);
            pTRX->message.clear();
    });

    connect(this, &MainWindow::ctsChanged, this, [=] (){
        if (cwPin)
        {
        if (cts) pTRX->send("TX;");
        else pTRX->send("RX;");
        //qDebug("rts");
        }
    });
    connect(this, &MainWindow::dsrChanged, this, [=] (){
        if (!cwPin)
        {
            if (dsr) pTRX->send("TX;");
            else pTRX->send("RX;");
            //qDebug("dtr");
        }
    });

    connect(pinCheckTmr, &QTimer::timeout, this, [=] () {
        QSerialPort::PinoutSignals sgn;
        sgn = vTRX->port->pinoutSignals();
        if(sgn.testFlag(QSerialPort::ClearToSendSignal) != cts) //Проверяет CTS сигнал.
        {
            cts = sgn.testFlag(QSerialPort::ClearToSendSignal);
            emit ctsChanged();
            //qDebug("CTS ch");
        }
        if(sgn.testFlag(QSerialPort::DataSetReadySignal) != dsr)
        {
            dsr = sgn.testFlag(QSerialPort::DataSetReadySignal);
            emit dsrChanged();
            //qDebug("DSR ch");
        }
    });


}

MainWindow::~MainWindow()
{
    if (pTRX->port->isOpen()) pTRX->port->close();
    if (vTRX->port->isOpen()) vTRX->port->close();
    delete ui;
}

void MainWindow::readCurrSettings()
{
    baud=this->ui->baudCBox->currentText().toInt();
    if (ui->stopBitsCBox->currentText()=="1") stopBits=true;
    else stopBits=false;
    if (ui->fCtrlCBox->currentText()=="Нет") flowcontrol=true;
    else flowcontrol=false;
    if (ui->cwPinCBox->currentText()=="RTS") cwPin=true;
    else cwPin=false;
    vPortName=ui->vPortCBox->currentText();
    pPortName=ui->pPortCBox->currentText();

    qint32 interval = 1000/baud;
    pinCheckTmr->setInterval(interval);
    vTRX->setupPort(vPortName, baud, stopBits, true);
    pTRX->setupPort(pPortName, baud, stopBits, flowcontrol);
}

void MainWindow::openPath(QString path)
{
    this->path = path;
    QFileInfo portset(path);
    if (!portset.exists()) {
        createSetFile();
    }

    qs = new QSettings(path, QSettings::IniFormat);
    readSettings();
}

void MainWindow::createSetFile()
{
    QFile newFile(path);
    newFile.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&newFile);
    stream << "vPortName = " << Qt::endl;
    stream << "pPortName = " << Qt::endl;
    stream << "baud = 4800" << Qt::endl;
    stream << "stopBits = true" << Qt::endl;
    stream << "flowcontrol = true" << Qt::endl;
    stream << "cwPin = true" << Qt::endl;
    newFile.close();
}

void MainWindow::readSettings()
{
    //qs->beginGroup("General");
    ui->vPortCBox->setCurrentText(qs->value("vPortName").toString());
    ui->pPortCBox->setCurrentText(qs->value("pPortName").toString());
    ui->baudCBox->setCurrentText(qs->value("baud").toString());
    if (!qs->value("flowcontrol").toBool()) ui->fCtrlCBox->setCurrentText("Аппаратное");
    else ui->fCtrlCBox->setCurrentText("Нет");
    if (!qs->value("stopBits").toBool()) ui->stopBitsCBox->setCurrentText("2");
    else ui->stopBitsCBox->setCurrentText("1");
    if (!qs->value("cwPin").toBool()) ui->cwPinCBox->setCurrentText("DTR");
    else ui->cwPinCBox->setCurrentText("RTS");
    //qs->endGroup();
    //readCurrSettings();
}

void MainWindow::saveSettings()
{
    readCurrSettings();
    qs->setValue("vPortName", vPortName);
    qs->setValue("pPortName", pPortName);
    qs->setValue("baud", baud);
    qs->setValue("stopBits", stopBits);
    qs->setValue("flowcontrol", flowcontrol);
    qs->setValue("cwPin", cwPin);
    qs->sync();
}



void MainWindow::start()
{
    readCurrSettings();
    if (!vTRX->port->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось подключится к порту " + vPortName);
    }

    if (!pTRX->port->open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось подключится к порту " + pPortName);
    }
    pinCheckTmr->start();
    //qDebug("started");
}
