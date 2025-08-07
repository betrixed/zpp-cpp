<?php

$SQL_NS = "Wcd\\Sql\\";
$DRIVER_NS = "Wcd\\Ext\\";

return [
    "drivers" => [
         "pdo_mysql" => $DRIVER_NS . "PdoMysql",
         "mysqli" => $DRIVER_NS . "Mysqli",
         "pdo_pgsql" => $DRIVER_NS . "PdoPgsql",
         "pdo_firebird" => $DRIVER_NS . "PdoFirebird",
         "pgfo" => $DRIVER_NS . "Pgsqlfn",
         "fbirdfn" => $DRIVER_NS . "Fbirdfn",
         "pdo_sqlite" => $DRIVER_NS . "PdoSqlite",
         "sqlite3" => $DRIVER_NS . "Sqlitefn"
    ],
    "sqls" => [
        "pdo_mysql" => $SQL_NS . "Mysql",
        "mysqli" => $SQL_NS . "Mysql",
        "pdo_pgsql" => $SQL_NS . "Postgres",
        "pgfo" => $SQL_NS . "Postgres",
        "pdo_firebird" => $SQL_NS . "Firebird",
        "fbirdfn" => $SQL_NS . "Firebird",
        "pdo_sqlite" => $SQL_NS . "Sqlite",
        "sqlite3" => $SQL_NS . "Sqlite"
    ],
    "db-config" => [
        'pcanex' => [
            'host' => 'localhost',
            'port' => 3306,
            'driver' => 'pdo_mysql',
            'charset' => 'utf8mb4',
            'collation' => 'utf8mb4_unicode_ci',
            'database' => 'pcanex',
            'username' => 'pcanex',
            'password' => 'WhatsUpDoc',
            'model_ns' => 'Pcan\Models',
        ],
        
        'sqlite3' => [
            'host' => 'na',
            'username' => 'na',
            'password' => 'na',
            'driver' => 'sqlite3',
            'database' => __DIR__ . '/_data/sqlite_test.sdb'       
        ],
        'sqlite' => [
            'host' => 'na',
            'username' => 'na',
            'password' => 'na',
            'driver' => 'pdo_sqlite',
            'database' => __DIR__ . '/_data/sqlite_test.sdb'         
        ],
        'db1' => [
            'host' => 'localhost',
            'port' => 3306,
            'driver' => 'pdo_mysql',
            'charset' => 'utf8mb4',
            'collation' => 'utf8mb4_unicode_ci',
            'database' => 'phpunit',
            'username' => 'phpunit',
            'password' => 'whatsUpDoc'
        ],
        // direct mysqli driver
        'ms1' => [
            'host' => 'localhost',
            'port' => 3306,
            'driver' => 'mysqli',
            'charset' => 'utf8mb4',
            'collation' => 'utf8mb4_unicode_ci',
            'database' => 'phpunit',
            'username' => 'phpunit',
            'password' => 'whatsUpDoc'
        ],
        
        /*
         * Postgresql install on debian. (now version 14)
         * sudo su - postgres
         * createuser testuser
         * createdb  testzo
         * psql
         * GRANT ALL PRIVILEGES ON DATABASE testzo to testuser;
         * ALTER ROLE testuser WITH PASSWORD 'y0ghurt+';
         * \q
         * 
         */
        'pg1' => [
            'password' => '',
            'driver' => 'pdo_pgsql',
            'database' => 'database-testuser',
            'username' => 'testuser',
            'host' => 'localhost',
            'charset' => 'UTF8'
        ],
        'pgfo' => [
            'password' => '',
            'driver' => 'pgfo',
            'database' => 'database-testuser',
            'username' => 'testuser',
            'host' => 'localhost',
            'charset' => 'UTF8'
        ],
        
        /*
         * Firebird server 3.0
         * using isql-fb,
         * https://github.com/FirebirdSQL/php-firebird
         * CONNECT '/var/lib/firebird/3.0/test.db' user SYSDBA password 'y0gh-urT-5uG*RH19';
         */
        'fb1' => [
            'host' => 'localhost',
            //'password' => 'y0ghurt+',
            'password' => 'y0gh-urT-5uG*RH19',
            'driver' => 'pdo_firebird',
            'database' => '/var/lib/firebird/3.0/test.db',
            'username' => 'SYSDBA',
            'host' => 'localhost',
            'charset' => 'UTF8'
        ],
        
        'fbfn' => [
            'host' => 'localhost',
            //'password' => 'y0ghurt+',
            'password' => 'y0gh-urT-5uG*RH19',
            'driver' => 'fbirdfn',
            'database' => '/var/lib/firebird/3.0/test.db',
            'username' => 'SYSDBA',
            'host' => 'localhost',
            'charset' => 'UTF8'
        ]
    ]
];
    

