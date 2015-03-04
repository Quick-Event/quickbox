import QtQml 2.0
import qf.core 1.0

SqlDataDocument {
	id: dataDocument
	Component.onCompleted:
	{
		queryBuilder
			.select2('classes', '*')
			.from('classes').where('classes.id={{ID}}');
	}
}