import os, shutil

def scan_dirs(directory, prefix=None, postfix=None):
    dirs_list = []

    for sub_dirs in os.listdir(directory):
        dirs_list.append(os.path.join(directory, sub_dirs))

    return dirs_list


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



# workbook_r = xlrd.open_workbook('G:\\FILM_LIB\\sys_info\\film_lib_db_20201015_cleared.xlsx')
# booksheet_r = workbook_r.sheet_by_index(0)
# n_rows = booksheet_r.nrows
# n_columns = booksheet_r.ncols

p_dir_list = scan_dirs('D:\\FILM_LIB\\FILM_OPT_TEST')
for f_dirname in p_dir_list:

    reel_name = f_dirname[f_dirname.rfind('\\') + 1: len(f_dirname)]
    # year_pos = reel_name.rfind('_')
    # year_str = reel_name[year_pos+1 : len(reel_name)]
    # if len(year_str) == 4:
    #     reel_name_ext_year = reel_name[0:len(reel_name)-5]
    # else:
    #     reel_name_ext_year = reel_name

    reel_name_count = int(reel_name[4:8])
    # if reel_name_count < 51:
    #     continue
    # if reel_name_count > 53:
    #     break

    print(reel_name_count)

    # new_reel_name = ''
    # for r_i in range(1, n_rows - 1):
    #     lib_id = booksheet_r.row_values(r_i, 0)[0]
    #     lib_name = booksheet_r.row_values(r_i, 1)[0]
    #     year = booksheet_r.row_values(r_i, 2)[0]
    #     lib_name_ext_year = lib_name
    #     if lib_name_ext_year == reel_name_ext_year:
    #         new_reel_name = reel_name_ext_year
    #         break

    video_des_path = f_dirname + "\\SceneVideo"
    img_des_path = f_dirname + "\\SceneImg"
    thn_img_des_path = f_dirname + "\\SceneImgThumbnail"
    thn_video_des_path = f_dirname + "\\SceneVideoThumbnail"

    p_file_video_list = scan_files(video_des_path, postfix="mp4")
    p_file_img_list = scan_files(img_des_path, postfix="jpg")
    p_file_thn_img_list = scan_files(thn_img_des_path, postfix="jpg")
    p_file_thn_video_list = scan_files(thn_video_des_path, postfix="mp4")

    for f_video in p_file_video_list:
        path_pos = f_video.rfind('\\')
        video_name = f_video[path_pos + 1: len(f_video)]
        new_video_name = video_name.replace('-', '_')
        v_dst = video_des_path + "\\" + new_video_name
        os.replace(f_video, v_dst)

    for f_video in p_file_thn_video_list:
        path_pos = f_video.rfind('\\')
        video_name = f_video[path_pos + 1: len(f_video)]

        new_video_name = video_name.replace('-', '_')
        v_dst = thn_video_des_path + "\\" + new_video_name
        os.replace(f_video, v_dst)

    for f_img in p_file_img_list:
        path_pos = f_img.rfind('\\')
        img_name = f_img[path_pos + 1: len(f_img)]

        new_img_name = img_name.replace('-', '_')
        i_dst = img_des_path + "\\" + new_img_name
        os.replace(f_img, i_dst)

    for f_img in p_file_thn_img_list:
        path_pos = f_img.rfind('\\')
        img_name = f_img[path_pos + 1: len(f_img)]

        new_img_name = img_name.replace('-', '_')
        i_dst = thn_img_des_path + "\\" + new_img_name
        os.replace(f_img, i_dst)

