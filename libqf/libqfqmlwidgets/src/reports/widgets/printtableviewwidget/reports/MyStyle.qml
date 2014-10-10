import QtQml 2.0
import qf.core 1.0
import qf.qmlreports.Style 1.0

Sheet {
	objectName: "MyStyleSheet"
	//basedOn: "by_name"
	colors: [
		Color {name: "none"; definition: "#00000000"},
		Color {name: "green"; definition: "#FF00"},
		Color {name: "khaki"; definition: "khaki"}
	]
	pens: [
        Pen {
            name: "black1"
            color: Color {definition:"black"}
            width: 1
        },
		Pen {name: "black1dash"; basedOn: "black1"; style: Pen.DashLine},
		Pen {
            name: "blue1dash"
            basedOn: "black1dash"
            color: Color {definition:"blue"}
        }
	]
}
