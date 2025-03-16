#ifndef SERIALTRX_H
#define SERIALTRX_H

#include <QObject>
#include <QSerialPort>

class serialTRX : public QObject
{
    Q_OBJECT
public:
    explicit serialTRX(QObject *parent = nullptr);
    QSerialPort *port;
    void setupPort(QString name, qint32 baud, bool stopBits, bool flowcontrol);
    QByteArray message;
    bool isSending;
    void send(QByteArray data);
    QByteArray buffer;

private slots:
    void read();

signals:
    void received();
};

#endif // SERIALTRX_H
