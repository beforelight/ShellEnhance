#ifndef TOP_H
#define TOP_H

#include <QMainWindow>
#include <memory>
QT_BEGIN_NAMESPACE
namespace Ui { class Top; }
QT_END_NAMESPACE

class Top : public QMainWindow
{
    Q_OBJECT

public:
    explicit Top(QWidget *parent = nullptr);
    ~Top() override;


public:
    void _saveState();
    void _restoreState();

private:
    Ui::Top *ui;
    struct impl_t;
    friend struct impl_t;
    std::shared_ptr<impl_t> impl;
};
#endif // TOP_H
