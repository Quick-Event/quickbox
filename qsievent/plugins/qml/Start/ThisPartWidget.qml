import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

PartWidget
{
	id: root

	title: "Start"

	Frame {
		TableView {
			id: table
			model: SqlQueryTableModel {
				id: model
				Component.onCompleted:
				{
					queryBuilder.select2('runners', '*').from('runners').orderBy('surname');
				}
			}
		}
	}

	Component.onCompleted:
	{
	}

	function canActivate(active_on)
	{
		Log.info(title, "canActivate:", active_on);
		if(active_on) {
			model.reload();
		}
		return true;
	}

}
