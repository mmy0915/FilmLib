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

src_dir = 'D:\\FILM_LIB\\FILM_OPT_TEST\\'
p_dir_list = scan_dirs(src_dir)
for f_dirname in p_dir_list:

    reel_name = f_dirname[f_dirname.rfind('\\') + 1: len(f_dirname)]
    reel_name_count = int(reel_name[4:8])
    # if reel_name_count < 12:
    #     continue
    # if reel_name_count > 53:
    #     break

    # new_reel_name = reel_name[0:8]
    # new_dir_name = f_dirname[0:f_dirname.rfind('\\')+1]+new_reel_name
    # os.rename(f_dirname, new_dir_name)

    print(reel_name_count)

    video_des_path = f_dirname + "\\SceneVideo"
    img_des_path = f_dirname + "\\SceneImg"

    p_file_video_list = scan_files(video_des_path, postfix="mp4")
    p_file_img_list = scan_files(img_des_path, postfix="jpg")

    for f_video in p_file_video_list:
        path_pos = f_video.rfind('\\')
        video_name = f_video[path_pos + 1: len(f_video)]

        num_pos = video_name.rfind('SC_')
        num_end_pos = video_name.rfind('.')
        num_str = video_name[num_pos+3: num_end_pos]

        if (len(num_str) >= 4):
            break

        new_video_name = video_name[0:num_pos+3] + num_str.zfill(4) + video_name[num_end_pos:len(video_name)]
        v_dst = video_des_path + "\\" + new_video_name
        os.replace(f_video, v_dst)
    
    video_des_path = f_dirname + "\\SceneVideoThumbnail"
    p_file_video_list = scan_files(video_des_path, postfix="mp4")

    for f_video in p_file_video_list:
        path_pos = f_video.rfind('\\')
        video_name = f_video[path_pos + 1: len(f_video)]

        num_pos = video_name.rfind('SC_')
        num_end_pos = video_name.rfind('_')
        num_str = video_name[num_pos+3: num_end_pos]

        if (len(num_str) >= 4):
            break

        new_video_name = video_name[0:num_pos+3] + num_str.zfill(4) + video_name[num_end_pos:len(video_name)]
        v_dst = video_des_path + "\\" + new_video_name
        os.replace(f_video, v_dst)

    for f_img in p_file_img_list:
        path_pos = f_img.rfind('\\')
        img_name = f_img[path_pos + 1: len(f_img)]

        num_pos = img_name.rfind('SC_')
        num_end_pos = img_name.rfind('_')
        num_str = img_name[num_pos + 3: num_end_pos]
        if (len(num_str) >= 4):
            break

        new_img_name = img_name[0:num_pos+3] + num_str.zfill(4) + img_name[num_end_pos:len(img_name)]
        img_dst = img_des_path + "\\" + new_img_name
        os.replace(f_img, img_dst)

    img_des_path = f_dirname + "\\SceneImgThumbnail"
    p_file_img_list = scan_files(img_des_path, postfix="jpg")
    for f_img in p_file_img_list:
        path_pos = f_img.rfind('\\')
        img_name = f_img[path_pos + 1: len(f_img)]

        num_pos = img_name.rfind('SC_')
        num_end_pos = img_name[0:img_name.rfind('_')].rfind('_')
        num_str = img_name[num_pos + 3: num_end_pos]
        if (len(num_str) >= 4):
            break
        new_img_name = img_name[0:num_pos+3] + num_str.zfill(4) + img_name[num_end_pos:len(img_name)]
        img_dst = img_des_path + "\\" + new_img_name
        os.replace(f_img, img_dst)





