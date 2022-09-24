#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QClipboard>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSettings>
#include <aes256.hpp>
extern QString layoutfile;

std::vector<unsigned char> toByteArray1000(QByteArray s) {
    std::vector<unsigned char> result;
    if(s.size() > 1000) s.resize(1000);
    result.insert(result.end(), s.begin(), s.end());
    return result;
}

std::vector<unsigned char> toByteArray(QString s) {
    auto str = s.toUtf8();
    std::vector<unsigned char> result;
    result.insert(result.end(), str.begin(), str.end());
    return result;
}


QString toQString(std::vector<unsigned char> arr) { return QString::fromUtf8((const char*)arr.data(), arr.size()); }

QString base64Encode(QString data) { return data.toUtf8().toBase64(); }

QString base64Encode(std::vector<unsigned char> data) {
    auto arr = QByteArray::fromRawData((const char*)data.data(), data.size());
    return arr.toBase64();
}

// QString base64Decode(QString data){
//     return QString::fromUtf8(QByteArray::fromBase64(data.toUtf8()));
// }

std::vector<unsigned char> base64Decode(QString data) {
    QByteArray arr = QByteArray::fromBase64(data.toUtf8());
    std::vector<unsigned char> result;
    result.insert(result.end(), arr.begin(), arr.end());
    return result;
}

void MainWindow::loadSettings() { //open settingsfile window group
    QSettings settings(settingsfile, QSettings::NativeFormat);
    settings.beginGroup("window");
    //load values into variables
    autocopy = settings.value("autocopy", false).toBool();
    ontop = settings.value("ontop", false).toBool();
    frameless = settings.value("frameless", false).toBool();
    revtext = settings.value("reversetext", false).toBool();
    revfilter = settings.value("inverse", false).toBool();
    useAES = settings.value("useAES", false).toBool();
    layoutfile = settings.value("currentfile", QDir::homePath() + "/.textfilter/layouts/default.flt").toString();
    this->resize(settings.value("size").toSize());
    this->move(settings.value("pos").toPoint());
    aesKeyPath = settings.value("aesKeyPath").toString();
    enableFilters = settings.value("enableFilters", false).toBool();
    allowRememberKey = settings.value("allowRememberKey", false).toBool();
    if (settings.value("aesKey", "").toString() != "") {
        aesKey = toByteArray(settings.value("aesKey", "").toString());
    }
    settings.endGroup();

    if (aesKeyPath != nullptr && aesKeyPath != "") {
        QFile f(aesKeyPath);
        f.open(QIODevice::ReadOnly);
        QByteArray contents = f.readAll();
        aesKey = toByteArray(contents);
    }
}


void MainWindow::saveSettings() {
    //open settingsfile window group
    QSettings settings(settingsfile, QSettings::NativeFormat);
    settings.beginGroup("window");
    //save values into file
    settings.setValue("autocopy", autocopy);
    settings.setValue("ontop", ontop);
    settings.setValue("frameless", frameless);
    settings.setValue("reversetext", revtext);
    settings.setValue("inverse", revfilter);
    settings.setValue("currentfile", layoutfile);
    if (!isMaximized()) settings.setValue("size", this->size());
    settings.setValue("pos", this->pos());
    settings.setValue("useAES", useAES);
    settings.setValue("aesKeyPath", aesKeyPath);
    settings.setValue("enableFilters", enableFilters);
    settings.setValue("allowRememberKey", allowRememberKey);
    if (allowRememberKey && aesKeyPath == "") {
        settings.setValue("aesKey", toQString(aesKey));
    } else {
        settings.setValue("aesKey", "");
    }
    settings.endGroup();
}


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    settingsfile = QDir::homePath() + "/.textfilter/settings";
    alreadyopen = false;
    loadSettings();
    //display loaded settings.
    ui->autocopy->setChecked(autocopy);
    ui->actionAlways_on_top->setChecked(ontop);
    ui->actionFrameless->setChecked(frameless);
    ui->revtext->setChecked(revtext);
    ui->checkboxAES->setChecked(useAES);
    ui->actionRemember_Key_Values->setChecked(allowRememberKey);
    ui->checkEnableFilter->setChecked(enableFilters);
    ui->checkBox->setChecked(revfilter);
    transparency = 0.9;
    //window flags
    if (ontop) flags |= Qt::WindowStaysOnTopHint;
    if (frameless) flags |= Qt::FramelessWindowHint;
    setWindowFlags(flags);
    //check if lilter directory exists
    layoutsDir = QDir::homePath() + "/.textfilter/layouts";
    if (!layoutsDir.exists()) { layoutsDir.mkpath("."); }
    //copy default file from opt if it does not exist
    if (!QFile::exists(QDir::homePath() + "/.textfilter/layouts/default.flt")) {
        // QFile::copy(QApplication::applicationDirPath() + "/layouts/default.key" , QDir::homePath() + "/.textfilter/layouts/default.flt");
        QFile::copy(":/assets/default.flt", QDir::homePath() + "/.textfilter/layouts/default.flt");
    }
    //error message if the layout file does not exist
    if (!QFile::exists(layoutfile)) {
        QMessageBox z;
        z.warning(this, "Warning", "The previously used filter does not exist.");
        on_actionOpen_Filter_triggered();
    }
}

