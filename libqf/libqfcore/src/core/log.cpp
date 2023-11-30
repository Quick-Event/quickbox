#include "log.h"
NecroLog operator<<(NecroLog log, const QString &s) { return log.operator<<(s.toStdString()); }
NecroLog operator<<(NecroLog log, const QDateTime &dt) { return log.operator<<(dt.toString(Qt::ISODateWithMs).toStdString()); }
NecroLog operator<<(NecroLog log, const QDate &dt) { return log.operator<<(dt.toString(Qt::ISODateWithMs).toStdString()); }
NecroLog operator<<(NecroLog log, const QTime &dt) { return log.operator<<(dt.toString(Qt::ISODateWithMs).toStdString()); }
NecroLog operator<<(NecroLog log, const QUrl &url) { return log.operator<<(url.toString().toStdString()); }
NecroLog operator<<(NecroLog log, const QStringList &sl) {
	QString s = '[' + sl.join(',') + ']';
	return log.operator<<(s.toStdString());
}
NecroLog operator<<(NecroLog log, const QByteArray &ba) {
	QString s = ba.toHex();
	return log.operator<<(s.toStdString());
}
NecroLog operator<<(NecroLog log, const QVariant &v) {
	QString s = v.toString();
	return log.operator<<(s.toStdString());
}
NecroLog operator<<(NecroLog log, const qf::core::String &s) { return log.operator<<(s.toStdString()); }
NecroLog operator<<(NecroLog log, const QPoint &p) {
	QString s = "QPoint(%1, %2)";
	return log.operator<<(s.arg(p.x()).arg(p.y()).toStdString());
}
NecroLog operator<<(NecroLog log, const QPointF &p) {
	QString s = "QPoint(%1, %2)";
	return log.operator<<(s.arg(p.x()).arg(p.y()).toStdString());
}
NecroLog operator<<(NecroLog log, const QSize &sz) {
	QString s = "QSize(%1, %2)";
	return log.operator<<(s.arg(sz.width()).arg(sz.height()).toStdString());
}
NecroLog operator<<(NecroLog log, const QSizeF &sz) {
	QString s = "QSize(%1, %2)";
	return log.operator<<(s.arg(sz.width()).arg(sz.height()).toStdString());
}
NecroLog operator<<(NecroLog log, const QRect &r) {
	QString s = "QRect(%1, %2, %3 x %4)";
	return log.operator<<(s.arg(r.x()).arg(r.y())
	                      .arg(r.width()).arg(r.height()).toStdString());
}
NecroLog operator<<(NecroLog log, const QRectF &r) {
	QString s = "QRect(%1, %2, %3 x %4)";
	return log.operator<<(s.arg(r.x()).arg(r.y())
	                      .arg(r.width()).arg(r.height()).toStdString());
}
