#include "reportdocument.h"

#include <qf/core/log.h>
#include <qf/core/string.h>
//#include <qf/core/exception.h>
//#include <qf/core/utils/fileutils.h>

//#include <QPair>
//#include <QFile>
//#include <QDir>

namespace qfc = qf::core;
namespace qfu = qf::core::utils;

using namespace qf::qmlwidgets::reports;

ReportDocument::ReportDocument(QQmlEngine *engine, const QString &file_name, QObject *parent) :
	Super(engine, file_name, parent)
{
}
