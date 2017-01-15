#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glob
import numpy as np
import os
import psycopg2


def main():
    EOFileList = map(lambda f: os.path.split(f)[1], glob.glob('./param/EO*'))
    outputList = map(lambda f: f.replace("EO_", ""), EOFileList)
    outputPath = os.path.abspath("./ptCloud")
    imgNameList = map(lambda f: f.replace("txt", "jpg"), outputList)
    imgNameStr = " ".join(imgNameList)

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
    sql = """
SELECT id, name
FROM image
WHERE name = ANY (string_to_array(%s, ' '))
ORDER BY id;
"""
    cur.execute(sql, (imgNameStr,))
    imgInfo = np.array(cur.fetchall())
    imgIdStr = " ".join(imgInfo[:, 0])

    sql = """
DROP VIEW IF EXISTS pt;
DROP VIEW IF EXISTS tmpxyz;
CREATE VIEW tmpxyz AS (
    SELECT id, x, y, z, stddev((r + g + b) / 3) std_i
    FROM point3d P3D JOIN (
        SELECT C.id CID, r, g, b, point3d_no, image_no image_id
        FROM colorinfo C
    ) IC ON IC.point3d_no = P3D.id
    WHERE z < 7.5
    GROUP BY P3D.id
HAVING position(%s in array_to_string(ARRAY_AGG(IC.image_id ORDER BY IC.image_id), ' ')) != 0);
"""

    cur.execute(sql, (imgIdStr,))
    conn.commit()

    for i in range(len(imgInfo[:, 1])):
        print imgInfo[i, 1]
        sql = """
DROP VIEW IF EXISTS pt;
CREATE VIEW pt AS (
    SELECT x, y, z, r, g, b, row, col, XYZ.id
    FROM tmpxyz XYZ JOIN colorinfo C ON C.point3d_no = XYZ.id
    WHERE C.image_no = %s
);
"""
        cur.execute(sql, (imgInfo[i, 0],))
        conn.commit()

        outputFileName = os.path.join(outputPath, imgInfo[i, 1].replace("jpg", "txt"))
        with open(outputFileName, 'w') as fout:
            sql = "COPY (SELECT * FROM pt) TO STDOUT DELIMITER ';' CSV HEADER;"
            cur.copy_expert(sql, fout)
    conn.close()


if __name__ == "__main__":
    main()
