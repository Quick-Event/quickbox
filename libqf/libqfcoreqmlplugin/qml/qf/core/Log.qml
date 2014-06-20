pragma Singleton
import QtQml 2.0
import qf.core 1.0

QtObject {
    function debug()
    {
        var args = Array.prototype.slice.call(arguments);
        var msg = args.join(' ');
        Log_helper.debug(msg);
    }
    function info()
    {
        var args = Array.prototype.slice.call(arguments);
        var msg = args.join(' ');
        Log_helper.info(msg);
    }
    function warning()
    {
        var args = Array.prototype.slice.call(arguments);
        var msg = args.join(' ');
        Log_helper.warning(msg);
    }
    function error()
    {
        var args = Array.prototype.slice.call(arguments);
        var msg = args.join(' ');
        Log_helper.error(msg);
    }
    function fatal()
    {
        var args = Array.prototype.slice.call(arguments);
        var msg = args.join(' ');
        Log_helper.fatal(msg);
    }
}
