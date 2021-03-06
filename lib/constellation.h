/*
 * Copyright (C) 2013 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef INCLUDED_IEEE802_11_CONSTELLATION_H
#define INCLUDED_IEEE802_11_CONSTELLATION_H

#include <ieee802-11/api.h>
#include <ieee802-11/ofdm_mapper.h>
#include <gnuradio/config.h>
#include <cinttypes>
#include <iostream>
#include <iomanip>	// std::setw
#include <complex>

#define dout d_debug && std::cout

#if defined(HAVE_LOG4CPP) && defined(ENABLE_GR_LOG)
#define mylog(msg) do { if(d_log) { *d_logger << log4cpp::Priority::INFO << msg << log4cpp::eol; }} while(0);
#else
#define mylog(msg) do { d_log && std::cout << msg << std::endl; } while(0);
#endif

/**********************************************************
                      constellation.h
**********************************************************/

// constellation-related declaration
extern const std::complex<float> QAM10_D[10];
extern const std::complex<float> QAM12_D[12];
extern const std::complex<float> QAM14_D[14];
extern const std::complex<float> QAM20_D[20];
extern const std::complex<float> QAM24_D[24];
extern const std::complex<float> QAM28_D[28];
// extern const std::complex<float> QAM32_D[32];
extern const std::complex<float> QAM40_D[40];
extern const std::complex<float> QAM48_D[48];
extern const std::complex<float> QAM56_D[56];
// extern const std::complex<float> QAM64_D[64];

extern const std::string QAM10_code[10];
extern const std::string QAM12_code[12];
extern const std::string QAM14_code[14];
extern const std::string QAM20_code[20];
extern const std::string QAM24_code[24];
extern const std::string QAM28_code[28];
extern const std::string QAM32_code[32];
extern const std::string QAM40_code[40];
extern const std::string QAM48_code[48];
extern const std::string QAM56_code[56];
extern const std::string QAM64_code[64];

extern const int QAM10_bitLen[10];
extern const int QAM12_bitLen[12];
extern const int QAM14_bitLen[14];
extern const int QAM20_bitLen[20];
extern const int QAM24_bitLen[24];
extern const int QAM28_bitLen[28];
extern const int QAM32_bitLen[32];
extern const int QAM40_bitLen[40];
extern const int QAM48_bitLen[48];
extern const int QAM56_bitLen[56];
extern const int QAM64_bitLen[64];

extern const int QAM10_lenSet[2];
extern const int QAM12_lenSet[2];
extern const int QAM14_lenSet[2];
extern const int QAM20_lenSet[2];
extern const int QAM24_lenSet[2];
extern const int QAM28_lenSet[2];
extern const int QAM32_lenSet[2];
extern const int QAM40_lenSet[2];
extern const int QAM48_lenSet[2];
extern const int QAM56_lenSet[2];
extern const int QAM64_lenSet[2];

extern const std::complex<float> QAM10_2bitMarker_D[4];
extern const std::complex<float> QAM12_2bitMarker_D[4];
extern const std::complex<float> QAM14_2bitMarker_D[4];
extern const std::complex<float> QAM20_3bitMarker_D[8];
extern const std::complex<float> QAM24_3bitMarker_D[8];
extern const std::complex<float> QAM28_3bitMarker_D[8];
extern const std::complex<float> QAM32_3bitMarker_D[8];
extern const std::complex<float> QAM40_3bitMarker_D[8];
extern const std::complex<float> QAM48_3bitMarker_D[8];
extern const std::complex<float> QAM56_3bitMarker_D[8];
extern const std::complex<float> QAM64_3bitMarker_D[8];
/**********************************************************
                      VlcMod.h
**********************************************************/
class VlcMod {
private:
	/*** Node structure ***/
	class BinNode {
	public:
		int data;
		BinNode * left, *right;
		// BinNode constructor
		BinNode(int item) {
			data = item;
			left = right = 0;
		}
	};
	typedef BinNode * BinNodePointer;
public:
	/*** Function members ***/
	/*---------------------------------------------------------------------------------
	 Constructor
	 Precondition: None.
	 Postcondition: A one-node binary tree with root node pointed to by myRoot
	 has been created. The number of constellation points is assigned.
	 ---------------------------------------------------------------------------------*/
	VlcMod(int nQAM, int nBitPerMarker, int nBackup);