MainWindow::~MainWindow() {
    //saves settings before destruction
    saveSettings();
    delete ui;
}

void MainWindow::filter(QString* text) {
    //open key configuration
    QSettings settings(layoutfile, QSettings::NativeFormat);
    //sets input output values
    QString input = "input";
    QString output = "output";

    checkAES();

    if (revfilter) {
        if (useAES) {
            std::vector<unsigned char> encryptedData = base64Decode(*text);
            std::vector<unsigned char> decryptedData;

            Aes256::decrypt(aesKey, encryptedData, decryptedData);
            *text = toQString(decryptedData);
        }

        //reversing the text
        if (revtext) {
            QString reversed;
            int size = text->size() - 1;
            for (int i = 0, j = size; i <= size; i++, j--) reversed[i] = (*text)[j];
            (*text) = reversed;
        }
        if (enableFilters) {
            input = "output";
            output = "input";
            ////char
            settings.beginReadArray("char");
            {
                const int total = settings.value("size").toInt();
                for (int i = 0; i <= total; i++) {
                    settings.setArrayIndex(i);
                    text->replace(settings.value(input).toString(), settings.value(output).toString());
                }
            }
            settings.endArray();
            ////word
            settings.beginReadArray("word");
            {
                const int total = settings.value("size").toInt();
                for (int i = 0; i <= total; i++) {
                    settings.setArrayIndex(i);
                    text->replace(settings.value(input).toString(), settings.value(output).toString());
                }
            }
            settings.endArray();
        }
    } else {
        if (enableFilters) {
            ////word
            settings.beginReadArray("word");
            {
                const int total = settings.value("size").toInt();
                for (int i = 0; i <= total; i++) {
                    settings.setArrayIndex(i);
                    text->replace(settings.value(input).toString(), settings.value(output).toString());
                }
            }
            settings.endArray();
            ////char
            settings.beginReadArray("char");
            {
                const int total = settings.value("size").toInt();
                for (int i = 0; i <= total; i++) {
                    settings.setArrayIndex(i);
                    text->replace(settings.value(input).toString(), settings.value(output).toString());
                }
            }
            settings.endArray();
        }

        //reversing the text
        if (revtext) {
            QString reversed;
            int size = text->size() - 1;
            for (int i = 0, j = size; i <= size; i++, j--) reversed[i] = (*text)[j];
            (*text) = reversed;
        }
        if (useAES) {
            std::vector<unsigned char> encryptedData;
            Aes256::encrypt(aesKey, toByteArray(*text), encryptedData);
            (*text) = base64Encode(encryptedData);
        }
    }
}

void MainWindow::on_input_textChanged() {
    //sets text to input value
    QString text = ui->input->toPlainText();
    ////experimental size limit
    ////if (text.size() > 50000){
    ////text.resize(50000);
    ////ui->input->setPlainText(text);}
    //filtered
    filter(&text);
    ui->output->setPlainText(text);
    //copies to clipboard if wanted
    if (autocopy) {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(text);
    }
}

void MainWindow::on_actionEdit_Filter_triggered() {
    //checks if window is already open
    if (!alreadyopen) {
        //window is open
        alreadyopen = true;
        //checks for existance
        if (!QFile::exists(layoutfile)) {
            //error message
            QMessageBox m;
            m.warning(this, "Warning", "The used filter does not exist.");
            on_actionOpen_Filter_triggered();
        }
        //creates edit window and activates alreadyopen
        KeyEdit z(this);
        //sets window flags to be the same as the mainwindow
        z.setWindowFlags(Qt::Window | flags);
        //moves the window to current window position
        z.move(pos());
        z.show();
        z.exec();
        //refreshes text
        on_input_textChanged();
        //window is closed
        alreadyopen = false;
    } else {
        //error message
        QMessageBox z;
        z.warning(this, "warning", "Window is already open.");
    }
}

void MainWindow::on_actionOpen_Filter_triggered() {
    //opens file browser and stores its return in checkfile
    QString checkfile = QFileDialog::getOpenFileName(
        this, tr("Open Filter"), QDir::homePath() + "/.textfilter/layouts", tr("Filter Files (*.flt *.key)")
    );
    //checks if the directory is valid
    if (checkfile != NULL) {
        //layoutfile updated
        layoutfile = checkfile;
    }
    //updates text
    on_input_textChanged();
}

