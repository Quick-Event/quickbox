// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
//import Components 1.0

Rectangle {
    id: cell
    property int dataIndex: -1

    property int textSpacing: 5
    property int textSize: 22
    property string textColor:"#f7f7df"
    property string textFont:"Arial"

    color: "yellow"

    state: ""
    states: [
        State {
            name: "category"
            PropertyChanges { target: category; visible: true}
            PropertyChanges { target: detail; visible: false}
        },
        State {
            name: "detail"
            PropertyChanges { target: category; visible: false}
            PropertyChanges { target: detail; visible: true}
        }
    ]
    /*
    Rectangle {
        id: empty
        color: detail.color
        border.width: 2
        border.color: color
        z: 1
        anchors.fill: parent
    }
    */
    Rectangle {
        id: category
        color: "#f14e0e"
        radius: 10
        visible: false
        border.width: 2
        border.color: color
        z: 2
        anchors.fill: parent

        Text {
            id: textClassName
            width: 137
            height: 80
            color: "#f7f7df"
            text: qsTr("H19N")
            anchors.leftMargin: cell.textSpacing
            horizontalAlignment: Text.AlignLeft
            font.bold: true
            font.family: cell.textFont
            anchors.left: parent.left
            verticalAlignment: Text.AlignVCenter
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            font.pixelSize: cell.textSize
        }

        Text {
            id: textClassDescription
            text: qsTr("Vydra Kamil")
            font.bold: false
            anchors.rightMargin: cell.textSpacing
            anchors.right: parent.right
            anchors.leftMargin: cell.textSpacing
            anchors.left: textClassName.right
            verticalAlignment: Text.AlignVCenter
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            font.pixelSize: cell.textSize
            color: cell.textColor
            font.family: cell.textFont
        }
        function loadData(data)
        {
            textClassName.text = data.classname;
            textClassDescription.text = data.length + "m";
			//if(cell.dataIndex == 0) console.log(cell.dataIndex + " data type: " + data.classname);
		}
    }
	Loader {
        id: detail
		anchors.fill: parent
		source: detailSourceForCurrentProfile();
    }
    function loadModelData(model) {
		var data = model.data(cell.dataIndex);
		if(dataIndex == 0) cell.state = "category"
		else cell.state = data.type
		//if(dataIndex == 0) console.log("load model data: " + dataIndex + " type: " + data.type + " state: " + cell.state);
		if(cell.state == "category") category.loadData(data[cell.state]);
		else if(cell.state == "detail") detail.item.loadData(data[cell.state]);
    }
	function detailSourceForCurrentProfile() {
		var profile = ctx_app.profile();
		return "./" + profile + "/Detail.qml";
	}
}
