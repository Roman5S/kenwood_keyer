#include "serialtrx.h"
#include <QSerialPort>

serialTRX::serialTRX(QObject *parent)
    : QObject{parent}
{
    port = new QSerialPort(this);

    connect(this->port, &QSerialPort::readyRead, this, &serialTRX::read);
    isSending = false;
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
    //message.clear();
    message.append(port->readAll());
    //qDebug("emited");
    emit received();
}


void serialTRX::send(QByteArray data)
{
    QByteArray toSend;
    if (!isSending)
    {
        isSending=true;
        port->write(data);
        port->waitForBytesWritten();
        isSending=false;


    if (!buffer.isEmpty() && !isSending) //or isNull
    {
        isSending=true;
        while (!buffer.isEmpty())
        {
        toSend=buffer;
        port->write(toSend);
        port->waitForBytesWritten();
        buffer.replace(0, toSend.length(), "");
        isSending=false;
        }
    }
    }

    else buffer.append(data);
}
