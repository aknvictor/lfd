/*
 * Main.cpp
 *
 *  Created on: Dec 31, 2014
 *      Author: jrparks
 *      Author: https://github.com/jrjparks
 * 
 * 	Extended for learning for demonstration project by
 *  Victor Akinwande
 * 	Feburary 2018
 */

#include "Main.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {
	setenv("DISPLAY", ":0", 0);

	fprintf(stdout, "Preparing to open camera.\n");

	camera.Open("/dev/video1");
	camera2.Open("/dev/video2");
	
	if (!camera.IsOpen()) {
		fprintf(stderr, "Unable to open camera.\n");
		return -1;
	}

	if (!camera2.IsOpen()) {
		fprintf(stderr, "Unable to open camera2.\n");
		return -1;
	}

	camera.ChangeCaptureSize(v4l2::CAPTURE_SIZE_800x600);

	camera.GetBrightness(Brightness);
	camera.GetContrast(Contrast);
	camera.GetSaturation(Saturation);
	camera.GetSharpness(Sharpness);
	camera.GetGain(Gain);
	camera.GetBacklightCompensation(BacklightCompensation);
	camera.GetWhiteBalanceTemperature(WhiteBalanceTemperature);
	++WhiteBalanceTemperature;
	camera.GetFocus(Focus);
	++Focus;

	camera2.ChangeCaptureSize(v4l2::CAPTURE_SIZE_800x600);

	camera2.GetBrightness(Brightness);
	camera2.GetContrast(Contrast);
	camera2.GetSaturation(Saturation);
	camera2.GetSharpness(Sharpness);
	camera2.GetGain(Gain);
	camera2.GetBacklightCompensation(BacklightCompensation);
	camera2.GetWhiteBalanceTemperature(WhiteBalanceTemperature);
	++WhiteBalanceTemperature;
	camera2.GetFocus(Focus);
	++Focus;

	cv::namedWindow("Adjustments", CV_WINDOW_NORMAL);
	cv::createTrackbar("Brightness", "Adjustments", &Brightness, 255);
	cv::createTrackbar("Contrast", "Adjustments", &Contrast, 255);
	cv::createTrackbar("Saturation", "Adjustments", &Saturation, 255);
	cv::createTrackbar("Sharpness", "Adjustments", &Sharpness, 255);
	cv::createTrackbar("Gain", "Adjustments", &Gain, 255);
	cv::createTrackbar("Backlight Compensation", "Adjustments", &BacklightCompensation, 1);
	
	// Off by one to account for -1 being auto.
	cv::createTrackbar("White Balance Temperature", "Adjustments", &WhiteBalanceTemperature, 6501);
	cv::createTrackbar("Focus", "Adjustments", &Focus, 256);

	int wait_key = 0;

	Ptr<Tracker> tracker;
	tracker = TrackerMIL::create();
	Rect2d bbox(287, 23, 86, 320);

	while (true) {

		camera.SetBrightness(Brightness);
		camera.SetContrast(Contrast);
		camera.SetSaturation(Saturation);
		camera.SetSharpness(Sharpness);
		camera.SetGain(Gain);
		camera.SetBacklightCompensation(BacklightCompensation);
		--WhiteBalanceTemperature;
		camera.SetWhiteBalanceTemperature(WhiteBalanceTemperature);
		++WhiteBalanceTemperature;
		--Focus;
		camera.SetFocus(Focus);
		++Focus;
		bool ok;
		bool init;

		if (camera.GrabFrame()) {

			if (camera.RetrieveMat(frame))
			{	
				// if(!init){ init = true; tracker->init(frame, bbox);}
				// if (ok) rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
				// else ok = tracker->update(frame, bbox);

				cv::imshow("Camera", frame);
			}

		} else {

			fprintf(stderr, "Unable to grab frame from camera.\n");
		
		}

		camera2.SetBrightness(Brightness);
		camera2.SetContrast(Contrast);
		camera2.SetSaturation(Saturation);
		camera2.SetSharpness(Sharpness);
		camera2.SetGain(Gain);
		camera2.SetBacklightCompensation(BacklightCompensation);
		--WhiteBalanceTemperature;
		camera2.SetWhiteBalanceTemperature(WhiteBalanceTemperature);
		++WhiteBalanceTemperature;
		--Focus;
		camera2.SetFocus(Focus);
		++Focus;

		if (camera2.GrabFrame()) {
			if (camera2.RetrieveMat(frame2))
				cv::imshow("Camera2", frame2);
		} else {
			fprintf(stderr, "Unable to grab frame from camera 2.\n");
		}

		wait_key = cv::waitKey(30);
		if (wait_key == 27 || wait_key == 32) //esc or space
			break;
	}

	fprintf(stdout, "Closing cameras.\n");
	camera.Close();
	camera2.Close();
	
}
