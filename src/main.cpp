#include "ardrone/ardrone.h"
#include <time.h>

double get_seconds() { /* routine to read time in seconds */
	time_t ltime = time(NULL);
	return (double)ltime;
}

using namespace std;
using namespace cv;

Point find_red_point(Mat image);

Point next_point(Mat image);
Point next_point2(Mat image);

bool autoDrive = 0;
bool redTrace = 0;
bool TurnSign = 0;
bool move_left = 0;
bool move_right = 0;

double redX=0,redY=0;
double lastRes = 0;
int missTime=5;

double speedTran(int val,int ref);




// --------------------------------------------------------------------------
// main(Number of arguments, Argument values)
// Description  : This is the entry point of the program.
// Return value : SUCCESS:0  ERROR:-1
// --------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	// AR.Drone class
	ARDrone ardrone;

	// Initialize
	if (!ardrone.open()) {
		cout << "Failed to initialize." << endl;
		return -1;
	}

	ardrone.setCamera(1);

	// Battery
	cout << "Battery = " << ardrone.getBatteryPercentage() <<
		"[%]" << endl;

	// Instructions
	cout << "***************************************" << endl;
	cout << "*       CV Drone program              *" << endl;
	cout << "*           - How to play -           *" << endl;
	cout << "***************************************" << endl;
	cout << "*                                     *" << endl;
	cout << "* - Controls -                        *" << endl;
	cout << "*    'Space' -- Takeoff/Landing       *" << endl;
	cout << "*    'Up'    -- Move forward          *" << endl;
	cout << "*    'Down'  -- Move backward         *" << endl;
	cout << "*    'Left'  -- Turn left             *" << endl;
	cout << "*    'Right' -- Turn right            *" << endl;
	cout << "*    'Q'     -- Move upward           *" << endl;
	cout << "*    'A'     -- Move downward         *" << endl;
	cout << "*                                     *" << endl;
	cout << "* - Others -                          *" << endl;
//	cout << "*    'C'     -- Change camera         *" << endl;
	cout << "*    'Esc'   -- Exit                  *" << endl;
	cout << "*                                     *" << endl;
	cout << "***************************************" << endl;

	double sec = get_seconds();
	double vx = 0, vy = 0, vz = 0, vr = 0;
	while (1) {
		double now = get_seconds();

		// Key input
		int key = waitKey(33);
		if (key == 0x1b) break;

		// Get an image
		Mat img = ardrone.getImage();
		Mat image;
		cvtColor(img, img, CV_BGR2GRAY);
		threshold(img, image, 20, 255, THRESH_BINARY_INV);

		// Take off / Landing 
		if (key == ' ') {
			if (ardrone.onGround()) {
				ardrone.takeoff();
			} else {
				ardrone.landing();
			}
		}
		
		//double vx = 0, vy = 0, vz = 0, vr = 0;
		// Move
		if (!autoDrive) {
			vx = vy = vz = vr = 0;
		}
		if (key == 'i' || key == CV_VK_UP) {
			vx = 1.0;
		}
		if (key == 'k' || key == CV_VK_DOWN) {
			vx = -1.0;
		}
		if (key == 'u' || key == CV_VK_LEFT)
		{
			vr = 1.0;
		}
		if (key == 'o' || key == CV_VK_RIGHT) {
			vr = -1.0;
		}
		if (key == 'j') {
			vy = 1.0;
		}
		if (key == 'l'){
			vy = -1.0;
		}
		if (key == 'q') {
			vz = 1.0;
		}
		if (key == 'a') {
			vz = -1.0;
		}
		if (key == 'm') {
			vx = 1.0;
			vy = 1.0;
		}
		if (key == 'n') {
			vx = -1.0;
			vy = -1.0;
		}
		if (key == 'd') autoDrive = 1;
		if (key == 's') autoDrive = 0;
		if (key == 'e') redTrace = 1;
		if (key == 'w') redTrace= 0;

		if (autoDrive)
		{
			if (now - sec > 0.2) {
				sec = now;
				Point p = next_point(image);
				Point p2 = next_point2(image);
				cout << p << " " << p2 << endl;
				if(p2.x==-1 && p2.y==-1)
				{
					if (p.x == -1 && p.y == -1)
					{
						/*
						if(!TurnSign)
						{	
							vx = vy = vz = vr = 0;
							printf("Next_point not found.\n");
						}
						*/
						if (move_left) {
							vy = 0.1;
							vx = vz = vr = 0;
						}
						else if (move_right) {
							vy = -0.1;
							vx = vz = vr = 0;
						}
						/*
						else
						{
							vx = -0.2;
							vy = vz = vr = 0;

						}  
						*/
					}
					else if (p.y == 0)
					{
						if (p.x >= 200 && p.x <= 440) {
							cout << "straight" << endl;
							vx = 0.2;
							vy = vz = vr = 0;
							TurnSign=0;
						} 
						else if(p.x > 440)
						{
							cout << "moveright" << endl; 
							vy = -0.2;
							vx = vr = vz = 0;
							TurnSign=1;
						}
						else if (p.x < 200)
						{
							cout << "moveleft" << endl;
							vy = 0.2; 
							vx = vr = vz = 0;
							TurnSign=1;
						}
					}
					else if (p.x == 0)
					{
						cout << "turnleft" << endl;
						vr = 0.2;
						vx = vy = vz = 0;
						TurnSign=1;
					}
					else if (p.x == 639)
					{
						cout << "turnright" << endl;
						vr = -0.2;
						vx = vy = vz = 0;
						TurnSign=1;
					}
				}
				else
				{
					move_left = move_right = 0;
					if(p.x-p2.x<-100 && p.x-p2.x>-300)
					{
						cout << "2left" << endl;
						vr = 0.5;
						vx = vy = vz = 0;
						TurnSign=1;


					}
					else if(p.x-p2.x<-300 )
					{
						cout << "2left" << endl;
						vr = 1.0;
						vx = vy = vz = 0;
						TurnSign=1;


					}

					else if(p.x-p2.x>100 && p.x-p2.x<300)
					{
						cout << "2right" << endl; 
						vr = -0.5;
						vx = vy = vz = 0;
						TurnSign=1;

					}
					else if(p.x-p2.x>300 )
					{
						cout << "2right" << endl;
						vr = -1.0;
						vx = vy = vz = 0;
						TurnSign=1;


					}
					else
					{
						cout << "2straight" << endl;
						vx = 0.2;
						vy = vz = vr = 0;
						TurnSign=0;

					}
					

					if (p.x < 200 && p2.x < 200)
					{
						cout << "2moveleft" << endl;
						move_left = 1;
						vy = 0.1;
						vx = vz = vr = 0;

					}
					else if (p.x >440 && p2.x >440)
					{
						cout << "2moveright" << endl;
						move_right = 1;
						vy = -0.1;
						vx = vz = vr = 0;

					}
				}
			}

		}

		if (redTrace)
		{
			Point p = find_red_point(img);
			redX=p.x;
			redY=p.y;
			cout << p << endl;
			
			vx = vy = vz = vr = 0;
			if(p.x==-1 && p.y == -1) 
			{
				if(missTime>0)
				{
					missTime--;	
					p.x=redX;
					p.y=redY;
				}
				else
				{	
					p.x=320;
					p.y=180;
				}
				printf("Next_point not found.\n");

			}
			else
				missTime=5;
			
			

			vy=speedTran(p.x,320);
			vx=speedTran(p.y,180);



			imshow("lala", img);
		}

		ardrone.move3D(vx, vy, vz, vr);

		// Change camera
		static int mode = 0;
		if (key == 'c') ardrone.setCamera(++mode % 4);

		// Display the image
		imshow("camera", image);
	}

	// See you
	ardrone.close();

	return 0;
}

