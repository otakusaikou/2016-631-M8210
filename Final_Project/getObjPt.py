#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glob
import numpy as np
import pandas as pd
import psycopg2


def main():
    MODE = "UNSTABLE"

    fileList = glob.glob("./ptCloud/occlusion/*_obj.txt")
    if MODE == "MAINAREA":
        outputFileName = "./ptCloud/cleaned.txt"
    elif MODE == "UNSTABLE":
        outputFileName = "./ptCloud/unstable.txt"
    elif MODE == "STABLE":
        outputFileName = "./ptCloud/stable.txt"
    elif MODE == "OCCOBJ":
        outputFileName = "./ptCloud/occObj.txt"

    # Define database connection parameters
    host = 'localhost'
    port = '5432'
    dbName = 'pointdb'
    user = 'postgres'

    # Connect to database
    try:
        conn = psycopg2.connect(
            "dbname='%s' user='%s' host='%s' port='%s'" %
            (dbName, user, host, port))
    except psycopg2.OperationalError:
        print "Unable to connect to the database."
        return -1

    cur = conn.cursor()     # Get cursor object of database connection

    idArr = np.array([], dtype=int)
    for f in fileList:
        idInFile = pd.read_csv(f, delimiter=' ').id
        idArr = np.append(idArr, idInFile.values)

    np.savetxt(
        "uniqueId.txt",
        np.unique(idArr),
        fmt="%d",
        comments='')

    sql = """
DROP TABLE IF EXISTS public.uniqueid;
CREATE TABLE public.uniqueid
(
    id integer
);
    """
    cur.execute(sql)
    conn.commit()

    with open("uniqueId.txt") as fin:
        cur.copy_from(fin, 'uniqueid')

    with open(outputFileName, 'w') as fout:
        if MODE == "MAINAREA":
            sql = """
COPY (
    SELECT x, y, z, 0 r, 0 g, 255 b
    FROM tmpxyz T
    WHERE T.id NOT IN (
        SELECT id
        FROM uniqueid
    )
)
TO STDOUT DELIMITER ';' CSV HEADER;"""
        if MODE == "UNSTABLE":
            sql = """
COPY (
    SELECT x, y, z, 0 r, 0 g, 255 b
    FROM tmpxyz T
    WHERE T.id NOT IN (
        SELECT id
        FROM uniqueid
    ) AND std_i > 40
)
TO STDOUT DELIMITER ';' CSV HEADER;"""
        elif MODE == "STABLE":
            sql = """
COPY (
    SELECT x, y, z, 0 r, 0 g, 255 b
    FROM tmpxyz T
    WHERE T.id NOT IN (
        SELECT id
        FROM uniqueid
    ) AND std_i <= 40
)
TO STDOUT DELIMITER ';' CSV HEADER;"""
        elif MODE == "OCCOBJ":
            sql = """
COPY (
    SELECT x, y, z, 255 r, 0 g, 0 b
    FROM tmpxyz JOIN uniqueid USING(id)
    )
TO STDOUT DELIMITER ';' CSV HEADER;"""
        cur.copy_expert(sql, fout)


if __name__ == "__main__":
    main()
