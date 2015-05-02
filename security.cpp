// Copyright David Zhu. All rights reserved.

#include <cmath>

#include <limits>

#include "covert.h"
#include "security.h"

double Security::calculateNumberOfCellsForDesiredSecurity( double desiredSecurityLevel, double population, bool sp2hdm )
{
    static const double precision = 0.05;

    if ( desiredSecurityLevel < precision )
    {
        return 0;
    }

    double eliteCellsNeeded = 0;

    // Security formula:
    // s(n) = M - M( 1 - t/(p+1) )^n
    // where:
    // M = SP2's maximum security level (0.95)
    // n = number of cells in country
    // t = average training level of cells in country (<=2)
    // p = population level (<=12 for SP2 1.5.1)

    // n = log( 1 - s/M ) / log( 1 - t/(p+1) )

    auto populationLevel = Security::calculatePopulationLevel( population, sp2hdm );

    if ( ( sp2hdm && ( populationLevel == 1 ) ) ||
        ( !sp2hdm && ( populationLevel - 1 <= precision ) ) )
    {
        eliteCellsNeeded = 1;
    }
    else
    {
        auto numerator = std::log( 1.0 - ( desiredSecurityLevel / Covert::MaximumSecurityLevel ) );
        auto denominator = std::log( 1.0 - ( Covert::CellStrength.at( Covert::TrainingLevel_Elite ) / ( populationLevel + 1 ) ) );
        eliteCellsNeeded = std::ceil( numerator / denominator );
    }

    return eliteCellsNeeded;
}

float Security::calculatePopulationLevel( double population, bool sp2hdm )
{
    static const long long populationConstant = 4000000;
    float populationLevel = 0;

    if ( population < populationConstant )
    {
        populationLevel = ( population / populationConstant ) + 1;
    }
    else
    {
        populationLevel = std::log2( population / populationConstant ) + 2;
    }

    if ( !sp2hdm )
    {
        populationLevel = std::fmin( populationLevel, 12 );
        populationLevel = std::floor( populationLevel );
    }

    return populationLevel;
}
