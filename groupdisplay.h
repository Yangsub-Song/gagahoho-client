#ifndef GROUPDISPLAY_H
#define GROUPDISPLAY_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class GroupDisplay;
}

class GroupDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit GroupDisplay(QWidget *parent = nullptr);
    ~GroupDisplay();

private:
    Ui::GroupDisplay *ui;
    QPushButton *buttonGroups[16];

signals:
    void groupClicked(int grp);
};

#endif // GROUPDISPLAY_H
