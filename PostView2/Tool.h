#pragma once
#include "PropertyList.h"

class QWidget;
class CDocument;
class CPropertyListForm;

//-----------------------------------------------------------------------------
// A tool implements a general purpose extension to PostView.
// TODO: rename to CExtension?
// TODO: Can this serve as the basis of a plugin?
class CAbstractTool : public QObject
{
public:
	// constructor. Requires a name for the plugin
	CAbstractTool(const QString& s) : m_name(s) {}

	// retrieve attributes
	const QString& name() { return m_name; }
	void setName(const QString& s) { m_name = s; }

	// override this to create a custum UI widget
	virtual QWidget* createUi() = 0;

	// activate the tool
	// The ui is about to be shown
	virtual void activate(CDocument* doc) {}

	// deactivate the tool
	// the ui is no longer shown
	virtual void deactivate() {}

	// update the Ui
	virtual void updateUi();

private:
	QString	m_name;
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
	CBasicTool(const QString& s, unsigned int flags = 0);

	// A form will be created based on the property list
	QWidget* createUi();

	// return the property list
	virtual CPropertyList* getPropertyList() = 0;

	// method called when user presses Apply button (optional)
	virtual void OnApply() {}

	// update the Ui
	virtual void updateUi();

private slots:
	void on_button_clicked();

private:
	CPropertyList*		m_list;
	CPropertyListForm*	m_form;
	unsigned int		m_flags;
};
