//
// Created by 17616 on 2022/11/23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_XtermPanel.h" resolved

#include <QMessageBox>
#include "XtermPanel.h"
#include "ui_XtermPanel.h"
#include "console.h"
#include "settingsdialog.h"


XtermPanel::XtermPanel(QObject *dialog, QWidget *parent) :
        m_settingDialog(dialog), QWidget(parent), ui(new Ui::XtermPanel) {
    ui->setupUi(this);
    m_serial = new QSerialPort(this);
    connect(m_serial, &QSerialPort::errorOccurred, this, &XtermPanel::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &XtermPanel::readData);
    connect(ui->plainTextEdit, &Console::getData, this, &XtermPanel::writeData);
}

XtermPanel::~XtermPanel() {
    delete ui;
}
void XtermPanel::readData() {
    const QByteArray data = m_serial->readAll();
    ui->plainTextEdit->putData(data);
}
void XtermPanel::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        openSerialPortSlot(false);
    }
}
void XtermPanel::writeData(const QByteArray &data) {
    m_serial->write(data);
}
void XtermPanel::openSerialPortSlot(bool open) {
    if (open) {
        SettingsDialog *settings = dynamic_cast<SettingsDialog *>(m_settingDialog);
        const SettingsDialog::Settings p = settings->settings();
        m_serial->setPortName(p.name);
        m_serial->setBaudRate(p.baudRate);
        m_serial->setDataBits(p.dataBits);
        m_serial->setParity(p.parity);
        m_serial->setStopBits(p.stopBits);
        m_serial->setFlowControl(p.flowControl);
        if (m_serial->open(QIODevice::ReadWrite)) {
            ui->plainTextEdit->setEnabled(true);
            qDebug() << tr("Connected to %1 : %2, %3, %4, %5, %6")
                    .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                    .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl);
            emit deviceStatusUpdateSignal(true);

        } else {
            emit deviceStatusUpdateSignal(false);
            QMessageBox::critical(this, tr("Error"), m_serial->errorString());
            qDebug() << (tr("Open error"));
        }
    } else {
        if (m_serial->isOpen())
            m_serial->close();
        emit deviceStatusUpdateSignal(false);
    }
}