	/*---------------------------------------------------------------------------------
	 Build the VlcMod decoding tree.
	 Precondition: None
	 Postcondition: A VlcMod decoding tree has been created with root node
	 pointed to by myRoot. The inserted nodes are based on member variable "codeword".
	 ---------------------------------------------------------------------------------*/
	void buildDecodingTree();

	/*---------------------------------------------------------------------------------
	 Insert a node for an integer in VlcMod decoding tree.
	 Precondition: code is the bit std::string that is the code for ch.
	 Postcondition: A node containing ch has been inserted into the VlcMod tree
	 with root pointed to by myRoot.
	 ---------------------------------------------------------------------------------*/
	void insert(int ch, std::string code);

	/*---------------------------------------------------------------------------------
	 Build the VlcMod decoding tree.
	 Precondition: ifstream messageIn is open and is connected to a file that
	 contains the message to be decoded.
	 Postcondition: The decoded message has been output to cout.
	 ---------------------------------------------------------------------------------*/
	int mapper(const char * bitsIn, const int nBitsIn, int * symOut, int *nSymOut);
	int demapper(const int * messageOut, char * out, int nSymOut);
	void modulate(const int *symIn, std::complex<float> *sigOut, int nSymIn, int nSigOut);
	int calcTotSymbol(int nSymIn);
	void printTree(std::ostream & out, BinNodePointer root, int indent);
	/*---------------------------------------------------------------------------------
	 Recursive function to display a binary tree with root pointed to by root.
	 Precondition: ostream out is open; root points to a binary tree; indent >= 0
	 is the amount to indent each level..
	 Postcondition: Graphical representation of the binary tree has been output
	 to out.
	 ---------------------------------------------------------------------------------*/
	void displayDecodingTree(std::ostream & out);
	/*---------------------------------------------------------------------------------
	 Display the decoding tree.
	 Precondition: ostream out is open.
	 Postcondition: The decoding tree has been output to out (via printTree().)
	 ---------------------------------------------------------------------------------*/

	void QAMDemod_nonMarker(const std::complex<float> *sigRec, int nSig, int *nonMarker, double *nonMarkerDist, int nBackup);
	void markerDecode(const std::complex<float> *sigRec, int nSig, int *marker, double *markerDist);
	void IndexSort(int *pIndex, double *buffer, int buffer_len);
	
	/*** Data members ***/
	const int *bitLen;
	const int *lenSet;
	int nMarker; // default 3-bit
	int nMarkerBackup;
private:
	BinNodePointer myRoot;
	int nQAM;
	const std::string *codeword;
	const std::complex<float> *constellation_nonMarker;
	const std::complex<float> *constellation_marker;
};
/*---------------------------------------------------------------------------------
 Constructor
 Parameters:
 	 n - number of constellation points used
 ---------------------------------------------------------------------------------*/
