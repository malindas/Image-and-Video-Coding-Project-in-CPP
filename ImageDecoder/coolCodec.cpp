#include "coolCodec.h"


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


/*
*-----------------------------------------------------
*	Breif:	Generates the Huffman dictionary
*	param:	(double) probability, (int) symbol,(int) size
*   ret  :	map<int, bool> map to huffman Dictionary
*=====================================================
*/

map<string, vector<bool>> coolCodec::Huffman_dict ( Mat image, string filename )
{
	huff_tree temptree;
	
	Mat img = image.clone ( );
	double min, max, min1, max1;

	double * prob = (double *)calloc ( image.rows * image.cols, sizeof ( double ) );
	double * ptr;

	// Reshape image into an array
	img = img.reshape ( 1, img.rows * img.cols );
	minMaxLoc ( img, &min1, &max1 );
	// Shift symbols above negative
	for (size_t i = 0; i < img.size().height; i++)
	{
			img.at<int16_t> ( i, 0) = img.at<int16_t> ( i, 0 ) - (int16_t)min1;	
	}

	minMaxLoc ( img, &min, &max );
	ptr = prob;
	cout << min << endl;
	cout << max << endl;

	// Determining the occurances of each symbol
	for (size_t i = 0; i < img.rows; i++)
	{
		int16_t val = img.at<int16_t> ( i, 0 );
		if (val <= max && val >= min) {
			ptr = prob + val;
			*ptr = *ptr + 1;
		}
	}

	int * symbol = (int *)calloc ( img.rows, sizeof ( int ) );
	int * sptr = symbol;
	int * probability = (int *)calloc ( img.rows, sizeof ( int ) );
	int * pptr = probability;

	ptr = prob;
	int sz = 1;
	for (size_t i = 0; i < img.rows; i++)
	{
		if (*ptr != 0) {
			*pptr = *ptr;
			*sptr = i + min1;
				pptr++; sptr++;
				sz++;
		}
		ptr++;
	}

	pptr = probability;
	sptr = symbol;
	for (int i = 0; i < sz; i++) {
		temptree.symbol = to_string(*sptr);
		temptree.probability = *pptr;//probability[i];
		temptree.code = 0;
		tree.push_back(temptree);
		ptr++;
		sptr++;
	}
	

	tree = h_sort(tree);


	int i = 0;
	while (tree.size() > 1) {
		tree = h_sort(tree);

		huff_tree newNode;
		tree[0].code = 1;
		tree[1].code = 0;

		newNode.probability = tree[0].probability + tree[1].probability;
		newNode.symbol = to_string(i) + "(" + tree[0].symbol + "," + tree[1].symbol + ")";

		newNode.child.push_back(tree[0]);
		newNode.child.push_back(tree[1]);

		tree.erase(tree.begin(), tree.begin() + 2);
		tree.push_back(newNode);

		tree = h_sort(tree);
		i = i + 1;
	}


	code_book cb;
	cb = get_code(cb, tree);

	ofstream file ( filename, ios::out | ios::app);
	stringstream outputs;
	file.seekp ( this->Write_marker );
	for (map<string, vector<bool>>::iterator it = cb.code.begin(); it != cb.code.end();it++) {
#ifdef _DEBUGMODE		
		cout << it->first << "::\t\t";		
#endif	
		file << it->first << ' ';
		for (vector<bool>::iterator inner = it->second.begin(); inner != it->second.end(); inner++) {
#ifdef _DEBUGMODE		
			cout << *inner;
#endif	
			file << *inner;
		}
#ifdef _DEBUGMODE		
		cout << endl;
#endif	
		file << ' ';
	}
	file << 's' << 'e' << 'c' << 't' << 'n' << ' ';
	this->Write_marker = file.tellp ( );
	file.close ( );
	tree.clear ( );
	return cb.code;
}


/*
*-----------------------------
*	Breif:	sort Huffman tree
*	param:	vector<huff_tree>  htree
*   ret  :	vector<huff_tree>
*=============================
*/
vector<huff_tree> coolCodec::h_sort(vector<huff_tree>  htree)
{
	huff_tree temp;
	for (int i = 1;i < htree.size();i++) {
		for (int j = 1;j < htree.size();j++) {
			if (htree[i].probability < htree[j].probability) {
				temp = htree[i];
				htree[i] = htree[j];
				htree[j] = temp;
			}
		}
	}
	return  htree;
}

