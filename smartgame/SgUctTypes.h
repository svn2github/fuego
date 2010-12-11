#ifndef SG_UCTTYPES_H
#define SG_UCTTYPES_H

#include "SgSystem.h"
#include "SgStatistics.h"

typedef double SgUctEval;

#ifdef ENABLE_LARGE_COUNTS

    typedef double SgUctEstimate;

    typedef double SgUctValue;

//  typedef unsigned long long SgUctCount;

    typedef double SgUctCount;

    typedef double SgUctRaveValue;

    typedef double SgUctRaveCount;


#else // !ENABLE_LARGE_COUNTS

    typedef float SgUctEstimate;
    
    typedef float SgUctValue;
    
//  typedef std::size_t SgUctCount;

    typedef float SgUctCount;
    
    typedef float SgUctRaveValue;
    
    typedef float SgUctRaveCount;
    
#endif // ENABLE_LARGE_COUNTS


typedef SgStatisticsBase<SgUctValue,SgUctCount> SgUctValueStatistics;

typedef SgStatisticsBase<SgUctRaveValue,SgUctRaveCount> SgUctRaveStatistics;

typedef SgStatisticsBase<volatile SgUctValue,volatile SgUctCount> SgUctValueStatisticsVolatile;

typedef SgStatisticsBase<volatile SgUctRaveValue,volatile SgUctRaveCount> SgUctRaveStatisticsVolatile;


#endif // SG_UCTTYPES_H
