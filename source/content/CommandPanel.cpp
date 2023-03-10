#include "CommandPanel.h"
#include "ui_CommandPanel.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFile>
#include <utility>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QKeyEvent>
#include <QComboBox>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>


class IntEdit : public QLineEdit {
Q_OBJECT
public:
    explicit IntEdit(QWidget *parent = nullptr) : QLineEdit(parent) {
        setValidator(new QIntValidator(this));
    }
    ~IntEdit() override = default;

    void keyPressEvent(QKeyEvent *e) override {
        switch (e->key()) {
            case Qt::Key_Up:
                e->accept();
                setText(QString("%1").arg(text().toInt() + 1));
                break;
            case Qt::Key_Down:
                e->accept();
                setText(QString("%1").arg(text().toInt() - 1));
                break;
            default:
                QLineEdit::keyPressEvent(e);
                break;
        }
    }
};

class DoubleEdit : public QLineEdit {
Q_OBJECT
public:
    explicit DoubleEdit(QWidget *parent = nullptr) : QLineEdit(parent) {
        setValidator(new QDoubleValidator(this));
    }
    ~DoubleEdit() override = default;

    QString text() {
        auto rtv = QLineEdit::text();
        if ((!rtv.contains('.')) && (!rtv.contains('E', Qt::CaseInsensitive)))
            rtv += ".0";
        return rtv;
    }
    void plusStep() {
        bool ok;
        auto val = text().toDouble(&ok);
        if (ok && val != 0.0) {
            auto v = abs(val);
            int step = FLT_MAX_10_EXP;
            for (; step >= FLT_MIN_10_EXP && pow(10.0, step) >= v; step--);
            step--;
            setText(QString("%1").arg((val + pow(10.0, (double) step))));
        }
    }
    void subStep() {
        bool ok;
        auto val = text().toDouble(&ok);
        if (ok && val != 0.0) {
            auto v = abs(val);
            int step = FLT_MAX_10_EXP;
            for (; step >= FLT_MIN_10_EXP && pow(10.0, step) >= v; step--);
            step--;
            setText(QString("%1").arg((val - pow(10.0, (double) step))));
        }
    }
    void keyPressEvent(QKeyEvent *e) override {
        switch (e->key()) {
            case Qt::Key_Up:
                e->accept();
                plusStep();
                break;
            case Qt::Key_Down:
                e->accept();
                subStep();
                break;
            default:
                QLineEdit::keyPressEvent(e);
                break;
        }
    }
};

class ComboBox : public QComboBox {
Q_OBJECT
public:
    explicit ComboBox(QWidget *parent = nullptr) : QComboBox(parent) {}

    void setText(const QString &str) {
        QString cur;
        cur = str.chopped(str.size() - str.indexOf(':'));
        QString list;
        list = str.sliced(str.indexOf(':') + 1);
        QStringList strList = list.split(',');

        for (auto &i: strList) {
            addItem(i);
        }
        setCurrentText(cur);
        m_listBack = list;
    }

    QString getFileString() {
        QString str;
        str += currentText();
        str += ':';
        str += m_listBack;
        return str;
    }
private:
    QString m_listBack;//????????????

};
#include "CommandPanel.moc"

// ?????????????????????
bool isDouble(const QString &str) {
    bool isNum;
    str.toDouble(&isNum);
    return isNum;
}

// ?????????????????????
bool isInt(const QString &str) {
    bool isInt;
    str.toInt(&isInt);
    return isInt && (!str.contains('.')) && (!str.contains('E', Qt::CaseInsensitive));
}

// ???????????????ComboBox
bool isComboBox(const QString &str) {
    return str.indexOf(':') >= 0;
};

class CommandPanel::CommandItem {
public:
    CommandItem(CommandPanel *_this) : m_this(_this) {}

