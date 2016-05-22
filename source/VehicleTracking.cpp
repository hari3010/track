#include "VehicleTracking.h"

using namespace cv;
/************************************************************************/
/*Filter out the Low saturation pixels of ROI histogram
*/
/************************************************************************/
void cObjectTracking::FilterLowSat(Mat const &input, Mat &temp)
{
	temp.create(input.size(), input.depth());
	mixChannels(input, temp, { 1, 0 });
	threshold(temp, temp, minSat, 255, THRESH_BINARY);
}

/************************************************************************/
/* Transform the image color space to HSV                               */
/************************************************************************/
void cObjectTracking::MatToHsv(Mat const &in, Mat &out)
{
	in.copyTo(out);
	cvtColor(out, out, COLOR_BGR2HSV);
}

/************************************************************************/
/*  Calculate back projecttion of the image.
First- Transform the pictures to HSV space
Second - Split the H channel out as a single grayscale image and get its histogram
Third - calculate the back projection of the image*/
/************************************************************************/
Mat cObjectTracking::GetBackProjection(Mat const &input, Mat const &roi)
{
	Mat output;

	float ranges[] = {0,180};
    	const float* dranges = ranges;	

	MatToHsv(roi, hsv_roi);
	MatToHsv(input, hsv_input);

	//Filter the low saturation pixels out for ROI hsv
	FilterLowSat(hsv_roi, sat_mask_roi);;

	//Variables initialised in list for future use to calculate histogram with dynamic values!
	std::initializer_list<int> channels = { 0 };
	std::initializer_list<int> hist_sizes = { 180 };
	//std::initializer_list<float[2]> ranges = { { 0, 180 } };
	//std::array<float const*, 1> dranges;
	//dranges[0] = *(std::begin(ranges));

	//Calculate Histogram
	calcHist(&hsv_roi, 1, std::begin(channels), sat_mask_roi, hist_roi, channels.size(), std::begin(hist_sizes), &dranges);

	//Filter the low saturation pixels out for Input hsv
	FilterLowSat(hsv_input, sat_mask_input);
	calcBackProject(&hsv_input, 1, std::begin(channels), hist_roi, output, &dranges);

	output &= sat_mask_input;
	return output;
}
