#include "letter.h"
#include "ui_letter.h"

Letter::Letter(QWidget* parent) : QWidget(parent), ui(new Ui::Letter) { ui->setupUi(this); }

Letter::~Letter() { delete ui; }

void Letter::on_Replace_textChanged(const QString& arg) {
    //set symbol, make active
    output = arg;
    destroyed = false;
}

void Letter::on_Find_textChanged(const QString& arg) {
    //set character, make active
    input = arg;
    destroyed = false;
}

void Letter::on_Clear_clicked() {
    //hide entry and set it to be ignored
    this->hide();
    destroyed = true;
}
