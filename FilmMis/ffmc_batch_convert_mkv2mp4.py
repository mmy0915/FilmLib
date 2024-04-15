import os

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


ffmpeg_path = 'D:\\VF-DEV\\FFMpeg\\bin\\ffmpeg.exe'
ffprobe_path = 'D:\\VF-DEV\\FFMpeg\\bin\\ffprobe.exe'

mp4_out_dir = 'K:\\B1_Film_mp4_tmp\\'


mov_files_list = scan_files('K:\\B1_Film', postfix='.mkv')
i_count = 0
for f_name in mov_files_list:

    file_pos = f_name.rfind("\\")
    file_name = f_name[file_pos+1 : len(f_name)]

    pos_fix = file_name.find(".mkv")
    render_out_path = mp4_out_dir + file_name[0:pos_fix] + '.mp4'

    i_count += 1
    print("%d / total: %d", i_count, len(mov_files_list))
    
    if os.path.exists(render_out_path):
        print("had processed %s" % f_name)
        continue

    param_str = " -codec copy "
    #conv = c.converter(f_name, render_out_path, param_str)
    cmd = ffmpeg_path + " -i " + "\"" + f_name + "\"" + param_str + render_out_path
    r = os.popen(cmd)
    r.close()
    print("finished check %s" % f_name)

