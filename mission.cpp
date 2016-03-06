// Copyright David Zhu. All rights reserved.

#include <cmath>
#include <cassert>

#include "mission.h"

const float Mission::MaximumSuccessRate = 0.8f;

const float Mission::AgainstSelfBaseSuccessRate = 0.4f;

const std::map< Mission::MissionComplexity, float > Mission::MissionComplexitySuccessRates =
{
    { MissionComplexity_Low, 2.f },
    { MissionComplexity_Medium, 1.f },
    { MissionComplexity_High, 0.5f },
};

const std::map< Mission::MissionType, Mission::MissionSuccessStats > Mission::MissionStats =
{
    { MissionType_Espionage, { 1.f, 0.5f } },
    { MissionType_Sabotage, { 1.f, 1.f } },
    { MissionType_Assassination, { 1.f, 0.75f } },
    { MissionType_Terrorism, { 1.25f, 1.25f } },
    { MissionType_Coup, { 0.5f, 1.5f } },
};

const float Mission::SpecificCategoryModifier = 0.75f;

const float Mission::FramingModifier = 0.25f;

auto Mission::findMissionProbabilities( bool targetingSelf,
                                        Covert::CellTrainingLevel level,
                                        std::map< Covert::CellTrainingLevel, int >& otherCells,
                                        MissionType type,
                                        MissionComplexity complexity,
                                        bool specificSector,
                                        double targetStability,
                                        bool framing,
                                        bool sp2hdm ) -> Probabilities
{
    auto probabilities = Probabilities();

    if ( !targetingSelf )
    {
        const float missionComplexitySuccessRate = Mission::MissionComplexitySuccessRates.at( complexity );
        const float cellLevelSuccessRate = Covert::CellStrength.at( level );

        float cellStrength = cellLevelSuccessRate;

        // Factor in other cells in the target country.
        assert( ( sp2hdm || otherCells.empty() ) && "Other cells are factoring in for non-SP2-HDM" );
        float totalTrainingOfOtherCells = 0.f;
        for ( const auto& cellPair : otherCells )
        {
            totalTrainingOfOtherCells += Covert::CellStrength.at( cellPair.first ) * cellPair.second;
        }

        cellStrength += ( level < Covert::CellTrainingLevel::TrainingLevel_Elite ) ?
                        ( 0.5f - ( 0.5f / sqrt( totalTrainingOfOtherCells + 1.f ) ) ) :
                        log( ( totalTrainingOfOtherCells / 5.f ) + 1.f);
        assert( ( cellStrength >= cellLevelSuccessRate ) && "Adding extra cells shouldn't reduce the success rate" );

        auto successRate = missionComplexitySuccessRate * cellStrength *
                           Mission::MissionStats.at( type ).m_typeModifier;

        // Target stability only affects coup success rate.
        if ( type == MissionType_Coup )
        {
            assert( ( complexity == Mission::MissionComplexity_High ) && "Mission type is coup, but complexity is not High" );

            successRate *= 1.f - targetStability;
        }

        if ( specificSector )
        {
            assert( ( ( type == MissionType_Espionage ) || ( type == MissionType_Sabotage ) ) && "Category is only supposed to be set for espionage or sabotage" );
            successRate *= Mission::SpecificCategoryModifier;
        }

        // Framing someone reduces the success rate.
        successRate *= framing ? Mission::FramingModifier : 1.f;

        // 95% security
        probabilities.m_minimumSuccessRate = std::min( successRate * ( 1.f - Covert::MaximumSecurityLevel ), Mission::MaximumSuccessRate );

        // 0% security
        probabilities.m_maximumSuccessRate = std::min( successRate, Mission::MaximumSuccessRate );

        // Chance of assassinating an enemy cell
        if ( sp2hdm && ( type == MissionType_Assassination ) )
        {
            const float assassinationComplexityModifier =
                    1.f / missionComplexitySuccessRate *
                    ( ( complexity == MissionComplexity_Low ) ? 0.5f : 1.f );

            probabilities.m_chanceOfAsssassinatingEnemyCell = assassinationComplexityModifier * cellStrength / 10.f;
        }
    }
    else
    {
        assert( ( type != MissionType_Coup ) && "Can't stage a coup against oneself" );

        // Success rate, complexity, cell level, mission type, security, stability, and category are not relevant for self-targetted missions.
        probabilities.m_minimumSuccessRate = probabilities.m_maximumSuccessRate = framing ? Mission::AgainstSelfBaseSuccessRate : Mission::MaximumSuccessRate;
    }

    assert( ( probabilities.m_minimumSuccessRate >= 0 ) && "Minimum success rate is less than 0" );
    assert( ( probabilities.m_maximumSuccessRate >= 0 ) && "Maximum success rate is less than 0" );
    assert( ( probabilities.m_minimumSuccessRate <= probabilities.m_maximumSuccessRate ) && "Minimum success rate is greater than maximum success rate" );
    assert( ( probabilities.m_maximumSuccessRate <= Mission::MaximumSuccessRate ) && "Maximum success rate is greater than game's maximum success rate" );

    // Chance of target knowing for real, even if self-targetted
    // 95% security
    probabilities.m_maximumFoundOutProbability =
            std::max( 0.f,
                      Mission::MissionStats.at( type ).m_chanceOfKnowingModifier * ( ( Covert::MaximumSecurityLevel * 0.75f ) - ( Covert::CellStrength.at( level ) * 0.2f ) ) );
    assert( ( probabilities.m_maximumFoundOutProbability >= 0 ) && "Maximum found-out probability is less than 0" );

    return probabilities;
}
