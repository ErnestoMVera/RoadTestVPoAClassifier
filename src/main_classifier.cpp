#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "DatasetReader.h"
using namespace std;
using namespace cv;
void crop_image(Mat* src, Mat* dest, int x, int y, int w, int h); 
void crop_image(Mat* src, Mat* dest, int x, int y, int w, int h) {
		// copiar frame
		Mat cpy = src->clone();
		// Setup a rectangle to define your region of interest
		Rect myROI(x, y, w, h);
		// Crop the full image to that image contained by the rectangle myROI
		Mat croppedRef(cpy, myROI);
		// Copy the data into new matrix
		croppedRef.copyTo(*dest);
}
string recognize_ts(tesseract::TessBaseAPI* api, Mat* image) {
	char *outText;
	// set image to recognize and recognize
	api->SetImage((uchar*)image->data, image->size().width, image->size().height, image->channels(), image->step1());
	api->Recognize(0);
	outText = api->GetUTF8Text();
	return string(outText);

}
int main(int argc, char *argv[]) {
	string output;
	if(argc < 2) {
		cout << "usage is classifier <video-file> <dataset-file>" << endl;
		return 0;
	}
	const string sourceReference = argv[1];
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	int frameNum = -1, delay = 10;          // Frame counter
	VideoCapture captRefrnc(sourceReference); 
	if (!captRefrnc.isOpened()) {
		cout  << "Could not open reference " << sourceReference << endl;
		return -1;
	}
	Size refS = Size((int) captRefrnc.get(CAP_PROP_FRAME_WIDTH), (int) captRefrnc.get(CAP_PROP_FRAME_HEIGHT));
	const char* WIN_RF = "Reference";
	// Windows
	namedWindow(WIN_RF, WINDOW_AUTOSIZE);
	//moveWindow(WIN_RF, 400       , 0);         //750,  2 (bernat =0)
	cout << "Reference frame resolution: Width=" << refS.width << "  Height=" << refS.height << " of nr#: " << captRefrnc.get(CAP_PROP_FRAME_COUNT) << endl;
	Mat frameReference;
	Mat cropped;
	for(;;) { 
		//Show the image captured in the window and repeat
		captRefrnc >> frameReference;
		if (frameReference.empty()) {
			cout << " < < <  Game over!  > > > ";
			break;
		}
		++frameNum;
		// cut the image so that only the timestamp is focused
		crop_image(&frameReference, &cropped, 0, 960, 960, 120);
		//imshow(WIN_RF, cropped);
		imshow(WIN_RF, frameReference);
		output = recognize_ts(api, &cropped);
		cout << output << endl;
		char c = (char)waitKey(delay);
		if (c == 27) break;
	}
	if(api) delete api;
	return 0;
}
