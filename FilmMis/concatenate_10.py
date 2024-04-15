import numpy as np
import matplotlib.pyplot as plt
import os
from PIL import Image


def list_files(directory):
    files = []

    for file in os.listdir(directory):
        if os.path.isfile(os.path.join(directory, file)):  # 加了一个判断是否是文件的步骤
            files.append(file)

    return files


# 使用示例
directory = 'D:\FILM_LIB\python\FilmMis\A0010102_main_colors_third\\'
files = os.listdir(directory)  # list
save_folder = 'D:\FILM_LIB\python\FilmMis\A0010102_main_colors_forth_pre'
# for file in files:
#     print(file)

# first_path = 'E:\FILM_LIB\FILM_OPT' #不能在末尾加\诶 后面影片多了的话再改
# second_path = '\A0010102\SceneImgThumbnail'
# paths = ['test10\A0010102_SC_0015_03_thn.jpg', 'test10\A0010102_SC_0016_01_thn.jpg','test10\A0010102_SC_0016_02_thn.jpg',
# 'test10\A0010102_SC_0016_03_thn.jpg', 'test10\A0010102_SC_0017_01_thn.jpg', 'test10\A0010102_SC_0017_02_thn.jpg',
# 'test10\A0010102_SC_0017_03_thn.jpg','test10\A0010102_SC_0018_01_thn.jpg','test10\A0010102_SC_0018_02_thn.jpg',
# 'test10\A0010102_SC_0018_03_thn.jpg']

length = len(files)

img_array = ''
img = ''
for i, v in enumerate(files):
    if i % 10 == 0:
        img = Image.open(directory + v)  # 打开图片
        img_array = np.array(img)  # 转化为np array对象
    if 0 < i % 10 <= 9:
        img_array2 = np.array(Image.open(directory + v))
        img_array = np.concatenate((img_array, img_array2), axis=1)  # 横向拼接
        # img_array = np.concatenate((img_array, img_array2), axis=0)  # 纵向拼接
        img = Image.fromarray(img_array)
    if i % 10 == 9 or i == length - 1:
        img.save(os.path.join(save_folder, 'concatenate1_' + str(i//10) + '.jpg'))
        print(str(os.path.join(save_folder, 'concatenate1_' + str(i//10) + '.jpg')) + ' saved successfully')
# 保存图片
# plt.figure()
# plt.axis("off")
# plt.imshow(img)
# plt.show()


