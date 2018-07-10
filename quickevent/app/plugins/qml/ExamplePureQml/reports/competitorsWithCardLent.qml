import QtQml 2.0
import qf.core 1.0
import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0

Report {
	id: root
	objectName: "root"

	property string reportTitle: qsTr("Competitors with card lent in stage %1").arg(eventPlugin? eventPlugin.currentStageId: "?");
	property var eventPlugin

	property SqlTableModel sqlModel: SqlTableModel {}

	//debugLevel: 1
	styleSheet: StyleSheet {
		basedOn: ReportStyleCommon { id: myStyle }
	}
	textStyle: myStyle.textStyleDefault

	width: 210
	height: 297
	hinset: 5
	vinset: 5
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
			columns: "%,%"
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
					//expandChildrenFrames: true
					Frame {
						fill: Brush {color: Color {def: "khaki"} }
						textStyle: myStyle.textStyleBold
						layout: Frame.LayoutHorizontal
						Cell {
							width: 18
							text: detail.data(detail.currentIndex, "classes.name");
						}
						Cell {
							width: "%"
						}
						Cell {
							width: 18
							text: qsTr("Lent")
						}
						Cell {
							width: 18
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
							console.info("detail.currentIndex:", detail.currentIndex)
							var stage_id = eventPlugin.currentStageId;
							sqlModel.sqlQueryBuilder().clear()
								.select2('competitors', 'registration')
								.select2('runs', 'cardLent, cardReturned')
								.select("COALESCE(competitors.lastName, '') || ' ' || COALESCE(competitors.firstName, '') AS competitorName")
								.from('competitors')
								.joinRestricted("competitors.id", "runs.competitorId", "runs.stageId={{stage_id}}")
								.where("competitors.classId={{class_id}}")
								.where("runs.cardLent")
								.orderBy('competitors.registration')//.limit(5);
							sqlModel.setQueryParameters({stage_id: stage_id, class_id: detail.data(detail.currentIndex, "classes.id")})
							sqlModel.reload();
							return sqlModel.toTreeTableData();
						}
						Detail {
							id: runnersDetail
							objectName: "runnersDetail"
							width: "%"
							layout: Frame.LayoutHorizontal
							Cell {
								width: 18
								textFn: function() {return runnersDetail.rowData("registration");}
							}
							Cell {
								width: "%"
								text: runnersDetail.data(runnersDetail.currentIndex, "competitorName");
							}
							Cell {
								width: 18
								text: runnersDetail.data(runnersDetail.currentIndex, "cardLent")? "Y": "N";
							}
							Cell {
								width: 18
								text: runnersDetail.data(runnersDetail.currentIndex, "cardReturned")? "Y": "N";
							}
						}
					}
				}
			}
		}
	}
}


