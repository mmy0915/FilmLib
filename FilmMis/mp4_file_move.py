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

mp4_files_list = scan_files('I:\\movie', postfix='.mp4')
i_count = 0
for f_name in mp4_files_list:

    file_pos = f_name.rfind("\\")
    file_name = f_name[file_pos+1 : len(f_name)]

    pos_fix = file_name.find(".mp4")
    render_out_file = mp4_out_dir + file_name[0:pos_fix] + ".mp4"

    i_count += 1
    print("%d / total: %d", i_count, len(mp4_files_list))
    if os.path.exists(render_out_file):
        print("had processed %s" % f_name)
        continue
    shutil.copyfile(f_name, render_out_file)
    print("finished check %s" % f_name)





