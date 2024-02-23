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
using namespace std;
using namespace cv;
static void help() {
	cout
	<< "------------------------------------------------------------------------------" << endl
	<< "This program shows how to read a video file with OpenCV. In addition, it "
	<< "tests the similarity of two input videos first with PSNR, and for the frames "
	<< "below a PSNR trigger value, also with MSSIM."                                   << endl
	<< "Usage:"                                                                         << endl
	<< "./video-input-psnr-ssim <referenceVideo> <useCaseTestVideo> <PSNR_Trigger_Value> <Wait_Between_Frames> " << endl
	<< "--------------------------------------------------------------------------"     << endl
	<< endl;
}
int main(int argc, char *argv[]) {
	help();
	char *outText;
	string output;
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	const string sourceReference = argv[1];
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
	moveWindow(WIN_RF, 400       , 0);         //750,  2 (bernat =0)
	cout << "Reference frame resolution: Width=" << refS.width << "  Height=" << refS.height << " of nr#: " << captRefrnc.get(CAP_PROP_FRAME_COUNT) << endl;
	Mat frameReference;
	Mat cpy;
	Mat cropped;
	for(;;) { //Show the image captured in the window and repeat
		captRefrnc >> frameReference;
		if (frameReference.empty()) {
			cout << " < < <  Game over!  > > > ";
			break;
		}
		++frameNum;
		// copiar frame
		cpy = frameReference.clone();
		// Setup a rectangle to define your region of interest
		Rect myROI(0, 960, 960, 120);
		// Crop the full image to that image contained by the rectangle myROI
		Mat croppedRef(cpy, myROI);
		// Copy the data into new matrix
		croppedRef.copyTo(cropped);
		//imshow(WIN_RF, frameReference);
		imshow(WIN_RF, cropped);
		api->SetImage((uchar*)cropped.data, cropped.size().width, cropped.size().height, cropped.channels(), cropped.step1());
		api->Recognize(0);
		outText = api->GetUTF8Text();
		output = string(outText);
		cout << output << endl;
		char c = (char)waitKey(delay);
		if (c == 27) break;
	}
	return 0;
}