    //??????????????????????????????
    void fromLineString(const QString &line) {
        QStringList strList = line.split(" ");
        //????????????m_itemStr
        {
            for (auto &i: strList) {
                if (i[0] != '<') {
                    m_itemStr.push_back({i, ""});
                } else {
                    m_itemStr.last().second = i.replace("<", "").replace(">", "");
                }
            }
        }

        //???????????????m_itemStr???????????? ????????????layout
        {
            if (m_itemStr.count() >= 2) {
                m_button = new QPushButton(m_this);
                m_button->setObjectName("QPushButton");
                m_button->setText(m_itemStr[0].first);
                m_button->setToolTip(m_itemStr[0].second);

                auto label = new QLabel(m_this);
                label->setObjectName("QLabel");
                label->setText(m_itemStr[1].first);
                label->setToolTip(m_itemStr[1].second);

                m_widgets.append(label);
                for (qsizetype i = 2; i < m_itemStr.count(); ++i) {
                    QWidget *back;
                    if (isComboBox(m_itemStr[i].first)) {
                        auto widget = new ComboBox(m_this);
                        widget->setObjectName("ComboBox");
                        widget->setText(m_itemStr[i].first);
                        widget->setToolTip(m_itemStr[i].second);
                        back = widget;
                    } else if (isInt(m_itemStr[i].first)) {
                        auto widget = new IntEdit(m_this);
                        widget->setObjectName("IntEdit");
                        widget->setText(m_itemStr[i].first);
                        widget->setToolTip(m_itemStr[i].second);
                        back = widget;
                    } else if (isDouble(m_itemStr[i].first)) {
                        auto widget = new DoubleEdit(m_this);
                        widget->setObjectName("DoubleEdit");
                        widget->setText(m_itemStr[i].first);
                        widget->setToolTip(m_itemStr[i].second);
                        back = widget;
                    } else {
                        auto widget = new QLineEdit(m_this);
                        widget->setObjectName("QLineEdit");
                        widget->setText(m_itemStr[i].first);
                        widget->setToolTip(m_itemStr[i].second);
                        back = widget;
                    }
                    m_widgets.push_back(back);
                }
                //???????????????layout
                m_hBoxLayout = new QHBoxLayout();
                m_hBoxLayout->addWidget(label);
                for (auto widget: m_widgets) {
                    m_hBoxLayout->addWidget(widget);
                }
            }
        }

        //????????????
        {
            connect(m_button, &QPushButton::released, [this]() {
                m_this->m_xtermPanel->sendCommand(toCmdLineSting());
            });
            auto lineEditSolt = [this]() {
                qDebug() << "QLineEdit?????????returnPressed";
                m_this->m_xtermPanel->sendCommand(toCmdLineSting());
            };
            for (auto widget: m_widgets) {
                if (widget->objectName() == "IntEdit")
                    connect(dynamic_cast<IntEdit *>(widget), &QLineEdit::returnPressed, lineEditSolt);
                else if (widget->objectName() == "DoubleEdit")
                    connect(dynamic_cast<DoubleEdit *>(widget), &QLineEdit::returnPressed, lineEditSolt);
                else if (widget->objectName() == "QLineEdit")
                    connect(dynamic_cast<QLineEdit *>(widget), &QLineEdit::returnPressed, lineEditSolt);
            }
        }

    }
    //?????????????????????????????????????????????
    QString toLineSting() {
        QString rtv;
        //button
        rtv += m_button->text() + ' ';
        if (m_button->toolTip().size() > 0) {
            rtv += QString() + '<' + m_button->toolTip() + '>' + ' ';
        }
        //??????
        for (auto widget: m_widgets) {
            if (widget->objectName() == "QLabel")
                rtv += dynamic_cast<QLabel *>(widget)->text();
            else if (widget->objectName() == "IntEdit")
                rtv += dynamic_cast<IntEdit *>(widget)->text();
            else if (widget->objectName() == "DoubleEdit")
                rtv += dynamic_cast<DoubleEdit *>(widget)->text();
            else if (widget->objectName() == "QLineEdit")
                rtv += dynamic_cast<QLineEdit *>(widget)->text();
            else if (widget->objectName() == "ComboBox")
                rtv += dynamic_cast<ComboBox *>(widget)->getFileString();
            rtv += ' ';
            if (widget->toolTip().size() > 0) {
                rtv += QString() + '<' + widget->toolTip() + '>' + ' ';
            }
        }
        rtv.chop(1);
        return rtv;
    }

