#include "process_pictures.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <string.h>
#include <stdio.h>

//resize the raw pictures into the same size
//if the number of picture increases, increase the number

void my_resize(int n, int width, int height, int **rgb){
  int x, y;
  IplImage *img, *dst;
  char str1[128] = {0};
  char str2[] = ".jpg";
  int success;
  int params[] = {CV_IMWRITE_JPEG_QUALITY, 50, -1}; //save image

  sprintf(str1, "%d", n);

  char raw_img[150] = "../raw_pictures/"; //raw_pictures directory
  char processed_img[150] = "../processed_pictures/"; //processed_pictures directory      strcat(str1, str2);
  strcat(raw_img, str1);
  strcat(raw_img, str2);

  strcat(processed_img, str1);
  strcat(processed_img, str2);

  //read image
  img = cvLoadImage(raw_img, CV_LOAD_IMAGE_COLOR);
  if(img == NULL){
    printf("%d no such file\n", n);
    exit(-1);
  }

  //resize
  dst = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
  cvResize(img, dst, CV_INTER_CUBIC);

  rgb[n] = (int*)malloc(sizeof(int)*width*height*3);
  if(rgb[n] == NULL){
    printf("malloc failed\n");
    exit(-1);
  }

  for(y=0; y<dst->height; y++){
    for(x=0; x<dst->width; x++){
      //dstのrgbを取る
      rgb[n][dst->widthStep*y+x*3] = dst->imageData[dst->widthStep*y+x*3];
  	  rgb[n][dst->widthStep*y+x*3+1] = dst->imageData[dst->widthStep*y+x*3+1];
  	  rgb[n][dst->widthStep*y+x*3+2] = dst->imageData[dst->widthStep*y+x*3+2];
    }
  }

  //save image
  if((success = cvSaveImage(processed_img, dst, params)) == 1){
    printf("%d finish\n", n);
  }
}

//最大公約数を求める
int getGCD(int x, int y){
  int tmp = 0;
  if(x<y){
    tmp = x;
    x = y;
    y = tmp;
  }

  if (y<=0){
    return -1;
  }

  if(x%y==0){
    return y;
  }
  //再帰
  return getGCD(y, x%y);
}
