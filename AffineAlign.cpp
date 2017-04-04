#include "AffineAlign.h"

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

AffineAlign::AffineAlign(void)
{
}


AffineAlign::~AffineAlign(void)
{
}

void AffineAlign::alignFace(const Mat& src,
							vector<Point>& eyeCoords, const Size& outputSize, Mat& out, 
							const float offset_pct_X, const float offset_pct_Y) {

								CV_Assert( src.rows > 0 && src.cols > 0 );
								CV_Assert( outputSize.width > 0 && outputSize.height > 0 );

								if(eyeCoords.size() != 2 || (eyeCoords[0].x == 0 && eyeCoords[0].y == 0 && eyeCoords[1].x == 0 && eyeCoords[1].y == 0) )
								{
									// Göz sayýsý uygun olmasa da src u dönder. En azýndan bulunan yüzler kaydedilir.
									out = src;
									// throw error if no data.
									string errMsg = "FaceAlignment Error: 2 eye points should be given!";
									CV_Error(CV_StsBadArg, errMsg);
								}

								// calculate offsets in original image
								int offset_x = cvFloor(offset_pct_X * outputSize.width);
								int offset_y = cvFloor(offset_pct_Y * outputSize.height);

								// get rotation amount wrt eyes
								float eye_diff_y = eyeCoords[1].y - eyeCoords[0].y;
								float eye_diff_x = eyeCoords[1].x - eyeCoords[0].x;

								float angle = atan2( eye_diff_y, eye_diff_x );


								//Soldaki gözün birinci göz gelmesi lazým. Eðer gelmezse..
								//..aþaðýdaki constraintlere uymuyor ve açý +- pi/2 range i..
								//..dýþýnda çýkýyor. Bu durumda açýyý düzelt ve swap et.

								if(angle > 0 && angle > CV_PI / 2)
								{
									//+- pi/2 aralýðýna al, tanjantý koruyarak
									angle = angle - CV_PI;

									//Swap göz coordinates
									Point e1 = eyeCoords[1];
									Point e2 = eyeCoords[0];

									eyeCoords.clear();
									eyeCoords.push_back(e1);
									eyeCoords.push_back(e2);


								}
								else if(angle < 0 && angle < -CV_PI / 2)
								{
									//+- pi/2 aralýðýna al, tanjantý koruyarak
									angle = angle + CV_PI;

									//Swap göz coordinates
									Point e1 = eyeCoords[1];
									Point e2 = eyeCoords[0];

									eyeCoords.clear();
									eyeCoords.push_back(e1);
									eyeCoords.push_back(e2);
								}



								// distance btw eyes
								float dist = sqrtf( eye_diff_y*eye_diff_y + eye_diff_x*eye_diff_x );

								// calculate the reference eye-width
								float reference = outputSize.width - 2.0f*offset_x;


								float scale;
								try{
									// compute scale factor
									scale = dist/reference;
								}
								catch( cv::Exception& e ){
									// throw the exception
									CV_Error(e.code, e.msg);
								}


								// rotate original around the left eye
								Mat transformed = scaleRotateTranslate(src, Size(src.cols, src.rows), angle,
									eyeCoords[0].x, eyeCoords[0].y, eyeCoords[0].x, eyeCoords[0].y,
									1.0, 1.0);

								// crop the rotated image
								int crop_x = cvFloor(eyeCoords[0].x - scale*offset_x);
								int crop_y = cvFloor(eyeCoords[0].y - scale*offset_y);
								int crop_size_x = cvFloor(scale*outputSize.width);
								int crop_size_y = cvFloor(scale*outputSize.height);

								// crop hatasi alinabiliyor!!
								// outputSize buyutmek cozum olmuyor, offset_pct_X kucultulmeli!

								if( crop_y < 0 )
								{
									crop_y = 0;
								}


								if( crop_x < 0 )
								{
									crop_x = 0;
								}


								// if the crop region goes beyond the image borders
								if( (crop_x + crop_size_x) > transformed.size().width-1 )
								{
									crop_size_x = transformed.size().width - 1 -  crop_x;
								}

								// if the crop region goes beyond the image borders
								if( (crop_y + crop_size_y) > transformed.size().height-1 )
								{
									crop_size_y = transformed.size().height - 1 -  crop_y;
								}


								Mat cropped;

								try{

									Rect roi(crop_x, crop_y, crop_size_x, crop_size_y);
									cropped =  transformed( roi );
								}
								catch( cv::Exception& e ){
									// throw the exception
									CV_Error(e.code, e.msg);
								}

								// resize it
								try{

									resize( cropped, out, outputSize, 0, 0, INTER_CUBIC);
								}
								catch( cv::Exception& e ){
									// throw the exception
									CV_Error(e.code, e.msg);
								}
}

Mat AffineAlign::scaleRotateTranslate(const Mat& src,
									  const Size& dstSize, float angle,
									  int centerX, int centerY, int newCenterX, int newCenterY,
									  float scaleX, float scaleY) {

										  CV_Assert( src.rows > 0 && src.cols > 0 );

										  float a,b,c,
											  d,e,f;

										  a = cos(angle)/scaleX;
										  b = sin(angle)/scaleX;
										  c = centerX-a*newCenterX - b*newCenterY;
										  d = -sin(angle)/scaleY;
										  e = cos(angle)/scaleY;
										  f = centerY-d*newCenterX - e*newCenterY;

										  float m23Data[] = { a, b, c,
											  d, e, f};

										  Mat m23 = Mat(2, 3, CV_32F, m23Data);

										  Mat dst;

										  warpAffine(src, dst, m23, dstSize);

										  return dst;
}
