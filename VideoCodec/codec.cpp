#include "codec.h"

void Encoder::Encode ( string filename,int Nframes )
{
	VideoCapture cap;
	
	cap.open ( "a.mp4" );
	double fps = cap.get ( CAP_PROP_FPS );

	Mat currentFrm, previousFrm;
	int tot_frame = cap.get ( CAP_PROP_FRAME_COUNT );
	int Iframe_interval = I_FRAME_INTERVAL;
	int frmCount = 10;
	int i = 0;
	Mat MotionCmp, output, residual;
	int Width = 0, Height = 0;
	VideoWriter video ( "Decoded.avi", CV_FOURCC ( 'M', 'J', 'P', 'G' ), fps, Size ( 640, 368 ) );
	cout << "No of frames " << tot_frame << endl;

	while (i < Nframes) {
		cout << "frame " << i << " Started Encoding..." << endl;
		if (i % I_FRAME_INTERVAL == 0) {
			// Retriving I frames using defined Interval
			cap >> currentFrm;
			previousFrm = currentFrm.clone ( );
		}
		else
		{	// Retriving P frames using defined Interval
			cap >> currentFrm;
		}
		
			// Calculate Motion Vectors
			MVholder MotionVec;
			MotionVec = GetMV (  currentFrm,previousFrm);

			// Get Frame dimensions
			Width = previousFrm.cols;
			Height = previousFrm.rows;

			// Re-Creating the Motion compansated Image
			MotionCmp = MotionComp ( previousFrm, MotionVec, 16 );

			// Get the Residual Image
			residual = currentFrm - MotionCmp;

#ifdef _DEBUGMODE			
			imshow ( "Iframe", currentFrm );
			imshow ( "Pframe", previousFrm );
			imshow ( "residual", residual );
			imshow ( "comp", MotionCmp );
#endif
			residual = intraDecode ( residual,5,8);

			// Regenerated Image
			output = residual + MotionCmp;
			video.write (output);
			cout << "frame "<< i << "Encoded Successfully..." << endl;
			i++;
#ifdef _DEBUGMODE
			imshow ( "output", output );
			waitKey ( );
#endif
		
	}

	cout << "Video Encoded successfully" << endl;
	video.release ( );

}


Mat Encoder::intraDecode (Mat img, int Qparam,int blkSize)
{
	// Get the DCT
	Mat dctimg[3];
	split ( img, dctimg );
	dctimg[0] = blockDCT ( dctimg[0], blkSize );
	dctimg[1] = blockDCT ( dctimg[1], blkSize );
	dctimg[2] = blockDCT ( dctimg[2], blkSize );

	dctimg[0].convertTo ( dctimg[0], CV_16SC1, 1, 0 );
	dctimg[1].convertTo ( dctimg[1], CV_16SC1, 1, 0 );
	dctimg[2].convertTo ( dctimg[2], CV_16SC1, 1, 0 );

	// Quantization
	dctimg[0] = dctimg[0] / Qparam;
	dctimg[1] = dctimg[1] / Qparam;
	dctimg[2] = dctimg[2] / Qparam;
							
	// Dequantization		
	dctimg[0] = dctimg[0] * Qparam;
	dctimg[1] = dctimg[1] * Qparam;
	dctimg[2] = dctimg[2] * Qparam;

	dctimg[0].convertTo ( dctimg[0], CV_32FC1, 1, 0 );
	dctimg[1].convertTo ( dctimg[1], CV_32FC1, 1, 0 );
	dctimg[2].convertTo ( dctimg[2], CV_32FC1, 1, 0 );

	// Get the IDCT
	dctimg[0] = blockIDCT ( dctimg[0], blkSize );
	dctimg[1] = blockIDCT ( dctimg[1], blkSize );
	dctimg[2] = blockIDCT ( dctimg[2], blkSize );

	vector<Mat> array_to_merge;
	array_to_merge.push_back ( dctimg[0] );
	array_to_merge.push_back ( dctimg[1] );
	array_to_merge.push_back ( dctimg[2] );

	Mat color ( img.rows, img.cols, CV_8SC3, Scalar::all ( 0 ) );
	merge ( array_to_merge, color );
	
	return color;

}


