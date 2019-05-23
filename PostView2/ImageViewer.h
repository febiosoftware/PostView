#pragma once
#include <QWidget>
#include <PostViewLib/FEModel.h>

class CImageModel;

class CImageViewer : public QWidget, public FEModelDependant
{
	Q_OBJECT

protected:
	class Ui;

public:
	CImageViewer(QWidget* parent = nullptr);

	void SetImageModel(CImageModel* img);

	void Update();

	void Update(FEModel* fem) override;

private:
	void UpdatePath();

private slots:
	void onSliderChanged(int val);
	void onOverlayChanged(int val);

private:
	CImageViewer::Ui*	ui;
};
