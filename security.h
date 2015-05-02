// Copyright David Zhu. All rights reserved.

#ifndef SECURITY_H
#define SECURITY_H

class Security
{
public:
    static double calculateNumberOfCellsForDesiredSecurity( double desiredSecurityLevel, double population, bool sp2hdm );

    static float calculatePopulationLevel( double population, bool sp2hdm );
};

#endif // SECURITY_H
