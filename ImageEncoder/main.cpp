#include "coolCodec.h"
#include <fstream>
#include <string>

using namespace std;
string scan ( string upto, ifstream& file) {
	string buf;
	while (buf != upto) {
		file >> buf;
	}
	file >> buf;

	return buf;
}

int main ( int argc, char** argv ) {

	ifstream file ( "Config.cfg", ios::in | ios::binary );
	int blockSize;
	string buf;
	string inputfilename;
	string outputfilename;
	String quality;
	imQuality q;

	if (file.is_open() == true) {
		blockSize = stoi ( scan ( "ScanBlockSize", file ) );
		inputfilename = scan ( "InputFile", file );
		outputfilename = scan ( "OutputFile", file );
		quality = scan ( "Quality", file );
	}
	else {
		cout << "Config file not found " << endl; 
		cin.get ( );
		return -1;
	}
	
	if (quality == "LOW")
		q = LOW_QUALITY;
	else if (quality == "MODARATE")
		q = MODARATE_QUALITY;
	else if (quality == "HIGH")
		q = HIGH_QUALITY;
	
	coolCodec codec;
	codec.sig ( );
	codec.setEncoderParam (blockSize,q);
	codec.imEncode (inputfilename,outputfilename);
	//codec.imDecode ( "output1.uop" ,"myFile.jpg");
	cin.get ( );
	file.close ( );
	return 0;
}