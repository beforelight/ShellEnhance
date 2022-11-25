#ifndef SHELLE_XTERMPANEL_H
#define SHELLE_XTERMPANEL_H

#include <QWidget>
#include <QSerialPort>


QT_BEGIN_NAMESPACE
namespace Ui { class XtermPanel; }
QT_END_NAMESPACE

class XtermPanel : public QWidget {
Q_OBJECT

public:
    explicit XtermPanel(QWidget *parent = nullptr);
    ~XtermPanel() override;

Q_SIGNALS:
    void deviceStatusUpdateSignal(bool link = true);
public slots:
    void openSerialPortSlot(bool open);

    void writeData(const QByteArray &data);
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void sendCommand(const QString &cmd);
private:
    Ui::XtermPanel *ui;
    QSerialPort *m_serial = nullptr;
};


#endif //SHELLE_XTERMPANEL_H
