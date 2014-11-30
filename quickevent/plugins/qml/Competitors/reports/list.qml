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
			}
		]
	}
	textStyle: myStyle.textStyleDefault
	width: 210
	height: 297
	vinset: 10
	hinset: 5
	//layout: Frame.LayoutVertical
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


