import QtQml 2.0
import qf.core 1.0
import qf.qmlreports 1.0

StyleSheet {
	objectName: "MyStyleSheet"
	//basedOn: "by_name"
	colors: [
		Color {name: "none"; def: "#00000000"},
		Color {name: "green"; def: "#FF00"},
        Color {id: colorKhaki; name: "khaki"; def: "khaki"}
	]
	pens: [
        Pen {
            name: "black1"
            color: Color {def:"black"}
            width: 1
        },
		Pen {name: "black1dash"; basedOn: "black1"; style: Pen.DashLine},
		Pen {
            name: "blue1dash"
            basedOn: "black1dash"
            color: Color {def:"blue"}
        }
	]
    brushes: [
        Brush {
            name: "khaki"
            color: colorKhaki
        }
    ]
    fonts: [
        Font {
            id: fontDefault
            pointSize: 19
        }

    ]
    textStyles: [
        TextStyle {
            name: "default"
            font: fontDefault
        }
    ]
}
