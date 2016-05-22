//!< Common includes
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include <math.h>

#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"



//!< User includes
#include "VehicleTracking.h"

using namespace std;
using namespace cv;

int track_object(string path, Rect rectROI);
int otherParam(string path);

/*!<***********************************************************************/
/* Read me for command line argument									*/
/************************************************************************/

void readme(std::string strValue)
{
	std::cerr << "Usage:  " << strValue.c_str() << " <Option/s> <image folder path>\n"
		<< "Options\n"
		<< "\t -a -> auto ROI from ground truth value for first frame eg: track -a /home/pictures/ \n"
		<< "\t -r -> enter the ROI value(bottomleft.x bottomleft.y topright.x topright.y) for the first frame eg: track -r /home/pictures/ 6 193 49 166 \n"
		<< "\t -other -> Find other parameter like camera values, speed of the car etc from groundtruth file (file name should be groundtruth.txt in the given folder)eg: track -other /home/document/ \n"
		<< std::endl;
}

/*!<***********************************************************************/
/* Project Main operation           									*/
/************************************************************************/
int project_main(int argc, char* argv[])
{
	//All values are taken auto if no ground truth file available, then take default value
	if (string(argv[1]) == "-a")
	{
		cout << "Reading first frame ROI from groundtruth.txt" << endl;

		//!< if the file not ending with slash, then add it
		string path(argv[2]);
		if ('/' != path.back())
		{
			path.append("/");
		}
		//!< Path to image folder
		string imageFolder(path);

		//!< Path to groungtruth.txt file
		path.append("groundtruth.txt");
		ifstream file(path);
		
		//Check file is open, if so, then read the first line
		if (file.is_open())
		{
			string line;
			//!< values in the file are written as float
			vector<float> vect;
			getline(file, line);

			stringstream ss(line);
			float f;

			//Copy all comma separated float value to a vector float 
			while (ss >> f)
			{
				vect.push_back(f);

				if (ss.peek() == ',')
					ss.ignore();
			}
			file.close();

			//index 0 and 1 represents the bottom left corner, index 4 and 5 represents top right corner locations
			Rect roi_given_initial(Point((int)vect[0], (int)vect[1]), Point((int)vect[4], (int)vect[5]));
			return track_object(imageFolder, roi_given_initial);
		}
		else
		{
			cout << "Unable to open groundtruth file" << endl;
			cout << "Setting default value for the ROI" << endl;
			Rect defaultROI(Point(6, 193), Point(49, 166));
			return track_object(imageFolder, defaultROI);
		}
	}
	//!< Values are given
	else if (string(argv[1]) == "-v")
	{
		if (argc < 7)
		{
			cout << "not enough values to set ROI" << endl;
			readme(argv[1]);
			return -1;
		}
		string path(argv[2]);
		if ('/' != path.back())
		{
			path.append("/");
		}
		vector<int> vect;
		int i;
		for (i = 3; i < 7; i++)
		{
			string str = argv[i];
			vect.push_back(atoi(str.c_str()));
		}

		Rect roi_given_initial(Point(vect[0], vect[1]), Point(vect[2], vect[3]));
		return track_object(path, roi_given_initial);
	}
	else if (string(argv[1]) == "-other")
	{
		//!< if the file not ending with slash, then add it
		string path(argv[2]);
		if ('/' != path.back())
		{
			path.append("/");
		}
		return otherParam(path);
		
	}
	else
	{
		cerr << "Invalid option" << endl;
		return -1;
	}
}

