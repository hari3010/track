#ifndef VEHICETRACKING_HPP
#define VEHICETRACKING_HPP

#include <iostream>
#include <array>
#include <vector>
#include <initializer_list>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//!< Main function to track the object in video frame
int project_main(int argc, char* argv[]);

//!< Function declarations
void readme(std::string);

//! Class that aids object tracking
class cObjectTracking
{
public:
	cv::Mat GetBackProjection(cv::Mat const &input, cv::Mat const &roi);
	void MatToHsv(cv::Mat const &input, cv::Mat &output);
	void FilterLowSat(cv::Mat const &input, cv::Mat  &temp);
	void mixChannels(cv::Mat const &source, cv::Mat &dest, std::initializer_list<int> list)
	{
		cv::mixChannels(&source, 1, &dest, 1, std::begin(list), list.size() / 2);
	}
private:
	int minSat = 65;
	cv::Mat hsv_roi;
	cv::Mat hsv_input;
	cv::Mat hist_roi;
	cv::Mat sat_mask_roi;
	cv::Mat sat_mask_input;
};

#endif
