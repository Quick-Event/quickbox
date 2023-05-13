// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qfqmlreports_plugin.h"

#include <qf/qmlwidgets/reports/processor/reportitempara.h>
#include <qf/qmlwidgets/reports/processor/reportitemimage.h>
#include <qf/qmlwidgets/reports/processor/reportitemframe.h>
#include <qf/qmlwidgets/reports/processor/reportitemdetail.h>
#include <qf/qmlwidgets/reports/processor/reportitemband.h>
#include <qf/qmlwidgets/reports/processor/banddatamodel.h>
#include <qf/qmlwidgets/reports/processor/reportitembreak.h>
#include <qf/qmlwidgets/reports/processor/reportitemreport.h>
#include <qf/qmlwidgets/reports/processor/style/sheet.h>
#include <qf/qmlwidgets/reports/processor/style/color.h>
#include <qf/qmlwidgets/reports/processor/style/pen.h>

#include <QtQml>

QT_BEGIN_NAMESPACE

void QFQmlRepoertPlugin::registerTypes(const char *uri)
{
	Q_ASSERT(uri == QLatin1String("qf.qmlreports"));

	qmlRegisterType<qf::qmlwidgets::reports::ReportItem>(uri, 1, 0, "Item");
	qmlRegisterType<qf::qmlwidgets::reports::ReportItemReport>(uri, 1, 0, "Report");
	qmlRegisterType<qf::qmlwidgets::reports::ReportItemFrame>(uri, 1, 0, "Frame");
	qmlRegisterType<qf::qmlwidgets::reports::ReportItemFrame>(uri, 1, 0, "Space");
	qmlRegisterType<qf::qmlwidgets::reports::BandDataModel>(uri, 1, 0, "DataModel");
	qmlRegisterType<qf::qmlwidgets::reports::ReportItemBand>(uri, 1, 0, "Band");
	qmlRegisterType<qf::qmlwidgets::reports::ReportItemDetail>(uri, 1, 0, "Detail");
	qmlRegisterType<qf::qmlwidgets::reports::ReportItemPara>(uri, 1, 0, "Para");
	qmlRegisterType<qf::qmlwidgets::reports::ReportItemImage>(uri, 1, 0, "Image");
	qmlRegisterType<qf::qmlwidgets::reports::ReportItemBreak>(uri, 1, 0, "Break");
	qmlRegisterType<qf::qmlwidgets::reports::style::Sheet>(uri, 1, 0, "StyleSheet");
	qmlRegisterType<qf::qmlwidgets::reports::style::Color>(uri, 1, 0, "Color");
	qmlRegisterType<qf::qmlwidgets::reports::style::Pen>(uri, 1, 0, "Pen");
	qmlRegisterType<qf::qmlwidgets::reports::style::Brush>(uri, 1, 0, "Brush");
	qmlRegisterType<qf::qmlwidgets::reports::style::Font>(uri, 1, 0, "Font");
	qmlRegisterType<qf::qmlwidgets::reports::style::Text>(uri, 1, 0, "TextStyle");

	// Auto-increment the import to stay in sync with ALL future QtQuick minor versions
	qmlRegisterModule(uri, 1, 0);
}

QT_END_NAMESPACE
