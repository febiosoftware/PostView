#pragma once
#include <QWidget>
#include <vector>

class CTimeController : public QWidget
{
	Q_OBJECT
public:
	CTimeController(QWidget* parent = 0);

	void clearData();

	// set the time points
	void setTimePoints(const std::vector<double>& time);

	void UpdateScale();

	void setSelection(int i);

protected:
	void paintEvent(QPaintEvent* ev);
	void resizeEvent(QResizeEvent* ev);
	void mousePressEvent(QMouseEvent* ev);

signals:
	void pointClicked(int n);

private:
	std::vector<double>	m_data;
	double	m_dataMin, m_dataMax;
	double	m_min, m_max, m_inc;
	QRect	m_dataRect, m_timeRect;
	int		m_nselect;
};