/*
*----------------------------------------------------
*	Breif:	Track the Huffman code along the tree
*	param:	code_book book, vector<huff_tree> tree
*   ret  :	code_book
*====================================================
*/
code_book coolCodec::get_code(code_book book, vector<huff_tree> tree)
{
	for (int j = 0; j < tree.size(); j++) {
		if (tree[j].child.size() == 0) {
			book.acc.push_back(tree[j].code);

			code_book final_acc;
			for (int i = 0; i < (book.acc.size() - 1); i++) {
				final_acc.acc.push_back(book.acc[i + 1]);
			}

			book.code.insert(make_pair(tree[j].symbol, final_acc.acc));
			book.acc.pop_back();
		}
		else {
			book.acc.push_back(tree[j].code);
			book = get_code(book, tree[j].child);
			book.acc.pop_back();
		}

	}

	return book;
}

/*
*----------------------------------
*	Breif:	Echo to the std output
*	param:	string msg
*   ret  :	void
*==================================
*/
void coolCodec::echo(string msg)
{
	cout << msg << endl;
}

/*
*------------------------------------
*	Breif:	Block  DCT
*	param:	Mat input, int blockSize
*   ret  :	Mat
*====================================
*/
Mat coolCodec::blockDCT(Mat input, int blockSize)
{
	int Width, Height;	
	
	Width = input.size().width;
	Height = input.size().height;

	int new_width = (int)((Width / blockSize)+1) * blockSize;
	int new_height = (int)((Height / blockSize)+1) * blockSize;
	
	cout << new_width << endl;
	cout << Width << endl;

	Mat output;
	input.copyTo ( output );
	resize ( output, output, Size ( new_width, new_height ), 0, 0 );
	output.convertTo ( output, CV_32FC1, 1 );

#ifdef _DEBUGMODE
	cout << "\n...Extra Padding..." << endl;
	cout << "Horizontal:: " << new_width - Width << " px" << endl;
	cout << "Vertical:: " << new_height - Height << " px" << endl;
#endif
	this->Width = new_width;
	this->Height = new_height;

	int wlen = (int)(new_width - block_size);
	int hlen = (int)(new_height - block_size);
	
	if (Width == 0 || Height == 0)
		return Mat();


	for (size_t i = 0; i <= wlen; i += blockSize)
	{
		for (size_t j  = 0; j  <= hlen; j += blockSize)
		{
			Mat mask = output(Rect(j,i, blockSize, blockSize));
			dct(mask, mask,0); 	
			mask.copyTo(output(Rect(j,i, blockSize, blockSize)));
		}

	}
	//cout << output(Rect(0, 0, wlen, 8)) << "," << endl;
	
	return output;
}

/*
*------------------------------------
*	Breif:	Block  Inverse DCT
*	param:	Mat input, int blockSize
*   ret  :	Mat
*====================================
*/
Mat coolCodec::blockIDCT(Mat input, int blockSize)
{
	
	size_t wlen = input.size().width - block_size;
	size_t hlen = input.size().height - block_size;

	Mat output;
	input.copyTo ( output );
	resize ( output, output, Size ( input.rows, input.cols ), 0, 0 );
	output.convertTo ( output, CV_8UC1,1);

	for (size_t i = 0; i <= wlen; i += blockSize)
	{
		for (size_t j = 0; j <= hlen ; j += blockSize)
		{	
			Mat mask = input(Rect(j,i, blockSize, blockSize));
			idct(mask, mask,0);
			mask.copyTo(output(Rect(j,i, blockSize, blockSize)));
		}

	}
	//cout << output(Rect(0, 0, wlen, 8)) << "," << endl;
	
	return output;
}


