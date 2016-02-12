#pragma once

//-----------------------------------------------------------------------------
// Forward declaration of FEModel class
class FEModel;

//-----------------------------------------------------------------------------
// Scale data by facor
void DataScale(FEModel& fem, int nfield, double scale);

//-----------------------------------------------------------------------------
// Apply a smoothing operation on data
void DataSmooth(FEModel& fem, int nfield, double theta, int niters);

//-----------------------------------------------------------------------------
// Apply a smoothing operation on data
void DataArithmetic(FEModel& fem, int nfield, int nop, int noperand);
