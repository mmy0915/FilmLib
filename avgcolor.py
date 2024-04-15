import cv2
import numpy as np
from skimage import io
#1.为什么又引入了一个新的库skimage，只是计算一个平均颜色需要这么复杂吗
import matplotlib.pyplot as plt

img = io.imread('https://i.stack.imgur.com/DNM65.png')[:, :, :-1]
#2.为什么这里要带切片参数？

average = img.mean(axis=0).mean(axis=0)
#3.用了两次mean函数 计算每个色度通道的平均值。

#应用k均值聚类来创建具有最具代表性的图像颜色的调色板(在此玩具示例中，n_colors设置为5)
pixels = np.float32(img.reshape(-1, 3))

n_colors = 5
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 200, .1)
flags = cv2.KMEANS_RANDOM_CENTERS

_, labels, palette = cv2.kmeans(pixels, n_colors, None, criteria, 10, flags)
_, counts = np.unique(labels, return_counts=True)

dominant = palette[np.argmax(counts)]
#主要颜色是在量化图像上最频繁出现的调色板颜色

avg_patch = np.ones(shape=img.shape, dtype=np.uint8) * np.uint8(average)
#此处np.ones是生成和ima形状相同的全零矩阵
#4.为什么要乘以后面那个玩意？

indices = np.argsort(counts)[::-1]
freqs = np.cumsum(np.hstack([[0], counts[indices] / float(counts.sum())]))
rows = np.int_(img.shape[0] * freqs)

dom_patch = np.zeros(shape=img.shape, dtype=np.uint8)
for i in range(len(rows) - 1):
    dom_patch[rows[i]:rows[i + 1], :, :] += np.uint8(palette[indices[i]])

fig, (ax0, ax1) = plt.subplots(1, 2, figsize=(12, 6))
ax0.imshow(avg_patch)
ax0.set_title('Average color')
ax0.axis('off')
ax1.imshow(dom_patch)
ax1.set_title('Dominant colors')
ax1.axis('off')
plt.show(fig)