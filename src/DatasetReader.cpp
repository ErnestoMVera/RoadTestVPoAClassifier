#include "DatasetReader.h"
DatasetReader::DatasetReader(std::string filename) {
	DatasetReader::filename = filename;
}
void DatasetReader::getHeader() {
	
}
unsigned long long int DatasetReader::getTimestampMilis() {
	return 0;	
}
std::string DatasetReader::getVPoA() {
	return filename;
}
