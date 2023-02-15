#ifndef SHELLE_XTERMPANEL_H
#define SHELLE_XTERMPANEL_H

#include <QWidget>
#include <QSerialPort>
#include <QMap>


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

public:
    void _saveState();
    void _restoreState();
private:
    Ui::XtermPanel *ui;
    QSerialPort *m_serial = nullptr;
    QStringList m_valueList;//历史记录保存列表
    qsizetype m_valueIter = ~0;//记录一下回溯到哪
    void historyAppend(QString &text);
};


#endif //SHELLE_XTERMPANEL_H