void MainWindow::on_actionSave_Filter_As_triggered() {
    //opens file browser and stores its return in savefile
    QString savefile = QFileDialog::getSaveFileName(
        this, tr("Save Filter"), QDir::homePath() + "/.textfilter/layouts", tr("Filter Files (*.flt *.key)")
    );
    //checks if the directory is valid
    if (savefile != NULL) {
        //adds .flt extention if not specified
        if (!savefile.endsWith(".key") xor savefile.endsWith(".flt")) savefile = savefile + ".flt";
        //saves a copy of the current filter and uses it
        QFile::copy(layoutfile, savefile);
        layoutfile = savefile;
    }
}

void MainWindow::on_actionNew_Filter_triggered() {
    //opens file browser and stores its return in testfile
    QString testfile = QFileDialog::getSaveFileName(
        this, tr("Save Filter"), QDir::homePath() + "/.textfilter/layouts", tr("Filter Files (*.flt *.key)")
    );
    //checks if the directory is valid
    if (testfile != NULL) {
        //adds .flt extention if not specified
        if (!testfile.endsWith(".key") xor testfile.endsWith(".flt")) testfile = testfile + ".flt";
        //new file created
        QFile f(testfile);
        f.open(QIODevice::WriteOnly);
        f.close();
        //layoutfile updated
        layoutfile = testfile;
    }
    //updates text
    on_input_textChanged();
}

void MainWindow::on_autocopy_toggled(bool checked) {
    //sets autocopy to be equal to its checkbox
    autocopy = checked;
}

void MainWindow::on_revtext_toggled(bool checked) {
    //sets reversetext to be equal to its checkbox
    revtext = checked;
    //updates text
    on_input_textChanged();
}

void MainWindow::on_actionAlways_on_top_triggered(bool checked) {
    //sets ontop to be equal to its checkbox
    ontop = checked;
    //give message about restart
    QMessageBox z;
    z.information(this, "Note", "Restart the program for changes to take effect.");
}

void MainWindow::on_actionFrameless_triggered(bool checked) {
    //sets frameless to be equal to its checkbox
    frameless = checked;
    //give message about restart
    QMessageBox z;
    z.information(
        this, "Note", "Restart the program for changes to take effect. To close the frameless window use Filter / Quit."
    );
}

void MainWindow::on_checkBox_toggled(bool checked) {
    revfilter = checked;
    on_input_textChanged();
}

void MainWindow::on_actionTransparent_triggered(bool checked) {
    if (checked) setWindowOpacity(transparency);
    else
        setWindowOpacity(1);
}

void MainWindow::on_actionOpen_Key_triggered() {
    //opens file browser and stores its return in checkfile
    QString filename = QFileDialog::getOpenFileName(this, tr("Open AES 256 key"), QDir::homePath());
    //checks if the directory is valid
    if (filename != nullptr && filename != "") {
        //layoutfile updated
        QFile f(filename);
        f.open(QIODevice::ReadOnly);
        QByteArray contents = f.readAll();
        aesKey = toByteArray1000(contents);
        useAES = true;
        ui->checkboxAES->setChecked(useAES);
    }
    aesKeyPath = filename;
    checkAES();
    //updates text
    on_input_textChanged();
}

void MainWindow::on_actionText_Key_triggered() {
    bool ok;
    QString text = QInputDialog::getText(
        this, tr("QInputDialog::getText()"), tr("Enter AES Key:"), QLineEdit::EchoMode::PasswordEchoOnEdit, "", &ok
    );
    if (ok) {
        aesKey = toByteArray(text);
        useAES = true;
        ui->checkboxAES->setChecked(useAES);
        aesKeyPath = "";
        checkAES();
        on_input_textChanged();
    }
}

void MainWindow::on_checkboxAES_toggled(bool checked) {
    useAES = checked;
    checkAES();
    on_input_textChanged();
}

void MainWindow::checkAES() {
    if (useAES && aesKey.size() == 0) {
        useAES = false;
        ui->checkboxAES->setChecked(useAES);
        QMessageBox().information(this, "Note", "AES 256 key is not set!");
    }
}

void MainWindow::on_actionForget_Keys_and_Paths_triggered() {
    useAES = false;
    ui->checkboxAES->setChecked(useAES);
    aesKey = {};
    aesKeyPath = "";
    saveSettings();
}

void MainWindow::on_actionRemember_Key_Values_toggled(bool checked) {
    allowRememberKey = checked;
    if (!allowRememberKey) {
        allowRememberKey = false;
        saveSettings();
    }
}

void MainWindow::on_checkEnableFilter_stateChanged(int checked) {
    enableFilters = checked;
    on_input_textChanged();
}
