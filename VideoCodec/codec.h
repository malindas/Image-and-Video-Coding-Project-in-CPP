#pragma once

/*This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <https://www.gnu.org/licenses/>.

________________________________________________________
|	Authour  : Malinda Sulochana Silva		|
|	Institute: Dept. of Electrical and Electronic	|
|		   Faculty of Engingeering		|
|		   Uinversity of Peradeniya.		|
|							|
|	 Modified: 2018.07.13				|
|_______________________________________________________|
*/

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

