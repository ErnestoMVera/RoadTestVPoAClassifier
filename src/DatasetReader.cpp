#include "DatasetReader.h"
DatasetReader::DatasetReader(std::string filename) {
	csvFilefp = fopen(filename.c_str(), "r");
	if(csvFilefp == NULL) {
		std::cout << "Error opening the file " << filename << std::endl;
		return;
	}
	nextpoint();
}
void DatasetReader::nextpoint() {
	size_t bytestoread = BUFFERSIZE;
	size_t bytesread;
	char* line = NULL;
	char* ts;
	char* templine;
	const char* delim = ",";
	char* point;
	bytesread = getline(&line, &bytestoread, csvFilefp);
	strncpy(currentline, line, BUFFERSIZE);
	free(line);
	templine = &(currentline[0]);
	ts = strsep(&templine, delim);
	point = strsep(&templine, delim);
	timestamp = atoll(ts);
	switch(point[0]) {
		case 'C':
			currentVpoa = CAMINO;
			break;
		case 'I':
			currentVpoa = IZQUIERDA;
			break;
		case 'D':
			currentVpoa = DERECHA;
			break;
		case 'P':
			currentVpoa = PIERNAS;
			break;
		case 'A':
			currentVpoa = AUDIOCLIMA;
			break;
	}
}
unsigned long long int DatasetReader::getTimestampMilis() {
	return timestamp;	
}
std::string DatasetReader::getTimestampFormatted() {
	time_t ts_local = timestamp/1000;
	struct tm *t = localtime(&ts_local);
	strftime(tsFormatted, BUFFERSIZE - 1, "%a %b %d %H:%M:%S %Y", t);
	std::string output(tsFormatted);
	return output;
}
std::string DatasetReader::getTimestampHour() {
	time_t ts_local = timestamp/1000;
	struct tm *t = localtime(&ts_local);
	strftime(tsFormatted, BUFFERSIZE - 1, "%H:%M:%S", t);
	std::string output(tsFormatted);
	return output;
}
enum VPOA DatasetReader::getVPoA() {
	return currentVpoa;
}
