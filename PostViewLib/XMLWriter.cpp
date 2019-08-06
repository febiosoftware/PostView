// XMLWriter.cpp: implementation of the XMLWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XMLWriter.h"
#include <string.h>
using namespace Post;

void XMLElement::value(int* pi, int n)
{
	m_szval[0] = 0;
	if (n==0) return;

	sprintf(m_szval, "%6d", pi[0]);
	int l = strlen(m_szval);
	for (int i=1; i<n; ++i)
	{
		sprintf(m_szval+l, ",%6d", pi[i]);
		l = strlen(m_szval);
	}
}

void XMLElement::value(double* pg, int n)
{
	m_szval[0] = 0;
	if (n==0) return;

	sprintf(m_szval, "%lg", pg[0]);
	int l = strlen(m_szval);
	for (int i=1; i<n; ++i)
	{
		sprintf(m_szval+l, ",%lg", pg[i]);
		l = strlen(m_szval);
	}
}

int XMLElement::add_attribute(const char* szn, const std::string& s)
{
	strcpy(m_attn[m_natt], szn);
	strcpy(m_attv[m_natt], s.c_str());
	m_natt++;
	return m_natt - 1;
}

int XMLElement::add_attribute(const char* szn, const char* szv)
{
	strcpy(m_attn[m_natt], szn);
	strcpy(m_attv[m_natt], szv);
	m_natt++;
	return m_natt-1;
}

int XMLElement::add_attribute(const char* szn, int n)
{
	strcpy(m_attn[m_natt], szn);
	sprintf(m_attv[m_natt], "%d", n);
	m_natt++;
	return m_natt-1;
}

int XMLElement::add_attribute(const char* szn, bool b)
{
	strcpy(m_attn[m_natt], szn);
	sprintf(m_attv[m_natt], "%s", (b?"true":"false"));
	m_natt++;
	return m_natt-1;
}

int XMLElement::add_attribute(const char* szn, double g)
{
	strcpy(m_attn[m_natt], szn);
	sprintf(m_attv[m_natt], "%lg", g);
	m_natt++;
	return m_natt-1;
}

int XMLElement::add_attribute(const char* szn, GLColor& c)
{
	strcpy(m_attn[m_natt], szn);
	sprintf(m_attv[m_natt], "#%02X%02X%02X%02X", c.r, c.g, c.b, c.a);
	m_natt++;
	return m_natt - 1;
}

int XMLElement::add_attribute(const char* szn, double* v, int n)
{
	strcpy(m_attn[m_natt], szn);
	char* sz = m_attv[m_natt];
	for (int i = 0; i < n; ++i)
	{
		sprintf(sz, "%lg", v[i]); sz += strlen(sz);
		if (i != n - 1) { sprintf(sz, ", "); sz += strlen(sz); }
	}
	m_natt++;
	return m_natt - 1;
}

int XMLElement::add_attribute(const char* szn, int* d, int n)
{
	strcpy(m_attn[m_natt], szn);
	char* sz = m_attv[m_natt];
	for (int i = 0; i < n; ++i)
	{
		sprintf(sz, "%d", d[i]); sz += strlen(sz);
		if (i != n - 1) { sprintf(sz, ", "); sz += strlen(sz); }
	}
	m_natt++;
	return m_natt - 1;
}

void XMLElement::set_attribute(int nid, const char* szv)
{
	strcpy(m_attv[nid], szv);
}

void XMLElement::set_attribute(int nid, int n)
{
	sprintf(m_attv[nid], "%d", n);
}

void XMLElement::set_attribute(int nid, bool b)
{
	sprintf(m_attv[nid], "%s", (b?"true":"false"));
}

