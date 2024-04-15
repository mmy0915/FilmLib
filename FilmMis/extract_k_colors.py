# USAGE
# python color_kmeans.py --image images/jp.png --clusters 3

# Author: Adrian Rosebrock
# Website: www.pyimagesearch.com

# import the necessary packages

import time
from sklearn.cluster import KMeans
import matplotlib.pyplot as plt
# import argparse
import utils
import cv2
from utils import centroid_histogram, plot_colors
from PIL import Image
import os

# 记录开始时间
start = time.perf_counter()

directory = 'D:\FILM_LIB\python\FilmMis\A0010102_main_colors_forth_pre\\'
files = os.listdir(directory)  # list
length = len(files)
save_folder = 'D:\FILM_LIB\python\FilmMis\A0010102_main_colors_forth'

# construct the argument parser and parse the arguments
# ap = argparse.ArgumentParser()
# ap.add_argument("-i", "--image", required=True, help="Path to the image")
# ap.add_argument("-c", "--clusters", required=True, type=int,
#                 help="# of clusters")
# args = vars(ap.parse_args())

# load the image and convert it from BGR to RGB so that
# we can dispaly  it with matplotlib
# image = cv2.imread(args["image"])
for i, v in enumerate(files):
    image = cv2.imread(directory + v)
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # show our image
    # plt.figure()
    # plt.axis("off")
    # plt.imshow(image)

    # reshape the image to be a list of pixels
    image = image.reshape((image.shape[0] * image.shape[1], 3))

    # cluster the pixel intensities
    # clt = KMeans(n_clusters=args["clusters"])
    clt = KMeans(n_clusters=10,n_init= 'auto')
    clt.fit(image)

    # build a histogram of clusters and then create a figure
    # representing the number of pixels labeled to each color
    hist = utils.centroid_histogram(clt)
    bar = utils.plot_colors(hist, clt.cluster_centers_)

    # show our color bart
    # plt.figure()
    # plt.axis("off")
    # plt.imshow(bar)
    # plt.show()

    bar_img = Image.fromarray(bar)
    bar_img.save(os.path.join(save_folder, 'main_colors_first' + str(328 + i) + '.jpg'))

end = time.perf_counter()
print('Running time: %s Seconds' % (end - start))
