//
// Created by 17616 on 2022/11/4.
//

// You may need to build the project (run Qt uic code generator) to get "ui_DebugView.h" resolved

#include "DebugView.h"
#include "ui_DebugView.h"
#include <atomic>

std::atomic<DebugView *> debugView = nullptr;
void myDebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (debugView != nullptr)
        debugView.load()->DebugMessageOutput(type, context, msg);
}

DebugView::DebugView(QWidget *parent) : QTextBrowser(parent), ui(new Ui::DebugView)
{
    ui->setupUi(this);
    debugView = this;
    qInstallMessageHandler(myDebugMessageOutput);
}

DebugView::~DebugView()
{
    debugView = nullptr;
    delete ui;
}
void DebugView::DebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    switch (type) {
        case QtWarningMsg:
            append(QString("<font color=\"#ff8d19\"> %1\t%2 </font>").arg(context.function, msg));
            break;
        case QtCriticalMsg:
        case QtFatalMsg:
            append(QString("<font color=red> %1\t%2 </font>").arg(context.function, msg));
            break;
        case QtMsgType::QtDebugMsg:
            #ifdef NDEBUG
            break;
            #endif
        case QtMsgType::QtInfoMsg:
        default:
            append(QString("<font color=black> %1\t%2 </font>").arg(context.function, msg));
            break;
    }
}
