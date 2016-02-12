#include "StatePanel.h"
#include <QBoxLayout>
#include <QTreeWidget>
#include <QLabel>

CStatePanel::CStatePanel(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* pg = new QVBoxLayout(this);
	QTreeWidget* ptree = new QTreeWidget(this);
	QLabel* plabel = new QLabel(this);
	plabel->setText("State data goes here");

	pg->addWidget(ptree);
	pg->addWidget(plabel);
}
