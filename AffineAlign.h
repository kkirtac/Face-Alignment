#pragma once

#include <opencv2/core/core.hpp>

using namespace cv;

class AffineAlign
{
public:
	AffineAlign(void);
	~AffineAlign(void);

	// offset_pct_X defines a ratio of the outputSize width: offset_pct_X * outputSize width
	// offset_pct_Y defines a ratio of the outputSize height: offset_pct_Y * outputSize height
	void alignFace( const Mat& faceRect, vector<Point>& eyeCoords, const Size& outputSize, Mat& out, 
		const float offset_pct_X = 0.2f, const float offset_pct_Y = 0.2f );

	Mat scaleRotateTranslate( const Mat& image, const Size& dstSize, float angle,
		int centerX, int centerY, int newCenterX, int newCenterY, float scaleX, float scaleY);


	Mat srcImage;
	Mat dstImage;
	vector<Point> eyeCoords;
	Size outputSize;

};

