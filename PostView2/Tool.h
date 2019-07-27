#pragma once
#include <PostViewLib/PropertyList.h>
#include <QWidget>

class CDocument;
class CMainWindow;
class CPropertyListForm;
class QHideEvent;
class QShowEvent;

//-----------------------------------------------------------------------------
// A tool implements a general purpose extension to PostView.
// TODO: rename to CExtension?
// TODO: Can this serve as the basis of a plugin?
class CAbstractTool : public QObject
{
public:
	// constructor. Requires a name for the plugin
	CAbstractTool(const QString& s, CMainWindow* wnd);

	// retrieve attributes
	const QString& name() { return m_name; }
	void setName(const QString& s) { m_name = s; }

	// override this to create a custum UI widget
    virtual QWidget* createUi() = 0;

	// activate the tool
	// The ui is about to be shown
	virtual void activate() {}

	// deactivate the tool
	// the ui is no longer shown
	virtual void deactivate() {}

	// update tool contents
	virtual void update(bool reset) {}

	// update the Ui
	virtual void updateUi();

	// get the active document
	CDocument* GetActiveDocument();

protected:
	QString			m_name;
	CMainWindow*	m_wnd;
};

//-----------------------------------------------------------------------------
class CToolUI : public QWidget
{
public:
	CToolUI(CAbstractTool* tool, QWidget* parent = 0) : QWidget(parent), m_tool(tool){}

	void hideEvent(QHideEvent* ev);

	void showEvent(QShowEvent* ev);

private:
	CAbstractTool*	m_tool;
};

//-----------------------------------------------------------------------------
// A tool based on a property list.
// It has an optional "Apply" button. If defined then the derived class
// must implement the OnApply method. 
class CBasicTool : public CAbstractTool
{
	Q_OBJECT

public:
	enum Flags {
		HAS_APPLY_BUTTON = 1
	};

public:
	CBasicTool(const QString& s, CMainWindow* wnd, unsigned int flags = 0);

	// A form will be created based on the property list
	QWidget* createUi();

	// return the property list
	virtual Post::CPropertyList* getPropertyList() = 0;

	// method called when user presses Apply button (optional)
	virtual void OnApply() {}

	// update the Ui
	virtual void updateUi();

private slots:
	void on_button_clicked();

private:
	Post::CPropertyList*	m_list;
	CPropertyListForm*		m_form;
	unsigned int			m_flags;
};
