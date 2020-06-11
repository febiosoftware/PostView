/*This file is part of the PostView source code and is licensed under the MIT license
listed below.

See Copyright-PostView.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#include "stdafx.h"
#include "ObjectProps.h"
#include <FSCore/FSObject.h>

//=======================================================================================
CObjectProps::CObjectProps(FSObject* po)
{
	m_po = 0;
	if (po) BuildParamList(po);
}

void CObjectProps::AddParameter(Param& p)
{
	CProperty* prop = nullptr;
	switch (p.GetParamType())
	{
	case Param_FLOAT: 
	{
		prop = addProperty(p.GetLongName(), CProperty::Float);
		if (p.UseRange())
		{
			prop->setFloatRange(p.GetFloatMin(), p.GetFloatMax());
			prop->setFloatStep(p.GetFloatStep());
		}
	}
	break;
	case Param_VEC3D: prop = addProperty(p.GetLongName(), CProperty::Vec3); break;
	case Param_STRING: prop = addProperty(p.GetLongName(), CProperty::String); break;
	case Param_MATH  : prop = addProperty(p.GetLongName(), CProperty::MathString); break;
	case Param_COLOR : prop = addProperty(p.GetLongName(), CProperty::Color); break;
	case Param_BOOL:
	{
		const char* ch = p.GetEnumNames();
		if (ch)
		{
			QStringList ops;
			ops << QString(ch);
			ch = ch + strlen(ch) + 1;
			ops << QString(ch);
			prop = addProperty(p.GetLongName(), CProperty::Enum);
			prop->setEnumValues(ops);
			break;
		}
		else prop = addProperty(p.GetLongName(), CProperty::Bool);
	}
	break;
	case Param_CHOICE:
	case Param_INT:
	{
		const char* ch = p.GetEnumNames();
		if (ch)
		{
			if (ch[0] == '@')
			{
				if (strcmp(ch, "@data_scalar") == 0)
				{
					prop = addProperty(p.GetLongName(), CProperty::DataScalar);
				}
				else if (strcmp(ch, "@data_vec3") == 0)
				{
					prop = addProperty(p.GetLongName(), CProperty::DataVec3);
				}
				else if (strcmp(ch, "@data_mat3") == 0)
				{
					prop = addProperty(p.GetLongName(), CProperty::DataMat3);
				}
				else if (strcmp(ch, "@color_map") == 0)
				{
					prop = addProperty(p.GetLongName(), CProperty::ColorMap);
				}
			}
			else
			{
				QStringList ops = GetEnumValues(ch);
				prop = addProperty(p.GetLongName(), CProperty::Enum);
				prop->setEnumValues(ops);
			}
			break;
		}
		else
		{
			prop = addProperty(p.GetLongName(), CProperty::Int);
			if (p.UseRange())
			{
				prop->setIntRange(p.GetIntMin(), p.GetIntMax());
			}
		}
	}
	break;
	default:
		prop = addProperty(p.GetLongName(), CProperty::String);
		prop->setFlags(CProperty::Visible);
		break;
	}
	if (prop && p.IsVariable()) prop->flags |= CProperty::Variable;
	if (prop) prop->param = &p;

	m_params.push_back(&p);
}

QStringList CObjectProps::GetEnumValues(const char* ch)
{
	QStringList ops;
	while (ch && (*ch))
	{
		ops << QString(ch);
		ch = ch + strlen(ch) + 1;
	}
	return ops;
}

void CObjectProps::BuildParamList(FSObject* po)
{
	m_po = po;
	int NP = po->Parameters();
	for (int i = 0; i<NP; ++i)
	{
		Param& p = po->GetParam(i);
		if (p.IsEditable())
		{
			AddParameter(p);
		}
	}
}

QVariant CObjectProps::GetPropertyValue(Param& p)
{
	switch (p.GetParamType())
	{
	case Param_CHOICE:
	case Param_INT: return p.GetIntValue(); break;
	case Param_FLOAT: return p.GetFloatValue(); break;
	case Param_STRING: return QString::fromStdString(p.GetStringValue()); break;
	case Param_MATH  : return QString::fromStdString(p.GetMathString()); break;
	case Param_BOOL:
	{
		if (p.GetEnumNames()) return (p.GetBoolValue() ? 1 : 0);
		else return p.GetBoolValue();
	}
	break;
	case Param_VEC3D:
	{
		vec3d r = p.GetVec3dValue();
		QString t = Vec3dToString(r);
		return t;
	}
	break;
	case Param_COLOR:
	{
		GLColor c = p.GetColorValue();
		QColor qcol = toQColor(c);
		return qcol;
	}
	break;
	default:
		return "(not supported)";
	}

	return QVariant();
}

QVariant CObjectProps::GetPropertyValue(int i)
{
	Param& p = *m_params[i];
	return GetPropertyValue(p);
}

void CObjectProps::SetPropertyValue(Param& p, const QVariant& v)
{
	switch (p.GetParamType())
	{
	case Param_CHOICE:
	case Param_INT: p.SetIntValue(v.toInt()); break;
	case Param_FLOAT: p.SetFloatValue(v.toDouble()); break;
	case Param_STRING: p.SetStringValue(v.toString().toStdString()); break;
	case Param_MATH  : p.SetMathString(v.toString().toStdString()); break;
	case Param_BOOL:
	{
		if (p.GetEnumNames()) p.SetBoolValue(v.toInt() != 0);
		else p.SetBoolValue(v.toBool());
	}
	break;
	case Param_VEC3D:
	{
		QString t = v.toString();
		vec3d r = StringToVec3d(t);
		p.SetVec3dValue(r);
	}
	break;
	case Param_COLOR:
	{
		QColor qcol = v.value<QColor>();
		GLColor c = toGLColor(qcol);
		p.SetColorValue(c);
	}
	break;
	default:
		assert(false);
	}
}

void CObjectProps::SetPropertyValue(int i, const QVariant& v)
{
	Param& p = *m_params[i];
	SetPropertyValue(p, v);
	m_po->UpdateData();
}
