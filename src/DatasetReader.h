#ifndef DATASETREADER_H
#define DATASETREADER_H
#include<string>
#include<sstream>
#include<fstream>
#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define BUFFERSIZE 64 
// Dataset structure is
// timestamp (ulong int), spot (string), speed (float), comportamiento (string), tarea_secundaria (string), grupo (grupo)
enum VPOA {
	CAMINO,
	IZQUIERDA,
	DERECHA,
	PIERNAS,
	AUDIOCLIMA
};
class DatasetReader {
	public:
		const char* filename;
		DatasetReader(std::string filename);
		void nextpoint();
		unsigned long long int getTimestampMilis(); // timestamp
		enum VPOA getVPoA(); // spot
		std::string getTimestampFormatted(); // timestamp
	private:
		char currentline[BUFFERSIZE];
		char tsFormatted[BUFFERSIZE];
		enum VPOA currentVpoa;
		FILE* csvFilefp;
		unsigned long long int timestamp;
};
#endif
