// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import "../scripts.js" as JS

Rectangle {
	id: detail
	color: "#07092b"
	radius: 10
	z: 3
	border.width: 2
	border.color: "yellow"
	anchors.fill: parent

	Text {
		id: textStartTime
        width: 100
        color: "#1df50e"
		text: qsTr("99")
		horizontalAlignment: Text.AlignRight
		font.bold: false
		verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		font.family: cell.textFont
	}

	Text {
		id: textName
		text: qsTr("Vydra Kamil")
		anchors.left: textStartTime.right
		anchors.right: textReg.left
        anchors.leftMargin: cell.textSpacing
        anchors.rightMargin: cell.textSpacing
        verticalAlignment: Text.AlignVCenter
		anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		color: cell.textColor
		font.family: cell.textFont
	}
	Text {
		id: textReg
		width: 80
		text: qsTr("CHT7007")
		anchors.right: textIDSI.left
        anchors.rightMargin: cell.textSpacing
        verticalAlignment: Text.AlignVCenter
		anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		color: cell.textColor
		font.family: cell.textFont
	}
	Text {
		id: textIDSI
        width: 110
        //color: "gold"
		text: qsTr("DISK")
        anchors.rightMargin: cell.textSpacing
        horizontalAlignment: Text.AlignRight
        anchors.right: parent.right
		verticalAlignment: Text.AlignVCenter
		anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		font.family: cell.textFont
	}
    function loadData(data)
	{
        //var data = _data[_data.type]
		var st_time = JS.secToOBTime(data.start);
		textStartTime.text = st_time;
		textName.text = data.name;
		textReg.text = data.reg;
        var idsi = data.idsi;
        //console.log("idsi: " + idsi);
        if(idsi < 200000 && idsi > 100000) idsi = idsi % 100000;
        if(idsi == 0) {
            textIDSI.text = "------";
            textIDSI.color = "red";
        }
        else {
            textIDSI.text = idsi;
            textIDSI.color = "gold";
        }
	}
}
