import os
from converter import Converter

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

mov_files_list = scan_files('G:\\FILM_LIB\\film_src\\', postfix='.mp4')
pro_des_path = 'G:\\FILM_LIB\\film_src_aac\\'

i_count = 0
for f_name in mov_files_list:

    file_pos = f_name.rfind("\\")
    file_name = f_name[file_pos+1 : len(f_name)]
    shot_pos = f_name[0:file_pos].rfind("\\")
    file_path = f_name[0:file_pos+1]
    shot_name = f_name[shot_pos+1:file_pos]

    pos_fix = file_name.find(".mp4")
    render_out_path = pro_des_path + shot_name + "\\"

    info = c.probe(f_name)
    try:
        mp4_ac_str = info.audio.codec
    except:
        mp4_ac_str = ""

    i_count += 1
    print("%d / total: %d" % (i_count, len(mov_files_list)))
    print("mp4 audio codex %s %s" %(mp4_ac_str, file_name))

    if mp4_ac_str.strip() != 'aac':
        if not os.path.exists(render_out_path):
            os.mkdir(render_out_path)

        render_out_file = render_out_path + file_name

        if os.path.exists(render_out_file):
            print("had processed %s" %f_name)
            continue

        param_str = " -codec:v copy -codec:a aac -b:a 320k "

        cmd = ffmpeg_path + " -i " + "\"" + f_name + "\"" + param_str + render_out_file
        r = os.popen(cmd)
        r.close()
        print("finished process %s" % f_name)