#include "groupdisplay.h"
#include "ui_groupdisplay.h"

#include <QSizePolicy>
#include <QTextCodec>

#define BUTTON_STYLESHEET "background-color: #5200FF; color: white;" \
    "border-style: solid; border-radius: 30px; border-width: 5px; border-color: #FFFFFF; margin: 10px;"

GroupDisplay::GroupDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GroupDisplay)
{
    ui->setupUi(this);

    ui->widget->setStyleSheet("margin: 10px;");
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");

    for(int i = 0; i < 16; i++) {
        buttonGroups[i] = new QPushButton();
        buttonGroups[i]->setStyleSheet(BUTTON_STYLESHEET);
        buttonGroups[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        buttonGroups[i]->setText(QString("%1 그룹").arg(i+1));
        auto font = buttonGroups[i]->font();
        font.setPointSize(22);
        font.setBold(true);
        font.setFamily(codec->toUnicode("나눔바른고딕"));
        buttonGroups[i]->setFont(font);
        ui->gridLayout->addWidget(buttonGroups[i], i / 4, i % 4);

        connect(buttonGroups[i], &QPushButton::clicked, [this, i](){
            emit groupClicked(i);
        });
    }
}

GroupDisplay::~GroupDisplay()
{
    delete ui;
}
