#ifndef QF_QMLWIDGETS_FRAMEWORK_PLUGIN_H
#define QF_QMLWIDGETS_FRAMEWORK_PLUGIN_H

#include "../qmlwidgetsglobal.h"
#include "application.h"

#include <qf/core/utils.h>

#include <QObject>

class QQmlEngine;

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT Plugin : public QObject
{
	Q_OBJECT
	QString m_featureId;
public:
	explicit Plugin(const QString &feature_id, QObject *parent = nullptr);
	explicit Plugin(QObject *parent = nullptr);
	~Plugin() Q_DECL_OVERRIDE;

	QString homeDir() const { return qf::qmlwidgets::framework::Application::instance()->pluginDataDir() + '/' +  featureId(); }
	QString qmlDir() const { return homeDir() + "/qml"; }
	QString featureId() const { return m_featureId; }

	QQmlEngine* qmlEngine();

	Q_SIGNAL void installed();
};

}}}

#endif