void XMLElement::set_attribute(int nid, double g)
{
	sprintf(m_attv[nid], "%lg", g);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XMLWriter::XMLWriter()
{
	m_fp = 0;
	m_level = 0;

	m_sztab[0] = 0;
}

XMLWriter::~XMLWriter()
{
	close();
}

bool XMLWriter::open(const char* szfile)
{
	if (m_fp) return false;

	m_fp = fopen(szfile, "wt");

	// write the first line
	fprintf(m_fp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");

	m_os = DEFAULT;
	
	return (m_fp != 0);
}

void XMLWriter::close()
{
	if (m_fp) fclose(m_fp); m_fp = 0;
}

void XMLWriter::SetOutputStyle(OUTPUT_STYLE os)
{
	m_os = os;
}

void XMLWriter::inc_level()
{
	++m_level;

	m_sztab[0] = 0;
	int l=0;
	for (int i=0; i<m_level; ++i) 
	{
		sprintf(m_sztab+l, "\t");
		++l;
	}
	m_sztab[l] = 0;
}

void XMLWriter::dec_level()
{
	if (m_level <= 0) return;

	--m_level;

	m_sztab[0] = 0;
	int l=0;
	for (int i=0; i<m_level; ++i) 
	{
		sprintf(m_sztab+l, "\t");
		++l;
	}
	m_sztab[l] = 0;
}


void XMLWriter::add_branch(XMLElement& el, bool bclear)
{
	char szformat[256] = {0};
	sprintf(szformat, "%s<%%s", m_sztab);

	if (m_os == OUTPUT_STYLE::DEFAULT)
	{
		fprintf(m_fp, szformat, el.m_sztag);

		for (int i = 0; i < el.m_natt; ++i)
		{
			fprintf(m_fp, " %s=\"%s\"", el.m_attn[i], el.m_attv[i]);
		}
	}
	else
	{
		fprintf(m_fp, "\n");
		fprintf(m_fp, szformat, el.m_sztag);
		fprintf(m_fp, "\n");
		for (int i = 0; i < el.m_natt; ++i)
		{
			fprintf(m_fp, "%s", m_sztab);
			fprintf(m_fp, "\t%s=\"%s\"", el.m_attn[i], el.m_attv[i]);
			if (i != el.m_natt - 1) fprintf(m_fp, "\n");
		}
	}

	fprintf(m_fp, ">%s\n", el.m_szval);
	strcpy(m_tag[m_level], el.m_sztag);

	inc_level();

	if (bclear) el.clear();
}

void XMLWriter::add_branch(const char* sz)
{
	char szformat[256] = {0};
	sprintf(szformat, "%s<%%s>\n", m_sztab);
	fprintf(m_fp, szformat, sz);

	strcpy(m_tag[m_level], sz);
	inc_level();
}

void XMLWriter::add_empty(XMLElement& el, bool bclear)
{
	char szformat[256] = {0};
	sprintf(szformat, "%s<%%s", m_sztab);

	if (m_os == OUTPUT_STYLE::DEFAULT)
	{
		fprintf(m_fp, szformat, el.m_sztag);

		for (int i = 0; i < el.m_natt; ++i)
		{
			fprintf(m_fp, " %s=\"%s\"", el.m_attn[i], el.m_attv[i]);
		}
	}
	else
	{
		fprintf(m_fp, "\n");
		fprintf(m_fp, szformat, el.m_sztag);
		fprintf(m_fp, "\n");
		for (int i = 0; i < el.m_natt; ++i)
		{
			fprintf(m_fp, "%s", m_sztab);
			fprintf(m_fp, "\t%s=\"%s\"", el.m_attn[i], el.m_attv[i]);
			if (i != el.m_natt - 1) fprintf(m_fp, "\n");
		}
	}

	fprintf(m_fp, "/>\n");

	if (bclear) el.clear();
}

void XMLWriter::add_leaf(XMLElement& el, bool bclear)
{
	char szformat[256] = {0};
	sprintf(szformat, "%s<%%s", m_sztab);

	fprintf(m_fp, szformat, el.m_sztag);

	for (int i=0; i<el.m_natt; ++i)
	{
		fprintf(m_fp, " %s=\"%s\"", el.m_attn[i], el.m_attv[i]);
	}

	fprintf(m_fp, ">%s</%s>\n", el.m_szval, el.m_sztag);

	if (bclear) el.clear();
}

void XMLWriter::add_leaf(const char* szn, const char* szv)
{
	char szformat[256] = {0};
	sprintf(szformat, "%s<%%s", m_sztab);

	fprintf(m_fp, szformat, szn);

	fprintf(m_fp, ">%s</%s>\n", szv, szn);
}

void XMLWriter::add_leaf(const char* szn, std::string& s)
{
	char szformat[256] = { 0 };
	sprintf(szformat, "%s<%%s", m_sztab);

	fprintf(m_fp, szformat, szn);

	fprintf(m_fp, ">%s</%s>\n", s.c_str(), szn);
}

void XMLWriter::add_leaf(const char* szn, float* pf, int n)
{
	char szformat[256] = { 0 };
	sprintf(szformat, "%s<%%s>", m_sztab);

	fprintf(m_fp, szformat, szn);

	if (n>0)
	{
		fprintf(m_fp, "%g", pf[0]);
		for (int i = 1; i<n; ++i) fprintf(m_fp, ", %g", pf[i]);
	}

	fprintf(m_fp, "</%s>\n", szn);

}

void XMLWriter::add_leaf(const char* szn, double* pg, int n)
{
	char szformat[256] = {0};
	sprintf(szformat, "%s<%%s>", m_sztab);

	fprintf(m_fp, szformat, szn);

	if (n>0)
	{
		fprintf(m_fp, "%.12lg", pg[0]);
		for (int i=1; i<n; ++i) fprintf(m_fp, ",%.12lg", pg[i]);
	}

	fprintf(m_fp, "</%s>\n", szn);

}

void XMLWriter::add_leaf(const char* szn, int* pi, int n)
{
	char szformat[256] = {0};
	sprintf(szformat, "%s<%%s>", m_sztab);

	fprintf(m_fp, szformat, szn);

	if (n>0)
	{
		fprintf(m_fp, "%d", pi[0]);
		for (int i=1; i<n; ++i) fprintf(m_fp, ",%d", pi[i]);
	}

	fprintf(m_fp, "</%s>\n", szn);
}

void XMLWriter::close_branch()
{
	if (m_level > 0)
	{
		dec_level();

		char szformat[256] = {0};
		sprintf(szformat, "%s</%%s>\n", m_sztab);

		if (m_os == ANDROID) fprintf(m_fp, "\n");
		fprintf(m_fp, szformat, m_tag[m_level]);
	}
}
