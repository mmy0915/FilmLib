import os
import xlsxwriter
import xlrd
import hashlib
import datetime
from converter import Converter

def getBigFileMD5(filepath):
    if os.path.exists(filepath):
        md5obj = hashlib.md5()
        maxbuf = 8192
        f = open(filepath, 'rb')
        while True:
            buf = f.read(maxbuf)
            if not buf:
                break
            md5obj.update(buf)
        f.close()
        hash = md5obj.hexdigest()
        return str(hash).upper()
    return None

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

ffmpeg_path = 'D:\\FFMpeg\\bin\\ffmpeg.exe'
ffprobe_path = 'D:\\FFMpeg\\bin\\ffprobe.exe'

c = Converter(ffmpeg_path, ffprobe_path)

book_w = xlsxwriter.Workbook('G:\FILM_LIB\sys_info\\film_lib_db_to_webdb_20220329.xlsx')
sheet_w = book_w.add_worksheet('film_src_name_20220329')

workbook_r = xlrd.open_workbook('G:\FILM_LIB\sys_info\\film_lib_db_20201015_cleared.xlsx')
booksheet_r = workbook_r.sheet_by_index(0)
n_rows = booksheet_r.nrows
n_columns = booksheet_r.ncols

workbook_base = xlrd.open_workbook('G:\FILM_LIB\sys_info\\douban_base_20200924.xlsx')
booksheet_base = workbook_base.sheet_by_index(0)
base_n_rows = booksheet_base.nrows
base_n_columns = booksheet_base.ncols

film_lib_path = 'E:\\FILM_LIB\\film_src\\'

for r_i in range(n_rows):
    row_val = booksheet_r.row_values(r_i)
    for c_i in range(n_columns):
        sheet_w.write(r_i, c_i, row_val[c_i])

    if (r_i > 0):
        f_name = booksheet_r.row_values(r_i, 6)[0]
        v_db_id = booksheet_r.row_values(r_i, 9)[0]
        u_name = booksheet_r.row_values(r_i, 1)[0]
        new_u_name = u_name[0:len(u_name)-5]

        sheet_w.write(r_i, 1, new_u_name)

        file_name = f_name
        pos_fix = file_name.find(".mp4")

        own_dir = film_lib_path + file_name[0:pos_fix]
        mp4_file = own_dir + "\\" + file_name[0:pos_fix] + ".mp4"

        if not os.path.exists(own_dir):
            continue

        if os.path.exists(mp4_file):
            print("processed %s" % mp4_file)
            info = c.probe(mp4_file)
            try:
                mp4_duration = info.format.duration
            except:
                mp4_duration = ""

            if mp4_duration != "":
                m, s = divmod(mp4_duration, 60)
                h, m = divmod(m, 60)
                mp4_duration_hms = ("%02d:%02d:%02d" % (h, m, s))
                sheet_w.write(r_i, 10, mp4_duration_hms)
                print("%d / total: %d", r_i, n_rows)

        # for base_r_i in range(1, base_n_rows-1):
        #     db_id_base = booksheet_base.row_values(base_r_i, 3)[0]
        #     db_name = booksheet_base.row_values(base_r_i, 4)[0]
        #     db_cover = booksheet_base.row_values(base_r_i, 6)[0]
        #     db_year = int(booksheet_base.row_values(base_r_i, 7)[0])
        #     db_regions = booksheet_base.row_values(base_r_i, 8)[0]
        #     db_genres = booksheet_base.row_values(base_r_i, 9)[0]
        #     db_languages = booksheet_base.row_values(base_r_i, 10)[0]
        #     db_storyline = booksheet_base.row_values(base_r_i, 15)[0]
        #     db_score = booksheet_base.row_values(base_r_i, 23)[0]
        #
        #     if v_db_id == '' or db_id_base == '':
        #         continue
        #     elif int(db_id_base) == int(v_db_id):
        #         sheet_w.write(r_i, 10, db_name)     #  1第一组添加
        #         sheet_w.write(r_i, 11, db_cover)     #  0该片尚未切分
        #         sheet_w.write(r_i, 12, db_year)
        #         sheet_w.write(r_i, 13, db_regions)
        #         sheet_w.write(r_i, 14, db_genres)
        #         sheet_w.write(r_i, 15, db_languages)
        #         sheet_w.write(r_i, 16, db_storyline)
        #         sheet_w.write(r_i, 17, db_score)
        #
        # for f_name in mov_files_list:
        #     file_pos = f_name.rfind("\\")
        #     file_name = f_name[file_pos + 1: len(f_name)]
        #
        #     if file_name.strip() == exit_file_name.strip():
        #         md5_str = getBigFileMD5(f_name)
        #         sheet_w.write(r_i, 3, md5_str)

book_w.close()