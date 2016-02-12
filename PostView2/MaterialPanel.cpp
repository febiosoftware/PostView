#include "MaterialPanel.h"
#include <QBoxLayout>
#include <QTreeWidget>
#include <QLabel>

CMaterialPanel::CMaterialPanel(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* pg = new QVBoxLayout(this);
	QTreeWidget* ptree = new QTreeWidget(this);
	QLabel* plabel = new QLabel(this);
	plabel->setText("Hello there");

	pg->addWidget(ptree);
	pg->addWidget(plabel);
}
