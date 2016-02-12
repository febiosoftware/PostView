#pragma once

class CGLView;
#include "GLWidget.h"
#include <vector>

class CGLWidgetManager
{
public:
	~CGLWidgetManager();

	static CGLWidgetManager* GetInstance();

	void AddWidget(GLWidget* pw);
	void RemoveWidget(GLWidget* pw);
	int Widgets() { return m_Widget.size(); }

	GLWidget* operator [] (int i) { return m_Widget[i]; }

	void AttachToView(CGLView* pview);

//	int handle(int nevent);

	void DrawWidgets();

protected:
	void SnapWidget(GLWidget* pw);

protected:
	CGLView*				m_pview;
	std::vector<GLWidget*>	m_Widget;

private:
	CGLWidgetManager() {}
	CGLWidgetManager(const CGLWidgetManager& m) {}

	static CGLWidgetManager*	m_pmgr;
};
