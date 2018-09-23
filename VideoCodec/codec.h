#pragma once
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <fstream>
#include <stdint.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>

// -- Enable/Disable Debug Mode
//#define _DEBUGMODE
#define I_FRAME_INTERVAL 6

using namespace std;
using namespace cv;


// Structs related to Motiton Vector
struct MV {
	int x;
	int y;
};

struct MVholder {
	vector<MV> vec[3];
};


class Encoder  {
public:	
	void Encode ( string filename,int Nframes );

private:	
	MVholder GetMV ( Mat refFrm, Mat predFrm );
	uint8_t cost ( Mat& crrentblk, Mat& refblk, int blk_size );
	Mat MotionComp ( Mat RefImg, MVholder MVH, int blk_size );
	Mat blockDCT ( Mat input, int blockSize );
	Mat blockIDCT ( Mat input, int blockSize );
	Mat intraDecode ( Mat img ,int Qparam ,int blkSize);
};

