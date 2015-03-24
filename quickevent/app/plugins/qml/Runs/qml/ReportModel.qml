import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

SqlTableModel {
	id: root
	ModelColumn {
		fieldName: 'id'
		readOnly: true
	}
	ModelColumn {
		fieldName: 'classes.name'
		caption: qsTr('class')
	}
	ModelColumn {
		fieldName: 'competitorName'
		caption: qsTr('Name')
	}
	ModelColumn {
		fieldName: 'registration'
		caption: qsTr('Reg')
	}
	ModelColumn {
		fieldName: 'siId'
		caption: qsTr('SI')
	}
	Component.onCompleted:
	{
		queryBuilder.select2('runs', '*')
			.select2('classes', 'name')
			.select("COALESCE(lastName, '') || ' ' || COALESCE(firstName, '') AS competitorName")
			.from('runs')
			.join("runs.classId", "classes.id")
			.orderBy('runs.id');//.limit(10);
	}
}
