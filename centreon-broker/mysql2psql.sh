#!/bin/sh

sed < mysql_schema.sql 's/ double / double precision /g' \
                 | sed 's/ int NOT NULL auto_increment/ serial/g' \
                 | sed 's/ ENGINE=.*;/;/g' \
                 | sed 's/ IF NOT EXISTS//g' \
                 | sed 's/ TABLE `\([^)]*\)`/ TABLE "\1"/g' \
                 | sed "s/\`/\"/g" \
                 | sed 's/unsigned//g' \
                 | sed 's/UNIQUE KEY "[^"]*"/UNIQUE/g' \
    > postgresql_schema.sql
