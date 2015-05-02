// Copyright David Zhu. All rights reserved.

#include "covert.h"

const std::map< Covert::CellTrainingLevel, float > Covert::CellStrength =
{
    { TrainingLevel_Recruit, 0.5f },
    { TrainingLevel_Regular, 1.f },
    { TrainingLevel_Veteran, 1.5f },
    { TrainingLevel_Elite, 2.f },
};

const float Covert::MaximumSecurityLevel = 0.95f;
