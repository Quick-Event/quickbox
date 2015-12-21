#ifndef QF_CORE_LOGCORE_H
#define QF_CORE_LOGCORE_H

#include "coreglobal.h"

#include <QMetaType>

class QLoggingCategory;

namespace qf {
namespace core {

class QFCORE_DECL_EXPORT Log
{
public:
	enum class Level : int { Invalid = -1, Fatal, Error, Warning, Info, Debug };
public:
	/*
	static const char* categoryDebugName;
	static const char* categoryInfoName;
	static const char* categoryWarningName;
	static const char* categoryErrorName;
	static const char* categoryFatalName;

	static const QLoggingCategory& categoryForLevel(Level level);
	*/
	static const char *levelName(Level level);
	static QString levelToString(Level level);
	Level stringToLevel(const QString &level_name);

	static void checkLogLevelMetaTypeRegistered();
public:
	static QString stackTrace();
};

}
}

Q_DECLARE_METATYPE(qf::core::Log::Level)

#endif
