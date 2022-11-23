#include "Top.h"
#include <QApplication>
#include <QFile>
QString loadStyleSheet(const QString &styleSheetFile) {
    QFile file(styleSheetFile);
    file.open(QFile::ReadOnly);
    QString styleSheet;
    if (file.isOpen()) {
        styleSheet += QLatin1String(file.readAll());//读取样式表文件
        file.close();
    }
    return styleSheet;
}
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setStyleSheet(loadStyleSheet(":/styleSheet/globalStyleSheet.qss"));
    Top w;
    w.show();
    return a.exec();
}
