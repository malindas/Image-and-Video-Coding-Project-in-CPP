#include <iostream>
#include "codec.h"

using namespace std;


int main (int argc, char** argv) {

	Encoder myecoder;
	VideoCapture cap;
	

	switch (argc) {
	case 1:
		cout << "\t\t______CoolCodec Help______" << endl;
		cout << "\t\tVideo_encoderx64 <filename> <Nframes>.uop" << endl;
		break;
	case 3:
		myecoder.Encode ( argv[1], stoi(argv[2]));
		break;
	default:
		cout << "Wrong number of Inputs" << endl;
		break;
	}

	cin.get ( );
	return 0;
}