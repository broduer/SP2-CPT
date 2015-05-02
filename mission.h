// Copyright David Zhu. All rights reserved.

#ifndef MISSION_H
#define MISSION_H

#include <map>

#include "covert.h"

class Mission
{
public:
    enum MissionType
    {
        MissionType_Espionage = 0,
        MissionType_Sabotage,
        MissionType_Assassination,
        MissionType_Terrorism,
        MissionType_Coup,
    };

    enum MissionComplexity
    {
        MissionComplexity_Low = 0,
        MissionComplexity_Medium,
        MissionComplexity_High,
    };

    struct Probabilities
    {
        float m_minimumSuccessRate;
        float m_maximumSuccessRate;

        float m_maximumFoundOutProbability;
    };

    static Probabilities findMissionProbabilities( bool targetingSelf, Covert::CellTrainingLevel level, std::map< Covert::CellTrainingLevel, int >& otherCells, MissionType type, MissionComplexity complexity, bool specificSector, double targetStability, bool framing );

private:
    static const float MaximumSuccessRate;

    static const float AgainstSelfBaseSuccessRate;

    static const std::map< MissionComplexity, float > MissionComplexitySuccessRates;

    static const float ExtraCellSuccessRateBonus;

    struct MissionSuccessStats
    {
        float m_typeModifier;
        float m_chanceOfKnowingModifier;
    };

    static const std::map< MissionType, MissionSuccessStats > MissionStats;

    static const float SpecificCategoryModifier;

    static const float FramingModifier;
};

#endif // MISSION_H
