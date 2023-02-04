#include "XPLineEdit.hpp"
#include <QKeyEvent>
void XPLineEdit::keyPressEvent(QKeyEvent *e) {
    if ((e->modifiers() & Qt::ControlModifier) != 0)
        switch (e->key()) {
            case Qt::Key_Up:
                e->accept();
                emit ctrlUpPressed();
                break;
            case Qt::Key_Down:
                e->accept();
                emit ctrlDownPressed();
                break;
            default:
                QLineEdit::keyPressEvent(e);
                break;
        }
    else
        QLineEdit::keyPressEvent(e);
}
