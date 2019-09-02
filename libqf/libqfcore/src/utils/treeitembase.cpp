
//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//

#include "treeitembase.h"

#include "../core/string.h"
#include "../core/log.h"

#include <QStringList>

using namespace qf::core::utils;

//=================================================
//             TreeItemPath
//=================================================

const TreeItemPath& TreeItemPath::invalidPath()
{
	static TreeItemPath p = QList<int>(); 
	if(p.isEmpty())
		p << -1;
	return p;
}

const TreeItemPath& TreeItemPath::rootPath()
{
	static TreeItemPath p = QList<int>();
	return p;
}

void TreeItemPath::ensureMetatypeRegistered()
{
	if(!QMetaType::isRegistered(qMetaTypeId<TreeItemPath>())) 
		qRegisterMetaType<TreeItemPath>();
}

bool TreeItemPath::isValid() const
{
	return !(QList<int>::length() == 1 && (*this)[0] == -1);
}

int TreeItemPath::length() const
{
	if(!isValid())
		return -1;
	return QList<int>::length();
}

QString TreeItemPath::toString() const
{
	if(!isValid())
		return "invalid path";
	QStringList sl;
	foreach(int i, *this)
		sl << QString::number(i);
	return sl.join("/");
}

TreeItemPath TreeItemPath::fromString(const QString &path_str)
{
	TreeItemPath ret = rootPath();
	QStringList sl = qf::core::String(path_str).splitAndTrim('/');
	foreach(QString s, sl)
		ret << s.toInt();
	return ret;
}

//=================================================
//             TreeItemBase
//=================================================
TreeItemBase::TreeItemBase(TreeItemBase *_parent)
{
	f_parent = nullptr;
	setParent(_parent);
}

TreeItemBase::~TreeItemBase()
{
	if(f_parent)
		f_parent->unlinkChild(this);
	clearChildren();
}


void TreeItemBase::setParent(TreeItemBase *_parent)
{
	if(f_parent)
		f_parent->unlinkChild(this);
	f_parent = _parent;
	if(f_parent)
		f_parent->f_children << this;
}

TreeItemBase* TreeItemBase::child(int ix) const
{
	if(ix < 0)
		ix = childrenCount() + ix;
	return f_children.value(ix);
}

void TreeItemBase::insertChild(int before_ix, TreeItemBase *it_child)
{
	if(!it_child) {
		qfError() << QF_FUNC_NAME << "child item is NULL";
		return;
	}
	if(before_ix < 0) {
		qfWarning() << QF_FUNC_NAME << "before_ix:" << before_ix << " should be >= 0. Changed to 0";
		before_ix = 0;
	}
	//qfInfo() << "before_ix:" << before_ix << "parent:" << parent() << "child it parent:" << it_child->parent();
	if(before_ix > childrenCount())
		before_ix = childrenCount();
	if(it_child->parent() == this) {
		/// pokud presouvam v ramci stejneho parentu, vyjmuti it_child zpusobi posunuti indexu
		/// takze ho vlastne musim vlozit na index o jednicku nizsi
		if(indexOfChild(it_child) < before_ix) before_ix--;
	}
	//qfInfo() << "before_ix2:" << before_ix;
	it_child->setParent(NULL);
	f_children.insert(before_ix, it_child);
	it_child->f_parent = this;
}

TreeItemBase* TreeItemBase::exchangeChild(int child_ix, TreeItemBase *it_child)
{
	TreeItemBase *ret = nullptr;
	if(!it_child) {
		qfError() << QF_FUNC_NAME << "child item is NULL";
		return ret;
	}
	if(child_ix < 0 || child_ix >= childrenCount()) {
		qfError() << QF_FUNC_NAME << "Child index:" << child_ix <<"is out of bounds 0 -" << childrenCount()-1;
		return ret;
	}
	ret = child(child_ix);
	ret->f_parent = nullptr;
	if(it_child->parent() && it_child->parent() != this)
		it_child->setParent(NULL);
	childrenRef()[child_ix] = it_child;
	it_child->f_parent = this;
	return ret;
}

