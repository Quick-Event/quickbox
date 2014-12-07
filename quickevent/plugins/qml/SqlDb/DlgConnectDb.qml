import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0

Dialog {
	id: root

	property alias connectionType: cbxConnectionType.currentData

	windowTitle: qsTr("Connect to database")
	persistentSettingsId: "DlgConnectDb"
	property Plugin coreFeature: FrameWork.plugin("Core")

	onVisibleChanged: {
		frmAllOptions.raiseCurrentOptionFrame();
	}

	Frame {
		Frame {
			layoutType: Frame.LayoutHorizontal
			Label {
				text: qsTr("Connection type")
			}
			ComboBox {
				id: cbxConnectionType
				onCurrentIndexChanged: {
					frmAllOptions.raiseCurrentOptionFrame();
				}
				Component.onCompleted: {
					setItems([
						[qsTr("SQL server"), "sqlServer"],
						[qsTr("Single file"), "singleFile"]
					])
					//console.warn("cbxConnectionType completed")
				}
			}
		}
		Frame {
			id: frmAllOptions
			layoutType: Frame.LayoutHorizontal

			function raiseCurrentOptionFrame()
			{
				var type = cbxConnectionType.currentData;
				if(type == 'sqlServer') {
					frmServerOptions.visible = true;
					frmSingleFileOptions.visible = false;
				}
				else if(type == 'singleFile') {
					frmServerOptions.visible = false;
					frmSingleFileOptions.visible = true;
				}
			}

			Frame {
				id: frmServerOptions
				objectName: "serverOptions"
				layoutType: Frame.LayoutForm
				LineEdit {
					id: edServerHost
					Layout.buddyText: qsTr("&Host")
					text: "localhost"
				}
				LineEdit {
					id: edServerUser
					Layout.buddyText: qsTr("&User")
				}
				LineEdit {
					id: edServerPassword
					Layout.buddyText: qsTr("&Password")
					echoMode: LineEdit.Password
				}
				LineEdit {
					id: edServerEvent
					Layout.buddyText: qsTr("&Event")
					placeholderText: qsTr("Event name can be specified later on in File/Open")
				}
			}
			Frame {
				id: frmSingleFileOptions
				objectName: "singleFileOptions"
				layoutType: Frame.LayoutForm
				Frame {
					layoutType: Frame.LayoutHorizontal
					Layout.buddyText: qsTr("&Working directory")
					LineEdit {
						id: edSingleWorkingDir
						placeholderText: qsTr("Directory where event files are placed.")
					}
					Button {
						text: "..."
						onClicked: {
							var wd = InputDialogSingleton.getExistingDirectory(root, qsTr("Select working directory"), edSingleWorkingDir.text);
							if(wd)
								edSingleWorkingDir.text = wd;
						}
					}
				}
				LineEdit {
					id: edSingleEvent
					Layout.buddyText: qsTr("&Event")
					placeholderText: qsTr("Event name can be specified later on in File/Open")
				}
			}
		}
	}

	buttonBox: ButtonBox {}

	Component.onCompleted: {
		//gc();
	}

	function loadSettings()
	{
		var settings = coreFeature.api.createSettings();
		settings.beginGroup("sql");
		var type = settings.value("connectionType", "singleFile");
		//console.warn("setting connection type to:", type);
		cbxConnectionType.currentData = type;
		settings.beginGroup("server");
		edServerHost.text = settings.value("host", "localhost");
		edServerUser.text = settings.value("user", "");
		edServerPassword.text = coreFeature.api.crypt.decrypt(settings.value("password", ""));
		edServerEvent.text = settings.value("event", "");
		settings.endGroup();
		settings.beginGroup("singleFile");
		edSingleWorkingDir.text = settings.value("workingDir", "");
		edSingleEvent.text = settings.value("event", "");
		settings.endGroup();
		settings.destroy();
	}

	function saveSettings()
	{
		var settings = coreFeature.api.createSettings();
		settings.beginGroup("sql");
		settings.setValue("connectionType", cbxConnectionType.currentData);
		settings.beginGroup("sqlServer");
		settings.setValue("host", edServerHost.text);
		settings.setValue("user", edServerUser.text);
		settings.setValue("password", coreFeature.api.crypt.encrypt(edServerPassword.text));
		settings.setValue("event", edServerEvent.text);
		settings.endGroup();
		settings.beginGroup("singleFile");
		settings.setValue("workingDir", edSingleWorkingDir.text);
		settings.setValue("event", edSingleEvent.text);
		settings.endGroup();
		settings.destroy();
	}

	function doneRequest_qml(result)
	{
		var ret = true;
		if(result === Dialog.ResultAccept) {
			if(root.connectionType == "singleFile") {
				if(!edSingleWorkingDir.text) {
					MessageBoxSingleton.critical(root, qsTr("Invalid working directory"));
					ret = false;
				}
			}
			else if(root.connectionType == "sqlServer") {
				if(!edServerUser.text) {
					MessageBoxSingleton.critical(root, qsTr("user is empty"));
					ret = false;
				}
			}
			else {
				MessageBoxSingleton.critical(root, qsTr("Unknown connection type: '%1'").arg(root.connectionType));
				ret = false;
			}
			if(ret) {
				saveSettings();
			}
		}
		return ret;
	}

	function loadSettingsAndExec()
	{
		loadSettings();
		return exec();
	}
}
