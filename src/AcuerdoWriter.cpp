#include "AcuerdoWriter.h"
AcuerdoWriter::AcuerdoWriter(std::string filename) {
	csvFilefp = fopen(filename.c_str(), "w");
	if(csvFilefp == NULL) {
		std::cout << "Error opening the file " << filename << std::endl;
		return;
	}
	writeHeader();
}
void AcuerdoWriter::writeHeader() {
	fprintf(csvFilefp, "timestamp,datasetvpoa,videovpoa\n");
}
void AcuerdoWriter::writeNextPoint(unsigned long long int timestamp, std::string vpoaDataset, std::string vpoaVideo) {
	fprintf(csvFilefp, "%lld,%s,%s\n", timestamp, vpoaDataset.c_str(), vpoaVideo.c_str());	
}
void AcuerdoWriter::closeFile() {
	fclose(csvFilefp);
}
