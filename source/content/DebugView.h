#ifndef MARKMAKER_DEBUGVIEW_H
#define MARKMAKER_DEBUGVIEW_H

#include <QTextBrowser>

QT_BEGIN_NAMESPACE
namespace Ui
{
class DebugView;
}
QT_END_NAMESPACE

class DebugView : public QTextBrowser
{
    Q_OBJECT

public:
    explicit DebugView(QWidget *parent = nullptr);
    ~DebugView() override;

    void DebugMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    Ui::DebugView *ui;
};

#endif // MARKMAKER_DEBUGVIEW_H
