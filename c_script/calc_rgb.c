#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "process_pictures.h"

#define mosaic 1  //mosaicの一辺の大きさ srcに関して実装できていない。
//calc rgb of src and img, put the src which has nearset rgb to img
//imgはsrcの数と同じもしくは少し多めに分割してRGBを計算
//srcと比較。同じのは使わないようにしたい。
//srcが増えるほど精密になるイメージ

int main(int argc, const char *argv[]){
  IplImage *img, *src, *dst, *dst2;  //img: picture want to make  src: source
  int i,j,k,l;
  int x, y;
  int aspX, aspY;
  char str1[128] = {0};
  char str2[] = ".jpg";
  int **img_rgb;
  int **src_rgb;
  int **dif;
  int img_number, src_number = 80;
  int tmp = 0;
  char raw_img[150] = "../pikachu.jpg";
  int n;
  int mosaic_x, mosaic_y; //分割する個の画像においてのmosaic_pixelの数
  int X, Y;
  int A = 1; //imgを何倍にするか
  int params[] = {CV_IMWRITE_JPEG_QUALITY, 50, -1}; //save image


  img = cvLoadImage(raw_img, CV_LOAD_IMAGE_COLOR);
  if(img == NULL){
    printf("no such file\n");
    exit(-1);
  }

  //今回はアンドロイドを想定して
  aspX = 16;
  aspY = 9;

  X = img->width/aspX;
  Y = img->height/aspY;
  if(img->width%aspX != 0){
    X++;
  }
  if(img->height%aspY != 0){
    Y++;
  }

  printf("X*Y = %d\n", X*Y);

  while(X*Y*A*A < src_number){
    A++;
  }

  X = X*A;
  Y = Y*A;

  //resize
  dst = cvCreateImage(cvSize(aspX*X, aspY*Y), IPL_DEPTH_8U, 3);
  cvResize(img, dst, CV_INTER_AREA);

  img_rgb = (int**)malloc(sizeof(int*)*Y*X);  //mosaicのRGBを入れる
  if(img_rgb == NULL){
    printf("malloc failed\n");
    exit(-1);
  }
  src_rgb = (int**)malloc(sizeof(int*)*(src_number));
  if(src_rgb == NULL){
    printf("malloc failed\n");
    exit(-1);
  }

  for (i=0; i<src_number; i++){
   my_resize(i, aspX, aspY, src_rgb);
  }


  for(y=0; y<Y; y++){
    for(x=0; x<X; x++){
      int r=0, g=0, b=0;
      img_rgb[X*y+x] = (int*)malloc(sizeof(int)*aspY*aspX*3/(mosaic*mosaic));
      if(img_rgb[X*y+x] == NULL){
        printf("malloc failed\n");
        exit(-1);
      }
      for(j=0; j<aspY/mosaic; j++){
	      for(i=0; i<aspX/mosaic; i++){
          for(l=0; l<mosaic; l++){
            for(k=0; k<mosaic; k++){
              b += dst->imageData[dst->widthStep*(y*aspY+j*mosaic+l)+(x*aspX+i*mosaic+k)*3];
              g += dst->imageData[dst->widthStep*(y*aspY+j*mosaic+l)+(x*aspX+i*mosaic+k)*3+1];
              r += dst->imageData[dst->widthStep*(y*aspY+j*mosaic+l)+(x*aspX+i*mosaic+k)*3+2];
            }
          }
          b = b/(mosaic*mosaic);
          g = g/(mosaic*mosaic);
          r = r/(mosaic*mosaic);
          img_rgb[X*y+x][(aspX/mosaic)*j+i*3] = b;
          img_rgb[X*y+x][(aspX/mosaic)*j+i*3+1] = g;
          img_rgb[X*y+x][(aspX/mosaic)*j+i*3+2] = r;
        }
      }
    }
  }

  //compare the rgb
  dif = (int**)malloc(sizeof(int*)*src_number);
  for(k=0; k<src_number; k++){
    dif[k] = (int*)malloc(sizeof(int)*Y*X);
    for(y=0; y<Y; y++){
      for(x=0; x<X; x++){
        dif[k][X*y+x] = 0;
        for(j=0; j<aspY/mosaic; j++){
          for(i=0; i<aspX/mosaic; i++){
            int dif_b, dif_g, dif_r;
            dif_b = img_rgb[X*y+x][(aspX/mosaic)*3*j+i*3]-src_rgb[k][(aspX/mosaic)*3*j+i*3];
            dif_g = img_rgb[X*y+x][(aspX/mosaic)*3*j+i*3+1]-src_rgb[k][(aspX/mosaic)*3*j+i*3+1];
            dif_r = img_rgb[X*y+x][(aspX/mosaic)*3*j+i*3+2]-src_rgb[k][(aspX/mosaic)*3*j+i*3+2];
            dif[k][X*y+x] += dif_b*dif_b + dif_g*dif_g + dif_r*dif_r;
          }
        }
        dif[k][X*y+x] = sqrt(dif[k][X*y+x]/(3*(aspY/mosaic)*(aspX/mosaic)));
      }
    }
  }

  //create mosaic picture
  dst2 = cvCreateImage(cvSize(aspX*X, aspY*Y), IPL_DEPTH_8U, 3);
  for(y=0; y<Y; y++){
    for(x=0; x<X; x++){
      int min = 0;
      for(i=1; i<src_number; i++){
        if(dif[i][X*y+x] < dif[min][X*y+x]){
          min = i;
        }
      }
      //printf("min = %d\n", min);
      for(j=0; j<aspY; j++){
        for(i=0; i<aspX; i++){
          dst2->imageData[dst2->widthStep*(y*aspY+j)+(x*aspX+i)*3] = src_rgb[min][aspX*3*j+i*3];
          dst2->imageData[dst2->widthStep*(y*aspY+j)+(x*aspX+i)*3+1] = src_rgb[min][aspX*3*j+i*3+1];
          dst2->imageData[dst2->widthStep*(y*aspY+j)+(x*aspX+i)*3+2] = src_rgb[min][aspX*3*j+i*3+2];
        }
      }
    }
  }

  //確認用
  /*dst2 = cvCreateImage(cvSize(aspX, aspY), IPL_DEPTH_8U, 3);
  for(j=0; j<dst2->height; j++){
    for(i=0; i<dst2->width; i++){
      dst2->imageData[dst2->widthStep*j+i*3] = src_rgb[0][dst2->widthStep*j+i*3];
      dst2->imageData[dst2->widthStep*j+i*3+1] = src_rgb[0][dst2->widthStep*j+i*3+1];
      dst2->imageData[dst2->widthStep*j+i*3+2] = src_rgb[0][dst2->widthStep*j+i*3+2];
    }
  }*/

  //save image
  if(cvSaveImage("dst_img.jpeg", dst2, params)== 1){
    printf("save finished\n");
  }


  free(img_rgb);
  free(src_rgb);
  free(dif);

  return 0;
}
