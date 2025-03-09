#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialtrx.h"

#include <QMainWindow>
#include <QSettings>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void readCurrSettings();

private:
    Ui::MainWindow *ui;
    qint32 baud;
    bool stopBits; //true = one stopbit
    bool flowcontrol; //true = no flwctrl
    bool cwPin; //true = rts
    QString vPortName;
    QString pPortName;
    QString path;
    QSettings *qs;
    void openPath(QString path);
    void createSetFile();
    void readSettings();
    serialTRX *vTRX;
    serialTRX *pTRX;
    QTimer *pinCheckTmr;
    //void readCurrSettings();
    bool cts;
    bool dsr;

private slots:
    //void readCurrSettings();
    void saveSettings();
    //void sendThrough(serialTRX *trx1, serialTRX *trx2);
    void start();

signals:
    void ctsChanged();
    void dsrChanged();

};
#endif // MAINWINDOW_H
