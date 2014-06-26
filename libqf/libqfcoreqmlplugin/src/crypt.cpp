#include "crypt.h"

using namespace qf::core::qml;

Crypt::Crypt(QObject *parent) :
	QObject(parent)
{
}

void Crypt::initGenerator(unsigned a, unsigned b, unsigned max_rand)
{
	auto gen = qf::core::utils::Crypt::createGenerator(a, b, max_rand);
	m_crypt.setGenerator(gen);
}

QString Crypt::encrypt(const QString &s, int min_length) const
{
	return QString::fromLatin1(m_crypt.encrypt(s, min_length));
}

QString Crypt::decrypt(const QString &s) const
{
	return m_crypt.decrypt(s.toLatin1());
}

