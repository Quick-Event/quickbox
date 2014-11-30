import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0

Report {
	id: root
	objectName: "root"
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

	width: 210
	height: 297
	hinset: 5
	vinset: 5
	Frame {
		width: "%"
		height: "%"
		layout: Frame.LayoutStacked
		Frame {
			width: "%"
			height: "%"
			Frame {
				id: header
				layout: Frame.LayoutHorizontal
				bottomBorder: pen_black1
				Para {
					text: {
						"Quick Event 0.1";
					}
				}
				Para {
					width: "%"
				}
				Para {
					text: {
						"" + new Date();
					}
				}
			}
			Frame {
				width: "%"
				height: "%"
				Para {
					text: "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
				}
			}
			Frame {
				id: footer
				width: "%"
				layout: Frame.LayoutHorizontal
				topBorder: pen_black1
				Para {
					width: "%"
				}
				Para {
					text: {
						"Strana 1/15";
					}
				}
			}
		}
		Frame {
			width: "%"
			height: "%"
			vinset: 10
			Band {
				id: band
				width: "%"
				height: "%"
				Frame {
					objectName: "frame00"
					width: "%"
					Para {
						textFn: function() {return band.data("title");}
						textStyle: TextStyle {basedOn: "big"}
					}
				}
				Frame {
					width: "%"
					layout: Frame.LayoutHorizontal
					bottomBorder: Pen {basedOn: "black1"}
					HeaderCell {
						id: colId
						width: 10
						text: "n"
					}
					HeaderCell {
						id: colClass
						width: "%"
						text: "class"
					}
					HeaderCell {
						id: colSI
						width: "%"
						text: "SI"
					}
					HeaderCell {
						id: colName
						width: "50%"
						text: "name"
					}
				}
				Detail {
					id: detail
					width: "%"
					layout: Frame.LayoutHorizontal
					expandChildrenFrames: true
					function dataFn(field_name) {return function() {return rowData(field_name);}}
					Cell {
						width: colId.renderedWidth
						text: detail.currentIndex + 1
					}
					Cell {
						width: colClass.renderedWidth
						textFn: detail.dataFn("classes.name");
					}
					Cell {
						width: colSI.renderedWidth
						textFn: function() {return detail.rowData("siId");}
					}
					Cell {
						width: colName.renderedWidth
						text: detail.data(detail.currentIndex, "competitorName")
					}
				}
			}
		}
	}
}


