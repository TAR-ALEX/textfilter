#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "keyedit.h"
#include <QDir>
#include <QMainWindow>
#include <vector>


namespace Ui {

    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

    void loadSettings();
    void saveSettings();

private slots:
    void filter(QString*);
    void on_input_textChanged();
    void on_output_textChanged();
    void updateText();
    void on_autocopy_toggled(bool);
    void on_actionEdit_Filter_triggered();
    void on_actionOpen_Filter_triggered();
    void on_actionSave_Filter_As_triggered();
    void on_actionNew_Filter_triggered();
    void on_revtext_toggled(bool);
    void on_actionAlways_on_top_triggered(bool);
    void on_actionFrameless_triggered(bool);
    void on_actionTransparent_triggered(bool checked);
    void on_actionOpen_Key_triggered();
    void on_actionText_Key_triggered();
    void on_checkboxAES_toggled(bool);
    void on_actionRemember_Key_Values_toggled(bool arg1);
    void on_actionForget_Keys_and_Paths_triggered();
    void on_checkEnableFilter_stateChanged(int arg1);
    void on_actionURL_encoder_triggered();
    void on_actionEnableCompression_toggled(bool arg1);

    virtual bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::MainWindow* ui;
    QString settingsfile;
    bool ontop;
    bool frameless;
    Qt::WindowFlags flags;
    bool autocopy;
    bool revtext;
    bool enableCompression = false;
    bool revfilter = false;
    bool enableFilters = false;
    bool alreadyopen;
    QDir layoutsDir;
    double transparency;
    bool integrated;
    bool useAES = false;
    bool allowRememberKey = false;
    QString aesKeyPath = "";
    std::vector<uint8_t> aesKey;
    QString urlEncoderString = "";
    bool lastTextFocus = 0;
    void checkAES();
};

#endif // MAINWINDOW_H
