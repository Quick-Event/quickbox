#pragma once

#include <QWidget>

namespace services {

class ServicesWidget : public QWidget
{
	Q_OBJECT
public:
	explicit ServicesWidget(QWidget *parent = nullptr);

	void reload();
private:
	QWidget *m_centralWidget = nullptr;
};

} // namespace services
