import os,shutil

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

mp4_out_dir = 'M:\\FILM_LIB\\film_src\\'

mp4_files_list = scan_files(mp4_out_dir, postfix='.mp4')
i_count = 0
for f_name in mp4_files_list:

    file_pos = f_name.rfind("\\")
    file_name = f_name[file_pos+1 : len(f_name)]
    pos_fix = file_name.find(".mp4")

    ori_own_dir = mp4_out_dir + file_name[0:pos_fix]

    new_file_name = file_name.replace(' ', '_')
    new_file_name = new_file_name.replace('。', '.')
    new_file_name = new_file_name.replace('：', '_')
    new_file_name = new_file_name.replace('、', '_')

    new_pos_fix = new_file_name.find(".mp4")
    new_own_dir = mp4_out_dir + new_file_name[0:new_pos_fix]

    if new_file_name != file_name:
        new_file_name = f_name[0:file_pos] + "\\" + new_file_name
        os.rename(f_name, new_file_name)
        print("finished check %s" % f_name)

    if ori_own_dir != new_own_dir:
        os.replace(ori_own_dir, new_own_dir)