/*
*------------------------------------
*	Breif:	Generate Motion Vector
*	param:	Mat Ifrm, Mat Pfrm
*   ret  :	Mat
*====================================
*/
MVholder Encoder::GetMV ( Mat refFrm, Mat predFrm )
{
	int bsum = 0;
	int mbsz = 16; // Marker block size
	int p = 2;
	int dx, dy;
	Mat Isamp[3];
	Mat Psamp[3];
	split ( refFrm, Isamp );
	split ( predFrm, Psamp );

	//cvtColor ( Ifrm, Ifrm, CV_BGR2GRAY );
	//cvtColor ( Pfrm, Pfrm, CV_BGR2GRAY );
	
	Mat costs = Mat ( 2 * p + 1, 2 * p + 1, CV_16UC1, Scalar::all ( 255 ) );
	vector<MV> Motnvec[3];


	for (int plane = 0; plane < 3; plane++) {
		Motnvec[plane].reserve ( refFrm.rows* refFrm.cols / (mbsz ^ 2) );


		for (int i = 0; i <= (Isamp[plane].rows-mbsz); i += mbsz) {
			for (int j = 0; j <= (Isamp[plane].cols-mbsz); j += mbsz) {
			
					Mat block = refFrm ( Rect ( j, i, mbsz, mbsz ) );
				//cout << " Entering<row,col> " << i << "," << j << endl;
				for (int m = -p; m <= p; m++) {
					for (int n = -p; n <= p;n++) {
						int refBV = j + n;
						int refBH = i + m;
						//	cout << "BV " << refBV;
						//	cout << "BH " << refBV << endl;
						if (refBV < 0 || (refBV + mbsz ) > Isamp[plane].cols ||
							 refBH < 0 || (refBH + mbsz ) > Isamp[plane].rows) {
							continue;
						}

						//cout << " Stride<row,col> " << refBH << "," << refBV << endl;
						costs.at<uint16_t> ( m + p, n + p ) = cost ( Isamp[plane] ( Rect ( refBV, refBH, mbsz, mbsz ) ),
																	 Psamp[plane] ( Rect ( j,i, mbsz,
																						   mbsz ) ), mbsz );
					}
				}
				// cout << "costs mat" << endl;
				// cout << costs << endl;
				/*-- Finding the Minimum cost Function --*/
				int min = 65536;
				dx = 0; dy = 0;
				for (int a = 0;a < costs.rows; a++) {
					for (int b = 0;b < costs.cols; b++) {
						if (costs.at<uint16_t> ( a, b ) <= min) {
							
								min = costs.at<uint16_t> ( a, b );
								dx = b; dy = a;
							
						}

					}
				}

				MV tempMv;
				tempMv.x = dx - p -1 ;
				tempMv.y = dy - p -1 ;

				Motnvec[plane].push_back ( tempMv );
				costs = Mat ( 2 * p + 1, 2 * p + 1, CV_16UC1, Scalar::all ( 255 ) );
			}
		}

	}

	MVholder output;
	output.vec[0] = Motnvec[0];
	output.vec[1] = Motnvec[1];
	output.vec[2] = Motnvec[2];

	return output;
}

/*
*------------------------------------
*	Breif:	Cost Function
*	param:	Mat crrentblk, Mat refblk,int blk_size
*   ret  :	vector<MV> *
*====================================
*/
uint8_t Encoder::cost ( Mat& crrentblk, Mat& refblk, int blk_size )
{
	uint8_t err = 0;
	for (int i = 0; i < blk_size; i++) {
		for (int j = 0; j < blk_size; j++) {

			err += abs ( crrentblk.at<uint8_t> ( i, j ) - refblk.at<uint8_t> ( i, j ) );
			
		}
		
	}

	return (err / (blk_size*blk_size));
}

