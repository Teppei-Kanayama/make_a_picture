# coding: utf-8

import imtools

raw_name_list = imtools.get_imlist("../raw_pictures/") #使用する元画像の名前のリストを作る
processed_name_list = ["../processed_pictures/" + raw_name.split('/')[2] for raw_name in raw_name_list] #加工後画像の名前のリストを作る

for raw_picture_name, processed_picture_name in zip(raw_name_list, processed_name_list):
    imtools.resize(raw_picture_name, processed_picture_name, 50, 50) #画像のサイズ変更
