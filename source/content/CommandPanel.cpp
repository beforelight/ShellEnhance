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
#include "CommandPanel.moc"

// 判断是否为数字
bool isDouble(const QString &str) {
    bool isNum;
    str.toDouble(&isNum);
    return isNum;
}

// 判断是否为整数
bool isInt(const QString &str) {
    bool isInt;
    str.toInt(&isInt);
    return isInt && (!str.contains('.')) && (!str.contains('E', Qt::CaseInsensitive));
}

class CommandPanel::CommandItem {
public:
    CommandItem(CommandPanel *_this) : m_this(_this) {}

    //构造之后再添加到界面
    void fromLineString(const QString &line) {
        QStringList strList = line.split(" ");
        //先解析到m_itemStr
        {
            for (auto &i: strList) {
                if (i[0] != '<') {
                    m_itemStr.push_back({i, ""});
                } else {
                    m_itemStr.last().second = i.replace("<", "").replace(">", "");
                }
            }
        }

        //从解析完的m_itemStr构造对象 并添加到layout
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
                    if (isInt(m_itemStr[i].first)) {
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
                //对象添加到layout
                m_hBoxLayout = new QHBoxLayout();
                m_hBoxLayout->addWidget(label);
                for (auto widget: m_widgets) {
                    m_hBoxLayout->addWidget(widget);
                }
            }
        }

        //连接信号
        connect(m_button, &QPushButton::released, [this]() {
            m_this->m_xtermPanel->sendCommand(toCmdLineSting());
        });
    }
//格式化为字符串，用于保存到文件
//    QString toLineSting() {
//    }

    //格式化为字符串，用于触发命令
    QString toCmdLineSting() {
        QString rtv;
        if (m_widgets.empty())
            return rtv;
        for (qsizetype i = 0; i < m_widgets.count(); ++i) {
            QString tmp = i == 0 ? "" : " ";
            if (m_widgets[i]->objectName() == "QLabel")
                tmp += dynamic_cast<QLabel *>(m_widgets[i])->text();
            else if (m_widgets[i]->objectName() == "IntEdit")
                tmp += dynamic_cast<IntEdit *>(m_widgets[i])->text();
            else if (m_widgets[i]->objectName() == "DoubleEdit")
                tmp += dynamic_cast<DoubleEdit *>(m_widgets[i])->text();
            else if (m_widgets[i]->objectName() == "QLineEdit")
                tmp += dynamic_cast<QLineEdit *>(m_widgets[i])->text();
            rtv += tmp;
        }
        return rtv;
    }
public:
    QVector<QPair<QString, QString> > m_itemStr;//第一项是item第二项是注释
    CommandPanel *m_this;// 供指针管理的
    QPushButton *m_button = nullptr; //命令的按钮
    QHBoxLayout *m_hBoxLayout = nullptr;//命令和后面采用的layout
    QVector<QWidget *> m_widgets;//命令和后面一系列玩意

};

CommandPanel::CommandPanel(QWidget *parent, XtermPanel *xtermPanel, QString cpiniFile) :
        QWidget(parent), ui(new Ui::CommandPanel),
        m_xtermPanel(xtermPanel), m_cpiniFile(std::move(cpiniFile)) {
    ui->setupUi(this);
    //构造子项
    {
        QFile file(m_cpiniFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString line;
            QTextStream in(&file);  //用文件构造流
            do {
                line = in.readLine();//读取一行放到字符串里
                if (!line.isNull()) {
                    auto item = QSharedPointer<CommandItem>(new CommandItem(this));
                    item->fromLineString(line);
                    m_items.append(item);
                }
            } while (!line.isNull());//字符串有内容
        }
    }

    //把子项添加到界面
    {
        auto layout = new QFormLayout(this);
        for (qsizetype i = 0; i < m_items.count(); ++i) {
            layout->setWidget(i, QFormLayout::LabelRole, m_items[i]->m_button);
            layout->setLayout(i, QFormLayout::FieldRole, m_items[i]->m_hBoxLayout);
        }
        setLayout(layout);
    }

}

CommandPanel::~CommandPanel() {
    delete ui;
}
