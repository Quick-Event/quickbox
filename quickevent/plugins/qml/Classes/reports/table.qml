import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0

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
			Para {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: "ix"
			}
			Para {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: qsTr("id")
			}
			Para {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: qsTr("name")
			}
			Para {
				border: Pen {basedOn: "black1"}
				width: "%"
				text: qsTr("Course ID")
			}
		}
		Detail {
			id: detail
			width: "%"
			layout: Frame.LayoutHorizontal
			Para {
				width: "%"
				text: detail.currentIndex
			}
			Para {
				width: "%"
				text: detail.data(detail.currentIndex, "id")
			}
			Para {
				width: "%"
				text: detail.data(detail.currentIndex, "name")
			}
			Para {
				width: "%"
				text: detail.data(detail.currentIndex, "courseId")
			}
		}
	}
}