inline VlcMod::VlcMod(int n, int nBitPerMarker, int nBackup) {
	myRoot = new BinNode(257);
	nQAM = n;
	
	nMarker = (int) pow(2,nBitPerMarker);
	nMarkerBackup = nBackup;
	if(nMarkerBackup > nMarker) {
		std::cerr << "Marker parameters are incorrect, nMarkerBackup = "<<nMarkerBackup
				<<", nMarker = " << nMarker;
	}
		
	switch (nQAM) {
		case 10:
			codeword = QAM10_code;
			constellation_nonMarker = QAM10_D;
			constellation_marker = QAM10_2bitMarker_D;
			bitLen = QAM10_bitLen;
			lenSet = QAM10_lenSet;
			break;
		case 12:
			codeword = QAM12_code;
			constellation_nonMarker = QAM12_D;
			constellation_marker = QAM12_2bitMarker_D;
			bitLen = QAM12_bitLen;
			lenSet = QAM12_lenSet;
			break;
		case 14:
			codeword = QAM14_code;
			constellation_nonMarker = QAM14_D;
			constellation_marker = QAM14_2bitMarker_D;
			bitLen = QAM14_bitLen;
			lenSet = QAM14_lenSet;
			break;
		case 20:
			codeword = QAM20_code;
			constellation_nonMarker = QAM20_D;
			constellation_marker = QAM20_3bitMarker_D;
			bitLen = QAM20_bitLen;
			lenSet = QAM20_lenSet;
			break;
		case 24:
			codeword = QAM24_code;
			constellation_nonMarker = QAM24_D;
			constellation_marker = QAM24_3bitMarker_D;
			bitLen = QAM24_bitLen;
			lenSet = QAM24_lenSet;
			break;
		case 28:
			codeword = QAM28_code;
			constellation_nonMarker = QAM28_D;
			constellation_marker = QAM28_3bitMarker_D;
			bitLen = QAM28_bitLen;
			lenSet = QAM28_lenSet;
			break;
		case 40:
			codeword = QAM40_code;
			constellation_nonMarker = QAM40_D;
			constellation_marker = QAM40_3bitMarker_D;
			bitLen = QAM40_bitLen;
			lenSet = QAM40_lenSet;
			break;
		case 48:
			codeword = QAM48_code;
			constellation_nonMarker = QAM48_D;
			constellation_marker = QAM48_3bitMarker_D;
			bitLen = QAM48_bitLen;
			lenSet = QAM48_lenSet;
			break;
		case 56:
			codeword = QAM56_code;
			constellation_nonMarker = QAM56_D;
			constellation_marker = QAM56_3bitMarker_D;
			bitLen = QAM56_bitLen;
			lenSet = QAM56_lenSet;
			break;
		default:
			std::cerr << "Constructor of VlcMod failed: The input QAM is not supported (" 
				<< nQAM << ")" << std::endl;
	}
	buildDecodingTree();
}
//--- Definition of displayDecodingTree()
inline void VlcMod::displayDecodingTree(std::ostream & out) {
	printTree(out, myRoot, 0);
}

/**********************************************************
                   Self-defined function
**********************************************************/
void generate_bits_vlc(const char *psdu, char *data_bits, int nBits);
void convolutional_encoding_vlc(const char *in, char *out, int nBits);

int dp_demod(int objIndex, const int *srcIndex, const double *dist, int nSym, int nBackup, int *out, bool outFmt);
/*---------------------------------------------------------------------------------
 max_min:
 Return the maximum and the minimum of an integer array
 Parameters:
 	 numbers 	- pointer to the integers to be compared
 	 n			- length of the integer array
 	 max 		- pointer to store the maximum
 	 min	 	- pointer to store the minimum
 Return
 	 None
 ---------------------------------------------------------------------------------*/
void max_min(const int *numbers, int n, int &max, int &min);

/*---------------------------------------------------------------------------------
 print2DArray:
 Print 2-D array
 Parameters:
 	 num 	- pointer to the first element of the array
 	 row	- Number of rows of the arrays
 	 col 	- Number of columns of the arrays
 Return
 	 None
 ---------------------------------------------------------------------------------*/
template<class TYPE>
void print2DArray(TYPE *num, int row, int col) {
	for(int i=0;i<row;i++) {
		for(int j=0;j<col;j++) {
			std::cout << std::setw(10) << *(num+i*col+j) << ",";
		}
		std::cout << std::endl;
	}
}

#endif /* INCLUDED_IEEE802_11_CONSTELLATION_H */
