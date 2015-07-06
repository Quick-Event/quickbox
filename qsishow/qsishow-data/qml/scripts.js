function secToOBTime(sec) 
{
    var m = Math.floor(sec / 60);
    var s = sec % 60;
    return m + "." + (s<10? "0"+s: s);
}


