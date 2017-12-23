#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QInputDialog"
#include "SubtitleApp.h"
//class SubtitleApp;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public SubtitleApp
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    virtual	int     askUserForInt( string description ) override {
        return QInputDialog::getInt(this, description.c_str(), "");
    }

    virtual	float   askUserForFloat( string description ) override {
        return (float) QInputDialog::getDouble(this, description.c_str(), "");
    }

    virtual	string  askUserForString( string description ) override {
        return QInputDialog::getText(this, description.c_str(), "").toStdString();
    }

    virtual Subtitle    getSubtitleAt(int index) const override ;
    virtual void    setSubtitleAt(int index, const Subtitle& sub) override ;
    virtual int    getSubtitlesCount() const override ;

private:
    Ui::MainWindow *ui;

    //SubtitleApp* m_app;

    bool m_isPopulatingTable = false ;

    void Load();
    void Save();

    vector<int> GetSelectedRows();

    vector<vector<string>>    GetTableEntries();

    void    CheckValidity( bool checkSortOrder = true );

    void    UpdateTable( const SubtitleContainer& subtitles );

public:



private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_actionLoad_plugins_triggered();
    void on_actionShow_stats_triggered();
    void on_tableWidget_cellChanged(int row, int column);
    void on_actionLoad_triggered();
    void on_actionAdd_triggered();
    void on_actionRemove_triggered();
    void on_actionRemove_all_triggered();
    void on_actionExit_triggered();
    void on_actionSave_triggered();
    void on_actionCheck_for_validity_triggered();
    void on_actionShift_in_time_triggered();
    void on_actionMerge_triggered();
    void on_actionSort_triggered();
    void on_actionSplit_triggered();
    void on_actionStretch_triggered();
};

#endif // MAINWINDOW_H
