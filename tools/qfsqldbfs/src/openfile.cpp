#include "openfile.h"

const OpenFile &OpenFile::sharedNull()
{
	static OpenFile n = OpenFile(SharedDummyHelper());
	return n;
}

OpenFile::OpenFile(OpenFile::SharedDummyHelper)
{
	d = new Data();
}

OpenFile::OpenFile()
{
	*this = sharedNull();
}

OpenFile::OpenFile(const qf::core::sql::DbFsAttrs &a)
{
	d = new Data(a);
}
