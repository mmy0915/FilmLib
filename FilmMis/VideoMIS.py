import pymysql
import xlrd
from datetime import datetime
from xlrd import xldate_as_tuple
# 打开数据库连接
conn = pymysql.connect(host='127.0.0.1',
                     port=3306,
                     user='root',
                     passwd='1230',
                     db='vvs_filmlib')

cursor = conn.cursor()

workbook_r = xlrd.open_workbook('D:\\FILM_LIB\\sys_info\\20231128_copy.xls')
booksheet_r = workbook_r.sheet_by_index(0)
n_rows = booksheet_r.nrows
n_columns = booksheet_r.ncols
i_count = 0

sence_num = "36"
pic_path = 'm_film_lib\\FILM_OPT\\'

for r_i in range(1, n_rows):
    lib_id = booksheet_r.row_values(r_i, 0)[0]
    lib_name = booksheet_r.row_values(r_i, 1)[0]
    year = booksheet_r.row_values(r_i, 2)[0]
    md5_str = booksheet_r.row_values(r_i, 3)[0]
    cut_version = int(booksheet_r.row_values(r_i, 5)[0])
    src_file_name = booksheet_r.row_values(r_i, 6)[0]
    db_name = booksheet_r.row_values(r_i, 8)[0]
    db_id = int(booksheet_r.row_values(r_i, 9)[0])

    ctype = booksheet_r.cell(r_i, 10).ctype  # 表格的数据类型
    cell = booksheet_r.cell_value(r_i, 10)

    date = xlrd.xldate_as_tuple(cell, 0)
    video_duration = str(date[3]).zfill(2)+":"+str(date[4]).zfill(2)+":"+str(date[5]).zfill(2)
    now_time = datetime.now()

    # db_poster = pic_path + lib_id + "\\SceneImg\\" + lib_id + "_SC_" + sence_num.zfill(4) + "_02.jpg"
    # sql_find = 'select count(*) from m_film_lib where lib_id = %s;'
    # cursor.execute(sql_find, lib_id)
    # data = cursor.fetchall()
    # print(data[0])
    # if len(data) == 1 and len(data[0]) == 1 and data[0] == 1:
    #     continue
    # else:
    #     sql_ins = """INSERT INTO m_film_lib(lib_id, lib_name, year, md5_str, cut_version, src_file_name, db_id, db_name, db_poster) values(%s, %s,%s, %s,%s, %s,%s, %s,%s)"""
    #     cursor.execute(sql_ins, (lib_id, lib_id,year,md5_str,cut_version,src_file_name,db_id,db_name,db_poster))
    #     conn.commit()



    # new_file_name = src_file_name.replace(' ', '_')
    # new_file_name = new_file_name.replace('。', '.')
    # new_file_name = new_file_name.replace('：', '_')
    # new_file_name = new_file_name.replace('、', '_')
    #
    # file_name_ext_mp4 = new_file_name[0:new_file_name.rfind('.mp4')]
    # video_url = '/m_film_lib/film_src/' + file_name_ext_mp4 + '/' + new_file_name
    # if(new_file_name != src_file_name):
    #     sql_update_video = 'UPDATE m_video SET url = %s WHERE lib_id = %s'
    #     cursor.execute(sql_update_video, (video_url, lib_id))
    #     conn.commit()



    # file_name_ext_mp4 = src_file_name[0:src_file_name.rfind('.mp4')]
    # video_url = '/m_film_lib/film_src/' + file_name_ext_mp4 + '/' + src_file_name
    # img_url = '/m_film_lib/FILM_OPT/' + lib_id + '/SceneImg/' + lib_id + '_SC_' + sence_num.zfill(4) + '_02.jpg'
    #
    # # sql_find = 'select count(*) from m_video where lib_id = %s;'
    # # cursor.execute(sql_find, lib_id)
    # # data = cursor.fetchall()
    # # print(data[0])
    # # if len(data) == 1 or len(data[0]) == 1 or data[0] == 1:
    # #     sql_update_video = 'UPDATE m_video SET duration = %s WHERE lib_id = %s'
    # #     cursor.execute(sql_update_video, (video_duration, lib_id))
    # # else:
    # sql_ins_video = """INSERT INTO m_video(title, duration, username, url, img, post_time, status, v_class_type, v_in_class_idx, v_class_id, views, del_flag, lib_id) values(%s, %s,%s, %s,%s, %s,%s, %s,%s, %s, %s, %s, %s)"""
    # cursor.execute(sql_ins_video, (db_name,video_duration, 'filmlib', video_url, img_url, now_time, '1', '0', '1', '-1', '0', '0', lib_id))
    # conn.commit()


cursor.close()
conn.close()
