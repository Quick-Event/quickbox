#pragma once

#include "string.h"
#include "coreglobal.h"
#include <QVariant>
#include <QMetaType>
#include <QString>
#include <QDateTime>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>
#include <QRect>
#include <QRectF>
#include <QUrl>

#define QF_LOG_NECROLOG

#ifdef QT_NO_DEBUG
#define QF_NO_DEBUG_LOG
#endif

#ifdef QF_NO_DEBUG_LOG
#define NECROLOG_NO_DEBUG_LOG
#endif

#include <necrolog.h>

#define qfCDebug(category) nCDebug(category)
#define qfCMessage(category) nCMessage(category)
#define qfCInfo(category) nCInfo(category)
#define qfCWarning(category) nCWarning(category)
#define qfCError(category) nCError(category)

#define qfLogScope(name) nLogScope(name)
#define qfLogFuncFrame() nLogFuncFrame()

#define qfDebug() qfCDebug("")
#define qfMessage() qfCMessage("")
#define qfInfo() qfCInfo("")
#define qfWarning() qfCWarning("")
#define qfError() qfCError("")

#define qfFatal(msg) {qfError() << msg; abort();}

NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QString &s);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QDateTime &dt);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QDate &dt);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QTime &dt);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QUrl &url);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QStringList &sl);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QByteArray &ba);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QVariant &v);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const qf::core::String &s);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QPoint &p);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QPointF &p);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QSize &sz);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QSizeF &sz);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QRect &r);
NecroLog QFCORE_DECL_EXPORT operator<<(NecroLog log, const QRectF &r);
