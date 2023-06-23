import QtQml 2.0
import qf.core 1.0
import qf.qmlreports 1.0
import shared.qml.reports 1.0
//import "qrc:/quickevent/core/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"

	property var options
	property string reportTitle: qsTr("Competitors with rented cards in stage %1").arg(stageId);
	property var stageId

	property SqlTableModel sqlModel: SqlTableModel {}

	//debugLevel: 1
	styleSheet: StyleSheet {
		basedOn: ReportStyleCommon { id: myStyle }
	}
	textStyle: myStyle.textStyleDefault

	width: root.options.pageWidth? root.options.pageWidth: 210
	height: root.options.pageHeight? root.options.pageHeight: 297
	hinset: root.options.horizontalMargin? root.options.horizontalMargin: 10
	vinset: root.options.verticalMargin? root.options.verticalMargin: 5
	Frame {
		width: "%"
		height: "%"
		layout: Frame.LayoutStacked
		QuickEventHeaderFooter {
			reportTitle: root.reportTitle
		}
		Frame {
			width: "%"
			height: "%"
			vinset: 10
			Frame {
				Para {
					textStyle: TextStyle {basedOn: "big"}
					text: root.reportTitle;
				}
			}
			Band {
				id: band
				width: "%"
				height: "%"
				columns: "%,%"
				modelData: {
					if(!root.created)
						return null;

					sqlModel.sqlQueryBuilder().clear()
						.select2('classes', 'id, name')
						.from('classes')
						.orderBy('classes.name')//.limit(3);
					sqlModel.reload();
					return sqlModel.toTreeTableData();
				}
				Detail {
					id: detail
					width: "%"
					//keepAll: true
					layout: Frame.LayoutVertical
					//expandChildFrames: true
					Frame {
						fill: Brush {color: Color {def: "khaki"} }
						textStyle: myStyle.textStyleBold
						layout: Frame.LayoutHorizontal
						Para {
							width: 36
							text: detail.data(detail.currentIndex, "classes.name");
						}
						Cell {
							width: "%"
						}
						Cell {
							width: 12
							text: qsTr("Off")
						}
						Cell {
							width: 12
							text: qsTr("Ret")
						}
					}
					Band {
						id: runnersBand
						objectName: "runnersBand"
						keepFirst: 3
						keepWithPrev: true
						htmlExportAsTable: true
						modelData: {
							if(!root.created || detail.currentIndex < 0)
								return null;
							//console.info("detail.currentIndex:", detail.currentIndex)
							var stage_id = root.stageId;
							sqlModel.sqlQueryBuilder().clear()
								.select2('competitors', 'registration')
								.select2('runs', 'siid, isRunning, cardLent, cardReturned')
								.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
								.select("lentcards.siid IS NOT NULL AS cardInLentTable")
								.from('competitors')
								.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}}")
								.joinRestricted("runs.siid", "lentcards.siid", "NOT lentcards.ignored")
								.where("competitors.classId={{class_id}}")
								.where("runs.cardLent OR lentcards.siid IS NOT NULL")
								.orderBy('runs.cardreturned, runs.siid')//.limit(5);
							sqlModel.setQueryParameters({stage_id: stage_id, class_id: detail.data(detail.currentIndex, "classes.id")})
							sqlModel.reload();
							return sqlModel.toTreeTableData();
						}
						Detail {
							id: runnersDetail
							objectName: "runnersDetail"
							width: "%"
							layout: Frame.LayoutHorizontal
							Para {
								width: 18
								textFn: function() {return runnersDetail.rowData("runs.siid");}
							}
							Para {
								width: 18
								textFn: function() {return runnersDetail.rowData("registration");}
							}
							Cell {
								width: "%"
								text: runnersDetail.data(runnersDetail.currentIndex, "competitorName");
							}
							Cell {
								width: 12
								text: runnersDetail.data(runnersDetail.currentIndex, "isRunning")? "": qsTr("Y"); // off-race
							}
							Cell {
								width: 12
								text: runnersDetail.data(runnersDetail.currentIndex, "cardReturned")? qsTr("Y"): "";
							}
						}
					}
				}
			}
		}
	}
}


