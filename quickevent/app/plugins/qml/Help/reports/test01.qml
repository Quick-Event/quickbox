import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0

Report {
	id: root
	objectName: "root"
	debugLevel: 1
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
	Frame {
		objectName: "frame00"
		width: "%"
//		height: "%"
		Para {
			text: "frame00"
		}
	}
	Frame {
		objectName: "frame"
		width: "%"
		height: "%"
		textStyle: TextStyle {basedOn: "big"}
		border: Pen {
			basedOn: "red1dot"
			color: Color {def: Qt.green}
		}
		//fill: Brush {name: "br1"; color: Color {def:Qt.blue}}
		Para {
			height: "%"
			text: "Čí ruka držela pistoli, jejíž kulka zabila doktora Johna Christowa? Další případ slavného belgického detektiva. Britský film, natočený podle románu Agathy Christie (2004). Hrají: D. Suchet, J. Cake, S. Milesová, E. Fox, L. Anthonyová a další. Režie Simon LangtonUž dlouho si Hercule Poirot přál strávit klidný víkend ve svém novém venkovském domku, tak půvabně zasazeném do anglické krajiny. A zdá se, že tentokrát se mu konečně jeho přání splní. A nejen to, bude mít dokonce příležitost těšit se z příjemné společnosti, která se schází v nedalekém sídle lady Angkatellové. Poirot si tu vybranou večeři vskutku vychutnává, netuší ovšem, když se vrací do své samoty, jak brzy jej sem jeho kroky zavedou znovu. Neboť hosté lady Lucy se druhý den - pochopitelně zcela neúmyslně -  rozdělí na dvě skupiny: na oběť a na podezřelé. A bez Poirotovy pomoci inspektor Grange těžko vypátrá, čí ruka držela pistoli, jejíž kulka zabila doktora Johna Christowa."
			fill: Brush { color: Color {def: "pink"}}
		}
	}
	Frame {
		objectName: "frame--"
		width: "%"
		Para {
			text: "frame==================="
		}
	}
	Para {
		width: "%"
		height: "%"
		text: "ČÍ RUKA DRŽELA PISTOLI, JEJÍŽ KULKA ZABILA DOKTORA JOHNA CHRISTOWA? DALŠÍ PŘÍPAD SLAVNÉHO BELGICKÉHO DETEKTIVA. BRITSKÝ FILM, NATOČENÝ PODLE ROMÁNU AGATHY CHRISTIE (2004). HRAJÍ: D. SUCHET, J. CAKE, S. MILESOVÁ, E. FOX, L. ANTHONYOVÁ A DALŠÍ. REŽIE SIMON LANGTONUŽ DLOUHO SI HERCULE POIROT PŘÁL STRÁVIT KLIDNÝ VÍKEND VE SVÉM NOVÉM VENKOVSKÉM DOMKU, TAK PŮVABNĚ ZASAZENÉM DO ANGLICKÉ KRAJINY. A ZDÁ SE, ŽE TENTOKRÁT SE MU KONEČNĚ JEHO PŘÁNÍ SPLNÍ. A NEJEN TO, BUDE MÍT DOKONCE PŘÍLEŽITOST TĚŠIT SE Z PŘÍJEMNÉ SPOLEČNOSTI, KTERÁ SE SCHÁZÍ V NEDALEKÉM SÍDLE LADY ANGKATELLOVÉ. POIROT SI TU VYBRANOU VEČEŘI VSKUTKU VYCHUTNÁVÁ, NETUŠÍ OVŠEM, KDYŽ SE VRACÍ DO SVÉ SAMOTY, JAK BRZY JEJ SEM JEHO KROKY ZAVEDOU ZNOVU. NEBOŤ HOSTÉ LADY LUCY SE DRUHÝ DEN - POCHOPITELNĚ ZCELA NEÚMYSLNĚ -  ROZDĚLÍ NA DVĚ SKUPINY: NA OBĚŤ A NA PODEZŘELÉ. A BEZ POIROTOVY POMOCI INSPEKTOR GRANGE TĚŽKO VYPÁTRÁ, ČÍ RUKA DRŽELA PISTOLI, JEJÍŽ KULKA ZABILA DOKTORA JOHNA CHRISTOWA."
		fill: Brush { color: Color {def: "khaki"}}
	}
	Frame {
		objectName: "frame99"
		width: "%"
		Para {
			text: "frame99"
		}
	}
}

