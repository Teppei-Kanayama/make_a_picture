# coding: utf-8

import imtools

raw_name_list = imtools.get_imlist("../animals/") #使用する元画像の名前のリストを作る
#processed_name_list = ["../processed_pictures/" + raw_name for raw_name in range(193)] #加工後画像の名前のリストを作る
processed_name_list = []
for i in range(len(raw_name_list)):
    processed_name_list.append("../animals/" + str(i) + ".jpg")

print processed_name_list

for raw_picture_name, processed_picture_name in zip(raw_name_list, processed_name_list):
    print 'a'
    imtools.resize(raw_picture_name, processed_picture_name, 500, 500) #画像のサイズ変更
