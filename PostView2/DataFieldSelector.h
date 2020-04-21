#pragma once
#include <QPushButton>
#include <QMenu>
#include <QComboBox>
#include <PostLib/FEMeshData.h>
#include <PostLib/FEPostModel.h>

// Abstract class for creating the menu of the CDataSelectorButton.
class CDataSelector
{
public:
	CDataSelector();
	virtual ~CDataSelector();

	virtual void BuildMenu(QMenu* menu) = 0;
};

// generic selector for displaying a list of options
class CGenericDataSelector : public CDataSelector
{
public:
	CGenericDataSelector();

	void AddOption(const QString& opt);

protected:
	void BuildMenu(QMenu* menu) override;

private:
	QStringList	m_ops;
};

// Derived class that selects Time or Steps
class CTimeStepSelector : public CDataSelector
{
public:
	CTimeStepSelector();

	void BuildMenu(QMenu* menu) override;
};

// Derived class that builds the menu of a CDataSelectorButton from the model's data
class CModelDataSelector : public CDataSelector, public Post::FEModelDependant
{
public:
	CModelDataSelector(Post::FEPostModel* fem, Post::Data_Tensor_Type ntype, bool btvec = false);
	~CModelDataSelector();

	void BuildMenu(QMenu* menu) override;

public:
	// inherited from FEModelDependant
	void Update(Post::FEPostModel* pfem) override;

private:
	Post::FEPostModel*			m_fem;
	Post::Data_Tensor_Type	m_class;
	bool m_bvec;
};

// Button with a menu for selecting data fields. 
// The menu is generated by a CDataSelector. 
class CDataSelectorButton : public QPushButton
{
	Q_OBJECT

public:
	CDataSelectorButton(QWidget* parent = 0);
	~CDataSelectorButton();

	// set the data selector
	void SetDataSelector(CDataSelector* dataSelector);

	// update the menu
	void UpdateMenu();

	// return the field ID of the selected menu
	int currentValue() const;

	// set the current field ID
	void setCurrentValue(int n);

	// set the current value based on string
	bool setCurrentValue(const char* sz);

protected slots:
	void onAction(QAction* pa);

signals:
	void currentValueChanged(int n);

private:
	CDataSelector*	m_src;
	QMenu*			m_menu;
	int				m_currentValue;
};

class CDataFieldSelector : public CDataSelectorButton
{
public:
	CDataFieldSelector(QWidget* parent = nullptr);

	void BuildMenu(Post::FEPostModel* fem, Post::Data_Tensor_Type ntype, bool btvec = false);
};

class CColorMapSelector : public QComboBox
{
public:
	CColorMapSelector(QWidget* parent = nullptr);
};
