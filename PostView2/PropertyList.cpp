#include "PropertyList.h"

CProperty& CProperty::operator = (const CProperty& p)
{
	type = p.type;
	flags = p.flags;
	name = p.name;
	info = p.info;
	values = p.values;
	imin = p.imin;
	imax = p.imax;
	fmin = p.fmin;
	fmax = p.fmax;
	fstep = p.fstep;
	return *this;
}

CProperty::CProperty(const QString& sname, CProperty::Type itype) : name(sname), type(itype), info(sname)
{
	flags = Visible | Editable;
	imin = -123456789;
	imax = 123457689;
	fmin = -1e99;
	fmax =  1e99;
	fstep = 0.01;
}

CProperty::CProperty(const QString& sname, CProperty::Type itype, const QString& sinfo) : name(sname), type(itype), info(sinfo)
{
	flags = Visible | Editable;
	imin = -123456789;
	imax = 123456789;
	fmin = -1e99;
	fmax =  1e99;
	fstep = 0.01;
}
