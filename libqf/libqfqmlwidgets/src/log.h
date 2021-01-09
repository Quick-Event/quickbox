#pragma once

#include <qf/core/log.h>

#include <QModelIndex>

inline NecroLog &operator<<(NecroLog log, const QModelIndex &ix) {
	QString s = "QModelIndex(%1, %2)";
	return log.operator<<(s.arg(ix.row()).arg(ix.column()).toStdString());
}