/*
*------------------------------------
*	Breif:	Encode using Huffman Table
*	param:	map<string, vector<bool>> dictionary
*   ret  :	void
*====================================
*/
void coolCodec::HuffmanEncode ( map<string, vector<bool>> dictionary, Mat image, string filename )
{
	int cnt = 0;
	uint8_t outbuff = 0;
	int pixcnt = 0;
	bool flag = true; // True if byte of data is complete
	ofstream file ( filename, ios::out | ios::app );
	file.seekp ( this->Write_marker );
	cout << "Write Marker Begining" << this->Write_marker << endl;
	cout << "Writting size " << this->Width << endl;
	cout << "Writting size " << this->Height << endl;

	for (size_t i = 0; i < this->Height; i++)
	{
		for (size_t j = 0; j < this->Width; j++)
		{
			// Get the (i,j)th Pixel value
			stringstream ss;
			ss << (image.at<int16_t> ( i, j )) ;
			string s = ss.str ( );	
			// Get the relavent Huffman code from the Table
			vector<bool> temp = dictionary.find (s)->second;
			pixcnt++; // Pixel count reference

			// clear output buffer if Byte is filled
			if (flag == true) { outbuff = 0; flag == false; }
			else { flag = true; }
			
			for (vector<bool>::iterator it = temp.begin ( ); it != temp.end ( );it++) {
				if (*it == true) {
					// push 1 into outbuffer
					outbuff <<= 1;
					outbuff |= 0x01;
					cnt++;
				}
				else {
					//push 0 into outbuffer
					outbuff <<= 1;
					cnt++;
				}
				if (cnt == 8) {
					// Save buffer after filling 8bits
					cnt = 0;  // pushed bit connt
					file.write ( (char*)&outbuff, sizeof ( uint8_t ) );
					outbuff = 0; // Clear output buffer
					flag = true; // Done filling a Byte to output buffer
				}
				else { flag = false; } // Write buffer is still not filled
			}
		}
	}

	if (flag == false) {
		outbuff <<= 8-cnt;
		file.write ( (char*)&outbuff, sizeof ( uint8_t ) );
		
	}

	outbuff = ' ';
	file.write ( (char*)&outbuff, sizeof ( uint8_t ) );
	this->Write_marker = file.tellp();
	cout << "Write marker " << this->Write_marker;
	cout << " Pixnt " << pixcnt << endl;
	file.close ( );
}


/*
*------------------------------------
*	Breif:	Decode using Huffman Table
*	param:	map<string, vector<bool>>* dictionary, string filename
*   ret  :	void
*====================================
*/
void coolCodec::HuffmanDecode (map<string, int> dict,Mat &img,string filename)
{
	ifstream file1 ( filename, ios::in | ios::binary);
	file1.seekg ( this->Read_marker );
	

	// -- Local Variables
	Size siz = img.size ( );
	uint8_t temp = 0;
	stringstream s;
	string cde;		// Place holder for read Code
	int8_t sym;		// Place holder for read symbol
	string cpr = ""; // String for comparision with Huffman Table

	size_t cnt = 0;		 // Byte counter
	size_t i = 0, j = 0; // Mat pixel position Markers
	size_t bitcnt = 8;	 // Bit counter
	bool flag = false;

	
	file1.read ( (char *)&temp, sizeof ( uint8_t ) );
	this->Read_marker = file1.tellg ( );
	cout << "Read Marker Begining " << this->Read_marker ;
	while (cnt < (img.size ( ).width * img.size ( ).height)) {

		//Load New byte for Huffman decoding
		if (bitcnt == 8) {
			file1.read ( (char *)&temp, sizeof ( uint8_t ) );
			this->Read_marker = file1.tellg ( );
			bitcnt = 0;
		}

		// check for Carry bit when rotated left
		if ((temp & 0x80) == 0x80) {
			cpr += "1";
			temp <<= 1;
			bitcnt++;
		}
		else {
			cpr += "0";
			temp <<= 1;
			bitcnt++;
		}

		// Check for availablity of a Huffman code from the 
		// Huffman Table
		if (dict.count ( cpr ) == true) {
			cde = dict.find ( cpr )->first;
			sym = dict.find ( cpr )->second;
			cpr = "";
			cnt++;
			if (cnt == (img.size ( ).width * img.size ( ).height)) {
				break;
			}
			// Fill the Image if symbol is found 
			if (i < img.size ( ).width) {
				img.at<int16_t> ( j, i ) = (int16_t)sym;
			}
			else {
				i = 0;j++;
				img.at<int16_t> ( j, i ) = (int16_t)sym;

			}
			i++;
		}



	}

	this->Read_marker = file1.tellg ( );
	cout << " Read Marker " << this->Read_marker;
	cout << " cnt " << cnt << endl;
	file1.close ( );
}

