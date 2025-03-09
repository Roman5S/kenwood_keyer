#include "serialtrx.h"
#include <QSerialPort>

serialTRX::serialTRX(QObject *parent)
    : QObject{parent}
{
    port = new QSerialPort(this);

    connect(this->port, &QSerialPort::readyRead, this, &serialTRX::read);
}


void serialTRX::setupPort(QString name, qint32 baud, bool stopBits, bool flowcontrol)
{
    port->setPortName(name);
    port->setBaudRate(baud);
    if (stopBits) port->setStopBits(QSerialPort::OneStop);
    else port->setStopBits(QSerialPort::TwoStop);
    if (flowcontrol) port->setFlowControl(QSerialPort::NoFlowControl);
    else port->setFlowControl(QSerialPort::HardwareControl);
}

void serialTRX::read()
{
    message.clear();
    message.append(port->readAll());
    //qDebug("emited");
    emit received();
}

void serialTRX::send(QByteArray data)
{
    port->write(data);
    port->waitForBytesWritten();

    /*QSerialPort::PinoutSignals sgn;
    sgn = port->pinoutSignals();
    if(sgn.testFlag(QSerialPort::ClearToSendSignal)) //Проверяет CTS сигнал.
    {
        qDebug("CTS 1");
    }*/
}