void TreeItemBase::exchangeChild(TreeItemBase *it_old_child, TreeItemBase *it_new_child)
{
	int ix = children().indexOf(it_old_child);
	if(ix < 0) {
		qfError() << "Old child is not a child of mine.";
	}
	else {
		exchangeChild(ix, it_new_child);
	}
}

void TreeItemBase::clearChildren()
{
	foreach(TreeItemBase *it, children()) {
		/// optimalizace, aby dite v destruktoru nemazelo zbytecne samo sebe z children, kdyz se to pak udela najednou.
		if(it) {
			it->f_parent = nullptr;
			delete it;
		}
	}
	f_children.clear();
}

void TreeItemBase::unlinkChild(TreeItemBase *child)
{
	if(f_children.isEmpty())
		return;
	int i = f_children.indexOf(child);
	if(i >= 0)
		f_children.removeAt(i);
	else
		qfWarning() << "item not found in children list, children count:" << f_children.count();
}

TreeItemBase* TreeItemBase::sibbling(int offset)
{
	TreeItemBase *ret = nullptr;
	TreeItemBase *par_it = parent();
	if(par_it) {
		int ix = par_it->indexOfChild(this);
		if(ix >= 0) {
			ix += offset;
			if(ix >= 0 && ix < par_it->childrenCount())
				ret = par_it->child(ix);
		}
	}
	return ret;
}

void TreeItemBase::shift(bool up, bool wrap)
{
	TreeItemBase *par_it = parent();
	if(par_it) {
		if(par_it->childrenCount() > 1) {
			int ix1 = par_it->indexOfChild(this);
			if(ix1 >= 0) {
				int ix2 = up? ix1+1: ix1-1;
				if(ix2 < 0) {
					if(!up && !wrap) return;
					ix2 = par_it->childrenCount() - 1;
				}
				if(ix2 >= par_it->childrenCount()) {
					if(up && !wrap) return;
					ix2 = 0;
				}
				if(ix2 != ix1) {
					par_it->childrenRef()[ix1] = par_it->childrenRef()[ix2];
					par_it->childrenRef()[ix2] = this;
				}
			}
		}
	}
}

TreeItemPath TreeItemBase::path() const
{
	//qfLogFuncFrame() << "for element id:" << f_elementId;
	TreeItemPath ret = TreeItemPath::rootPath();
	TreeItemBase *par_it = this->parent();
	const TreeItemBase *it = this;
	while(par_it) {
		for(int i=0; i<par_it->childrenCount(); i++) {
			TreeItemBase *ch_it = par_it->child(i);
			if(ch_it == it) {
				ret.prepend(i);
				break;
			}
		}
		it = par_it;
		par_it = par_it->parent();
	}
	//if(ret.isEmpty()) ret = TreeItemPath::rootPath();
	#if 0 //defined QT_DEBUG
	{
		QStringList sl;
		foreach(int i, ret) sl << QString::number(i);
		//qfDebug() << "\t return:" << sl.join("-");
	}
	#endif
	return ret;
}

TreeItemBase* TreeItemBase::cd(const TreeItemPath &path) const
{
	qfLogFuncFrame();
	//qfInfo() << "path:" << path.toString() << "is valid:" << path.isValid();
	TreeItemBase *ret = nullptr;
	qfDebug() << "\t path:" << path.toString() << "is valid:" << path.isValid();
	if(path.isValid()) {
		ret = const_cast<TreeItemBase*>(this);
		for(int i=0; i<path.count(); i++) {
			int ix = path.value(i);
			ret = ret->child(ix);
			qfDebug() << "\t path index #" << i << "->" << ix << "=" << ret;
			if(!ret)
				break;
		}
	}
	return ret;
}

