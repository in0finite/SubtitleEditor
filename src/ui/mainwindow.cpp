#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
//#include "SubtitleAppQt.h"
#include "QMessageBox"
#include "SRT.h"
#include "MicroDVD.h"
#include "MPlayer.h"
#include <iostream>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //m_app = new SubtitleAppQt(this);
    this->AddPlugin(new SRT(this));
    this->AddPlugin(new MicroDVD(this));
    this->AddPlugin(new MPlayer(this));

    // populate combo box based on supported formats
    for( auto io : this->getIoHandlers() ) {
        auto str = io->GetFormatDescription();
        QString qstr(str.c_str());
        ui->comboBox->addItem(qstr);

        std::cout << "Added io handler to combobox with desc: " << str << endl ;
    }

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setColumnWidth(2, 300);

}

MainWindow::~MainWindow()
{
    delete ui;

    //delete m_app ;
}

Subtitle MainWindow::getSubtitleAt(int index) const
{
    Subtitle sub;
    sub.appearanceTime.parse( ui->tableWidget->item(index, 0)->text().toStdString() );
    sub.removalTime.parse( ui->tableWidget->item(index, 1)->text().toStdString() );
    sub.text = ui->tableWidget->item(index, 2)->text().toStdString() ;

    return sub;
}

void MainWindow::setSubtitleAt(int index, const Subtitle &sub)
{
    ui->tableWidget->setItem(index, 0, new QTableWidgetItem(sub.appearanceTime.formatted().c_str()));
    ui->tableWidget->setItem(index, 1, new QTableWidgetItem(sub.removalTime.formatted().c_str()));
    ui->tableWidget->setItem(index, 2, new QTableWidgetItem(sub.text.c_str()));

    ui->tableWidget->resizeRowToContents(index);

    /*
    QTextEdit *edit = new QTextEdit();
    edit->setText(ui->tableWidget->item(index, 2)->text());
    ui->tableWidget->setCellWidget(index, 2, edit);
    */

}

int MainWindow::getSubtitlesCount() const
{
    return ui->tableWidget->rowCount();
}


void MainWindow::Load()
{

    try {

        if( ui->comboBox->currentIndex() < 0 ) {
            throw runtime_error("Format is not selected");
        }

        // open file dialog
        auto fileName = QFileDialog::getOpenFileName(this,
                 tr("Select subtitle file"), "", tr("All files (*.*)"));

        //auto selectedFormat = ui->comboBox->itemText(ui->comboBox->currentIndex());
        auto ioHandler = this->getIoHandlers()[ui->comboBox->currentIndex()] ;

        auto subtitles = this->LoadSubtitles(fileName.toStdString(), ioHandler);

        // update gui
        this->UpdateTable(subtitles);

    } catch(exception& ex) {
        QMessageBox::information(this, "Error", ex.what());
    }

}

void MainWindow::Save()
{

    try {

        if( ui->comboBox->currentIndex() < 0 ) {
            throw runtime_error("Format is not selected");
        }

        // open save file dialog
        auto fileName = QFileDialog::getSaveFileName(this,
                 tr("Select subtitle file"), "", tr("All files (*.*)"));

        //auto selectedFormat = ui->comboBox->itemText(ui->comboBox->currentIndex());
        auto ioHandler = this->getIoHandlers()[ui->comboBox->currentIndex()] ;

        this->SaveSubtitles(fileName.toStdString(), ioHandler, this->GetAllSubtitles());

    } catch(exception& ex) {
        QMessageBox::information(this, "Error", ex.what());
    }

}

vector<int> MainWindow::GetSelectedRows()
{

    vector<int> selectedRows ;

    auto ranges = ui->tableWidget->selectedRanges();
    //std::sort();
    for( int r=ranges.size() - 1; r >= 0 ; r-- ) {
        auto range = ranges[r];

        if(range.rowCount() < 0)
            continue;

        for(int i=range.bottomRow(); i >= range.topRow(); i--) {
            selectedRows.push_back(i);
        }
    }

    return selectedRows;
}

