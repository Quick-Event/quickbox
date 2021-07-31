# QuickHTML

Generator of HTML exports for [QuickEvent/QuickBox](https://github.com/Quick-Event/quickbox).

### Features

  * Class results
  * Class start lists
  * Class overall results (for multi-stage events)

### Description

QuickHTML is meant as an addon for [QuickEvent/QuickBox](https://github.com/Quick-Event/quickbox). It generates various HTML reports
from QuickBox database (PostgreSQL or SQLite).

Typical use is to generate HTML reports for orienteering event which are then published by separate WWW server
(NGiNX, Apache, ...)

Program is written in Python and uses Jinja templates for HTML pages layout definition. Only simple, Czech language
templates are provided so far.

### Dependencies

  * Python 3
  * Python module jinja2
  * Python module psycopg2 (if using PostgreSQL DB)


### Usage

See help (-h):
```
usage: quickhtml.py [-h] [--sql-driver {psql,sqlite}] [-s SQL_SERVER] [--sql-port SQL_PORT] [-u USER] [-p PASSWORD] [-b SQL_DATABASE] [-n STAGE] [-m {r,s,t,a}] [--main-index] [-d HTML_DIR]
                    [-r REFRESH_INTERVAL] [-l LOG_FILE] [-v] [-q]
                    event

positional arguments:
  event                 Event - name of DB schema (psql) or filename (sqlite)

optional arguments:
  -h, --help            show this help message and exit
  --sql-driver {psql,sqlite}
                        SQL database to connect (psql|sqlite) [psql]
  -s SQL_SERVER, --sql-server SQL_SERVER
                        Server to connect [localhost]
  --sql-port SQL_PORT   TCP port to connect [5432]
  -u USER, --user USER  User for DB connection [quickevent]
  -p PASSWORD, --password PASSWORD
                        Password for DB connection [None]
  -b SQL_DATABASE, --sql-database SQL_DATABASE
                        Database name [quickevent]
  -n STAGE, --stage STAGE
                        Stage number [1]
  -m {r,s,t,a}, --mode {r,s,t,a}
                        Output mode {results|starts|total|all} [r,s]
  --main-index          Create main index file [Automatically on in 'all' mode]
  -d HTML_DIR, --html-dir HTML_DIR
                        Directory where HTML pages will be stored [./html]
  -r REFRESH_INTERVAL, --refresh-interval REFRESH_INTERVAL
                        Refresh time interval in seconds [60]
                        0 = generate once and exit
  -t TEMPLATES, --templates TEMPLATES
                        Templates folder [./templates]
  -l LOG_FILE, --log-file LOG_FILE
                        Log activity into file
  -v, --verbose         More information
  -q, --quiet           Less information
```

#### Examples

`./quickhtml.py sudety2021` ... generates results and startlist for current stage once a minute into `./html`

`./quickhtml.py oblastak_2021 --sql-driver sqlite -n 3 -r 0 -m t -d /var/www/html/oblastak` ... generates overall results for three stages from sqlite DB file into specified dir and exits

### Templates

Program uses following template structure for HTML pages generation:
```
TEMPLATES/        ...  templates folder (configurable)
  index.html      ...    startlists and results index for stage
  main_index.html ...    stages index
  base.html       ...    common page layout
  results/        ...    results
    class.html    ...      one page per class
    index.html    ...      classes index
  startlists/     ...    startlists
    class.html    ...      one page per class
    index.html    ...      classes index
  total/          ...    overall results
    class.html    ...      one page per class
    index.html    ...      classes index
```

### TODO

  * One page results/startlists/overall
  * I18n:
    * English templates
    * Other language templates
    * Parameter for language selection
  * Class filter
