import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0

Report {
	id: root
	objectName: "report"
	debugLevel: 1
	width: 210
	height: 297
	vinset: 10
	hinset: 5
	styleSheet: StyleSheet {
		id: myStyle
	}
	textStyle: myStyle.textStyleDefault
	Frame {
		objectName: "stack"
		width: "%"
		height: "%"
		layout: Frame.LayoutStacked
		Para {
			objectName: "para_rubber"
			width: "%"
			textVAlign: Para.AlignVCenter
			text: "FFFFFFFFFF 001"
		}
		Para {
			objectName: "para_expanding"
			textVAlign: Para.AlignVCenter
			width: "%"
			height: "%"
			text: "FFFFFFFFFF 002"
		}
	}
	/*
	Frame {
		objectName: "frame00"
		width: "%"
		Para {
			text: "frame00"
		}
	}
	Frame {
		objectName: "frame"
		width: "%"
		height: "%"
		Para {
			height: "%"
			text: "Čí ruka držela pistoli, jejíž kulka zabila doktora Johna Christowa? Další případ slavného belgického detektiva. Britský film, natočený podle románu Agathy Christie (2004). Hrají: D. Suchet, J. Cake, S. Milesová, E. Fox, L. Anthonyová a další. Režie Simon LangtonUž dlouho si Hercule Poirot přál strávit klidný víkend ve svém novém venkovském domku, tak půvabně zasazeném do anglické krajiny. A zdá se, že tentokrát se mu konečně jeho přání splní. A nejen to, bude mít dokonce příležitost těšit se z příjemné společnosti, která se schází v nedalekém sídle lady Angkatellové. Poirot si tu vybranou večeři vskutku vychutnává, netuší ovšem, když se vrací do své samoty, jak brzy jej sem jeho kroky zavedou znovu. Neboť hosté lady Lucy se druhý den - pochopitelně zcela neúmyslně -  rozdělí na dvě skupiny: na oběť a na podezřelé. A bez Poirotovy pomoci inspektor Grange těžko vypátrá, čí ruka držela pistoli, jejíž kulka zabila doktora Johna Christowa."
		}
	}
	Frame {
		objectName: "frame99"
		width: "%"
		Para {
			text: "frame99"
		}
	}
	*/
}

