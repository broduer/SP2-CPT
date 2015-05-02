// Copyright David Zhu. All rights reserved.

#include <cassert>

#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "security.h"
#include "mission.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->m_otherCellSpinBoxes[ Covert::TrainingLevel_Recruit ] = this->ui->recruitSpinBox;
    this->m_otherCellSpinBoxes[ Covert::TrainingLevel_Regular ] = this->ui->regularSpinBox;
    this->m_otherCellSpinBoxes[ Covert::TrainingLevel_Veteran ] = this->ui->veteranSpinBox;
    this->m_otherCellSpinBoxes[ Covert::TrainingLevel_Elite ] = this->ui->eliteSpinBox;

    this->ui->cellTrainingLevelComboBox->insertItem( Covert::TrainingLevel_Recruit, "Recruit" );
    this->ui->cellTrainingLevelComboBox->insertItem( Covert::TrainingLevel_Regular, "Regular" );
    this->ui->cellTrainingLevelComboBox->insertItem( Covert::TrainingLevel_Veteran, "Veteran" );
    this->ui->cellTrainingLevelComboBox->insertItem( Covert::TrainingLevel_Elite, "Elite" );
    this->ui->cellTrainingLevelComboBox->setCurrentIndex( Covert::TrainingLevel_Elite );

    this->ui->missionTypeComboBox->insertItem( Mission::MissionType_Espionage, "Espionage" );
    this->ui->missionTypeComboBox->insertItem( Mission::MissionType_Sabotage, "Sabotage" );
    this->ui->missionTypeComboBox->insertItem( Mission::MissionType_Assassination, "Assassination" );
    this->ui->missionTypeComboBox->insertItem( Mission::MissionType_Terrorism, "Terrorism" );
    this->ui->missionTypeComboBox->insertItem( Mission::MissionType_Coup, "Coup" );

    this->ui->missionComplexityComboBox->insertItem( Mission::MissionComplexity_Low, "Low" );
    this->ui->missionComplexityComboBox->insertItem( Mission::MissionComplexity_Medium, "Medium" );
    this->ui->missionComplexityComboBox->insertItem( Mission::MissionComplexity_High, "High" );

    this->connect( this->ui->sp2hdmRadioButton, SIGNAL( clicked() ), this, SLOT( toggleTargetCellGroupBox() ) );
    this->connect( this->ui->sp2RadioButton, SIGNAL( clicked() ), this, SLOT( toggleTargetCellGroupBox() ) );

    this->connect( this->ui->targetingSelfCheckBox, SIGNAL( clicked() ), this, SLOT( toggleMissionDetailsGroupBox() ) );

    this->connect( this->ui->missionTypeComboBox, SIGNAL( currentIndexChanged( int ) ), this, SLOT( missionTypeComboBoxIndexChanged( int ) ) );

    this->connect( this->ui->calculateCellsButton, SIGNAL( clicked() ), this, SLOT( calculateCellsButtonClicked() ) );
    this->connect( this->ui->calculateSuccessButton, SIGNAL( clicked() ), this, SLOT( calculateSuccessButtonClicked() ) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleTargetCellGroupBox( void ) const
{
    this->ui->otherCellsInTargetGroupBox->setEnabled( this->ui->sp2hdmRadioButton->isChecked() );
}

void MainWindow::toggleMissionDetailsGroupBox( void ) const
{
    this->ui->missionDetailsGroupBox->setEnabled( !this->ui->targetingSelfCheckBox->isChecked() );
    if ( this->ui->missionDetailsGroupBox->isEnabled() )
    {
        this->missionTypeComboBoxIndexChanged( this->ui->missionTypeComboBox->currentIndex() );
    }
    else if ( this->ui->missionComplexityComboBox->currentIndex() == Mission::MissionType_Coup )
    {
        // Can't coup oneself, so change it to something else (it's irrelevant when targeting oneself anyway)
        this->ui->missionTypeComboBox->setCurrentIndex( Mission::MissionType_Terrorism );
    }
}

void MainWindow::missionTypeComboBoxIndexChanged( int index ) const
{
    switch ( index )
    {
    case Mission::MissionType_Espionage:
    case Mission::MissionType_Sabotage:
        this->ui->specificSectorCheckBox->setEnabled( true );
        this->ui->missionComplexityComboBox->setEnabled( true );
        this->ui->targetStabilitySpinBox->setEnabled( false );
        break;

    case Mission::MissionType_Coup:
        this->ui->specificSectorCheckBox->setEnabled( false );
        this->ui->missionComplexityComboBox->setEnabled( false );
        this->ui->missionComplexityComboBox->setCurrentIndex( Mission::MissionComplexity_High );
        this->ui->targetStabilitySpinBox->setEnabled( true );
        break;

    default:
        this->ui->specificSectorCheckBox->setEnabled( false );
        this->ui->missionComplexityComboBox->setEnabled( true );
        this->ui->targetStabilitySpinBox->setEnabled( false );
        break;
    }
}

void MainWindow::calculateCellsButtonClicked( void ) const
{
    auto population = this->ui->populationSpinBox->value();
    auto desiredSecurityLevel = this->ui->securityLevelSpinBox->value() / 100.0;

    if ( this->ui->sp2hdmRadioButton->isChecked() )
    {
        assert( !this->ui->sp2RadioButton->isChecked() && "Both SP2-HDM and SP2 1.5.1 checkboxes are checked!" );
    }

    auto eliteCellsNeeded = Security::calculateNumberOfCellsForDesiredSecurity( desiredSecurityLevel, population, this->ui->sp2hdmRadioButton->isChecked() );

    this->ui->eliteCellsNeededLabel->setText( QString::number( eliteCellsNeeded, 'f', 0 ) );
}

void MainWindow::calculateSuccessButtonClicked( void ) const
{
    bool targetingSelf = this->ui->targetingSelfCheckBox->isChecked();

    auto trainingLevel = static_cast< Covert::CellTrainingLevel >( this->ui->cellTrainingLevelComboBox->currentIndex() );
    std::map< Covert::CellTrainingLevel, int > otherCells;
    if ( this->ui->otherCellsInTargetGroupBox->isEnabled() )
    {
        assert( ( this->ui->sp2hdmRadioButton->isChecked() && !this->ui->sp2RadioButton->isChecked() ) && "SP2-HDM box isn't checked, or SP2 1.5.1 checkbox is checked, but group box for other cells is enabled!" );

        for ( const auto& spinBoxPair : this->m_otherCellSpinBoxes )
        {
            otherCells[ spinBoxPair.first ] = spinBoxPair.second->value();
        }
    }

    auto missionType = static_cast< Mission::MissionType >( this->ui->missionTypeComboBox->currentIndex() );

    auto missionComplexity = static_cast< Mission::MissionComplexity >( this->ui->missionComplexityComboBox->currentIndex() );

    bool specificSector = this->ui->specificSectorCheckBox->isEnabled() ? this->ui->specificSectorCheckBox->isChecked() : false;

    auto targetStability = this->ui->targetStabilitySpinBox->value() / 100.0;

    bool framing = this->ui->framingCheckBox->isChecked();

    auto probabilities = Mission::findMissionProbabilities( targetingSelf, trainingLevel, otherCells, missionType, missionComplexity, specificSector, targetStability, framing );

    this->ui->successRateRangeLabel->setText( QString::number( probabilities.m_minimumSuccessRate*100, 'f', 0 ) + "% to " + QString::number( probabilities.m_maximumSuccessRate*100, 'f', 0 )  + '%' );
    this->ui->perpetratorDiscoveredLabel->setText( QString::number( probabilities.m_maximumFoundOutProbability*100, 'f', 0 ) + '%' );
}
