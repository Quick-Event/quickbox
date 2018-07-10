import QtQml 2.0
import qf.core 1.0
import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0

Report {
	id: root
	objectName: "root"

	property var eventPlugin

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
		objectName: "frame"
		border: Pen {basedOn: "black1"}
		width: "%"
		height: "%"
		columns: "%,%"
		Frame {
			height: 10
			id: runnersDetail
			objectName: "paraFrame"
			border: Pen {
				color: Color {def:Qt.magenta}
				width: 0.5
			}
			width: "%"
			layout: Frame.LayoutHorizontal
			Cell {
				border: Pen {
					color: Color {def:Qt.green}
					width: 0.5
				}
				width: 40
				text: "fixed1"
			}
			Cell {
				border: Pen {
					color: Color {def:Qt.red}
					width: 0.5
				}
				width: "%"
				text: "text1 text2 text3 text4 text5 text6 text7 text8 text9"
			}
			Cell {
				border: Pen {
					color: Color {def:Qt.green}
					width: 0.5
				}
				width: 40
				text: "fixed2"
			}
		}
		/*
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
		*/
	}
}


