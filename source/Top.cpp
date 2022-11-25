#include "Top.h"
#include "./ui_Top.h"
#include "DebugView.h"
#include "DockManager.h"
#include <QDebug>
#include <QSettings>
#include "Agent.h"
#include "XtermPanel.h"
#include "settingsdialog.h"
#include <QTimer>
#include <QFile>
#include <QFileInfoList>
#include <QDir>
#include "CommandPanel.h"
struct Top::impl_t {
    Top *_this;
    ads::CDockManager *m_DockManager; // Qt 界面对象不需要使用智能指针
    Agent::Agent &m_agent = Agent::Agent::instance();
    DebugView *m_debugView;
    XtermPanel *m_xtermPanel;
    SettingsDialog *m_settings;

    explicit impl_t(Top *_this_) : _this(_this_) {


        m_DockManager = new ads::CDockManager(_this);

        auto addDockWidget = [this](QWidget *widget, const QString &title) {
            auto *DockWidget = new ads::CDockWidget(title);
            widget->setWindowTitle(title);
            widget->setObjectName(title + "_widget");
            DockWidget->setWidget(widget);
            DockWidget->setObjectName(title + "_dockWidget");
            _this->ui->menu_panel->addAction(DockWidget->toggleViewAction());
            m_DockManager->addDockWidget(ads::NoDockWidgetArea, DockWidget);
            return DockWidget;
        };

        m_debugView = new DebugView(_this);
        addDockWidget(m_debugView, "日志");

        m_settings = new SettingsDialog(_this);
        m_settings->setObjectName("SettingsDialog");
        connect(_this->ui->action_com, &QAction::triggered, m_settings, &SettingsDialog::show);
        QTimer::singleShot(500, [this]() {
            _this->ui->action_com->trigger();
        });

        m_xtermPanel = new XtermPanel(m_settings, _this);
        addDockWidget(m_xtermPanel, "命令行");

        connect(m_settings, &SettingsDialog::openDeviceSignal, m_xtermPanel, &XtermPanel::openSerialPortSlot);
        connect(m_xtermPanel, &XtermPanel::deviceStatusUpdateSignal, m_settings, &SettingsDialog::setDeviceStatusSlot);
        connect(m_xtermPanel, &XtermPanel::deviceStatusUpdateSignal, _this->ui->action_com, &QAction::setChecked);

        {
            //遍历当前指定目录下所有文件路径
            QFileInfoList list = QDir(qApp->applicationDirPath()).entryInfoList();
            //循环判断是否是后缀为csv的文件
                    foreach(QFileInfo info, list) {
                    if (!info.isFile() || (0 != info.suffix().compare("cpini", Qt::CaseInsensitive)))
                        continue;
                    qInfo() << "发现命令面板文件" << info.baseName();
                    auto cmdPanel = new CommandPanel(_this, m_xtermPanel, info.filePath());
                    addDockWidget(cmdPanel, info.baseName());
                }
        }

        qDebug() << "ShellE";
    }
};
Top::Top(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::Top) {
    ui->setupUi(this);
    #ifndef NDEBUG
    setWindowTitle("ShellE(debug版本)");
    #else
    setWindowTitle("ShellE");
    #endif
    impl = std::make_shared<Top::impl_t>(this);
    _restoreState();
}

void Top::_saveState() {
    QSettings Settings("Settings.ini", QSettings::IniFormat);
    Settings.setValue("top/Geometry", this->saveGeometry());
    Settings.setValue("top/State", this->saveState());
    Settings.setValue("top/DockingState", impl->m_DockManager->saveState());
}
void Top::_restoreState() {
    QSettings Settings("Settings.ini", QSettings::IniFormat);
    this->restoreGeometry(Settings.value("top/Geometry").toByteArray());
    this->restoreState(Settings.value("top/State").toByteArray());
    impl->m_DockManager->restoreState(Settings.value("top/DockingState").toByteArray());
}

Top::~Top() {
    _saveState();
    delete ui;
}
