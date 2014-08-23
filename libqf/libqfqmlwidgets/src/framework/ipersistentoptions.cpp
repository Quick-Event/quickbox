#include "ipersistentoptions.h"

#include <qf/core/assert.h>

using namespace qf::qmlwidgets::framework;

IPersistentOptions::IPersistentOptions(QObject *controlled_object)
	: m_controlledObject(controlled_object)
{
	QF_ASSERT_EX(m_controlledObject,
				 "controlled object cannot be NULL");
}

QString IPersistentOptions::persistentOptionsPath()
{
	return m_path;
}

void IPersistentOptions::setPersistentOptionsPath(const QString &path)
{
	m_path = path;
}
