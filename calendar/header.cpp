#include "header.h"

Header::Header(QWidget *parent) : QWidget(parent)
{
    m_rootLayout = new QHBoxLayout(this);

    m_buttonBack = new QPushButton(QString("      <      "), this);
    m_buttonNext = new QPushButton(QString("      >      "), this);
    m_labelDate = new QLabel(this);

    m_buttonBack->setMaximumWidth(60);
    m_buttonNext->setMaximumWidth(60);
    m_buttonBack->setMaximumHeight(40);
    m_buttonNext->setMaximumHeight(40);
    m_buttonBack->setStyleSheet("border-style: solid; border-width: 2px; border-radius: 12px; border-color: #000000;");
    m_buttonNext->setStyleSheet("border-style: solid; border-width: 2px; border-radius: 12px; border-color: #000000;");
    m_buttonBack->setShortcut(QKeySequence(Qt::Key_Left));
    m_buttonNext->setShortcut(QKeySequence(Qt::Key_Right));

    m_labelDate->setMaximumHeight(40);
    m_labelDate->setFixedWidth(400);
    m_labelDate->setStyleSheet(LABELDATE_STYLE);
    m_labelDate->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    m_labelDate->setStyleSheet("border-style: solid; border-width: 0px; border-color: #000000;");
    m_labelDate->setFont(QFont("나눔고딕", 25, QFont::Bold));

    m_rootLayout->addWidget(m_buttonBack);
    m_rootLayout->addWidget(m_labelDate);
    m_rootLayout->addWidget(m_buttonNext);

    connect(m_buttonBack, SIGNAL(clicked(bool)), this, SIGNAL(onPreviousClicked()));
    connect(m_buttonNext, SIGNAL(clicked(bool)), this, SIGNAL(onNextClicked()));
}


void Header::setDate(QString &date) {
    m_labelDate->setText(date);
}
