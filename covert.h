// Copyright David Zhu. All rights reserved.

#ifndef COVERT_H
#define COVERT_H

#include <map>

class Covert
{
public:
    enum CellTrainingLevel
    {
        TrainingLevel_Recruit = 0,
        TrainingLevel_Regular,
        TrainingLevel_Veteran,
        TrainingLevel_Elite,
    };

    static const std::map< CellTrainingLevel, float > CellStrength;

    static const float MaximumSecurityLevel;
};

#endif // COVERT_H