vector<vector<string>> MainWindow::GetTableEntries()
{
    vector<vector<string>> table;

    for(int i=0; i < ui->tableWidget->rowCount(); i++) {
        vector<string> entries;
        entries.push_back( ui->tableWidget->item(i, 0)->text().toStdString() );
        entries.push_back( ui->tableWidget->item(i, 1)->text().toStdString() );
        entries.push_back( ui->tableWidget->item(i, 2)->text().toStdString() );
        table.push_back(entries);
    }

    return table;
}

void MainWindow::CheckValidity( bool checkSortOrder )
{

    // check if timestamps are valid
    for( int i=0; i < this->getSubtitlesCount(); i++ ) {

        Subtitle sub;
        try {
            sub = this->getSubtitleAt(i);
        } catch(exception& ex) {
            throw runtime_error("Invalid subtitle at row " + to_string(i+1) + ": " + ex.what());
        }

        /*
        if(!TimeStamp::isValid(entries[0])) {
            throw runtime_error("Invalid timestamp at row " + to_string(index+1) + ": " + entries[0] );
        }

        if(!TimeStamp::isValid(entries[1])) {
            throw runtime_error("Invalid timestamp at row " + to_string(index+1) + ": " + entries[1] );
        }
        */

        if(sub.removalTime.miliseconds < sub.appearanceTime.miliseconds) {
            throw runtime_error("Invalid subtitle at row "  + to_string(i+1) );
        }

    }

    // check if any of the timestamps overlap

    // retrieve all subtitles at once, so we don't try to convert string to timestamp every time
    auto subtitles = this->GetAllSubtitles();

    for(int i=0; i < this->getSubtitlesCount(); i++ ) {
        for(int j=i+1; j < this->getSubtitlesCount(); j++ ) {
            auto sub1 = subtitles[i];
            auto sub2 = subtitles[j];
            int a1 = sub1.appearanceTime.miliseconds ;
            int b1 = sub2.appearanceTime.miliseconds ;
            int a2 = sub1.removalTime.miliseconds ;
            int b2 = sub2.removalTime.miliseconds ;

            if( (a1 > b1 && a1 < b2) || (a2 > b1 && a2 < b2 ) || (a1 <= b1 && a2 >= b2) ) {
                // they overlap
                throw runtime_error("Subtitle timestamps at " + to_string(i+1) + " and " + to_string(j+1) + " overlap");
            }

        }
    }

    if(checkSortOrder) {
        // check if they are sorted
        for(int i=1; i < this->getSubtitlesCount(); i++) {
            if(subtitles[i].appearanceTime.miliseconds < subtitles[i-1].appearanceTime.miliseconds) {
                throw runtime_error("Subtitles are not sorted - at " + to_string(i+1) );
            }
        }
    }


    // everything is ok

}

void MainWindow::UpdateTable(const vector<Subtitle> &subtitles)
{

    ui->tableWidget->setRowCount(subtitles.size());

    for(int i=0; i < subtitles.size(); i++) {
        this->setSubtitleAt(i, subtitles[i]);
    }

}

void MainWindow::on_pushButton_clicked()
{
    // load

    this->Load();

}

void MainWindow::on_pushButton_2_clicked()
{
    // save

    this->Save();

}

void MainWindow::on_actionLoad_plugins_triggered()
{
    try {
        auto dir = QFileDialog::getExistingDirectory(this,
                 tr("Select plugins directory"));

        int count = this->LoadPlugins(dir.toStdWString());

        string str = "Loaded " + to_string(count) + " plugins" ;
        QMessageBox::information(this, "", str.c_str());

    } catch(exception& ex) {
        QMessageBox::information(this, "Error", ex.what());
    }

}

void MainWindow::on_actionShow_stats_triggered()
{

    string str;

    str += "Number of subtitles " + to_string( this->getSubtitlesCount() ) + "\n";

    int timeLength = 0;
    for( int i=0; i < this->getSubtitlesCount(); i++ ) {
        try {
            auto sub = this->getSubtitleAt(i);
            timeLength += sub.removalTime.miliseconds - sub.appearanceTime.miliseconds ;
        } catch(...) { }
    }
    TimeStamp t;
    t.miliseconds = timeLength;

    str += "Length of subtitles " + t.formatted() + "\n";

    //str += "Size in bytes " + to_string( this->GetSizeInBytes() ) + "\n";

    QMessageBox::information(this, "Stats", str.c_str() );

}