    //??????????????????????????????????????????
    QString toCmdLineSting() {
        QString rtv;
        if (m_widgets.empty())
            return rtv;
        for (auto widget: m_widgets) {
            if (widget->objectName() == "QLabel")
                rtv += dynamic_cast<QLabel *>(widget)->text();
            else if (widget->objectName() == "IntEdit")
                rtv += dynamic_cast<IntEdit *>(widget)->text();
            else if (widget->objectName() == "DoubleEdit")
                rtv += dynamic_cast<DoubleEdit *>(widget)->text();
            else if (widget->objectName() == "QLineEdit")
                rtv += dynamic_cast<QLineEdit *>(widget)->text();
            else if (widget->objectName() == "ComboBox")
                rtv += dynamic_cast<ComboBox *>(widget)->currentText();
            rtv += ' ';
        }
        rtv.chop(1);
        return rtv;
    }
public:
    QVector<QPair<QString, QString> > m_itemStr;//????????????item??????????????????
    CommandPanel *m_this;// ??????????????????
    QPushButton *m_button = nullptr; //???????????????
    QHBoxLayout *m_hBoxLayout = nullptr;//????????????????????????layout
    QVector<QWidget *> m_widgets;//??????????????????????????????
};
void clearLyoutItem(QLayout *layout) {
    for (auto i = layout->count() - 1; i >= 0; i--) {
        auto item = layout->itemAt(i);
        layout->removeItem(item);
        if (item->layout())
            clearLyoutItem(item->layout());
        if (item->widget() != nullptr)
            delete item->widget();
    }
}
CommandPanel::CommandPanel(QWidget *parent, XtermPanel *xtermPanel, QString cpiniFile) :
        QWidget(parent), ui(new Ui::CommandPanel),
        m_xtermPanel(xtermPanel), m_cpiniFile(std::move(cpiniFile)) {
    ui->setupUi(this);
    auto pFormLayout = new QFormLayout();
    auto build_ui = [this, pFormLayout]() {
        //????????????
        {
            m_items.clear();
            QFile file(m_cpiniFile);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString line;
                QTextStream in(&file);  //??????????????????
                do {
                    line = in.readLine();//??????????????????????????????
                    if (!line.isNull() && !line.isEmpty()) {
                        auto item = QSharedPointer<CommandItem>(new CommandItem(this));
                        item->fromLineString(line);
                        m_items.append(item);
                    }
                } while (!line.isNull());//??????????????????
            }
        }
        //????????????????????????
        {
            for (qsizetype i = 0; i < m_items.count(); ++i) {
                pFormLayout->setWidget(i, QFormLayout::LabelRole, m_items[i]->m_button);
                pFormLayout->setLayout(i, QFormLayout::FieldRole, m_items[i]->m_hBoxLayout);
            }
        }
    };
    build_ui();

    auto *vboxLayout = new QVBoxLayout(this);
    auto *hboxLayout = new QHBoxLayout();

    auto edit_bt = new QPushButton(this);
    auto refresh_bt = new QPushButton(this);

    edit_bt->setText("????????????");
    refresh_bt->setText("????????????");
    hboxLayout->addWidget(edit_bt);
    hboxLayout->addWidget(refresh_bt);
    vboxLayout->addLayout(hboxLayout);
    vboxLayout->addLayout(pFormLayout);
    connect(edit_bt, &QPushButton::released, [this, build_ui, pFormLayout]() {
        qInfo() << QDir(
                qApp->applicationDirPath()).absoluteFilePath(
                "BowPad.exe");
        QProcess::startDetached(QDir(
                qApp->applicationDirPath()).absoluteFilePath(
                "BowPad.exe"), QStringList({m_cpiniFile}));
    });

    connect(refresh_bt, &QPushButton::released, [this, build_ui, pFormLayout]() {
        qInfo() << "????????????";
        clearLyoutItem(pFormLayout);
        qInfo() << "????????????";
        build_ui();
    });

}

CommandPanel::~CommandPanel() {
    //????????????????????????
    {
        QFile file(m_cpiniFile);
        if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
            for (const auto &i: m_items) {
                file.write((i->toLineSting() + '\n').toUtf8());
            }
        }
    }
    delete ui;
}
