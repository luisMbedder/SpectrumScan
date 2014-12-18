#ifndef GUI_H
#define GUI_H

#include <QDialog>

namespace Ui {
class Gui;
}

class Gui : public QDialog
{
    Q_OBJECT

public:
    explicit Gui(QWidget *parent = 0);
    ~Gui();

private:
    Ui::Gui *ui;
};

#endif // GUI_H
