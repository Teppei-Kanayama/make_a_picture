#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <string.h>
#include <stdio.h>

//resize the raw pictures into the same size
//if the number of picture increases, increase the number

int main(int argc, const char *argv[]){
  IplImage *img, *dst;
  int i;
  char str1[128] = {0};
  char str2[] = ".jpg";
  int success;
  int params[] = {CV_IMWRITE_JPEG_QUALITY, 50, -1}; //save image

  for(i=1; i<=80; i++){
    sprintf(str1, "%d", i);
    
    char raw_img[150] = "../raw_pictures/"; //raw_pictures directory 
    char processed_img[150] = "../processed_pictures/"; //processed_pictures directory  
    strcat(str1, str2);
    strcat(raw_img, str1);
    strcat(processed_img, str1);
    
    //read image
    img = cvLoadImage(raw_img, CV_LOAD_IMAGE_COLOR);
    if(img == NULL){
      printf("%d no such file\n", i);
      exit(-1);
    }

    //resize
    dst = cvCreateImage(cvSize(50, 50), IPL_DEPTH_8U, 3);
    cvResize(img, dst, CV_INTER_CUBIC);

    //save image
    if((success = cvSaveImage(processed_img, dst, params)) == 1){
      printf("%d finish\n", i);
    }
  }

  return 0;
}
