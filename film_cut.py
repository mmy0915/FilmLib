import os
import xlrd
import xlwt
from xlutils.copy import copy
import _thread as thread
import subprocess

film_src_path = 'D:\\FILM_LIB\\film_src'
film_out_path = 'D:\\FILM_LIB\\FILM_OPT_TEST'

workbook_r = xlrd.open_workbook('D:\\FILM_LIB\\sys_info\\20231128_new_add.xls')

def cutting_process(from_id, end_id):
    global film_src_path
    global film_out_path
    global  booksheet_r
    global n_rows

    for r_i in range(from_id, end_id):
        row_idx = r_i + 1

        row_val = booksheet_r.row_values(row_idx)
        f_id = row_val[0]
        f_name_short = row_val[1]
        f_year = row_val[2]
        f_md5 = row_val[3]
        f_cut_flag = row_val[4]
        f_cut_version = row_val[5]
        f_name = row_val[6]

        postfix_pos = f_name.rfind('.')
        filename = f_name[0:postfix_pos]

        src_f_name = str(row_val[7]) + "\\" + filename + "\\" + str(row_val[6])
        u_filename = str(f_name_short)

        cut_out_path = film_out_path + "\\" + u_filename

        print("%d / total: %d" % (row_idx, n_rows))
        if os.path.exists(cut_out_path):
            print("had processed %s, %s" % (f_name, f_id))
            continue

        if round(f_cut_flag) == 0:
            print("cut flag %s, skip %s" % (f_cut_flag, f_name))
            continue

        param_str_io = " --input " + '\"' + src_f_name + '\"' + " --output " + '\"' + cut_out_path + '\"' + " -d 4 " + "-s " + '\"' + u_filename + ".stats.csv" + '\"'
        param_str_v = " detect-content "
        param_str_v += "list-scenes -f " + u_filename + "_SCs.csv "
        param_str_v += "save-images -o " + cut_out_path + "\\SceneImg" + " -f " + u_filename + "_SC-$SCENE_NUMBER-$IMAGE_NUMBER "
        param_str_v += "split-video -o " + cut_out_path + "\\SceneVideo" + " -f " + u_filename + "_SC-$SCENE_NUMBER -a \"-c:v libx264 -preset fast -crf 22 -c:a aac\" "
        param_str_v += "export-html -f " + u_filename + "_SCs.html --image-width 240 --image-height 135 "

        cmd = "scenedetect" + param_str_io + param_str_v
        r = os.system(cmd)
        # r.close()

        print("finished check %s" % f_name)

booksheet_r = workbook_r.sheet_by_index(0)
n_rows = booksheet_r.nrows
n_columns = booksheet_r.ncols
n_films_count = n_rows - 1

n_tread = 4
cutting_process(0, 1)
"""try:
    for i in range(n_tread):
        step = round(n_films_count/n_tread)
        thread.start_new_thread(cutting_process, (i*step, (i+1)*step))
        print("tread runing %s" % str(i+1))
except:
   print ("Error: 无法启动线程")"""
