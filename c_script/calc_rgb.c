#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define mosaic 1  //mosaicの一辺の大きさ srcに関して実装できていない。
#define N 1 //上位N個から画像を選ぶ
//calc rgb of src and img, put the src which has nearset rgb to img
//imgはsrcの数と同じもしくは少し多めに分割してRGBを計算
//srcと比較。同じのは使わないようにしたい。
//srcが増えるほど精密になるイメージ
void _my_resize(int n, int width, int height, unsigned char **rgb);
int main(int argc, const char *argv[]){
  IplImage *img, *src, *dst, *dst2;  //img: picture want to make  src: source
  int i,j,k,l;
  int x, y;
  int aspX, aspY;
  char str1[128] = {0};
  char str2[] = ".jpg";
  unsigned char **img_rgb;
  unsigned char **src_rgb;
  float **dif;
  int img_number, src_number = 273;
  int tmp = 0;
  char raw_img[150] = "../okada.jpg";
  int n;
  int mosaic_x, mosaic_y; //分割する個の画像においてのmosaic_pixelの数
  int X, Y;
  int A = 1; //imgを何倍にするか
  int params[] = {CV_IMWRITE_JPEG_QUALITY, 50, -1}; //save image
  printf(" ");
  //j: here img = pikachu image.
  img = cvLoadImage(raw_img, CV_LOAD_IMAGE_COLOR);
  if(img == NULL){
    printf("no such file\n");
    exit(-1);
  }

  //今回はアンドロイドを想定して

  //aspX aspYは4x4あたりがベスト。
  //aspX = 16;
  //aspY = 16;
  aspX = 48;
  aspY = 48;
  //j:setting X Y by pikachu image
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
  A = 10;
  //j: why scale?
  X = X*A;
  Y = Y*A;

  //resize:now dst = pikachu
  dst = cvCreateImage(cvSize(aspX*X, aspY*Y), IPL_DEPTH_8U, 3);
  cvResize(img, dst, CV_INTER_AREA);

  //j:rgb -> why data X*Y? (why not X*Y*3??)
  img_rgb = (unsigned char**)malloc(sizeof(unsigned char*)*Y*X*3);  //mosaicのRGBを入れる
  if(img_rgb == NULL){
    printf("malloc failed\n");
    exit(-1);
  }
  //j:rgb -> why data X*Y? (why not X*Y*3??)
  src_rgb = (unsigned char**)malloc(sizeof(unsigned char*)*(src_number)*3);
  if(src_rgb == NULL){
    printf("malloc failed\n");
    exit(-1);
  }

  for (i=0; i<src_number; i++){
   _my_resize(i, aspX, aspY, src_rgb);
  }


  //src_rgb = rgb data for 'blocks'

  for(y=0; y<Y; y++){
    for(x=0; x<X; x++){
      unsigned char r=0, g=0, b=0;
      img_rgb[X*y+x] = (unsigned char*)malloc(sizeof(unsigned char)*aspY*aspX*3/(mosaic*mosaic));
      if(img_rgb[X*y+x] == NULL){
        printf("malloc failed\n");
        exit(-1);
      }
      for(j=0; j<aspY/mosaic; j++){
	      for(i=0; i<aspX/mosaic; i++){
          // for(l=0; l<mosaic; l++){
          //   for(k=0; k<mosaic; k++){
          //     b += (unsigned char)dst->imageData[dst->widthStep*(y*aspY+j*mosaic+l)+(x*aspX+i*mosaic+k)*3];
          //     g += (unsigned char)dst->imageData[dst->widthStep*(y*aspY+j*mosaic+l)+(x*aspX+i*mosaic+k)*3+1];
          //     r += (unsigned char)dst->imageData[dst->widthStep*(y*aspY+j*mosaic+l)+(x*aspX+i*mosaic+k)*3+2];
          //
          //   }
          // }

          b = (unsigned char)dst->imageData[dst->widthStep*(y*aspY+j)+(x*aspX+i)*3];
          g = (unsigned char)dst->imageData[dst->widthStep*(y*aspY+j)+(x*aspX+i)*3+1];
          r = (unsigned char)dst->imageData[dst->widthStep*(y*aspY+j)+(x*aspX+i)*3+2];

          //float _mosaic = (float)mosaic;
          // b = b/(_mosaic*_mosaic);
          // g = g/(_mosaic*_mosaic);
          // r = r/(_mosaic*_mosaic);
          img_rgb[X*y+x][(aspX/mosaic)*j+i*3] = b;
          img_rgb[X*y+x][(aspX/mosaic)*j+i*3+1] = g;
          img_rgb[X*y+x][(aspX/mosaic)*j+i*3+2] = r;
          //printf ("(%d %d %d) \n",r,g, b);
        }
      }
    }
  }

  //compare the rgb
  dif = (float**)malloc(sizeof(float*)*src_number);
  for(k=0; k<src_number; k++){
    dif[k] = (float*)malloc(sizeof(float)*Y*X*3);
    for(y=0; y<Y; y++){
      for(x=0; x<X; x++){
        dif[k][X*y+x] = 0;
        for(j=0; j<aspY/mosaic; j++){
          for(i=0; i<aspX/mosaic; i++){
            float dif_b, dif_g, dif_r;
            dif_b = (float)img_rgb[X*y+x][(aspX/mosaic)*j+i*3  ] - src_rgb[k][(aspX/mosaic)*j+i*3  ];
            dif_g = (float)img_rgb[X*y+x][(aspX/mosaic)*j+i*3+1] - src_rgb[k][(aspX/mosaic)*j+i*3+1];
            dif_r = (float)img_rgb[X*y+x][(aspX/mosaic)*j+i*3+2] - src_rgb[k][(aspX/mosaic)*j+i*3+2];

            dif[k][X*y+x] += dif_b*dif_b + dif_g*dif_g + dif_r*dif_r;
            //これは勘。2乗和は僅かな誤差の影響をかなり受けるから、supressしたほうが良さそうとおもった。しすぎてもだめ
            //あと割り算するんならfloat型に統一しないとダメよ
            dif[k][X*y+x]*=0.01;

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
      int min[N];
      for (i=0; i<N; i++){
        min[i] = i;
      }
      int temp;
      for(k=0; k<N-1; k++){
        for(l=N-1; l>k; l--){
          if(dif[min[l-1]][X*y+x]>dif[min[l]][X*y+x]){ //if prior data is bigger
            temp = min[l]; //exchange
            min[l] = min[l-1];
            min[l-1] = temp;
          }
        }
      }
      for(i=N; i<src_number; i++){
        if(dif[i][X*y+x] < dif[min[N-1]][X*y+x]){
          min[N-1] = i;
          k=N-1;
          while(k){
            if(dif[min[k-1]][X*y+x] < dif[min[k]][X*y+x]){
              temp = min[k];
              min[k] = min[k-1];
              min[k-1] = temp;
              k--;
            }else{
              break;
            }
          }
        }
      }
      //printf("min = %d %d %d %d %d\n", min[0], min[1], min[2], min[3], min[4]);
      //choose index from min by random
      int min_;
      //srand((unsigned)time(NULL));
      min_ = min[rand()%N];
      //printf("chosen min = %d\n",min_);
      for(j=0; j<aspY; j++){
        for(i=0; i<aspX; i++){
          dst2->imageData[dst2->widthStep*(y*aspY+j)+(x*aspX+i)*3  ] = src_rgb[min_][aspX*3*j+i*3  ];
          dst2->imageData[dst2->widthStep*(y*aspY+j)+(x*aspX+i)*3+1] = src_rgb[min_][aspX*3*j+i*3+1];
          dst2->imageData[dst2->widthStep*(y*aspY+j)+(x*aspX+i)*3+2] = src_rgb[min_][aspX*3*j+i*3+2];
        }
      }
    }
  }


  //save image
  if(cvSaveImage("dst_img.jpeg", dst2, params)== 1){
    printf("save finished\n");
  }


  free(img_rgb);
  free(src_rgb);
  free(dif);

  return 0;
}

void _my_resize(int n, int width, int height, unsigned char **rgb){
  int x, y;
  IplImage *img, *dst;
  char str1[128] = {0};
  char str2[] = ".jpg";
  int success;
  int params[] = {CV_IMWRITE_JPEG_QUALITY, 50, -1}; //save image

  sprintf(str1, "%d", n);

  char raw_img[150] = "../raw_pictures_/"; //raw_pictures directory
  char processed_img[150] = "../processed_pictures/"; //processed_pictures directory      strcat(str1, str2);
  strcat(raw_img, str1);
  strcat(raw_img, str2);

  strcat(processed_img, str1);
  strcat(processed_img, str2);
  printf("%s\n",str1);
  //read image
  img = cvLoadImage(raw_img, CV_LOAD_IMAGE_COLOR);
  if(img == NULL){
    printf("%d no such raw file\n", n);
    exit(-1);
  }

  //resize
  dst = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
  cvResize(img, dst, CV_INTER_CUBIC);

  rgb[n] = (unsigned char*)malloc(sizeof(unsigned char)*width*height*3);
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
    //printf("%d finish\n", n);
  }
}
