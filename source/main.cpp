#include "Top.h"
#include <QApplication>
#include <QFile>
#include <QFileSystemWatcher>
#include <QDir>
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Top w;
    w.show();

    //设置qss
    {
        #define STYLE_SHEET_PATH QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("style.qss")
        QFile file(STYLE_SHEET_PATH);
        if (file.open(QIODevice::ReadOnly)) {
            qobject_cast<QApplication *>(QApplication::instance())->setStyleSheet(file.readAll());
            file.close();
        }
        #ifdef QT_DEBUG
        auto *fileWatcher = new QFileSystemWatcher(QApplication::instance());
        fileWatcher->addPath(STYLE_SHEET_PATH);
        QObject::connect(fileWatcher, &QFileSystemWatcher::fileChanged, [](const QString &path) {
            qDebug() << "fileChanged: " << path;
            QFile file(path);
            if (file.open(QIODevice::ReadOnly)) {
                qobject_cast<QApplication *>(QApplication::instance())->setStyleSheet(file.readAll());
                file.close();
            }
        });
        #endif
    }


    return a.exec();
}
