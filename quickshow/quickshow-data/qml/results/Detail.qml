import QtQuick 2.0
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
		id: textPos
		width: 40
		height: 80
		color: cell.textColor
		text: qsTr("99")
		horizontalAlignment: Text.AlignRight
		font.bold: false
		anchors.leftMargin: 4
		anchors.left: parent.left
		verticalAlignment: Text.AlignVCenter
		anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		font.family: cell.textFont
	}

	Text {
		id: textName
		text: qsTr("Vydra Kamil")
		anchors.left: textPos.right
		anchors.right: textReg.left
		anchors.leftMargin: cell.textSpacing
		verticalAlignment: Text.AlignVCenter
		anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		color: cell.textColor
		font.family: cell.textFont
	}
	Text {
		id: textReg
		width: 60
		text: qsTr("CHT7007")
		anchors.right: textTime.left
		anchors.rightMargin: cell.textSpacing
		verticalAlignment: Text.AlignVCenter
		anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		color: cell.textColor
		font.family: cell.textFont
	}
	Text {
		id: textTime
		width: 100
		text: qsTr("123.55")
		anchors.right: textStatus.left
		anchors.rightMargin: cell.textSpacing
		horizontalAlignment: Text.AlignRight
		verticalAlignment: Text.AlignVCenter
		anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		color: cell.textColor
		font.family: cell.textFont
	}
	Text {
		id: textStatus
		width: 60
		text: qsTr("DISK")
		anchors.rightMargin: cell.textSpacing
		anchors.right: parent.right
		verticalAlignment: Text.AlignVCenter
		anchors.bottom: parent.bottom
		anchors.top: parent.top
		font.pixelSize: cell.textSize
		color: cell.textColor
		font.family: cell.textFont
	}
	function loadData(data)
	{
		//console.info(JSON.stringify(data))
		var ok = !data.runs__disqualified;
		textPos.text = (ok)? data.pos + ".": "";
		textName.text = data.competitorname;
		textReg.text = data.competitors__registration;
		textTime.text = JS.msecToOBTime(data.runs__timems);
		textStatus.text = (ok)? "": "DISK";
	}
}
