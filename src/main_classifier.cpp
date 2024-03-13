#include<iostream> // for standard I/O
#include<string>   // for strings
#include<iomanip>  // for controlling float print precision
#include<sstream>  // string to number conversion
#include<algorithm>  // string to number conversion
#include<opencv2/core.hpp>     // Basic OpenCV structures (cv::Mat, Scalar)
#include<opencv2/imgproc.hpp>  // Gaussian Blur
#include<opencv2/videoio.hpp>
#include<opencv2/highgui.hpp>  // OpenCV window I/O
#include<tesseract/baseapi.h>
#include<leptonica/allheaders.h>
#include<unistd.h>
#include<getopt.h>
#include<ctype.h>
#include<cmath>
#include<string.h>
#include"DatasetReader.h"
#include"AcuerdoWriter.h"
using namespace std;
using namespace cv;
void usage();
void crop_image(Mat* src, Mat* dest, int x, int y, int w, int h); 
string recognize_first_frame(string videofilename, int* count);
string recognize_ts(tesseract::TessBaseAPI* api, Mat* image);
int isValidTimestamp(string ts);
string trim_string(string s); 
void offsetdataset(int offset, int freq, DatasetReader* reader);
void usage() {
	cout << "usage is classifier <video-file> <dataset-file>" << endl;
	cout << "use -f or --firsttimestamp option to get first timestamp in a dataset file e.g. \n classifier -f <video-file> <dataset-file>" << endl;
	cout << "use -o or --offset option to set an offset in the video file e.g. \n classifier -o n <video-file> <dataset-file>" << endl;
	cout << "use -d or --datasetoffset option to set an offset in the dataset file e.g. \n classifier -d n <video-file> <dataset-file>" << endl;
	cout << "use -v or --writevideo option to generate a classified video file e.g. \n classifier -v <video-file> <dataset-file>" << endl;
	cout << "use -c or --classify option to generate a classified csv file e.g. \n classifier -c <video-file> <dataset-file>" << endl;
	cout << "use the spacebar to pause the process while classifying." << endl;
}
string trim_string(string s) {
	string::iterator end_pos = std::remove(s.begin(), s.end(), ' ');
	s.erase(end_pos, s.end());
	string::iterator end_pos1 = std::remove(s.begin(), s.end(), '\n');
	s.erase(end_pos1, s.end());
	return s;
}
void offsetdataset(int offset, int freq, DatasetReader* reader) {
	int i = 0;
	for(i = 0; i < offset*freq; i++)
		reader->nextpoint();
}
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
string recognize_first_frame(string videofilename, int offset, int* count) {
	Mat frame, cropped;
	Mat onlyhr;
	int fps = 60;
	string output;
	string outhr;
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	VideoCapture captRefrnc(videofilename); 
	if (!captRefrnc.isOpened()) {
		cout  << "Could not open video file " << videofilename << endl;
		exit(1);
	}
	captRefrnc.set(CAP_PROP_POS_FRAMES, offset*fps);
	captRefrnc >> frame;
	crop_image(&frame, &cropped, 0, 960, 960, 120);
	crop_image(&frame, &onlyhr, 420, 960, 335, 120);
	outhr = recognize_ts(api, &onlyhr);
	while(!isValidTimestamp(outhr)) {
		captRefrnc >> frame;
		crop_image(&frame, &onlyhr, 420, 960, 335, 120);
		outhr = recognize_ts(api, &onlyhr);
		(*count)++;
	}
	crop_image(&frame, &cropped, 0, 960, 960, 120);
	output = recognize_ts(api, &cropped);
	if(api) delete api;
	return output;
}
string recognize_ts(tesseract::TessBaseAPI* api, Mat* image) {
	char *outText;
	// set image to recognize and recognize
	api->SetImage((uchar*)image->data, image->size().width, image->size().height, image->channels(), image->step1());
	api->Recognize(0);
	outText = api->GetUTF8Text();
	return string(outText);
}
string mapVPoA(enum VPOA vpoa) {
	switch(vpoa) {
		case CAMINO:
		return "CAMINO";
		break;
		case IZQUIERDA:
		return "IZQUIERDA";
		break;
		case DERECHA:
		return "DERECHA";
		break;
		case PIERNAS:
		return "PIERNAS";
		break;
		case AUDIOCLIMA:
		return "AUDIOCLIMA";
		break;
		default:
		return "???";
	}
}
int isValidTimestamp(string ts) {
	int i;
	string::iterator end_pos = std::remove(ts.begin(), ts.end(), ' ');
	ts.erase(end_pos, ts.end());
	string::iterator end_pos1 = std::remove(ts.begin(), ts.end(), '\n');
	ts.erase(end_pos1, ts.end());
	// the expected format is 00:00:00
	if(ts.size() != 8) {
		return 0;
	}
	else {
		for(i = 0; i < 8; i++) {
			if(i == 2 || i == 5) {
				if(ts[i] != ':') return 0;	
			}
			else {
				if(!isdigit(ts[i])) return 0;
			}
		}
	}
	return 1;
}
int main(int argc, char *argv[]) {
	int opt, offset = 0, offset_d = 0;
	int fps = 60;
	int freqSistema = 40, writevideo = 0, classify = 0;
	int frameNum = -1, delay = 10;          // Frame counter
	int pause_flag = 0;
	string sourceReference, datasetFile;
	if(argc < 2) {
		usage();
		return 0;
	}
	sourceReference = argv[argc - 2];
	datasetFile = argv[argc - 1];
	// parse options
	// Get first timestamp in file with -f option
	while(true) {
		int option_index;
		struct option long_options[] = {
			{"firsttimestamp", no_argument, 0, 'f'},
			{"offset", required_argument, 0, 'o'},
			{"datasetoffset", required_argument, 0, 'd'},
			{"classify", no_argument, 0, 'c'},
			{"writevideo", no_argument, 0, 'v'}
		};
		opt = getopt_long(argc, argv, "fo:d:vc", long_options, &option_index);
		if(opt == -1) {
			break;
		}
		switch(opt) {
			case 'f': {
				int count = 0;
				string framets;
				sourceReference = argv[argc - 2];
				datasetFile = argv[argc - 1];
				DatasetReader readerOpt(datasetFile);
				readerOpt.nextpoint();
				offsetdataset(offset_d, freqSistema, &readerOpt);
				framets = recognize_first_frame(sourceReference, offset, &count);
				cout << "Timestamp dataset: " << readerOpt.getTimestampFormatted() << endl;
				cout << "Timestamp video:   " << framets << "after " << count << " frames" << endl;
				return 0;
				break;
			}
			case 'o':
				offset = atoi(optarg);
				break;
			case 'd':
				offset_d = atoi(optarg);
				break;
			case 'c':
				classify = 1;
				delay = 50;
				break;
			case 'v':
				writevideo = 1;
				break;
			case '?':
				usage();
				return 0;
				break;		
			default:
				usage();
				return 0;
				break;
		}
	}
	string output;
	tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
	DatasetReader reader(datasetFile);
	// Initialize tesseract-ocr with English, without specifying tessdata path
	if (api->Init(NULL, "eng")) {
		fprintf(stderr, "Could not initialize tesseract.\n");
		exit(1);
	}
	VideoCapture captRefrnc(sourceReference); 
	if (!captRefrnc.isOpened()) {
		cout  << "Could not open reference " << sourceReference << endl;
		return -1;
	}
	Size refS = Size((int) captRefrnc.get(CAP_PROP_FRAME_WIDTH), (int) captRefrnc.get(CAP_PROP_FRAME_HEIGHT));
	const char* WIN_RF = "Reference";
	// Windows
	namedWindow(WIN_RF, WINDOW_AUTOSIZE);
	moveWindow(WIN_RF, 0, 0); 
	Mat frameReference;
	Mat cropped;
	string time;
	captRefrnc.set(CAP_PROP_POS_FRAMES, offset*fps);
	string last_out;
	reader.nextpoint();
	offsetdataset(offset_d, freqSistema, &reader);
	captRefrnc >> frameReference;
	crop_image(&frameReference, &cropped, 420, 960, 335, 120);
	output = recognize_ts(api, &cropped);
	output = trim_string(output);
	time = reader.getTimestampHour();
	if(isValidTimestamp(output)) {
		cout << "OCR: " << output << endl;
		cout << "DATASET: " << reader.getTimestampHour() << reader.getVPoA() << endl;
		if(output.substr(6, 2) != time.substr(6, 2)) {
			cout << "TIMESTAMPS NO SINCRONIZADOS" << endl;
			if(api) delete api;
			return 0;
		}
	}
	else {
		cout << "TIMESTAMP DE VIDEO NO VALIDADO CORRECTAMENTE ";
		cout << "OCR: " << output << " (invalid)" <<endl;
		if(api) delete api;
		return 0;
	}
	char tsbuffer[16];
	int vfourcc;
	if(writevideo) {
		vfourcc = VideoWriter::fourcc('m', 'p', '4', 'v');
	}
	VideoWriter videoToWrite("output.avi", vfourcc, fps, frameReference.size());
	AcuerdoWriter writer("classified.csv");
	string human;
	for(;;) { 
		char c = (char)waitKey(delay);
		if (c == 82) human = "CAMINO";
		else if (c == 81) human = "IZQUIERDA";
		else if (c == 83) human = "DERECHA";
		else if (c == 84) human = "PIERNAS";
		else if (c == 'a') human = "AUDIOCLIMA";
		else if (c == 32) pause_flag = !pause_flag;
		else if (c == 27) break;
		while(pause_flag) {
			c = (char)waitKey(delay);
			if (c == 32) pause_flag = !pause_flag;
		}
		putText(frameReference, human, Point2d(50, 300), FONT_HERSHEY_SIMPLEX, 2, 50, 3);
		if (frameReference.empty()) {
			cout << " < < <  Game over!  > > > ";
			break;
		}
		++frameNum;
		// cut the image so that only the timestamp is focused
		// THIS LINE CROPS TO THE ENTIRE DATE AND TIME
		// crop_image(&frameReference, &cropped, 0, 960, 960, 120);
		// THIS LINE CROPS ONLY THE TIME
		crop_image(&frameReference, &cropped, 420, 960, 335, 120);
		//imshow(WIN_RF, cropped);
		//imshow(WIN_RF, frameReference);
		time = reader.getTimestampHour();
		if(frameNum % 3 < 2) {
			reader.nextpoint();
			snprintf(&tsbuffer[0], (int) log(reader.getTimestampMilis()), "%lld", reader.getTimestampMilis());
			if(classify) {
				writer.writeNextPoint(reader.getTimestampMilis(), mapVPoA(reader.getVPoA()), human);
			}
			else {
				putText(frameReference, tsbuffer, Point2d(50, 50), FONT_HERSHEY_SIMPLEX, 2, 255, 3);
				putText(frameReference, mapVPoA(reader.getVPoA()), Point2d(50, 150), FONT_HERSHEY_SIMPLEX, 2, 255, 3);
			}
		}
		if(writevideo) {
			videoToWrite.write(frameReference);
		}
		imshow(WIN_RF, frameReference);
		captRefrnc >> frameReference;
	}
	if(writevideo) {
		videoToWrite.release();
	}
	if(classify) {
		writer.closeFile();
	}
	if(api) delete api;
	return 0;
}
