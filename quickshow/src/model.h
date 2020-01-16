#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QVariantList>
#include <QStringList>

class Model : public QObject
{
	Q_OBJECT
public:
	explicit Model(QObject *parent = nullptr);
public slots:
	void shift();
	QVariantMap data(int index);
protected:
	//void reset();
	void reloadCategories();
	bool addCategoryToStorage();
protected:
	QVariantList m_storage;
	QVariantList m_categoriesToProceed;
	QVariantMap m_queryPlaceholders;
	int m_shiftOffset;
};

#endif // MODEL_H
