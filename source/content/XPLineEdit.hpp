#ifndef SHELLE_XPLINEEDIT_HPP
#define SHELLE_XPLINEEDIT_HPP
#include <QLineEdit>

class XPLineEdit : public QLineEdit {
Q_OBJECT

signals:
    void ctrlUpPressed();
    void ctrlDownPressed();

public:
    explicit XPLineEdit(QWidget *parent = nullptr) : QLineEdit(parent) {}
    void keyPressEvent(QKeyEvent *e) override;


};
#endif //SHELLE_XPLINEEDIT_HPP
