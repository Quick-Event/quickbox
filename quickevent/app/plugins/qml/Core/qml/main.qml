import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Core 1.0

CorePlugin {
	id: root
	property QtObject api: QtObject
	{
		property Crypt crypt: Crypt
		{
			Component.onCompleted: {
				initGenerator(16808, 11, 2147483647);
			}
		}

		function createSettings()
		{
			// it is better to create and destroy new Settings object for each particular use
			// because it handles situation when one doesn't endGroup properly (groups can be nested)
			return settingsComponent.createObject(null);
		}

		function downloadContent(url, callback)
		{
			Log.info("http get request:", url);
			var reply = networkAccessManager.get(url);
			reply.downloadProgress.connect(FrameWork.showProgress);
			reply.finished.connect(function(get_ok) {
				Log.info("http get finished:", get_ok, reply.url);
				if(get_ok) {
					callback(true, reply.textData);
				}
				else {
					console.error("http get error:", reply.errorString, 'on:', reply.url);
					callback(false, reply.errorString);
				}
				reply.destroy();
			});
		}
	}

	property QfObject internals: QfObject
	{
		Component {
			id: settingsComponent
			Settings {}
		}
		Component {
			id: cLangAction
			Action {
				property string langName
				property string langAbbr
				checkable: true
				text: langName
				function changeLanguage()
				{
					MessageBoxSingleton.information(null, qsTr("Language change to '%1' will be applied after application restart.").arg(langName));
					checked = true;
					var settings = api.createSettings();
					settings.setValue(FrameWork.settingsPrefix_application_locale_language(), langAbbr);
					settings.destroy();
				}
			}
		}
		NetworkAccessManager {
			id: networkAccessManager
		}
		ActionGroup {
			id: actGroupLanguages
			exclusive: true
		}
		property list<Action> actions: [
			Action {
				id: actQuit
				oid: 'quit'
				text: qsTr('&Quit')
				onTriggered: {
					Log.info(text, "triggered");
					Qt.quit();
				}
			},
			Action {
				id: actLaunchSqlTool
				//oid: 'quit'
				text: qsTr('&SQL tool')
				onTriggered: {
					Log.info(text, "triggered");
					root.launchSqlTool();
				}
			},
			Action {
				id: actAboutQuickEvent
				//oid: 'quit'
				text: qsTr('&About Quick event')
				onTriggered: {
					root.aboutQuickEvent();
				}
			},
			Action {
				id: actAboutQt
				//oid: 'quit'
				text: qsTr('About &Qt')
				onTriggered: {
					root.aboutQt();
				}
			}
		]

	}

	onInstalled:
	{
		Log.debug("debug log test, use console.debug() for qml file and line information");
		Log.info("info log test, use console.info() for qml file and line information");
		Log.warning("warn log test, use console.warn() for qml file and line information");
		//Log.error("error log test, use console.error() for qml file and line information");
		console.debug("Core log test");
		console.info("Core log test");
		console.warn("Core log test");
		//console.error("Core log test");
		console.debug("Core plugin installed");

		console.debug(FrameWork.menuBar);
		var act_file = FrameWork.menuBar.actionForPath('file');
		act_file.text = qsTr('&File');
		var act_file_import = act_file.addMenuInto("import", qsTr("&Import"));
		act_file.addActionInto(act_file_import);
		act_file.addSeparatorInto();
		act_file.addActionInto(actQuit);

		var act_tools = FrameWork.menuBar.actionForPath('tools');
		act_tools.text = qsTr('&Tools');
		act_tools.addActionInto(actLaunchSqlTool);

		var settings = api.createSettings();

		var curr_lang = settings.value(FrameWork.settingsPrefix_application_locale_language(), "system");

		var act_tools_locale = act_tools.addMenuInto('locale', qsTr('&Locale'));
		var act_tools_locale_language = act_tools_locale.addMenuInto('language', qsTr('&Language'));
		var languages = [
			[qsTr("System"), "system"],
			[qsTr("Czech"), "cs_CZ"],
            [qsTr("English"), "en_US"],
            [qsTr("Norwegian"), "nb_NO"],
            [qsTr("Polish"), "pl_PL"],
            [qsTr("Russian"), "ru_RU"]
		]
		for(var i=0; i<languages.length; i++) {
			var lang = languages[i];
			var act = cLangAction.createObject(root.internals, {"langName": lang[0], "langAbbr": lang[1]});
			act.checked = (lang[1] == curr_lang);
			act.triggered.connect(act.changeLanguage);
			actGroupLanguages.addAction(act);
			act_tools_locale_language.addActionInto(act);
		}

		settings.destroy();

		FrameWork.menuBar.actionForPath('view').text = qsTr('&View');
		FrameWork.menuBar.actionForPath('view/toolbar').text = qsTr('&Toolbar');

		var act_help = FrameWork.menuBar.actionForPath('help');
		act_help.text = qsTr('&Help');
		act_help.addActionInto(actAboutQuickEvent);
		act_help.addActionInto(actAboutQt);
	}

}
