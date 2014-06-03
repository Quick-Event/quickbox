import mywidgets 1.0

Frame {
    id: root
    layoutType: Frame.LayoutVertical
    Label {
        id: label
        text: "zeru hovna"
    }
    Label {
        id: lblLayout
        text: label.text + root.layoutType
    }
    Button {
        text: "sprc my mrdka"
    }
    Button {
        text: "mrd my mrdka"
    }
    Button {
        text: "set layout to " + ((root.layoutType === Frame.LayoutHorizontal)? "vertical": "horizontal")
        onClicked: {
            root.layoutType = (root.layoutType === Frame.LayoutHorizontal)? Frame.LayoutVertical: Frame.LayoutHorizontal
        }
    }
    Button {
        text: "chlastam mrdku"
        onClicked: {
            var s = label.text;
            label.text = text;
            text = s;
        }
    }
}
