import QtQml 2.0
import qf.core 1.0

QtObject
{
	property string name
    property string table: ''
    property var fields: []
    property string onUpdate: 'RESTRICT' // 'NO ACTION', 'CASCADE', 'SET NULL', 'SET DEFAULT'
    property string onDelete: 'RESTRICT' // 'NO ACTION', 'CASCADE', 'SET NULL', 'SET DEFAULT'
}
