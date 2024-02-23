#ifndef DATASETREADER_H
#define DATASETREADER_H
#include<string>
#include<sstream>
#include<fstream>
#include<iostream>

class DatasetReader {
	public:
		std::string filename;
		DatasetReader(std::string filename);
		void getHeader();
		unsigned long long int getTimestampMilis();
		std::string getVPoA();
};
#endif
