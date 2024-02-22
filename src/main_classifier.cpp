#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc.hpp>  // Gaussian Blur
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>  // OpenCV window I/O
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
	for(;;) { //Show the image captured in the window and repeat
		captRefrnc >> frameReference;
		if (frameReference.empty()) {
			cout << " < < <  Game over!  > > > ";
			break;
		}
		++frameNum;
		cout << "Frame: " << frameNum << "# ";
		cout << endl;
		imshow(WIN_RF, frameReference);
		char c = (char)waitKey(delay);
		if (c == 27) break;
	}
	return 0;
}
