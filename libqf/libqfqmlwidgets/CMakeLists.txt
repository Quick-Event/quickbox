add_library(libqfqmlwidgets SHARED
    src/action.cpp
    src/actiongroup.cpp
    src/checkbox.cpp
    src/combobox.cpp
    src/datacontroller.cpp
    src/dateedit.cpp
    src/datetimeedit.cpp
    src/dialogbuttonbox.cpp
    src/dialogs/dialog.cpp
    src/dialogs/filedialog.cpp
    src/dialogs/getiteminputdialog.cpp
    src/dialogs/inputdialog.cpp
    src/dialogs/internal/captionframe.cpp
    src/dialogs/messagebox.cpp
    src/dialogs/previewdialog.cpp
    src/dialogs/qmldialog.cpp
    src/exportcsvtableviewwidget.cpp
    src/exporttableviewwidget.cpp
    src/frame.cpp
    src/framework/application.cpp
    src/framework/centralwidget.cpp
    src/framework/cursoroverrider.cpp
    src/framework/datadialogwidget.cpp
    src/framework/dialogwidget.cpp
    src/framework/dockablewidget.cpp
    src/framework/dockwidget.cpp
    src/framework/ipersistentoptions.cpp
    src/framework/ipersistentsettings.cpp
    src/framework/logwidget.cpp
    src/framework/mainwindow.cpp
    src/framework/partswitch.cpp
    src/framework/partwidget.cpp
    src/framework/plugin.cpp
    src/framework/pluginmanifest.cpp
    src/framework/stackedcentralwidget.cpp
    src/graphics/graphics.cpp
    src/headerview.cpp
    src/htmlviewwidget.cpp
    src/idatawidget.cpp
    src/internal/desktoputils.cpp
    src/internal/dlgtableviewcopyspecial.cpp
    src/internal/tableviewchoosecolumnswidget.cpp
    src/internal/tableviewcopytodialogwidget.cpp
    src/label.cpp
    src/layoutpropertiesattached.cpp
    src/layouttypeproperties.cpp
    src/lineedit.cpp
    src/menubar.cpp
    src/progressbar.cpp
    src/reports/processor/banddatamodel.cpp
    src/reports/processor/reportdocument.cpp
    src/reports/processor/reportitem.cpp
    src/reports/processor/reportitem_html.cpp
    src/reports/processor/reportitemband.cpp
    src/reports/processor/reportitembreak.cpp
    src/reports/processor/reportitemdetail.cpp
    src/reports/processor/reportitemframe.cpp
    src/reports/processor/reportitemimage.cpp
    src/reports/processor/reportitempara.cpp
    src/reports/processor/reportitemreport.cpp
    src/reports/processor/reportpainter.cpp
    src/reports/processor/reportprocessor.cpp
    src/reports/processor/style/brush.cpp
    src/reports/processor/style/color.cpp
    src/reports/processor/style/compiledtextstyle.cpp
    src/reports/processor/style/font.cpp
    src/reports/processor/style/pen.cpp
    src/reports/processor/style/sheet.cpp
    src/reports/processor/style/styleobject.cpp
    src/reports/processor/style/text.cpp
    src/reports/widgets/itemvalueeditorwidget.cpp
    src/reports/widgets/printtableviewwidget/printtableviewwidget.cpp
    src/reports/widgets/reportviewwidget.cpp
    src/saveoptionswidget.cpp
    src/spinbox.cpp
    src/splitter.cpp
    src/sqltableitemdelegate.cpp
    src/statusbar.cpp
    src/style.cpp
    src/tableitemdelegate.cpp
    src/tableview.cpp
    src/tableviewproxymodel.cpp
    src/tableviewtoolbar.cpp
    src/textedit.cpp
    src/texteditwidget.cpp
    src/timeedit.cpp
    src/toolbar.cpp
    style/style.qrc
    images/qfqmlwidgets_images.qrc
    images/flat/qfqmlwidgets_flat.qrc
    )

if(USE_QT6)
    qt6_add_translations(libqfqmlwidgets TS_FILES
        libqfqmlwidgets-cs_CZ.ts
        libqfqmlwidgets-fr_FR.ts
        libqfqmlwidgets-nb_NO.ts
        libqfqmlwidgets-nl_BE.ts
        libqfqmlwidgets-pl_PL.ts
        libqfqmlwidgets-ru_RU.ts
        libqfqmlwidgets-uk_UA.ts
        )
else()
    qt5_add_translation(QM_FILES
        libqfqmlwidgets-cs_CZ.ts
        libqfqmlwidgets-fr_FR.ts
        libqfqmlwidgets-nb_NO.ts
        libqfqmlwidgets-nl_BE.ts
        libqfqmlwidgets-pl_PL.ts
        libqfqmlwidgets-ru_RU.ts
        libqfqmlwidgets-uk_UA.ts
        )
    target_sources(libqfqmlwidgets PRIVATE ${QM_FILES})
endif()

target_link_libraries(libqfqmlwidgets PUBLIC Qt::Svg Qt::PrintSupport Qt::Sql Qt::Qml Qt::Widgets Qt::Xml libqfcore)
target_include_directories(libqfqmlwidgets PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(libqfqmlwidgets PUBLIC include)
target_compile_definitions(libqfqmlwidgets PRIVATE QFQMLWIDGETS_BUILD_DLL)

install(TARGETS libqfqmlwidgets)
