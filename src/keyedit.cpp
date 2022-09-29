#include "keyedit.h"
#include "letter.h"
#include "mainwindow.h"
#include "ui_keyedit.h"
#include "ui_letter.h"
#include <QMessageBox>
#include <QtGui>

extern QString layoutfile;

struct Entry {
    QString input;
    QString output;
};


KeyEdit::KeyEdit(QWidget* parent) : QDialog(parent), ui(new Ui::KeyEdit) {
    ui->setupUi(this);
    //by default total entries = 0
    char_alphabetical = false;
    word_alphabetical = false;
    //load entries by refreshing
    ui->refresh->hide();
    on_refresh_clicked();
}

KeyEdit::~KeyEdit() {
    delete ui;
    for (auto& e : ltr) { delete e; }
    for (auto& e : wrd) { delete e; }
    ltr.resize(0);
    wrd.resize(0);
}

void KeyEdit::on_addSlot_char_clicked() {
    //add a new entry from last value
    ltr.push_back(new Letter(ui->char_contents));
    //will be deleted unless changed
    ltr.back()->destroyed = true;
    //entrie's location
    ui->char_entries->addWidget(ltr.back());
    //scroll down
    //ui->scrollArea->ensureWidgetVisible(ui->newslot,100,100);
}


void KeyEdit::on_addSlot_word_clicked() {
    //add a new entry from last value
    wrd.push_back(new Letter(ui->word_contents));
    //will be deleted unless changed
    wrd.back()->destroyed = true;
    //entrie's location
    ui->word_entries->addWidget(wrd.back());
    //scroll down
    //ui->scrollArea->ensureWidgetVisible(ui->newslot,100,100);
}

void KeyEdit::on_refresh_clicked() {
    ////for char
    //cleans entries
    for (auto& e : ltr) { delete e; }
    ltr.resize(0);
    //begins to read filter
    QSettings settings(layoutfile, QSettings::NativeFormat);
    settings.beginReadArray("char");
    //creates all entries based on total value
    size_t totalltr = settings.value("size").toInt();
    for (size_t i = 0; i < totalltr; i++) {
        settings.setArrayIndex(i);
        //initializes entry
        ltr.push_back(new Letter(ui->char_contents));
        //sets-loads input values
        ltr[i]->ui->Find->setMaxLength(3);
        ltr[i]->ui->Replace->setMaxLength(3);
        ltr[i]->input = settings.value("input").toString();
        ltr[i]->ui->Find->setText(ltr[i]->input);
        //sets-loads output valuse
        ltr[i]->output = settings.value("output").toString();
        ltr[i]->ui->Replace->setText(ltr[i]->output);
        //loaded values are not to be destroyed
        ltr[i]->destroyed = false;
        //sets location
        ui->char_entries->addWidget(ltr[i]);
    }
    settings.endArray();
    ////for word
    //cleans entries
    for (auto& e : wrd) { delete e; }
    wrd.resize(0);
    //begins to read filter
    settings.beginReadArray("word");
    //creates all entries based on total value
    size_t totalwrd = settings.value("size").toInt();
    for (int i = 0; i < totalwrd; i++) {
        settings.setArrayIndex(i);
        //initializes entry
        wrd.push_back(new Letter(ui->word_contents));
        //sets-loads input values
        wrd[i]->input = settings.value("input").toString();
        wrd[i]->ui->Find->setText(wrd[i]->input);
        //sets-loads output valuse
        wrd[i]->output = settings.value("output").toString();
        wrd[i]->ui->Replace->setText(wrd[i]->output);
        //loaded values are not to be destroyed
        wrd[i]->destroyed = false;
        //sets location
        ui->word_entries->addWidget(wrd[i]);
    }
    settings.endArray();
}

bool sort_size(const Entry& s1, const Entry& s2) { return s1.input.size() > s2.input.size(); }

bool sort_value(const Entry& s1, const Entry& s2) { return s1.input.toLower() < s2.input.toLower(); }

void KeyEdit::on_apply_clicked() {
    ////for char
    //begins to write filter
    QSettings settings(layoutfile, QSettings::NativeFormat);
    //sorts by size
    QList<Entry> entry_char;
    for (size_t i = 0; i < ltr.size(); i++) {
        //skips destroyed values
        if (!ltr[i]->destroyed) {
            //store values
            Entry tmp;
            tmp.input = ltr[i]->input;
            tmp.output = ltr[i]->output;
            entry_char << tmp;
        }
    }
    //sort by size
    if (char_alphabetical) qStableSort(entry_char.begin(), entry_char.end(), sort_value);
    qStableSort(entry_char.begin(), entry_char.end(), sort_size);
    //begins to write filter
    settings.beginWriteArray("char");
    //clears array
    settings.remove("");
    //write individual valuses
    {
        int total = entry_char.size();
        for (int i = 0; i < total; i++) {
            //saves values
            settings.setArrayIndex(i);
            settings.setValue("input", entry_char[i].input);
            settings.setValue("output", entry_char[i].output);
        }
    }
    //ends char array
    settings.endArray();
    ////for word
    //sorts by size
    QList<Entry> entry_word;
    for (size_t i = 0; i < wrd.size(); i++) {
        //skips destroyed values
        if (!wrd[i]->destroyed) {
            //store values
            Entry tmp;
            tmp.input = wrd[i]->input;
            tmp.output = wrd[i]->output;
            entry_word << tmp;
        }
    }
    //sort by size
    if (word_alphabetical) qStableSort(entry_word.begin(), entry_word.end(), sort_value);
    qStableSort(entry_word.begin(), entry_word.end(), sort_size);
    //begins to write filter
    settings.beginWriteArray("word");
    //clears array
    settings.remove("");
    //write individual valuses
    {
        int total = entry_word.size();
        for (int i = 0; i < total; i++) {
            //saves values
            settings.setArrayIndex(i);
            settings.setValue("input", entry_word[i].input);
            settings.setValue("output", entry_word[i].output);
        }
    }
    //syncs-saves settings and refreshes
    settings.endArray();
    settings.sync();
    on_refresh_clicked();
}

void KeyEdit::on_alphabetical_word_toggled(bool checked) { word_alphabetical = checked; }

void KeyEdit::on_alphabetical_char_toggled(bool checked) { char_alphabetical = checked; }

void KeyEdit::on_erase_all_char_clicked() {
    QMessageBox x;
    if (QMessageBox::Yes == x.warning(
                                this,
                                "Warning",
                                "This will delete all of the entries.\nAre you sure you want to continue?",
                                QMessageBox::Yes,
                                QMessageBox::No
                            )) {
        for (auto& e : ltr) { delete e; }
        ltr.resize(0);
    }
}

void KeyEdit::on_erase_all_word_clicked() {
    QMessageBox x;
    if (QMessageBox::Yes == x.warning(
                                this,
                                "Warning",
                                "This will delete all of the entries.\nAre you sure you want to continue?",
                                QMessageBox::Yes,
                                QMessageBox::No
                            )) {
        for (auto& e : wrd) { delete e; }
        wrd.resize(0);
    }
}
