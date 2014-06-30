pragma Singleton

import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

QfObject {
	QfObject {
		id: internal
		Component {
			id: inputDialogComponent
			InputDialog {}
		}
	}

	function getText(parent, title, label, text)
	{
		if(!parent)
			parent = FrameWork
		var dlg = inputDialogComponent.createObject(parent);
		dlg.windowTitle = title;
		dlg.labelText = label;
		dlg.textValue = text;
		//dialog.setTextEchoMode(mode);
		//dialog.setInputMethodHints(inputMethodHints);
		var ok = dlg.exec();
		dlg.destroy();
		var ret = (ok)? dlg.textValue: '';
		return ret;
	}
}
