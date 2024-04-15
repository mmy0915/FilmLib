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

mp4_out_dir = 'H:\\FILM_LIB\\mp4_ipv6_tmp\\'

mp4_files_list = scan_files(mp4_out_dir, postfix='.mp4')
i_count = 0
for f_name in mp4_files_list:

    file_pos = f_name.rfind("\\")
    file_name = f_name[file_pos+1 : len(f_name)]

    pos_fix = file_name.find(".mp4")
    file_name_nospace = file_name[0:pos_fix].strip()

    if file_name_nospace != file_name[0:pos_fix]:
        new_file_name = f_name[0:file_pos] + "\\" + file_name_nospace + ".mp4"
        os.rename(f_name, new_file_name)
        print("finished check %s" % f_name)

