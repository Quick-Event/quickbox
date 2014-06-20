pragma Singleton
import QtQml 2.0
import qf.core 1.0
// import qf.qmlwidgets 1.0

QtObject {
    function createWidget(class_name, widget_parent)
    {
        var o = Qt.createQmlObject("import qf.qmlwidgets 1.0; " + class_name + " {}", widget_parent);
        Log.debug("Create widget:", class_name, o);
        return o;
    }
}
