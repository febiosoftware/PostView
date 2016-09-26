#include "stdafx.h"
#include "Palette.h"
#include "XMLWriter.h"
#include "XMLReader.h"

CPalette::CPalette(const string& name) : m_name(name)
{
}

CPalette::CPalette(const CPalette& p)
{
	m_col = p.m_col;
	m_name = p.m_name;
}

void CPalette::operator = (const CPalette& p)
{
	m_col = p.m_col;
	m_name = p.m_name;
}

int CPalette::Colors() const
{ 
	return (int)m_col.size(); 
}

void CPalette::AddColor(const GLCOLOR& c)
{
	m_col.push_back(c);
}

GLCOLOR CPalette::Color(int i) const
{
	if ((i>=0) && (i< Colors())) return m_col[i];
	return GLCOLOR(0,0,0);
}

//=============================================================================
CPaletteManager*	CPaletteManager::m_this = 0;

CPaletteManager::CPaletteManager()
{
	// add standard palette
	CPalette pal("Standard");

	pal.AddColor(GLCOLOR(128, 255, 0));
	pal.AddColor(GLCOLOR(128,  0,255));
	pal.AddColor(GLCOLOR(  0,  0,255));
	pal.AddColor(GLCOLOR(255,255,  0));
	pal.AddColor(GLCOLOR(255,  0,255));
	pal.AddColor(GLCOLOR(  0,255,255));
	pal.AddColor(GLCOLOR(255,128,  0));
	pal.AddColor(GLCOLOR(255,  0,128));
	pal.AddColor(GLCOLOR(255,  0,  0));
	pal.AddColor(GLCOLOR(  0,255,128));
	pal.AddColor(GLCOLOR(  0,255,  0));
	pal.AddColor(GLCOLOR(  0,128,255));
	pal.AddColor(GLCOLOR(128,  0,  0));
	pal.AddColor(GLCOLOR(  0,128,  0));
	pal.AddColor(GLCOLOR(  0,  0,128));
	pal.AddColor(GLCOLOR(128,128,  0));
	pal.AddColor(GLCOLOR(128,  0,128));
	pal.AddColor(GLCOLOR(  0,128,128));
	pal.AddColor(GLCOLOR(255,255,255));
	pal.AddColor(GLCOLOR(192,192,192));
	pal.AddColor(GLCOLOR(164,164,164));
	pal.AddColor(GLCOLOR(128,128,128));
	pal.AddColor(GLCOLOR( 92, 92, 92));
	pal.AddColor(GLCOLOR( 64, 64, 64));
	pal.AddColor(GLCOLOR( 32, 32, 32));

	AddPalette(pal);

	m_currentIndex = 0;
}

CPaletteManager& CPaletteManager::GetInstance()
{
	if (m_this == 0) m_this = new CPaletteManager;
	return *m_this;
}

void CPaletteManager::AddPalette(const CPalette& p)
{
	m_pal.push_back(p);
}

int CPaletteManager::Palettes() const
{
	return (int) m_pal.size();
}

const CPalette& CPaletteManager::Palette(int i) const 
{
	 return m_pal[i]; 
}

const CPalette& CPaletteManager::CurrentPalette()
{
	CPaletteManager& This = CPaletteManager::GetInstance();
	return This.m_pal[This.m_currentIndex];
}

void CPaletteManager::SetCurrentIndex(int n)
{
	if ((n>=0) && (n<Palettes())) m_currentIndex = n;
}

bool CPaletteManager::Save(const string& file, const CPalette& pal)
{
	XMLWriter xml;
	if (xml.open(file.c_str()) == false) return false;

	XMLElement el("PostViewPalette");
	el.add_attribute("version", "1.0");
	xml.add_branch(el);
	{
		XMLElement el("Palette");
		el.add_attribute("name", pal.Name().c_str());
		xml.add_branch(el);
		{
			int NCOL = pal.Colors();
			for (int i=0; i<NCOL; ++i)
			{
				GLCOLOR c = pal.Color(i);
				int v[3] = {c.r, c.g, c.b};
				xml.add_leaf("color", v, 3);
			}
		}
		xml.close_branch();
	}
	xml.close_branch();
	xml.close();
	return true;

}

bool CPaletteManager::Load(const string& file)
{
	FILE* fp = fopen(file.c_str(), "rt");
	if (fp == 0) return false;

	// attach an xml parser to the file stream
	XMLReader xml;
	if (xml.Attach(fp) == false) { fclose(fp); return false; }

	// find the root tag
	XMLTag tag;
	if (xml.FindTag("PostViewPalette", tag) == false) { fclose(fp); return false; }

	char szbuf[256] = { 0 };
	++tag;
	do
	{
		if (tag == "Palette")
		{
			// get the palette's name, or create a new one
			const char* szname = tag.AttributeValue("name", true);
			if (szname == 0)
			{
				sprintf(szbuf, "Palette %d", Palettes() + 1);
				szname = szbuf;
			}

			// create the palette
			CPalette pal(szname);

			if (tag.isempty() == false)
			{
				++tag;
				do
				{
					if (tag == "color")
					{
						int c[3] = {0,0,0};
						tag.value(c, 3);

						GLCOLOR col((byte) c[0], (byte) c[1], (byte) c[2]);
						pal.AddColor(col);

						++tag;
					}
					else xml.SkipTag(tag);
				}
				while (!tag.isend());

				AddPalette(pal);
			}
			
			++tag;
		}
		else xml.SkipTag(tag);
	}
	while (!tag.isend());

	fclose(fp);

	return true;
}
