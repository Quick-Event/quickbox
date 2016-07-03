#ifndef TXTIMPORTER_H
#define TXTIMPORTER_H

#include <QObject>

class TxtImporter : public QObject
{
	Q_OBJECT
public:
	explicit TxtImporter(QObject *parent = 0);

signals:

public slots:
};

#endif // TXTIMPORTER_H