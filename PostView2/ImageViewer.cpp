#include "stdafx.h"
#include "ImageViewer.h"
#include <QGraphicsView>
#include <QBoxLayout>
#include <QSlider>
#include <PostViewLib/ImageModel.h>
#include <PostViewLib/3DImage.h>
#include <QGraphicsScene>
#include <QPixMap>
#include <QGraphicsPixmapItem>

class CImageViewer::Ui
{
public:
	CImageModel*		m_img;

	QGraphicsScene*	m_gs;

	QGraphicsView*	m_gv;
	QSlider*		m_slider;

public:
	void setup(QWidget* w)
	{
		m_img = nullptr;

		QVBoxLayout* l = new QVBoxLayout;
		l->setMargin(0);

		m_slider = new QSlider;
		m_slider->setOrientation(Qt::Horizontal);

		m_gs = new QGraphicsScene;

		m_gv = new QGraphicsView;
		m_gv->setScene(m_gs);

		l->addWidget(m_slider);
		l->addWidget(m_gv);
		w->setLayout(l);

		QObject::connect(m_slider, SIGNAL(valueChanged(int)), w, SLOT(onSliderChanged(int)));
	}
};

CImageViewer::CImageViewer(QWidget* parent) : QWidget(parent), ui(new CImageViewer::Ui)
{
	ui->setup(this);
}

void CImageViewer::SetImageModel(CImageModel* img)
{
	ui->m_img = img;
	if (img)
	{
		C3DImage& im3d = *ui->m_img->Get3DImage();

		int n = im3d.Depth();
		ui->m_slider->setRange(0, n-1);
	}
	Update();
}

void CImageViewer::onSliderChanged(int val)
{
	Update();
}

void CImageViewer::Update()
{
	ui->m_gs->clear();
	if (ui->m_img == nullptr) return;

	C3DImage& im3d = *ui->m_img->Get3DImage();

	CImage im;
	int slice = ui->m_slider->value();
	im3d.GetSliceZ(im, slice);

	QImage qim(im.GetBytes(), im.Width(), im.Height(), QImage::Format::Format_Grayscale8);

	QPixmap pixmap = QPixmap::fromImage(qim);

	ui->m_gs->setSceneRect(0, 0, im.Width(), im.Height());
	QGraphicsPixmapItem* item = ui->m_gs->addPixmap(pixmap);
	ui->m_gv->fitInView(item, Qt::AspectRatioMode::KeepAspectRatio);
}
