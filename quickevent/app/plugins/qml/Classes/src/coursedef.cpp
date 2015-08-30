#include "coursedef.h"

QString CourseDef::toString() const
{
	QString ret;
	QStringList sl_codes;
	for(auto c : codes())
		sl_codes << c.toString();
	/*
	QStringList sl_classes;
	for(auto c : classes())
		sl_classes << c.toString();
	*/
	ret += "course: " + name() + " classes: " + classes().join(',') + " length: " + QString::number(lenght()) + " climb: " + QString::number(climb());
	ret += "\n\tcodes: " + sl_codes.join(',');
	return ret;
}