void coolCodec::ReadHuffmanDict ( string filename )
{
	ifstream file (filename, ios::in );
 	file.seekg ( this->Read_marker );

	string code, sym;
	// Read Header For Huffman Tables
	for (int i = 0;i < 3;i++) {
		
		do {
			file >> sym;
			if (sym == "sectn") break;
			file >> code;

			dict[i].insert ( make_pair (code,stoi( sym )) );
		} while (1);

	}

	this->Read_marker = file.tellg ( );
	file.close ( );
}

/*
*---------------------------------------------------
*	
*	_________________Signature_________________
*  
*===================================================
*/
void coolCodec::sig(void)
{
	echo("...................................................................\n");
	echo("	            :::::::  //::::    //::::   ::");
	echo("	            ::      ::    ::  ::    ::  ::");
	echo("	            ::      ::    ::  ::    :: 	::");
	echo("	            ::	    ::    ::  ::    ::  ::");
	echo("	            :::::::  :::://    :::://   :::::::\n");
	echo("            :::::::  //::::   :::::::   ::::::::  :::::::");
	echo("            ::      ::    ::   ::   ::   ::       ::");
	echo("            ::      ::    ::   ::    ::  :::::    ::");
	echo("            ::	    ::    ::   ::   ::   ::       ::");
	echo("            :::::::  :::://   :::::::   ::::::::  :::::::\n");
	echo("...................................................................\n");
	echo("\t\tAuthor : Malinda Sulochana Silva");
	echo("\t\tDate :\t 2018/05/31");
	echo("\n\t\tDept. Electrical and Electronic Engineering.");
	echo("\t\tFaculty of Engineering,");
	echo("\t\tUniversity of Peradeniya.\n");
	echo("__________________Copyright. All RIghts Reserved. _________________\n");
	echo("...................................................................\n\n");
}



/*
*---------------------------------------------------
*	Breif:	Set Encoder Parameters
*	param:	int Block_size,imQuality Quality
*	ret  :	void
*===================================================
*/
void coolCodec::setEncoderParam(int Block_size,imQuality Quality)
{
	this->block_size = Block_size;
	this->qFactor = Quality;
}



