//import qf.core 1.0
import qf.qmlwidgets 1.0

Frame {
	id: root
	layoutType: Frame.LayoutVertical
	Label {
		id: label
		text: "ping"
	}
	Label {
		id: lblLayout
		text: label.text + root.layoutType
	}
	Button {
		text: "set layout to " + ((root.layoutType === Frame.LayoutHorizontal)? "vertical": "horizontal")
		onClicked: {
			root.layoutType = (root.layoutType === Frame.LayoutHorizontal)? Frame.LayoutVertical: Frame.LayoutHorizontal
		}
	}
	Button {
		text: "pong"
		onClicked: {
			var s = label.text;
			label.text = text;
			text = s;
		}
	}
}
