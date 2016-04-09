# coding: utf-8

import cv2
import math
import numpy as np
import imtools
    
name_list = imtools.get_imlist("../processed_pictures/")
rgb_ave_list = []
for filename in name_list:
    img = cv2.imread(filename, 1)
    img_rgb = np.array(cv2.split(img))
    rgb_ave = []
    for i in range(3):
        tmp = img_rgb[i, :, :].sum() / (50 * 50)
        rgb_ave.append(tmp)
    
    rgb_ave_list.append(rgb_ave)

samp_img = np.array(cv2.imread("../pikachu.jpg", 1))
rows, cols, channels = samp_img.shape

for j in range(rows):
    for i in range(cols):
        mse = float("inf")
        img_num = 0
        for count, rgb_average in enumerate(rgb_ave_list):
            tmp = (rgb_average[0] - samp_img[j, i, 0]) ** 2 + (rgb_average[1] - samp_img[j, i, 1]) ** 2 + (rgb_average[2] - samp_img[j, i, 2]) ** 2
            if tmp < mse:
                mse = tmp
                img_num = count
        img = cv2.imread(name_list[img_num])
        if i != 0:
            img_tmp = cv2.imread('tmp_dst.jpg')
            img = cv2.hconcat([img_tmp, img])
        cv2.imwrite('tmp_dst.jpg', img)
    img = cv2.imread('tmp_dst.jpg')
    if j != 0:
        img_tmp = cv2.imread('dst.jpg')
        img = cv2.vconcat([img_tmp, img])
    cv2.imwrite('dst.jpg', img)
        
