#ifndef LETTER_H
#define LETTER_H

#include <QWidget>

namespace Ui {
class Letter;
}

class Letter : public QWidget
{
    Q_OBJECT

public:
    explicit Letter(QWidget *parent = 0);
    ~Letter();

    Ui::Letter *ui;

    QString input;

    QString output;

    bool destroyed;

private slots:
     void on_Replace_textChanged(const QString &arg1);

     void on_Find_textChanged(const QString &arg1);

     void on_Clear_clicked();

private:

};

#endif // LETTER_H
