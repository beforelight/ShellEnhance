#ifndef SHELLE_XTERMPANEL_H
#define SHELLE_XTERMPANEL_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class XtermPanel; }
QT_END_NAMESPACE

class XtermPanel : public QWidget {
Q_OBJECT

public:
    explicit XtermPanel(QWidget *parent = nullptr);
    ~XtermPanel() override;

private:
    Ui::XtermPanel *ui;
};


#endif //SHELLE_XTERMPANEL_H
