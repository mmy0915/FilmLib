import os,shutil
import math
import cv2
import numpy as np
from converter import Converter

def scan_dirs(directory, prefix=None, postfix=None):
    dirs_list = []
    for root, sub_dirs, files in os.walk(directory):
        for special_dirs in sub_dirs:
            if postfix:
                if special_dirs.endswith(postfix):
                    dirs_list.append(os.path.join(root, special_dirs))
            elif prefix:
                if special_dirs.startswith(prefix):
                    dirs_list.append(os.path.join(root, special_dirs))
            else:
                dirs_list.append(os.path.join(root, special_dirs))

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

ffmpeg_path = 'D:\\FFMpeg\\bin\\ffmpeg.exe'
ffprobe_path = 'D:\\FFMpeg\\bin\\ffprobe.exe'

c = Converter(ffmpeg_path, ffprobe_path)

src_dir = 'D:\\FILM_LIB\\FILM_OPT_TEST'
#src_dir = 'E:\\FILM_OPT'

scene_video_dir_list = scan_dirs(src_dir, prefix="SceneVideo", postfix="SceneVideo")

for f_dirname in scene_video_dir_list:
    if os.path.isdir(f_dirname):
       im_src_dir = f_dirname

    dir_first_ext_pos = im_src_dir.rfind("\\")

    reel_f_name = im_src_dir[im_src_dir[0: dir_first_ext_pos].rfind("\\")+1 : dir_first_ext_pos]
    reel_name_count = int(reel_f_name[4:8])

    # if reel_name_count < 999:
    #     continue
    #if reel_name_count > 50:
    #    break

    print(reel_name_count)

    thumbnail_out_path = im_src_dir[0: dir_first_ext_pos] + "\\SceneVideoThumbnail\\"
    if not os.path.exists(thumbnail_out_path):
        os.mkdir(thumbnail_out_path)

    im_files_list = []
    im_files_list = scan_files(im_src_dir, postfix='.mp4')
    i_count = 0
    for f_name in im_files_list:

        file_pos = f_name.rfind("\\")
        file_name = f_name[file_pos+1 : len(f_name)]
        pos_fix = file_name.find(".mp4")
        thumbnail_file_name = file_name[0:pos_fix] + "_thn.mp4"
        thumbnail_out_file = thumbnail_out_path + thumbnail_file_name

        i_count += 1
        print("%d / total: %d in this" %(i_count, len(im_files_list)))
        if os.path.exists(thumbnail_out_file):
            print("had processed %s" % f_name)
            continue

        param_str = " -filter:v scale=480:-2 "
        # conv = c.converter(f_name, render_out_path, param_str)
        cmd = ffmpeg_path + " -i " + "\"" + f_name + "\"" + param_str + thumbnail_out_file
        r = os.popen(cmd)
        r.close()
        print("finished resize %s" %thumbnail_out_file)