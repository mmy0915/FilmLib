import os,shutil
import math
import cv2
import numpy as np

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

src_dir = 'D:\\FILM_LIB\\FILM_OPT_TEST'
#src_dir = 'E:\\FILM_OPT'

scene_img_dir_list = scan_dirs(src_dir, prefix= "SceneImg", postfix="SceneImg" )

for f_dirname in scene_img_dir_list:
    if os.path.isdir(f_dirname):
       im_src_dir = f_dirname

    dir_first_ext_pos = im_src_dir.rfind("\\")

    reel_f_name = im_src_dir[im_src_dir[0: dir_first_ext_pos].rfind("\\")+1 : dir_first_ext_pos]
    reel_name_count = int(reel_f_name[4:8])

    # if reel_name_count < 262:
    #     continue
    #if reel_name_count > 50:
    #    break

    print(reel_name_count)

    thumbnail_out_path = im_src_dir[0: dir_first_ext_pos] + "\\SceneImgThumbnail\\"
    if not os.path.exists(thumbnail_out_path):
        os.mkdir(thumbnail_out_path)

    im_files_list = []
    im_files_list = scan_files(im_src_dir, postfix='.jpg')
    i_count = 0
    for f_name in im_files_list:

        file_pos = f_name.rfind("\\")
        file_name = f_name[file_pos+1 : len(f_name)]
        pos_fix = file_name.find(".jpg")
        thumbnail_file_name = file_name[0:pos_fix] + "_thn.jpg"
        thumbnail_out_file = thumbnail_out_path + thumbnail_file_name

        i_count += 1
        print("%d / total: %d in this" %(i_count, len(im_files_list)))
        if os.path.exists(thumbnail_out_file):
            print("had processed %s" % f_name)
            continue

        src_img = cv2.imread(f_name)
        src_img_w = src_img.shape[1]
        src_img_h = src_img.shape[0]
        des_img_w = 480
        des_img_h = 270
        ratio_ori = src_img_w / src_img_h
        ratio_des = des_img_w / des_img_h

        if ratio_ori > ratio_des :
            new_img = cv2.resize(src_img, (des_img_w,  math.floor(des_img_w / ratio_ori)))
            roi_x = 0
            roi_y = math.floor(0.5*( des_img_h - math.floor(des_img_w / ratio_ori )))
        else:
            new_img = cv2.resize(src_img, (math.floor(des_img_h * ratio_ori), des_img_h))
            roi_x = math.floor(0.5*( des_img_w - math.floor(des_img_h * ratio_ori)))
            roi_y = 0

        canvas_img = np.zeros([des_img_h, des_img_w, 3], np.uint8)
        canvas_img[roi_y:(roi_y + new_img.shape[0]), roi_x:(roi_x + new_img.shape[1])] = new_img
        cv2.imwrite(thumbnail_out_file, canvas_img)
        print("finished resize %s" %thumbnail_out_file)