void MainWindow::on_tableWidget_cellChanged(int row, int column)
{

    if(m_isPopulatingTable) {
        return ;
    }

    //auto itemText = this->ui->tableWidget->item(row, column)->text();

    // check if new value is valid
    // if not, display message box


    //QMessageBox::information(this, "Cell changed", itemText );

}

void MainWindow::on_actionLoad_triggered()
{
    this->Load();
}

void MainWindow::on_actionAdd_triggered()
{
    // insert new subtitle

    // insert it at the end

    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
    // set subtitle number
//    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(std::to_string(ui->tableWidget->rowCount()).c_str()) );

    Subtitle sub;
    this->setSubtitleAt(this->getSubtitlesCount() - 1, sub);

}

void MainWindow::on_actionRemove_triggered()
{
    // remove selected subtitles

    int count = 0;

    for( int index : this->GetSelectedRows() ) {
        ui->tableWidget->removeRow(index);
        count ++;
    }

    QMessageBox::information(this, "", ("Removed " + to_string(count) + " subtitles").c_str() );
}

void MainWindow::on_actionRemove_all_triggered()
{

    ui->tableWidget->setRowCount(0);

}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit(0);
}

void MainWindow::on_actionSave_triggered()
{
    this->Save();
}

void MainWindow::on_actionCheck_for_validity_triggered()
{

    try {

        this->CheckValidity();

        QMessageBox::information(this, "", "Subtitles are ok." );

    } catch( exception& ex ) {
        QMessageBox::information(this, "Subtitles are not valid", ex.what() );
    }

}

void MainWindow::on_actionShift_in_time_triggered()
{

    try {

        //this->CheckValidityAndThrow();

        auto selectedRows = this->GetSelectedRows();
        if(0 == selectedRows.size()) {
            throw runtime_error("No subtitles are selected");
        }

        //bool ok = false;
        int time = this->askUserForInt("Enter time in miliseconds");    //QInputDialog::getInt(this, "Enter time in miliseconds", "", 0, 1, 5000000, 1, & ok);

        //if(!ok) {
        //    throw runtime_error("Incorrect time");
        //}

        for(int index : selectedRows) {

            bool isValid = true;
            Subtitle sub;
            try {
                sub = this->getSubtitleAt(index);
            } catch(...) {
                isValid = false ;
            }

            if(!isValid) continue;

            /*
            auto startTime = ui->tableWidget->item(index, 0)->text();
            auto endTime = ui->tableWidget->item(index, 1)->text();

            TimeStamp t;

            if( t.parse(startTime.toStdString()) ) {
                t.miliseconds += time ;

                ui->tableWidget->setItem( index, 0, new QTableWidgetItem( t.formatted().c_str() ) );
            }

            if( t.parse(endTime.toStdString()) ) {
                t.miliseconds += time ;

                ui->tableWidget->setItem( index, 1, new QTableWidgetItem( t.formatted().c_str() ) );
            }
            */

            sub.appearanceTime.miliseconds += time ;
            sub.removalTime.miliseconds += time ;

            this->setSubtitleAt(index, sub);
        }

    } catch( exception& ex ) {
        QMessageBox::information(this, "Error", ex.what());
    }

}

