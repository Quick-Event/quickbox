import qf.qmlreports 1.0
import shared.qml 1.0

Report {
	id: root
	objectName: "root"
	//debugLevel: 1
	styleSheet: StyleSheet {
		basedOn: ReportStyleCommon { id: myStyle }
	}
	textStyle: myStyle.textStyleDefault
	width: 210
	height: 297
	vinset: 10
	hinset: 5
	//layout: Frame.LayoutVertical
	Frame {
		objectName: "frame00"
		width: "%"
		Para {
			text: "Title"
			textStyle: TextStyle {basedOn: "big"}
		}
	}
	Band {
		width: "%"
		height: "%"
		Frame {
			width: "%"
			layout: Frame.LayoutHorizontal
			fill: Brush {color: Color { def: "lightgray" }}
			HeaderCell {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: "ix"
			}
			HeaderCell {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: qsTr("id")
			}
			HeaderCell {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: qsTr("name")
			}
			HeaderCell {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: qsTr("Course ID")
			}
		}
		Detail {
			id: detail
			width: "%"
			layout: Frame.LayoutHorizontal
			Cell {
				width: "%"
				text: detail.currentIndex
			}
			Cell {
				width: "%"
				text: detail.data(detail.currentIndex, "id")
			}
			Cell {
				width: "%"
				text: detail.data(detail.currentIndex, "name")
			}
			Cell {
				width: "%"
				text: detail.data(detail.currentIndex, "classes.courseId")
			}
		}
	}
}


