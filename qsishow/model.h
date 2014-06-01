#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVariantList>
#include <QStringList>

class Model : public QObject
{
	Q_OBJECT
public:
	explicit Model(QObject *parent = 0);
	
signals:
	
public slots:
	void shift();
	QVariant data(int index);
protected:
	//void reset();
	void reloadCategories();
	bool addCategoryToStorage();
protected:
	QVariantList f_storage;
	QStringList f_categoriesToProceed;
	QVariantMap f_queryPlaceholders;
	int f_shiftOffset;
};

#endif // MODEL_H
