#pragma once

#include <QQmlExtensionPlugin>

QT_BEGIN_NAMESPACE

class QFCorePlugin : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
	QFCorePlugin(QObject *parent = 0) : QQmlExtensionPlugin(parent) { }
	void registerTypes(const char *uri) override;
};

QT_END_NAMESPACE
