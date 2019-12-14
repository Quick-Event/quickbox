#pragma once

#include "corepluginglobal.h"

#include <QObject>

class QSettings;

namespace Core {

class COREPLUGIN_DECL_EXPORT Plugin : public QObject
{
	Q_OBJECT
	using Super = QObject;
	//typedef qf::qmlwidgets::framework::Plugin Super;
public:
	Plugin(QObject *parent = nullptr);

	Q_INVOKABLE void launchSqlTool();
	Q_INVOKABLE void aboutQuickEvent();
	Q_INVOKABLE void aboutQt();

	COREPLUGIN_DECL_EXPORT static const QString SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE();

	void onInstalled();
};

}