/*
*---------------------------------------------------
*	Breif:	Encode the image using custom encoder
*	param:	filename
*	ret  :  void
*===================================================
*/
 void coolCodec::imEncode(string filename,string outfile)
{
	int Width, Height;

	// Load Image
	Mat Original = imread(filename, CV_LOAD_IMAGE_COLOR);
	Width = Original.size().width;
	Height = Original.size().height;
	
	
	// Splitting the RGB planes
	Mat Original_brg[3];
	split(Original, Original_brg);

	//Save image RAW pixels
	ofstream file1 ( "RAW.txt", ios::out | ios::binary );

	cout << "Wait..... Raw file is being saved..." << endl;

	for (size_t i = 0; i < Height; i++)
	{
		for (size_t j = 0; j < Width; j++)
		{
			file1 << (int)Original_brg[0].at<int8_t> ( i, j );
			file1 << (int)Original_brg[1].at<int8_t> ( i, j );
			file1 << (int)Original_brg[2].at<int8_t> ( i, j );
		}
	}

	file1.close ( );


#ifdef _DEBUGMODE
	imshow("Original Image", Original);
#endif
	if (!Original.data) {
		echo("__Loading the image failed...");
		cin.get();
		return;
	}

	echo("__Image loaded successfully...");
	
	echo("__Encoding the image, please wait....");
	cout << "Height:: " <<  Height << endl;
	cout << "Width:: " <<  Width << endl;

	// Discrete Cosine Transform
	//Get the block dct
	Mat sample[3];
	sample[0] = blockDCT(Original_brg[0], block_size);
	sample[1] = blockDCT(Original_brg[1], block_size);
	sample[2] = blockDCT(Original_brg[2], block_size);

#ifdef _DEBUGMODE
	imshow("DCT B Image", sample[0]);
	imshow("DCT G Image", sample[1]);
	imshow("DCT R Image", sample[2]);
#endif
	
	// Header file
	ofstream file(outfile, ios::out | ios::binary);

	// Encoder Header
	Width = this->Width;
	Height = this->Height;
	file.seekp ( this->Write_marker );
	file << Width << ' ' << Height << ' ' << "sectn ";
	
	if (qFactor == LOW_QUALITY) {	
		file << block_size << ' ' << 0 << " sectn ";
	}
	else if (qFactor == MODARATE_QUALITY) {
		file << block_size << ' ' << 1 << " sectn ";
	}
	else if (qFactor == HIGH_QUALITY) {
		file << block_size << ' ' << 2 << " sectn ";
	}

	
	cout << Width << endl;
	cout << Height << endl;
	this->Write_marker = file.tellp ( );
	file.close ( );
	// Reduce Pixel container size
	sample[0].convertTo(sample[0], CV_16SC1, 1, 0);
	sample[1].convertTo(sample[1], CV_16SC1, 1, 0);
	sample[2].convertTo(sample[2], CV_16SC1, 1, 0);
	
	
	//cout << sample[0] ( Rect ( 0, 0, 8, 8 ) );

	// Quantizing
	sample[0] = sample[0] / qFactor;
	sample[1] = sample[1] / qFactor;
	sample[2] = sample[2] / qFactor;

	cout << sample[0] ( Rect ( 0, 0, 8, 8 ) );
	// Generating Huffman Table
	map<string, vector<bool>> dict[3];
	dict[0] = Huffman_dict ( sample[0] ( Rect ( 0, 0, Width, Height ) ),outfile );
	dict[1] = Huffman_dict ( sample[1] ( Rect ( 0, 0, Width, Height ) ),outfile );
	dict[2] = Huffman_dict ( sample[2] ( Rect ( 0, 0, Width, Height ) ),outfile );
	
	// Save Image with Huffman Encoding
	cout << "Wait..... Encoding is in process..." << endl;
	HuffmanEncode ( dict[0], sample[0],outfile ); cout << "Plane_1 done Encoding..." << endl;
	HuffmanEncode ( dict[1], sample[1],outfile ); cout << "Plane_2 done Encoding..." << endl;
	HuffmanEncode ( dict[2], sample[2],outfile ); cout << "Plane_3 done Encoding..." << endl;

	


}

 /*
 *-------------------------------------------------
 *	Breif: Decode the image using custom decoder
 *	param: filename
 *  ret  : void
 *=================================================
 */
 void coolCodec::imDecode(string filename,string outfile)
 {
	ifstream file(filename, ios::out | ios::binary);
	 int Width;
	 int Height;
	 file.seekg ( this->Read_marker );
	 string W,H;
	 string block,qf;
	 
	 do {
		 file >> W;
		 if (W == "sectn") { break; }
		Width =(int)stoi ( W );
		 file >> H;
	 } while (1);
	Height = (int)stoi ( H );

	do {
		file >> block;
		if (block == "sectn") { break; }
		this->block_size = (int)stoi ( block );
		file >> qf;
	} while (1);

	switch ((int)stoi(qf)) {

	case 0:
		this->qFactor = LOW_QUALITY;
		break;
	case 1:
		this->qFactor = MODARATE_QUALITY;
		break;
	case 2:
		this->qFactor = HIGH_QUALITY;
		break;
	default:
		break;
	
	}


	
	// Read Image Header
	this->Read_marker = file.tellg ( );
	 file.close ( );
	 
	 // RGB plane placeholders
	 Mat B = Mat(Height, Width, CV_16SC1, Scalar::all(0));
	 Mat R = Mat(Height, Width, CV_16SC1, Scalar::all(0));
	 Mat G = Mat(Height, Width, CV_16SC1, Scalar::all(0));

	 int16_t temp;
	 
	 ReadHuffmanDict (filename );

	 HuffmanDecode ( dict[0], B ,filename);
	 HuffmanDecode ( dict[1], G ,filename);
	 HuffmanDecode ( dict[2], R ,filename);
	
	 // Dequantize planes
	 B = B * qFactor;
	 G = G * qFactor;
	 R = R * qFactor;

	 // Fixing for IDCT
	 Mat sample[3];
	 B.convertTo(B, CV_32FC1, 1, 0);
	 G.convertTo(G, CV_32FC1, 1, 0);
	 R.convertTo(R, CV_32FC1, 1, 0);

	 // Get inverse dct
	 B = blockIDCT(B, block_size);
	 G = blockIDCT(G, block_size);
	 R = blockIDCT(R, block_size);

	 Mat B_plane = B(Rect(0, 0, Width, Height));
	 Mat G_plane = G(Rect(0, 0, Width, Height));
	 Mat R_plane = R(Rect(0, 0, Width, Height));

#ifdef _DEBUGMODE
	 imshow("IDCT B Image", B_plane ( Rect ( 0, 0, Width - block_size, Height - block_size ) ) );
	 imshow("IDCT G Image", G_plane ( Rect ( 0, 0, Width - block_size, Height - block_size ) ) );
	 imshow("IDCT R Image", R_plane ( Rect ( 0, 0, Width - block_size, Height - block_size ) ) );
#endif 
	 imwrite ( "IDCT B Image.jpg", B_plane ( Rect ( 0, 0, Width - block_size, Height - block_size ) ) );
	 imwrite ( "IDCT G Image.jpg", G_plane ( Rect ( 0, 0, Width - block_size, Height - block_size ) ) );
	 imwrite ( "IDCT R Image.jpg", R_plane ( Rect ( 0, 0, Width - block_size, Height - block_size ) ) );

	 // Merging RGB planes together
	 vector<Mat> array_to_merge;
	 Mat color(Height, Width, CV_16SC3, Scalar::all(0));
	 array_to_merge.push_back(B_plane);
	 array_to_merge.push_back(G_plane);
	 array_to_merge.push_back(R_plane);
	 merge(array_to_merge, color);
	 
#ifdef _DEBUGMODE
	 imshow("output", color(Rect(0,0,Width,Height)));
#endif
	 imwrite ( outfile, color ( Rect ( 0, 0,Width, Height ) ) );
 }

 /*
 *-------------------------------------------------
 *	Breif: encode the video using custom decoder
 *	param: filename
 *  ret  : void
 *=================================================
 */
 void coolCodec::vEncode ( string &filename )
 {
	 VideoCapture cap;
	 cap.open ( filename );
	 double fps = cap.get ( CAP_PROP_FPS );

#ifdef _DEBUGMODE
	 namedWindow ( "myVideo", WINDOW_AUTOSIZE );
#endif
	 
	 Mat frame0, frame1;
	 int tot_frame = cap.get ( CAP_PROP_FRAME_COUNT );
	 int Iframe_interval = I_FRAME_INTEVAL;
	 int Pframe_interval = P_FRAME_INTEVAL;

	 vector<Mat> I, P, B;
	 int i, p;// I and P frame Indexes
	 i = 0;
	 p = Pframe_interval;


	 cout << "Total Frame" << endl;
	 cout << tot_frame << endl;
	 /*
		Frame Format :: I B B P B B P I B B P ...
		Frame Index  :: 0 1 2 3 4 5 6 7 8 9 10 ...  
	 */
	 while (1){

		 Mat P_residual;

		 if (i < tot_frame) {
			 // Retriving I frames using defined Interval
			 cap.set ( CAP_PROP_POS_FRAMES, i );
			 cap >> frame0; 
			 I.push_back ( frame0 );
				i += Iframe_interval; // Update next I frame location

			 // Retriving P frames using defined Interval
			 cap.set ( CAP_PROP_POS_FRAMES, p );
			 cap >> frame1;
			 P_residual = frame1 - frame0;
			 P.push_back ( P_residual );

			 // Retriving P frames using defined Interval
			 cap.set ( CAP_PROP_POS_FRAMES, i-1 );
			 cap >> frame1;
			 P_residual = frame1 - frame0;
			 P.push_back ( P_residual );
			 
			 p += Iframe_interval;
		 }
		 else
			 break;

		 if (frame0.empty()) {
			 cout << "Could not load the video frames. \n";
			 break;		
		 }

			 Mat channels[3];
			 Mat Residual = frame0 ; // Get the Residual Image
#ifdef _DEBUGMODE
			 imshow ( "Residual", Residual ); 
#endif
			 split(frame0, channels);
			 
			 int block_size = 32;
			 Mat output;
			 output = blockDCT(channels[0], block_size);
#ifdef _DEBUGMODE
			 imshow("myVideo", output);
#endif		 

		
		 if (waitKey(10) > 0) {
			 cout << "Escape pressed \n";
			 break;
		 }
		 
	 }

 }


 /*
 *-------------------------------------------------
 *	Breif: Decode the video using custom decoder
 *	param: filename
 *  ret  : void
 *=================================================
 */
 void coolCodec::vDecode(string filename)
 {



 }
