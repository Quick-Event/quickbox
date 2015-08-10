import QtQml 2.0
import "qrc:/qf/core/qml/js/stringext.js" as StringExt

QtObject
{
	property string metaTypeName: metaTypeNameFn()
	function createSqlType(options)
	{
		return '';
	}

	function metaTypeNameFn()
	{
		return "Invalid";
	}
}