/*!<***********************************************************************/
/* Read all the image frames in a given folder
*  Place the ROI in the first frame
*  Compare with the other frame using CAM Shift algorithm in openCV*/	 
/************************************************************************/
int track_object(string path, Rect rectROI)
{
	string folderPath(path);
	folderPath.append("%08d.jpg");

	//!< Assuming all the frames in the path has the naming convention of %08d.jpg. Example: 00000001.jpg, 00000002.jpg....etc
	//!< Retrieve all the images in the folder
	VideoCapture cap(folderPath); // for for capturing all the images inside folder car
	
	//!< Check files image files present and read
	if (!cap.isOpened())
	{
		cerr << "Error: Cannot open the files" << endl;
		cout << "Folder does not contain image frames" << endl;
		return -1;
	}

	//!< Path to groungtruth.txt file
	//!< This file read only if the car going out of tracking window

	path.append("groundtruth.txt");
	vector<float> vectVertices;

	ifstream file(path);
	if (file.is_open())
	{
		string line;
		getline(file, line);
	}
	//!< Read first frame from the captured stream
	Mat frame;
	bool bSuccess = cap.read(frame);

	if (!bSuccess)
	{
		cerr << "Error: Cannot read the image frame" << endl;
		return -1;
	}

	Mat Roi = frame(rectROI).clone();
	Mat backProject;
	
	//Setup termination criteria
	TermCriteria termCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1);

	while (cap.read(frame))
	{
		//!> Find the back projection base (histogram) on the frame and ROI, it is the probablity of finding ROI content on the frame
		backProject = cObjectTracking().GetBackProjection(frame, Roi).clone();

		//Cam shift - track and find ROI
		Rect rectCamShift = rectROI;
		CamShift(backProject, rectCamShift, termCriteria);
		rectangle(frame, rectCamShift, cv::Scalar(0, 0, 255));

		//Draw the frame
		cv::imshow("output", frame);
		
		string line;
		if (file.is_open())
		{
			getline(file, line);
		}

		int c = waitKey(30);
		if (c == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}else if (c == 84 || c == 116) // To track press 't' - ascii values checked
		{
			if (file.is_open())
			{
				vector<float> vect;
				stringstream ss(line);

				float f;
				//Copy all comma separated float value to a vector float 
				while (ss >> f)
				{
					vect.push_back(f);

					if (ss.peek() == ',')
						ss.ignore();
				}

				//index 0 and 1 represents the bottom left corner, index 4 and 5 represents top right corner locations
				Rect tempRect(Point((int)vect[0], (int)vect[1]), Point((int)vect[4], (int)vect[5]));
				rectROI = tempRect;
				Roi = frame(rectROI).clone();
			}
		}

	}
	return 0;
}

/************************************************************************/
/* This function to find different parameter associated with the set of frames
such as shape of the road, variation in the speed of the road, etc 
using ground truth values												*/
/************************************************************************/
int otherParam(string path)
{
	//!< Finding the shape of the road
	cout << "Find the shape of the road with the ground truth value" << endl;
	path.append("groundtruth.txt");
	ifstream file(path);

	// Create a light grey image in 8 bit gre scale color space
	// Find the average of 
	Mat img(400, 502, CV_8U, cv::Scalar(200));
	int frame_num = 0;
	int minHeight = 600;
	int maxHeight = 0;
	//Check file is open, if so, then read the first line
	//
	if (file.is_open())
	{
		string line;
		float iCentroid = 0;
		float iHeight = 0;
		while (!file.eof())
		{
			vector<float> vect;
			getline(file, line);

			stringstream ss(line);
			float f;

			//Copy all comma separated float value to a vector float 
			while (ss >> f)
			{
				vect.push_back(f);

				if (ss.peek() == ',')
					ss.ignore();
			}
			
			//Height of car in pixels
			iHeight = vect[1] - vect[3];
			
			if(iHeight < minHeight)
				minHeight = iHeight;
			if(iHeight > maxHeight)
				maxHeight = iHeight;
						

			//Centroid value to plot the shape of the track
			iCentroid = (int)((vect[1] + vect[3]) / (float)2);
			//Drawing four pixels to black
			img.at<uchar>(iCentroid, frame_num) = 0;
			img.at<uchar>(iCentroid + 1, frame_num) = 0;
			img.at<uchar>(iCentroid + 1, frame_num + 1) = 0;
			img.at<uchar>(iCentroid, frame_num + 1) = 0;

			frame_num += 2;
			if (frame_num > 500)
				break;
		}
		file.close();
		imshow("roi", img);
		cout << "\nThis picture is the estimated shape of the road, plotted centrality of car dimensions against\
				 the frame number. Please press escape to find other param" << endl;
		cout << "maxHeight in pixels" << maxHeight << endl;
		cout << "minHeight in pixels" << minHeight << endl;
		cout << "Assuming car's track is a straight line, and using trignometry and the magnification,\
				 the value of the camera rotation found to be : " << acos((double)minHeight/(double)maxHeight) * 180/3.14 << endl;
		waitKey();

		return 0;
	}
	return -1;
}
