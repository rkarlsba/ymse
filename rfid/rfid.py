#!/usr/bin/env python3
# vim:ts=4:sw=4:sws=4:et:ai

import sqlite3

sqlitedb="tags.db"

try:
    conn=sqlite3.connect(sqlitedb)
    c = conn.cursor()

    c.execute('SELECT max(length(name)) FROM rfid_tags')
    maxlength=c.fetchone()[0];

    c.execute('SELECT name,id FROM rfid_tags ORDER BY name')
    allrows = c.fetchall()
    for row in allrows:
        print(row[0].ljust(maxlength+1) + hex(row[1]))


except sqlite3.Error as e:
    print("An error occurred:", e.args[0])

finally:
    conn.close()