Point find_red_point(Mat image)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	
	threshold(image, image, 250, 255, THRESH_BINARY);

	findContours(image, contours, hierarchy,
		     CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,
		     Point(0, 0));

	drawContours(image, contours, 0, Scalar(255,0,0),
		     1, 8, vector<Vec3i>(), 0, Point());

	if (contours.size() == 0) {
		return Point(-1, -1);
	}
	Rect rect = boundingRect(contours[0]);

	return Point(rect.x + rect.width/2, rect.y + rect.height/2);
}

Point next_point(Mat image)
{
	for (int i = 0; i < 320; ++i) {
		if (image.at<uchar>(0, 320 - i) == 255) {
			return Point(320 - i, 0);
		}

		if (image.at<uchar>(0, 320 + i) == 255) {
			return Point(320 + i, 0);
		}
	}

	for (int i = 0; i < 180; ++i) {
		if (image.at<uchar>(i, 0) == 255) {
			return Point(0, i);
		}

		if (image.at<uchar>(i, 639) == 255) {
			return Point(639, i);
		}
	}

	return Point(-1, -1);
}

Point next_point2(Mat image)
{
	for (int i = 0; i < 320; ++i) {
		if (image.at<uchar>(359, 320 - i) == 255) {
			return Point(320 - i, 359);
		}

		if (image.at<uchar>(359, 320 + i) == 255) {
			return Point(320 + i, 359);
		}
	}


	return Point(-1, -1);
}

double speedTran(int val, int ref)
{
	double dev = double(val - ref) / ref;
	double res = 0;
	bool flag = 0;

	if (dev<0) { flag = 1; dev = -dev; }

	if (dev <= 0.2 && dev >= 0)
		res = -0;
	else if (dev <= 0.4 && dev >= 0.2)
		res = -0.5;
	else if (dev <= 0.6 && dev >= 0.4)
		res = -0.8;
	else if (dev <= 1.0 && dev >= 0.6)
		res = -1.0;

	res -= lastRes*0.2;


	if (flag) res = -res;

	lastRes = res;
	return res;
}
