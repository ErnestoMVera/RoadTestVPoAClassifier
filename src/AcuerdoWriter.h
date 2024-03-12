#ifndef ACUERDOWRITER_H 
#define ACUERDOWRITER_H
#include<string>
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
class AcuerdoWriter {
	public:
		const char* filename;
		AcuerdoWriter(std::string filename);
		void writeHeader();
		void writeNextPoint(unsigned long long int timestamp, std::string vpoaDataset, std::string vpoaVideo);
		void closeFile();
	private:
		FILE* csvFilefp;
};
#endif
