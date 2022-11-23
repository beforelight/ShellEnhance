//
// Created by 17616 on 2022/11/23.
//

// You may need to build the project (run Qt uic code generator) to get "ui_XtermPanel.h" resolved

#include "XtermPanel.h"
#include "ui_XtermPanel.h"


XtermPanel::XtermPanel(QWidget *parent) :
        QWidget(parent), ui(new Ui::XtermPanel) {
    ui->setupUi(this);
}

XtermPanel::~XtermPanel() {
    delete ui;
}
