// Copyright David Zhu. All rights reserved.

#include <cassert>

#include "mission.h"

const float Mission::MaximumSuccessRate = 0.8f;

const float Mission::AgainstSelfBaseSuccessRate = 0.4f;

const float Mission::ExtraCellSuccessRateBonus = 0.4f / 100.f;

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

auto Mission::findMissionProbabilities( bool targetingSelf, Covert::CellTrainingLevel level, std::map< Covert::CellTrainingLevel, int >& otherCells, MissionType type, MissionComplexity complexity, bool specificSector, double targetStability, bool framing ) -> Probabilities
{
    Probabilities probabilities;

    if ( !targetingSelf )
    {
        float baseSuccessRate =
                Mission::MissionComplexitySuccessRates.at( complexity ) *
                Covert::CellStrength.at( level ) *
                Mission::MissionStats.at( type ).m_typeModifier;

        // Current stability only affects coup success rate.
        if ( type == MissionType_Coup )
        {
            assert( ( complexity == Mission::MissionComplexity_High ) && "Mission type is coup, but complexity is not High" );

            baseSuccessRate *= 1.f - targetStability;
        }

        if ( specificSector )
        {
            assert( ( ( type == MissionType_Espionage ) || ( type == MissionType_Sabotage ) ) && "Category is only supposed to be set for espionage or sabotage" );
            baseSuccessRate *= Mission::SpecificCategoryModifier;
        }

        // Framing someone reduces the success rate.
        if ( framing )
        {
            baseSuccessRate *= Mission::FramingModifier;
        }

        // 95% security
        probabilities.m_minimumSuccessRate = std::min( baseSuccessRate * ( 1.f - Covert::MaximumSecurityLevel ), Mission::MaximumSuccessRate );

        // 0% security
        probabilities.m_maximumSuccessRate = std::min( baseSuccessRate, Mission::MaximumSuccessRate );
    }
    else
    {
        assert( ( type != MissionType_Coup ) && "Can't stage a coup against oneself" );

        // For success rate, complexity, cell level, mission type, security, stability, and category are not relevant for self-targetted missions.
        if ( framing )
        {
            probabilities.m_minimumSuccessRate = probabilities.m_maximumSuccessRate = Mission::AgainstSelfBaseSuccessRate;
        }
        else
        {
            probabilities.m_minimumSuccessRate = probabilities.m_maximumSuccessRate = Mission::MaximumSuccessRate;
        }
    }

    assert( ( probabilities.m_minimumSuccessRate >= 0 ) && "Minimum success rate is less than 0" );
    assert( ( probabilities.m_maximumSuccessRate >= 0 ) && "Maximum success rate is less than 0" );
    assert( ( probabilities.m_minimumSuccessRate <= probabilities.m_maximumSuccessRate ) && "Minimum success rate is greater than maximum success rate" );
    assert( ( probabilities.m_maximumSuccessRate <= Mission::MaximumSuccessRate ) && "Maximum success rate is greater than game's maximum success rate" );

    // Cells, applies to both targeting self and targeting others.
    float otherCellsSuccessRateBonus = 0.f;
    for ( auto cellPair : otherCells )
    {
        otherCellsSuccessRateBonus += Covert::CellStrength.at( cellPair.first ) * cellPair.second * Mission::ExtraCellSuccessRateBonus;
    }

    probabilities.m_minimumSuccessRate = std::min( probabilities.m_minimumSuccessRate + otherCellsSuccessRateBonus, Mission::MaximumSuccessRate );
    probabilities.m_maximumSuccessRate = std::min( probabilities.m_maximumSuccessRate + otherCellsSuccessRateBonus, Mission::MaximumSuccessRate );

    // Chance of target knowing for real, even if self-targetted
    // 95% security
    probabilities.m_maximumFoundOutProbability =
            std::max( 0.f,
                      Mission::MissionStats.at( type ).m_chanceOfKnowingModifier * ( ( Covert::MaximumSecurityLevel * 0.75f ) - ( Covert::CellStrength.at( level ) * 0.2f ) ) );
    assert( ( probabilities.m_maximumFoundOutProbability >= 0 ) && "Maximum found-out probability is less than 0" );

    return probabilities;
}
