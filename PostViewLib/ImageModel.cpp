#include "stdafx.h"
#include "ImageModel.h"
#include "3DImage.h"
#include "GLImageRenderer.h"
#include <assert.h>

CImageModel::CImageModel()
{
	m_pImg = 0;
	m_box = BOUNDINGBOX(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
}

CImageModel::~CImageModel()
{
	if (m_pImg) { delete m_pImg; m_pImg = 0; }
	for (int i = 0; i < (int)m_render.size(); ++i) delete m_render[i];
	m_render.clear();
}

void CImageModel::SetFileName(const std::string& fileName)
{
	m_file = fileName;
}

void CImageModel::Set3DImage(C3DImage* img, BOUNDINGBOX b)
{
	assert(m_pImg == nullptr);
	m_pImg = img;
	m_box = b;
}

bool CImageModel::RemoveRenderer(CGLImageRenderer* render)
{
	for (int i = 0; i < (int)m_render.size(); ++i)
	{
		if (m_render[i] == render)
		{
			delete render;
			m_render.erase(m_render.begin() + i);
			return true;
		}
	}
	return false;
}

void CImageModel::AddImageRenderer(CGLImageRenderer* render)
{
	assert(render);
	m_render.push_back(render);
}
