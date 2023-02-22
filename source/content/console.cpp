/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "console.h"

#include <QScrollBar>
#include <QKeyEvent>
#include <QAction>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
Console::Console(QWidget *parent) :
        QTextEdit(parent) {
    setReadOnly(true);
    document()->setMaximumBlockCount(10000);
//    QPalette p = palette();
//    p.setColor(QPalette::Base, Qt::black);
//    p.setColor(QPalette::Text, Qt::green);
//    setPalette(p);
    auto copyAction = new QAction(this);
    copyAction->setIcon(QIcon(":/icon/copy.svg"));
    copyAction->setText("复制选中内容");
    connect(copyAction, &QAction::triggered, [this]() { copy(); });
    addAction(copyAction);

    auto clr_action = new QAction("清空历史", this);
    addAction(clr_action);
    connect(clr_action, &QAction::triggered, [this]() { clear(); });

    //读取历史
    {
        QFile file(QDir(
                QStandardPaths::writableLocation(
                        QStandardPaths::AppDataLocation)).absoluteFilePath(
                "ConsoleHistory.txt"));
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            setText(in.readAll());
            QScrollBar *bar = verticalScrollBar();
            bar->setValue(bar->maximum());
            MoveTextCursorToEnd();
        }
    }

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    m_timer->setInterval(50);
    connect(m_timer, &QTimer::timeout, [this]() {
        MoveTextCursorToEnd();
        insertPlainText(m_data);
        m_data.clear();
        MoveTextCursorToEnd();
    });
}

void Console::putData(const QByteArray &data) {
    m_data.append(data);
    #ifndef NDEBUG
    {
        auto idx = m_data.lastIndexOf('\n') + 1;
        if (idx > 0) {
            auto d1 = m_data.chopped(m_data.size() - idx);
            auto d2 = m_data.sliced(idx);
            Q_UNUSED(d1);
            Q_UNUSED(d2);
        }
    };
    #endif
    if (m_data.size() > 1000) {
        //尾项查找\n
        auto idx = m_data.lastIndexOf('\n') + 1;
        if(idx>0){
            MoveTextCursorToEnd();
            insertPlainText(m_data.chopped(m_data.size() - idx));
            m_data = m_data.sliced(idx);
            MoveTextCursorToEnd();
        }
    }
    m_timer->start();
}

void Console::setLocalEchoEnabled(bool set)
{
    m_localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
        case Qt::Key_Backspace:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
        default:
            if ((e->key() == Qt::Key_C || e->key() == Qt::Key_X) &&
                e->modifiers() == Qt::ControlModifier) { //复制粘贴
                copy();
            } else {
                emit getData(e->text().toLocal8Bit());
            }
            break;
    }
}
Console::~Console() {
    //保存历史
    {
        QFile file(QDir(
                QStandardPaths::writableLocation(
                        QStandardPaths::AppDataLocation)).absoluteFilePath(
                "ConsoleHistory.txt"));
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            file.write(toPlainText().toUtf8());
            file.write(QString(
                    "\n=================================================\n"
                    "******    以上为%1之前的消息     ********\n"
                    "=================================================\n"
            ).arg(
                    QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ddd")
            ).toUtf8());
        }
    }
}
void Console::MoveTextCursorToEnd() {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
}
