import pymysql
import csv
import codecs
import os
import datetime
# 打开数据库连接
conn = pymysql.connect(host='127.0.0.1',
                     port=3306,
                     user='root',
                     passwd='1230',
                     db='vvs_filmlib')

cursor = conn.cursor()

def scan_files(directory, prefix=None, postfix=None):
    files_list = []

    for root, sub_dirs, files in os.walk(directory):
        for special_file in files:
            if postfix:
                if special_file.endswith(postfix):
                    files_list.append(os.path.join(root, special_file))
            elif prefix:
                if special_file.startswith(prefix):
                    files_list.append(os.path.join(root, special_file))
            else:
                files_list.append(os.path.join(root, special_file))

    return files_list

src_dir = 'E:\\FILM_LIB\\FILM_OPT\\'
#src_dir = 'E:\\FILM_OPT\\'

csv_list = scan_files(src_dir, postfix='.csv')

for csv_f_name in csv_list:
    if csv_f_name.find('stats') < 0:
        pos = csv_f_name.rfind("\\")
        file_name = csv_f_name[pos+1: len(csv_f_name)]
        lib_id = file_name[0:8]

        reel_name_count = int(lib_id[4:8])
        # if reel_name_count <= 50:
        #     continue
        print(reel_name_count)

        with codecs.open(filename=csv_f_name, mode='r', encoding='utf-8') as f:
            data = csv.reader(f)
            for index, rows in enumerate(data):
                if index > 1:  # 不要首行
                    row = rows
                    scene_id = row[0]
                    start_fr = row[1]
                    start_tc = row[2]
                    start_sec = row[3]
                    end_fr = row[4]
                    end_tc = row[5]
                    end_sec = row[6]
                    len_fr = row[7]
                    len_tc = row[8]
                    len_sec = row[9]

                    # sql_find = 'select count(*) from vvs.m_film_scene where lib_id = %s;'
                    # cursor.execute(sql_find, lib_id)
                    # data = cursor.fetchall()
                    # print(data[0])
                    # if len(data) >= 1 and len(data[0]) >= 1 and data[0] >= 1:
                    #     break
                    # else:
                    sql_ins = """INSERT INTO m_film_scene(lib_id, scene_id, start_fr, start_tc, start_sec, end_fr, end_tc, end_sec, len_fr, len_tc, len_sec) values(%s, %s,%s, %s,%s, %s,%s, %s,%s,%s, %s)"""
                    cursor.execute(sql_ins, (lib_id, scene_id,start_fr,start_tc,start_sec,end_fr,end_tc,end_sec,len_fr,len_tc,len_sec))
                    conn.commit()

cursor.close()
conn.close()
