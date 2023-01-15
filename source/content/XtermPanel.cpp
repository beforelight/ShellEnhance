//
// Created by 17616 on 2022/11/23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_XtermPanel.h" resolved

#include <QMessageBox>
#include "XtermPanel.h"
#include "ui_XtermPanel.h"
#include "console.h"
#include "settingsdialog.h"
#include <QAction>
#include <QStandardPaths>
#include <QSettings>
#include <QDir>
#include <QCompleter>
#include <QStringListModel>
#include "Agent.h"
XtermPanel::XtermPanel(QWidget *parent) :
        QWidget(parent), ui(new Ui::XtermPanel) {
    ui->setupUi(this);
    m_serial = new QSerialPort(this);

    //实现输入栏自动补全和历史记录
    auto *completer = new QCompleter(this); //提供自动补全的对象可用于lineEdit和combobox
    auto *listModel = new QStringListModel(m_valueList, this);//填充模型
    completer->setModel(listModel);//填充数据来源模型
    completer->setCaseSensitivity(Qt::CaseSensitive);
    ui->lineEdit->setCompleter(completer);//使lineEdit提供自动完成
    //实现输入栏自动补全和历史记录end

    connect(m_serial, &QSerialPort::errorOccurred, this, &XtermPanel::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &XtermPanel::readData);
    connect(ui->textEdit, &Console::getData, this, &XtermPanel::writeData);
    auto lineEditReturnPressedSlot = [this, listModel]() {
        auto str = ui->lineEdit->text() + '\n';
        writeData(str.toUtf8());
        {
            //保存历史记录
            QString text = ui->lineEdit->text();//获取lineEdit的数据
            if (QString::compare(text, QString("")) != 0) {//判断是不是为空
                bool flag = m_valueList.contains(text, Qt::CaseSensitive);//忽略大小写搜索历史记录中(calueList中的text文本),如果list中包含text返回ture,为了不保存重复值,就在返回false是再想list中添加text
                if (!flag) {
                    m_valueList.append(text);
                    if (m_valueList.size() > 100)
                        m_valueList.pop_front();
                    listModel->setStringList(m_valueList);
                }
            }
        }
    };
    connect(ui->toolButton_send, &QToolButton::clicked, lineEditReturnPressedSlot);
    connect(ui->lineEdit, &QLineEdit::returnPressed, lineEditReturnPressedSlot);

    _restoreState();
    ui->lineEdit->setClearButtonEnabled(true);
    listModel->setStringList(m_valueList);
}

XtermPanel::~XtermPanel() {
    _saveState();
    delete ui;
}
void XtermPanel::readData() {
    QByteArray data = m_serial->readAll();
    //替换\r\n
    //    qsizetype j = data.count() - 1;
    //    while ((j = data.lastIndexOf("\r\n", j)) != -1) {
    //        data.replace(j, 2, "\n");
    //        --j;
    //    }
    ui->textEdit->putData(data);
}
void XtermPanel::handleError(QSerialPort::SerialPortError error) {
    auto obj = Agent::Agent::instance().find("action_com", "QAction");
    auto action = std::dynamic_pointer_cast<Agent::Agent::Ptr<QAction> >(obj);
    switch (error) {
        case QSerialPort::NoError:
            break;
        case QSerialPort::ResourceError:
//            QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
            qCritical() << m_serial->errorString();
            emit action->get()->triggered(true);
            openSerialPortSlot(false);
            break;
        default:
            QMessageBox::warning(this, tr("warning"), m_serial->errorString());
            openSerialPortSlot(false);
            break;
    }
}
void XtermPanel::writeData(const QByteArray &data) {
    if (m_serial->isOpen())
        m_serial->write(data);
    else {
        QMessageBox::warning(nullptr, tr("Warning"), "串口未打开");
        qWarning() << "串口未打开";
        auto obj = Agent::Agent::instance().find("action_com", "QAction");
        auto action = std::dynamic_pointer_cast<Agent::Agent::Ptr<QAction> >(obj);
        emit action->get()->triggered(true);
    }
}
void XtermPanel::openSerialPortSlot(bool open) {
    if (open) {
        auto obj = Agent::Agent::instance().find("m_settings", "SettingsDialog");
        auto settings = std::dynamic_pointer_cast<Agent::Agent::Ptr<SettingsDialog> >(obj)->get();
        const SettingsDialog::Settings p = settings->settings();
        m_serial->setPortName(p.name);
        m_serial->setBaudRate(p.baudRate);
        m_serial->setDataBits(p.dataBits);
        m_serial->setParity(p.parity);
        m_serial->setStopBits(p.stopBits);
        m_serial->setFlowControl(p.flowControl);
        if (m_serial->open(QIODevice::ReadWrite)) {
            qDebug() << tr("Connected to %1 : %2, %3, %4, %5, %6")
                    .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                    .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl);
            emit deviceStatusUpdateSignal(true);
            readData();//刷新一次缓存，可能在打开之前已经接收了很多字节了
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
void XtermPanel::sendCommand(const QString &cmd) {
    ui->lineEdit->setText(cmd);
    emit ui->toolButton_send->clicked();
}
void XtermPanel::_saveState() {
    //保存m_valueList
    {
        QFile file(QDir(
                QStandardPaths::writableLocation(
                        QStandardPaths::AppDataLocation)).absoluteFilePath(
                "CommandHistory.txt"));
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            for (auto &i: m_valueList) {
                file.write(i.toUtf8());
                file.write("\n");
            }
        }
    }
}
void XtermPanel::_restoreState() {
    //恢复m_valueList
    {
        QFile file(QDir(
                QStandardPaths::writableLocation(
                        QStandardPaths::AppDataLocation)).absoluteFilePath(
                "CommandHistory.txt"));
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            QTextStream in(&file);  //用文件构造流
            QString line = in.readLine();;
            while (!line.isNull())//字符串有内容
            {
                m_valueList.push_back(line);
                line = in.readLine();//循环读取下行
            }
        }
    }
}