Mat Encoder::MotionComp ( Mat RefImg, MVholder MVH, int blk_size )
{

	int pxlcnt = 0;
	Mat img[3];
	vector<Mat> array_to_merge;
	split ( RefImg, img );
	
	for (int plane = 0; plane < 3; plane++) {
		pxlcnt = 0;	
		/*
			 This will evaluate cost for (2p + 1) blocks vertically
			 and (2p + 1) blocks horizontaly
			 m is row ( vertical ) index
			 n is col ( horizontal ) index
		 */
		Mat output = Mat ( RefImg.rows, RefImg.cols, CV_8UC1, Scalar::all ( 255 ) );
		for (int i = 0; i <= RefImg.rows - blk_size;i += blk_size) {
			for (int j = 0; j <= RefImg.cols - blk_size; j += blk_size) {
				
				int dx = MVH.vec[plane][pxlcnt].x;
				int dy = MVH.vec[plane][pxlcnt].y;
				pxlcnt++;

				int refBV = j + dx ;
				int refBH = i + dy ;
			
				img[plane] ( Rect ( refBV, refBH, blk_size, blk_size ) ).copyTo ( output ( Rect ( j, i, blk_size, blk_size) ) );
		
#ifdef _DEBUGMODE
/*
				cout << "\t< " << j << ", " << i << " >" << endl;
			//	cout << "< " << dx << ", " << dy << " >" << endl;
				cout << "< " << refBV << ", " << refBH << " >" << endl;
				Rect r = Rect ( refBV, refBH, 16,16 );
				Rect r1 = Rect ( j, i, 16, 16 );
				Mat im = RefImg.clone ( );
				rectangle ( im, r, Scalar ( 255, 0, 0 ), 1, 8, 0 );
				rectangle (im, r1, Scalar ( 0, 0,255), 1, 8, 0 );
				imshow ( "A", im );
				waitKey ( );  */
#endif
				
			}
			
		}

		array_to_merge.push_back ( output );
	}

	Mat color ( RefImg.rows, RefImg.cols, CV_8SC3, Scalar::all ( 0 ) );
	merge ( array_to_merge, color );
	return color;
}

/*
*------------------------------------
*	Breif:	Block  DCT
*	param:	Mat input, int blockSize
*   ret  :	Mat
*====================================
*/
Mat Encoder::blockDCT ( Mat input, int blockSize )
{
	int Width, Height;

	Width = input.size ( ).width;
	Height = input.size ( ).height;

	Mat output;
	input.copyTo ( output );
	resize ( output, output, Size ( Width, Height ), 0, 0 );
	output.convertTo ( output, CV_32FC1, 1 );

	int wlen = (int)(Width - blockSize);
	int hlen = (int)(Height - blockSize);

	if (Width == 0 || Height == 0)
		return Mat ( );


	for (size_t i = 0; i <= hlen; i += blockSize)
	{
		for (size_t j = 0; j <= wlen; j += blockSize)
		{
			Mat mask = output ( Rect ( j, i, blockSize, blockSize ) );
			dct ( mask, mask, 0 );
			mask.copyTo ( output ( Rect ( j, i, blockSize, blockSize ) ) );
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
Mat Encoder::blockIDCT ( Mat input, int blockSize )
{

	int wlen = input.size ( ).width - blockSize;
	int hlen = input.size ( ).height - blockSize;

	Mat output;
	input.copyTo ( output );
	resize ( output, output, Size ( input.cols, input.rows ), 0, 0 );
	output.convertTo ( output, CV_8UC1, 1 );

	for (size_t i = 0; i <= hlen; i += blockSize)
	{
		for (size_t j = 0; j <= wlen; j += blockSize)
		{
			Mat mask = input ( Rect ( j, i, blockSize, blockSize ) );
			idct ( mask, mask, 0 );
			mask.copyTo ( output ( Rect ( j, i, blockSize, blockSize ) ) );
		}

	}
	//cout << output(Rect(0, 0, wlen, 8)) << "," << endl;

	return output;
}

