import QtQml 2.0
import qf.core 1.0
import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/core/js/ogtime.js" as OGTime

Report {
	id: root

	property int stageCount: 1
	property string reportTitle: qsTr("Start list by classes")
	property bool isBreakAfterEachClass: false
	property bool isColumnBreak: false
	property bool isPrintStartNumbers: false
	property var options

	property QfObject internals: QfObject {
		Component {
			id: cHeaderCell
			Cell {
				textStyle: myStyle.textStyleBold
			}
		}
		Component {
			id: cStartTimeCell
			Cell {
				property string fieldName
				property string isRunning
				textFn: function() {
					var run = runnersDetail.data(runnersDetail.currentIndex, isRunning);
					if (run)
						return OGTime.msecToString_mmss(runnersDetail.data(runnersDetail.currentIndex, fieldName));
					else
						return "-";
				}
			}
		}
	}

	//debugLevel: 1
	styleSheet: StyleSheet {
		objectName: "portraitStyleSheet"
		basedOn: ReportStyleCommon { id: myStyle }
		colors: [
		]
		pens: [
			Pen {name: "red1dot"
				basedOn: "black1"
				color: Color {def:"red"}
				style: Pen.DotLine
			},
			Pen {
				id: pen_black1
				basedOn: "black1"
			}
		]
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
			Band {
				id: band
				objectName: "band"
				width: "%"
				height: "%"
				QuickEventReportHeader {
					dataBand: band
					reportTitle: root.reportTitle
					showStageNumber: false
				}
				Detail {
					id: detail
					objectName: "detail"
					width: "%"
					//keepAll: true
					layout: Frame.LayoutVertical
					function dataFn(field_name) {return function() {return rowData(field_name);}}
					Space { height: 5 }
					Break {
						breakType: root.isColumnBreak? Break.Column: Break.Page;
						visible: root.isBreakAfterEachClass;
						skipFirst: true
					}
					Frame {
						id: classHeader
						width: "%"
						layout: Frame.LayoutHorizontal
						fill: Brush {color: Color {def: "khaki"} }
						Cell {
							width: "%"
							textFn: detail.dataFn("classes.name");
							textStyle: myStyle.textStyleBold
						}
						Cell {
							visible: root.isPrintStartNumbers
							width: 16
							textStyle: myStyle.textStyleBold
							halign: Frame.AlignRight
							text: qsTr("Bib");
						}
						Cell {
							id: hdrRegistration
							width: 25
							textStyle: myStyle.textStyleBold
							text: qsTr("Registration");
						}
						Cell {
							id: hdrSI
							width: 18
							textStyle: myStyle.textStyleBold
							halign: Frame.AlignRight
							text: qsTr("SI");
						}
						Component.onCompleted: {
							//console.warn("=============", root.stageCount)
							for(var i=0; i<root.stageCount; i++) {
								var runs_table = "runs" + (i+1);
								var c = cHeaderCell.createObject(null, {"width": 15, "text": qsTr("Stage") + (i+1)});
								classHeader.addItem(c);
							}
						}
					}
					//expandChildFrames: true
					Band {
						id: runnersBand
						objectName: "runnersBand"
						keepFirst: 3
						keepWithPrev: true
						htmlExportAsTable: true
						Detail {
							id: runnersDetail
							objectName: "runnersDetail"
							width: "%"
							layout: Frame.LayoutHorizontal
							function dataFn(field_name) {return function() {return rowData(field_name);}}
							Cell {
								width: "%"
								textFn: runnersDetail.dataFn("competitorName");
							}
							Cell {
								visible: root.isPrintStartNumbers
								width: 16
								halign: Frame.AlignRight
								textFn: runnersDetail.dataFn("startNumber");
							}
							Cell {
								width: hdrRegistration.width
								textFn: runnersDetail.dataFn("registration");
							}
							Cell {
								width: hdrSI.width
								halign: Frame.AlignRight
								textFn: runnersDetail.dataFn("competitors.siId");
							}
							Component.onCompleted: {
								//console.warn("=============", root.stageCount)
								for(var i=0; i<root.stageCount; i++) {
									var runs_table = "runs" + (i+1);
									var c = cStartTimeCell.createObject(null, {"width": 15, "halign": Frame.AlignRight, "fieldName": runs_table + ".startTimeMs", "isRunning": runs_table + ".isRunning" });
									runnersDetail.addItem(c);
								}
							}
						}
					}
				}
			}
		}
	}
}