void MainWindow::on_actionMerge_triggered()
{

    try {

        this->CheckValidity();

        bool ok = false;
        int time = QInputDialog::getInt(this, "Enter time in miliseconds", "", 0, 1, 5000000, 1, & ok);

        if(!ok) {
            throw runtime_error("Incorrect time");
        }

        auto subtitles = this->GetAllSubtitles();

        for(int i=0; i < subtitles.size() - 1; i++) {
            auto sub1 = subtitles[i];
            auto sub2 = subtitles[i + 1];

            bool closeEnough = sub2.appearanceTime.miliseconds - sub1.removalTime.miliseconds <= time ;
            if(closeEnough) {
                // merge
                sub1.text += " " + sub2.text ;
                sub1.removalTime = sub2.removalTime ;

                subtitles[i] = sub1 ;

                // remove the second subtitle
                subtitles.erase(subtitles.begin() + i + 1, subtitles.begin() + i + 2);

            }

        }

        this->UpdateTable(subtitles);

        QMessageBox::information(this, "", "Subtitles are merged");

    } catch( exception& ex ) {
        QMessageBox::information(this, "Error", ex.what());
    }

}

void MainWindow::on_actionSort_triggered()
{

    try {

        this->CheckValidity(false);

        auto subtitles = this->GetAllSubtitles();
        for(int i=0; i < subtitles.size(); i++) {
            for(int j=i+1; j < subtitles.size(); j++) {
                if(subtitles[j].appearanceTime.miliseconds < subtitles[i].appearanceTime.miliseconds) {
                    auto t = subtitles[i];
                    subtitles[i] = subtitles[j];
                    subtitles[j] = t ;
                }
            }
        }

        this->UpdateTable(subtitles);

        QMessageBox::information(this, "", "Subtitles are sorted");

    } catch( exception& ex ) {
        QMessageBox::information(this, "Error", ex.what());
    }

}

void MainWindow::on_actionSplit_triggered()
{

    try {

        this->CheckValidity();

        int maxChars = this->askUserForInt("Enter maximum number of characters:");
        //int delta = this->askUserForInt("Enter time in miliseconds between new subtitles:");
        if(maxChars < 1) {
            throw runtime_error("Invalid input");
        }

        auto subtitles = this->GetAllSubtitles();

        for(int i=0; i < subtitles.size(); i++) {

            if(subtitles[i].text.length() > maxChars) {
                int count = subtitles[i].text.length() / maxChars ; // num subtitles to add

                int subLength = (subtitles[i].removalTime.miliseconds - subtitles[i].appearanceTime.miliseconds) / (count + 1);
                //int delta = subLength / 10 ;
                int textLength = maxChars ; // subtitles[i].text.length() / (count + 1) ;

                if(subLength < 1 || textLength < 1) {
                    continue ;
                }

                auto text = subtitles[i].text ;

                // modify current subtitle
                subtitles[i].removalTime.miliseconds = subtitles[i].appearanceTime.miliseconds + subLength ;
                subtitles[i].text = text.substr(0, textLength );

                // add new subtitles
                int startTime = subtitles[i].removalTime.miliseconds ;
                for(int k=0; k < count; k++) {
                    Subtitle sub;
                    sub.appearanceTime.miliseconds = startTime + k * subLength;
                    sub.removalTime.miliseconds = sub.appearanceTime.miliseconds + subLength ;

                    sub.text = text.substr( (k+1) * textLength, min(textLength, (int) ( text.length() - (k+1) * textLength ) ) );

                    subtitles.insert(subtitles.begin() + i + k + 1, sub);
                }

                i += count ;
            }

        }

        this->UpdateTable(subtitles);

        QMessageBox::information(this, "", "Subtitles are splitted");

    } catch( exception& ex ) {
        QMessageBox::information(this, "Error", ex.what());
    }

}

void MainWindow::on_actionStretch_triggered()
{

    try {

        auto selectedRows = this->GetSelectedRows();
        if(selectedRows.size() < 1) {
            throw runtime_error("No sybtitles are selected");
        }


        int lastTime = this->getSubtitleAt(selectedRows[0]).removalTime.miliseconds;
        for(int i=0; i < selectedRows.size(); i++) {
            auto sub = this->getSubtitleAt(selectedRows[i]);
       //     sub.appearanceTime.miliseconds +=

            lastTime = sub.removalTime.miliseconds ;
        }

        this->UpdateTable(subtitles);

        QMessageBox::information(this, "", "Subtitles are splitted");

    } catch( exception& ex ) {
        QMessageBox::information(this, "Error", ex.what());
    }

}
