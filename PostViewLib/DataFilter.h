#pragma once
#include <string>

class FEDataField;

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

//-----------------------------------------------------------------------------
// Calculate the gradient of a scale field
void DataGradient(FEModel& fem, int vecField, int sclField);

//-----------------------------------------------------------------------------
// Extract a component from a data field
FEDataField* DataComponent(FEModel& fem, FEDataField* dataField, int ncomp, const std::string& sname);
