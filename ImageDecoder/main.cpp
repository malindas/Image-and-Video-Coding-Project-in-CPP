#include <iostream>
#include "coolCodec.h"

using namespace std;

int main ( int argc, char** argv ) {
	coolCodec codec;
	codec.sig ( );
	
	switch (argc) {
	case 1:
		cout << "\t\t______CoolCodec Help______" << endl;
		cout << "\t\tDecoderx64 (filename).uop" << endl;
		break;
	case 3:
		codec.imDecode ( argv[1], argv[2] );
		break;
	default:
		cout << "Wrong number of Inputs" << endl;
		break;
	}


	

	return 0;

}