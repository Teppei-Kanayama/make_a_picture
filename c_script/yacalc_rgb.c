#include <opencv/cv.h>
#include <time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <string>
#include <opencv2/legacy/legacy.hpp>
#include <vector>
#include <numeric>

#define OPENCV_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#define OPENCV_VERSION_CODE OPENCV_VERSION(CV_MAJOR_VERSION, CV_MINOR_VERSION, CV_SUBMINOR_VERSION)

#if OPENCV_VERSION_CODE>=OPENCV_VERSION(2,4,0)
//#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#endif

/* 分割数の定義 */
#define DIVX  (50)
#define DIVY  (50)
#define DIVXY (DIVX*DIVY)

using namespace std;

int main (int argc, char **argv)
{

  int w, h, i, j;
  IplImage *img1,*img2;
  cv::Mat image1,image2;
  CvRect roi[DIVXY];
  IplImage *v[DIVXY],*z[DIVXY];;
  cv::Mat m[DIVXY],n[DIVXY];

　//カラー画像の読み込み
  img1 = cvLoadImage("C:/Users/IML_PC/Pictures/taylor(300).png", 1);
  img2 = cvLoadImage("C:/Users/IML_PC/Pictures/becky(300).png", 1);

  if(img1 == 0 ) return -1;
  if(img2 == 0 ) return -1;
　
  image1 = cv::cvarrToMat(img1);　　　//IplImageをcv::Matに変換
  image2 = cv::cvarrToMat(img2);

  w = img1->width;　　　//画像の幅と高さを取得
  h = img1->height;

  for (i = 0; i < DIVX; i++) {　　　//画像を分割するための矩形を設定
  　　for (j = 0; j < DIVY; j++) {
      　　roi[DIVX * j + i].x = w / DIVX * i;
      　　roi[DIVX * j + i].y = h / DIVY * j;
      　　roi[DIVX * j + i].width = w / DIVX;
      　　roi[DIVX * j + i].height = h / DIVY;
    　}
  }

  for(i=0;i<DIVXY;i++){
cvSetImageROI (img1, roi[i]);　　　//画像を矩形に分割
v[i]=img1;　　　　　　　　　　　  //分割した画像を配列に格納
cvSetImageROI (img2, roi[i]);
z[i]=img2;

      m[i] = cv::cvarrToMat(v[i]);　　　//IplImageをcv::Mat に変換
 n[i] = cv::cvarrToMat(z[i]);
  }
  cv::Mat gray1[DIVXY],gray2[DIVXY];
  cv::Mat descriptors1[DIVXY];
  cv::Mat descriptors2[DIVXY];
  std::vector<cv::KeyPoint> keypoints1;
  std::vector<cv::KeyPoint> keypoints2;

  cv::SiftFeatureDetector detector1;
  cv::SiftDescriptorExtractor extractor1;

  cv::SiftFeatureDetector detector2;
  cv::SiftDescriptorExtractor extractor2;

  for(i=0;i<DIVXY;i++){
//特徴点抽出用のグレー画像用意
cv::cvtColor(m[i], gray1[i], CV_BGR2GRAY); 　　//グレースケール化
cv::normalize(gray1[i], gray1[i], 0, 255, cv::NORM_MINMAX); 　//配列の正規化
cv::cvtColor(n[i], gray2[i], CV_BGR2GRAY);
cv::normalize(gray2[i], gray2[i], 0, 255, cv::NORM_MINMAX);

      detector1.detect(gray1[i], keypoints1);　　　//画像から特徴点を検出
      detector2.detect(gray2[i], keypoints2);

      //画像の特徴点における特徴量を抽出
      extractor1.compute(gray1[i], keypoints1, descriptors1[i]);
      extractor2.compute(gray2[i], keypoints2, descriptors2[i]);
  }

  std::vector<double> data1,data2;
  double sum1[DIVXY],sum2[DIVXY];
  double sumsift[DIVXY][DIVXY];
  double dsift[DIVXY][DIVXY];

  std::vector<cv::Mat> bgr1,bgr2;
  double Y1=0.0,Cb1=0.0,Cr1=0.0,Y2=0.0,Cb2=0.0,Cr2=0.0;
  int b1=0,g1=0,r1=0,b2=0,g2=0,r2=0;
double sumcolor[DIVXY][DIVXY];
  double dcolor[DIVXY][DIVXY];

  for(i=0;i<DIVXY;i++){
for(int x=0; x<descriptors1[i].rows; ++x){
descriptors1[i].row(x).copyTo(data1);  //各特徴点における特徴量を配列に格納
      }
sum1[i]=std::accumulate(data1.begin(), data1.end(), 0.0);　 //特徴量の合計を計算
 data1.clear();
for(int x=0; x<descriptors2[i].rows; ++x){
descriptors2[i].row(x).copyTo(data2);  //各特徴点における特徴量を配列に格納
}
sum2[i]=std::accumulate(data2.begin(), data2.end(), 0.0);   //特徴量の合計を計算
 data2.clear();
  }

for(i=0;i<DIVXY;i++){
// 3つのチャネルB, G, Rに分離 (OpenCVではデフォルトでB, G, Rの順)
cv::split(m[i], bgr1);
int num1=m[i].cols*m[i].rows;

　　　for(int y=0;y<m[i].rows;y++){　　
for(int x=0;x<m[i].cols;x++){
b1 = (int)bgr1[0].data[y*m[i].cols+x];　　　//RGB値の算出
　　　g1 = (int)bgr1[1].data[y*m[i].cols+x];
　　　r1 = (int)bgr1[2].data[y*m[i].cols+x];

　　　Y1 += 0.2990*r1+0.5870*g1+0.1140+b1;　　//YCbCr値の計算
              Cb1 += -0.1687*r1-0.3313*g1+0.5000*b1+128;
              Cr1 += 0.5000*r1-0.4187*g1-0.0813*b1+128;
　　　 　  }
 　　　}
Y1/=num1;     //各分割画像のYCbCr値の平均を算出
Cb1/=num1;
       Cr1/=num1;


for(j=0;j<DIVXY;j++){
        　　cv::split(n[j], bgr2);　　// 3つのチャネルB, G, Rに分離
          　int num2=n[j].cols*n[j].rows;

      　 for(int y=0;y<n[j].rows;y++){
    　　for(int x=0;x<n[j].cols;x++){
　　b2 = (int)bgr2[0].data[y*n[j].cols+x];　　　//RGB値の算出
    g2 = (int)bgr2[1].data[y*n[j].cols+x];
    r2 = (int)bgr2[2].data[y*n[j].cols+x];

    Y2 += 0.2990*r2+0.5870*g2+0.1140+b2;　　//YCbCr値の計算
                 　 Cb2 += -0.1687*r2-0.3313*g2+0.5000*b2+128;
                  　Cr2 += 0.5000*r2-0.4187*g2-0.0813*b2+128;
　　　　　　　　}
　　}
　　Y2/=num2;　　　//各分割画像のYCbCr値の平均を算出
         　 Cb2/=num2;
          　Cr2/=num2;
          //色差分の計算
      sumcolor[i][j]=(Y1-Y2)*(Y1-Y2)+(Cb1-Cb2)*(Cb1-Cb2)+(Cr1-Cr2)*(Cr1-Cr2);
  dcolor[i][j] = sqrt(sumcolor[i][j]);
  　　}
　}

  double alpha=0.0;　　//評価関数の重み変数
  vector<int> array;
  double D[DIVXY][DIVXY];

for(i=0;i<DIVXY;i++){
for(j=0;j<DIVXY;j++){
sumsift[i][j]=(sum1[i]-sum2[j])*(sum1[i]-sum2[j]);　　//特徴量差分の計算
　dsift[i][j] = sqrt(sumsift[i][j]);
  /*
　　　　　　　//特徴量差分が閾値より小さい場合配列に格納
if(dsift[i][j]<10)　array.push_back(j);　
　*/
　D[i][j]=alpha*dsift[i][j]+(1-alpha)*dcolor[i][j];　　//評価関数
　　　}

/*
　　//最適画像の決定
　　//特徴量差分が閾値より小さいものの中で色が最も似ているものを判定
  vector<int>::iterator it = array.begin();
  double min=dcolor[i][*it];
  int x=0;
  for(it = array.begin(); it != array.end(); ++it) {
if(min>dcolor[i][*it]){
　　　　　min=dcolor[i][*it];
　x=*it;
　　　}
  }
*/
double min=D[i][0];
int x=0;
for(j=0;j<DIVXY;j++){　　//評価関数Dの値が最小となる画像を最適画像とする
if(min>D[i][j]){
　　　min=D[i][j];
　　　x=j;
　　　　　}
　　　}
cv::Mat Roi(image1,roi[i]);　　//画像マッチング
n[x].copyTo(Roi);
  }

  cv::namedWindow("result", CV_WINDOW_AUTOSIZE);
  cv::imshow("result",image1);

  cv::waitKey(0);
  return 0;
} 
