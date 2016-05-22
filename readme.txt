To Run:
Track <Options> <Path> <ROI coordinates optional>
	Options:
	-a – auto values to ROI of the first frame taken from ground truth file, or a hard coded value if not available.  Eg: track –a /home/user/Desktop/car
	-v – manual values Eg: track –v /home/user/Desktop/car 20 25 45 235. The last four values are bottomleft.x bottomleft.y topright.x topright.y
	-other - find other values such as shape of the road and camera rotation.

To compile:
	Used OpenCV library for image tracking
	Cmake, g++ for build and compile.

	To install OpenCV. visit: http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html
	For easy installation using a script in Ubuntu 14.04 refer http://rodrigoberriel.com/download/script-install-opencv-3-0-0/
	
	other links:
	Cmake help for compiling in C++ 11 with g++ taken from http://pageant.ghulbus.eu/?p=664

	Steps:
	1. install Opencv and its dependencies
	2. goto the project folder
	3. run cmake .
	4. make

Description:
	1. Vehicle Tracking - In this code, CAM Shift algorithm from OpenCV library used to track the object. It involves finding the histogram of the target to back project the each image on the frame to find the mean shift. While running the iamge if the object is not tracked properly press 't' to refresh the tracking. This algorithm gives the moderate results compared to finding feature points in the ROI and mapping it with other image. First, I tried with comparing the feature points of known objects the next image frames, this did not give any nice results hence moved to using CAM shift algorithm.
	2. The shape of the road can be found by plotting centroid of the image(with respect to height) to the frame number. This actually gives the better estimation of the road shape.
	3. Rotation of the camera can be found using the following formula
		Focal length(f) = (object pixels(p))/(original width of the car(w)) x distance to the object from lens(d) => f = p/w * d; so d = f * p/w
		considering the road is straight line using previous question plot. with repect to the camera the maximum magnification object is the closest point and stays 90 degree. the minimum magnification is the farthest point. so cos t = adjacent/hypotenuse. this ratio can be found using the above formula. cos t = p2/p1 = > t = inv cos p2/p1.  where t is the angular roation of the camera, p1 is the lowest height in pixels and p2 is the highest height in pixels.
	4. Other parameter such as finding focal length, intrinsic calibration parameter of the camera  needs one or more additional information to calculate.
	5. if more time available using the depth information available from the captured ROI, a part of vehicle's 3d mesh can be extracted.
	
