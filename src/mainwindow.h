#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QDir>
#include <QMainWindow>
#include "keyedit.h"


namespace Ui {

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void loadSettings();

    void saveSettings();

private slots:

    void filter (QString *);

    void on_input_textChanged();

    void on_autocopy_toggled(bool);

    void on_actionEdit_Filter_triggered();

    void on_actionOpen_Filter_triggered();

    void on_actionSave_Filter_As_triggered();

    void on_actionNew_Filter_triggered();

    void on_revtext_toggled(bool);

    void on_actionAlways_on_top_triggered(bool);

    void on_actionFrameless_triggered(bool);

    void on_checkBox_toggled(bool);

    void on_actionTransparent_triggered(bool checked);

private:

    Ui::MainWindow *ui;

    QString settingsfile;

    bool ontop;

    bool frameless;

    Qt::WindowFlags flags;

    bool autocopy;

    bool revtext;

    bool revfilter;

    bool alreadyopen;

    QDir layoutsDir;

    double transparency;

    bool integrated;

};

#endif // MAINWINDOW_H
