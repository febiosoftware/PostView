#pragma once
#include <QWidget>

class CImageModel;

class CImageViewer : public QWidget
{
	Q_OBJECT

protected:
	class Ui;

public:
	CImageViewer(QWidget* parent = nullptr);

	void SetImageModel(CImageModel* img);

	void Update();

private slots:
	void onSliderChanged(int val);

private:
	CImageViewer::Ui*	ui;
};
