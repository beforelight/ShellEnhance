//
// Created by 17616 on 2022/11/25.
//

#ifndef SHELLE_COMMANDPANEL_H
#define SHELLE_COMMANDPANEL_H

#include <QWidget>
#include <memory>
#include "XtermPanel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CommandPanel; }
QT_END_NAMESPACE

class CommandPanel : public QWidget {
Q_OBJECT

public:
    explicit CommandPanel(QWidget *parent, XtermPanel *xtermPanel, QString cpiniFile);
    ~CommandPanel() override;

private:
    Ui::CommandPanel *ui;
    XtermPanel *m_xtermPanel;
    QString m_cpiniFile;
    class CommandItem;
    friend class CommandItem;
    QVector<QSharedPointer<CommandItem> > m_items;
};


#endif //SHELLE_COMMANDPANEL_H
