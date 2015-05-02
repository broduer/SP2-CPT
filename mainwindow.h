// Copyright David Zhu. All rights reserved.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <memory>

#include <QMainWindow>

#include "covert.h"

class QSpinBox;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void toggleTargetCellGroupBox( void ) const;

    void toggleMissionDetailsGroupBox( void ) const;

    void missionTypeComboBoxIndexChanged( int index ) const;

    void calculateCellsButtonClicked( void ) const;
    void calculateSuccessButtonClicked( void ) const;

private:
    Ui::MainWindow *ui;

    std::map< Covert::CellTrainingLevel, QSpinBox* > m_otherCellSpinBoxes;
};

#endif // MAINWINDOW_H
