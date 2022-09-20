#ifndef KEYEDIT_H
#define KEYEDIT_H

#include <QDialog>
#include "letter.h"

namespace Ui {
class KeyEdit;
}

class KeyEdit : public QDialog
{
    Q_OBJECT

public:
    explicit KeyEdit(QWidget *parent = 0);
    ~KeyEdit();

private slots:

    void on_addSlot_char_clicked();

    void on_refresh_clicked();

    void on_apply_clicked();

    void on_addSlot_word_clicked();

    void on_alphabetical_word_toggled(bool checked);

    void on_alphabetical_char_toggled(bool checked);

    void on_erase_all_char_clicked();

    void on_erase_all_word_clicked();

private:
    Ui::KeyEdit *ui;

    //bool sort_size(const QString * , const QString *);

    int totalltr;

    int totalwrd;

    Letter* ltr[345];

    Letter* wrd[345];

    bool char_alphabetical;

    bool word_alphabetical;

};

#endif // KEYEDIT_H
