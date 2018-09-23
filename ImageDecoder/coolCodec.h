#pragma once
//-- Includes
#include <iostream>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <fstream>
#include <stdint.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

// -- Enable/Disable Debug Mode
#define _DEBUGMODE

// -- Pre Processor definitions
#define I_FRAME_INTEVAL  6 // Interval between 2 I_frames
#define P_FRAME_INTEVAL  3 // Interval between I_frame and P_frame 

using namespace std;
using namespace cv;

// --  Structures
struct huff_tree {
	string  symbol;
	double  probability;
	vector<huff_tree> child;
	bool  code;
};

struct code_book {
	int idx;
	map<string, vector<bool>> code;
	vector<bool> acc;
};

// -- Enumerations
enum imQuality :uint16_t
{
	HIGH_QUALITY = 50,
	MODARATE_QUALITY = 100,
	LOW_QUALITY = 500
};


/*_____________________coolCodec Class____________________*/
 class coolCodec {

	public:	
		coolCodec ( ) /*Constructor*/
			:block_size ( 8 ), qFactor ( HIGH_QUALITY ),
			Write_marker ( 0 ),Read_marker( 0 ),Width( 0 ), Height( 0 )

		{}
		
		void imEncode(string filename, string outfile); 
		void imDecode(string filename,string outfile); 
		void vEncode(string &filename);
		void vDecode(string filename);
		void sig(void);
		void setEncoderParam(int Block_size,imQuality Quality);
		
		~coolCodec ( ) /*Destructor*/
		{
		}



	private:
		map<string, vector<bool>> Huffman_dict(Mat image,string filename);
		vector<huff_tree> h_sort(vector<huff_tree> tree);
		code_book get_code(code_book book,vector<huff_tree> tree);
		void echo(string msg);
		Mat blockDCT(Mat input,int blockSize);
		Mat blockIDCT(Mat input, int blockSize);
		void HuffmanEncode ( map<string, vector<bool>> dictionary, Mat image, string filename );
		void HuffmanDecode ( map<string, int> dict, Mat &img, string filename );
		void ReadHuffmanDict ( string filename );
	//--Private Variables
		int block_size;
		vector<huff_tree> tree;
		imQuality qFactor;
		map<string, int> dict[3];
		streampos Read_marker;
		streampos Write_marker;
		int Width, Height;
		
};


