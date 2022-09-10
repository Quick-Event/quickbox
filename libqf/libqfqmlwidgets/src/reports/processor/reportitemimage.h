//
// Author: Frantisek Vacek <fanda.vacek@volny.cz>, (C) 2006, 2014
//
// Copyright: See COPYING file that comes with this distribution
//

#ifndef QF_QMLWIDGETS_REPORTS_REPORTITEMIMAGE_H
#define QF_QMLWIDGETS_REPORTS_REPORTITEMIMAGE_H

#include "reportitemframe.h"
#include "../../qmlwidgetsglobal.h"

namespace qf {
namespace qmlwidgets {
namespace reports {

class QFQMLWIDGETS_DECL_EXPORT ReportItemImage : public ReportItemFrame
{
	Q_OBJECT

	Q_ENUMS(DataFormat)
	Q_ENUMS(DataEncoding)
	Q_ENUMS(DataCompression)
	Q_ENUMS(AspectRatio)

	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
	Q_PROPERTY(QString data READ data WRITE setData NOTIFY dataChanged)
	Q_PROPERTY(DataFormat dataFormat READ dataFormat WRITE setDataFormat NOTIFY dataFormatChanged)
	Q_PROPERTY(DataEncoding dataEncoding READ dataEncoding WRITE setDataEncoding NOTIFY dataEncodingChanged)
	Q_PROPERTY(DataCompression dataCompression READ dataCompression WRITE setDataCompression NOTIFY dataCompressionChanged)
	Q_PROPERTY(AspectRatio aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
	Q_PROPERTY(bool suppressPrintout READ isSuppressPrintout WRITE setSuppressPrintout NOTIFY suppressPrintoutChanged)
private:
	typedef ReportItemFrame Super;
public:
	ReportItemImage(ReportItem *parent = nullptr);
public:
	enum DataFormat {
		FormatAuto,
		FormatSvg,
		FormatQPicture,
		FormatPng,
		FormatJpg
	};
	enum DataEncoding {
		EncodingRaw,
		EncodingBase64,
		EncodingHex
	};
	enum DataCompression {
		CompressionNone,
		CompressionQCompress
	};
	enum AspectRatio {
		AspectRatioIgnore = Qt::IgnoreAspectRatio,
		AspectRatioKeep = Qt::KeepAspectRatio,
		AspectRatioKeepExpanding = Qt::KeepAspectRatioByExpanding
	};
	QF_PROPERTY_IMPL(QString, d, D, ataSource)
	QF_PROPERTY_IMPL(QString, d, D, ata)
	QF_PROPERTY_IMPL2(DataFormat, d, D, ataFormat, FormatAuto)
	QF_PROPERTY_IMPL2(DataEncoding, d, D, ataEncoding, EncodingRaw)
	QF_PROPERTY_IMPL2(DataCompression, d, D, ataCompression, CompressionNone)
	QF_PROPERTY_IMPL2(AspectRatio, a, A, spectRatio, AspectRatioKeep)
	QF_PROPERTY_BOOL_IMPL(s, S, uppressPrintout)

private:
	void updateResolvedDataSource();
protected:
	//--virtual bool childrenSynced();
	//--virtual void syncChildren();
	virtual PrintResult printMetaPaint(ReportItemMetaPaint *out, const Rect &bounding_rect);
	virtual PrintResult printMetaPaintChildren(ReportItemMetaPaint *out, const ReportItem::Rect &bounding_rect);
protected:
	QString m_resolvedDataSource;
	QString m_currentDataSource;
	//--QDomElement fakeLoadErrorPara;
	//--QDomDocument fakeLoadErrorParaDocument;
};

}}}

#endif // QF_QMLWIDGETS_REPORTS_REPORTITEMIMAGE_H
