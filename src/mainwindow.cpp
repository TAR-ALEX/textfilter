#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QFileDialog>
#include <QFileDialog>

extern QString layoutfile;

void MainWindow::loadSettings()
{   //open settingsfile window group
    QSettings settings(settingsfile, QSettings::NativeFormat);
    settings.beginGroup("window");
    //load values into variables
    autocopy = settings.value("autocopy", false).toBool();
    ontop = settings.value("ontop", false).toBool();
    frameless = settings.value("frameless", false).toBool();
    revtext = settings.value("reversetext", false).toBool();
    revfilter = false;
    layoutfile = settings.value("currentfile", QDir::homePath() + "/.textfilter/layouts/default.flt").toString();
    this->resize(settings.value("size").toSize());
    this->move(settings.value("pos").toPoint());
    settings.endGroup();
}


void MainWindow::saveSettings()
{
    //open settingsfile window group
    QSettings settings(settingsfile, QSettings::NativeFormat);
    settings.beginGroup("window");
    //save values into file
    settings.setValue("autocopy", autocopy);
    settings.setValue("ontop", ontop);
    settings.setValue("frameless", frameless);
    settings.setValue("reversetext", revtext);
    settings.setValue("currentfile", layoutfile);
    settings.setValue("size", this->size());
    settings.setValue("pos", this->pos());
    settings.endGroup();
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settingsfile = QDir::homePath () + "/.textfilter/settings";
    alreadyopen = false;
    loadSettings();
    //display loaded settings.
    ui->autocopy->setChecked(autocopy);
    ui->actionAlways_on_top->setChecked(ontop);
    ui->actionFrameless->setChecked(frameless);
    ui->revtext->setChecked(revtext);
    transparency = 0.9;
    //window flags
    if (ontop)
        flags |= Qt::WindowStaysOnTopHint;
    if (frameless)
        flags |= Qt::FramelessWindowHint;
    setWindowFlags(flags);
    //check if lilter directory exists
    layoutsDir = QDir::homePath() + "/.textfilter/layouts";
    if (!layoutsDir.exists()){
        layoutsDir.mkpath(".");
    }
    //copy default file from opt if it does not exist
    if (!QFile::exists(QDir::homePath() + "/.textfilter/layouts/default.flt")){
        QFile::copy(QApplication::applicationDirPath() + "/layouts/default.key" , QDir::homePath() + "/.textfilter/layouts/default.flt");
    }
    //error message if the layout file does not exist
    if (!QFile::exists(layoutfile))
    {
        QMessageBox z;
        z.warning(this, "Warning", "The previously used filter does not exist.");
        on_actionOpen_Filter_triggered();
    }
}

MainWindow::~MainWindow()
{
    //saves settings before destruction
    saveSettings();
    delete ui;
}


void MainWindow::filter (QString *text)
{
    //open key configuration
    QSettings settings(layoutfile, QSettings::NativeFormat);
    //sets input output values
    QString input = "input";
    QString output = "output";
    if (revfilter)
    {
        input = "output";
        output = "input";
        ////char
        settings.beginReadArray("char");
        {
            //get total amount of replacement cycles
            const int total = settings.value("size").toInt();
            //replacement cycle
            for (int i = 0; i <= total; i++)
            {
                settings.setArrayIndex(i);
                text->replace(settings.value(input).toString(),settings.value(output).toString());
            }
        }
        settings.endArray();
        ////word
        settings.beginReadArray("word");
        {
            //get total amount of replacement cycles
            const int total = settings.value("size").toInt();
            //replacement cycle
            for (int i = 0; i <= total; i++)
            {
                settings.setArrayIndex(i);
                text->replace(settings.value(input).toString(),settings.value(output).toString());
            }
        }
        settings.endArray();
    }else{
        ////word
        settings.beginReadArray("word");
        {
            //get total amount of replacement cycles
            const int total = settings.value("size").toInt();
            //replacement cycle
            for (int i = 0; i <= total; i++)
            {
                settings.setArrayIndex(i);
                text->replace(settings.value(input).toString(),settings.value(output).toString());
            }
        }
        settings.endArray();
        ////char
        settings.beginReadArray("char");
        {
            //get total amount of replacement cycles
            const int total = settings.value("size").toInt();
            //replacement cycle
            for (int i = 0; i <= total; i++)
            {
                settings.setArrayIndex(i);
                text->replace(settings.value(input).toString(),settings.value(output).toString());
            }
        }
        settings.endArray();
    }

    //reversing the text
    if (revtext)
    {
        QString reversed;
        //size is the text size - 1 to remove the \0 terminating character
        int size = text->size() - 1;
        //cycle to store the reversed text
        for (int i=0, j=size; i <= size; i++ , j--)
            reversed[i] = (*text)[j];
        //replace text with reversed text
        (*text) = reversed;
    }
}

