<?php

require_once('qsqlmon_proxy_config.php');

function logToFile($string)
{
    if (QSQLMON_PROXY_DEBUG) {
        $f = @fopen(QSQLMON_PROXY_DEBUG_FILE, 'a');
        @fwrite($f, $string);
        @fwrite($f, "\n");
        @fclose($f);
    }
}

/**
 * Fetch JSON data from POST and try to decode it
 */
$json_string_data = $rawdata = file_get_contents('php://input');
$json_data = json_decode($json_string_data, true);

// switch(json_last_error())
// {
//     case JSON_ERROR_DEPTH:
//         logToFile(' - Maximum stack depth exceeded');
//     break;
//     case JSON_ERROR_CTRL_CHAR:
//         logToFile(' - Unexpected control character found');
//     break;
//     case JSON_ERROR_SYNTAX:
//         logToFile(' - Syntax error, malformed JSON');
//     break;
//     case JSON_ERROR_NONE:
//         logToFile(' - No errors');
//     break;
// }


$conn_opts = $json_data["open"];
$mysql_server = QSQLMON_PROXY_DB_SERVER;
$mysql_port = QSQLMON_PROXY_DB_PORT;

$mysql_user = $conn_opts["user"];
$mysql_pass = $conn_opts["password"];

/**
 * connect to database using options from config INI file and authentication passed via JSON
 */

logToFile("REQUEST:");
logToFile($json_string_data);

$conn = @mysql_connect($mysql_server.":".$mysql_port, $mysql_user, $mysql_pass);
if (!$conn) {
    error_response(2, "Failed to connect to DB");
    die();
}

if (isset($json_data["query"])) {
    $mysql_db = $conn_opts["database"];
    $sel_db = @mysql_select_db($mysql_db, $conn);
    if (!$sel_db) {
        error_response(3, "Failed to select DB " . $mysql_db);
        die();
    }
    
    $result = @mysql_query($json_data["query"], $conn);
    if (!$result) {
        if ($json_data["query"] == "SHOW DATABASES;") {
            // fix when missing permission to list databases
            header('Content-type: application/json');
            forceShowDbResponse($conn_opts["database"]);
            die();
        } else {
            error_response(mysql_errno($conn), mysql_error($conn));
            die();
        }
    }
    $fieldsInfo = getFieldsInfo($result, $mysql_db);

    /**
     * in case of processing query the response is streamed out instead of preparing whole bunch of data in memory
     */
    logToFile("RESPONSE:");
    header('Content-type: application/json');
    $responseBegin = '{"resultset": {"fields": '.json_encode(getFieldsInfo($result, $mysql_db)).', "rows":[';
    echo $responseBegin;
    logToFile($responseBegin);
    streamResultRows($result);
    $responseEnd = ']}, "numRowsAffected": '.json_encode(mysql_affected_rows($conn)).', "lastInsertId": '.json_encode(mysql_insert_id($conn)).'}';
    echo $responseEnd;
    logToFile($responseEnd);

} else if (isset($json_data["close"])) {
    json_response(array("close" => 1));
} else {
    json_response(array("open" => 1));
}


logToFile("\n");
logToFile("================================================================\n");
logToFile("\n");


@mysql_close($conn);
die();


function getFieldsInfo($result, $mysql_db)
{
    $numFields = mysql_num_fields($result);
    $i = 0;
    $retval = array();
    while ($i < $numFields) {
        $fieldMeta = mysql_fetch_field($result, $i);
        $retval[] = array('name' => $mysql_db.".".$fieldMeta->table.".".$fieldMeta->name, 'type' => type2QtType($fieldMeta));
        $i++;
    }
    return $retval;
}


function streamResultRows($result)
{
    while ($row = mysql_fetch_array($result, MYSQL_NUM)) {
        $json_row = json_encode($row);
        if ($first) { $first = false; } else { $json_row = ",".$json_row; }
        
        echo $json_row;
        logToFile($json_row);
    }
    if ($response_db_if_empty && $first) {
        $json_row = json_encode( array($response_db_if_empty) );
        echo $json_row;
        logToFile($json_row);
    }
}


function json_response($data)
{
    header('Content-type: application/json');

    $response = json_encode($data);
    logToFile("RESPONSE: " . $response);
    echo $response;
}

function error_response($errno, $msg)
{
    json_response(array('exception' => array('errmsg' => $errno . ": " . $msg)));
}

function type2QtType($fieldMeta)
{
    logToFile('FIELD '.$fieldMeta->name.': '.$fieldMeta->type.' ('.$fieldMeta->blob.', '.$fieldMeta->zerofill.')');

    if ($fieldMeta->type == 'blob' && $fieldMeta->type != 1) {
        $type = 'string';
    } else {
        $type = $fieldMeta->type;
    }
    
    $map = array(
        'string' => 'QString',
        'blob' =>   'QByteArray',
        'date' =>   'QDate',
        'time' =>   'QTime',
        'datetime' =>   'QDateTime',
    );
    if (isset($map[$type])) {
        return $map[$type];
    } else {
        return $type;
    }
}

function forceShowDbResponse($db_name)
{
    echo '{"resultset": {"fields": [{"name":"'.$db_name.'.SCHEMATA.Database","type":"QString"}], "rows":[["'.$db_name.'"]]}, "numRowsAffected": 1, "lastInsertId": 0}';
}

?>
