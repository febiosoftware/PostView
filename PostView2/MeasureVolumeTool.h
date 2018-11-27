#include "Tool.h"

//-----------------------------------------------------------------------------
class CDocument;

//-----------------------------------------------------------------------------
class CMeasureVolumeTool : public CBasicTool
{
	class Props : public CPropertyList
	{
	public:
		Props(CMeasureVolumeTool* ptool);
		QVariant GetPropertyValue(int i);
		void SetPropertyValue(int i, const QVariant& v);
	private:
		CMeasureVolumeTool*	m_ptool;
	};

public:
	// constructor
	CMeasureVolumeTool(CDocument* doc);

	// get the property list
	CPropertyList*	getPropertyList();

	// Apply button
	void OnApply();

private:
	int		m_nsel;		// selected faces
	double	m_vol;		// volume of selection
	int		m_nformula;	// choose formula

	friend class Props;
};