void MainWindow::on_input_textChanged()
{
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
    if(autocopy)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
    }
}

void MainWindow::on_actionEdit_Filter_triggered()
{
    //checks if window is already open
    if (!alreadyopen)
    {
        //window is open
        alreadyopen = true;
        //checks for existance
        if (!QFile::exists(layoutfile))
        {
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
    }
    else
    {
        //error message
        QMessageBox z;
        z.warning(this, "warning", "Window is already open.");
    }
}

void MainWindow::on_actionOpen_Filter_triggered()
{
    //opens file browser and stores its return in checkfile
    QString checkfile = QFileDialog::getOpenFileName(this, tr("Open Filter"), QDir::homePath() + "/.textfilter/layouts", tr("Filter Files (*.flt *.key)"));
    //checks if the directory is valid
    if (checkfile != NULL)
    {
       //layoutfile updated
       layoutfile = checkfile;
    }
    //updates text
    on_input_textChanged();
}

void MainWindow::on_actionSave_Filter_As_triggered()
{
    //opens file browser and stores its return in savefile
    QString savefile = QFileDialog::getSaveFileName(this, tr("Save Filter"), QDir::homePath() + "/.textfilter/layouts", tr("Filter Files (*.flt *.key)"));
    //checks if the directory is valid
    if(savefile != NULL)
    {
        //adds .flt extention if not specified
        if (!savefile.endsWith(".key") xor savefile.endsWith(".flt"))
            savefile = savefile + ".flt";
        //saves a copy of the current filter and uses it
        QFile::copy(layoutfile, savefile);
        layoutfile = savefile;
    }
}

void MainWindow::on_actionNew_Filter_triggered()
{
    //opens file browser and stores its return in testfile
    QString testfile = QFileDialog::getSaveFileName(this, tr("Save Filter"), QDir::homePath() + "/.textfilter/layouts", tr("Filter Files (*.flt *.key)"));
    //checks if the directory is valid
    if (testfile != NULL)
    {
        //adds .flt extention if not specified
        if (!testfile.endsWith(".key") xor testfile.endsWith(".flt"))
            testfile = testfile + ".flt";
        //new file created
        QFile f( testfile );
        f.open( QIODevice::WriteOnly );
        f.close();
        //layoutfile updated
        layoutfile = testfile;
    }
    //updates text
    on_input_textChanged();
}

void MainWindow::on_autocopy_toggled(bool checked)
{
    //sets autocopy to be equal to its checkbox
     autocopy = checked;
}

void MainWindow::on_revtext_toggled(bool checked)
{
    //sets reversetext to be equal to its checkbox
    revtext = checked;
    //updates text
    on_input_textChanged();
}

void MainWindow::on_actionAlways_on_top_triggered(bool checked)
{
    //sets ontop to be equal to its checkbox
    ontop=checked;
    //give message about restart
    QMessageBox z;
    z.information(this, "Note", "Restart the program for changes to take effect.");
}

void MainWindow::on_actionFrameless_triggered(bool checked)
{
    //sets frameless to be equal to its checkbox
    frameless=checked;
    //give message about restart
    QMessageBox z;
    z.information(this, "Note", "Restart the program for changes to take effect. To close the frameless window use Filter / Quit.");
}

void MainWindow::on_checkBox_toggled(bool checked)
{
    revfilter = checked;
    on_input_textChanged();
}

void MainWindow::on_actionTransparent_triggered(bool checked)
{
    if(checked) setWindowOpacity(transparency);
    else setWindowOpacity(1);
}
