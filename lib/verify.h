#ifndef INCLUDED_IEEE802_11_VERIFY_H
#define INCLUDED_IEEE802_11_VERIFY_H

#include <itpp/itcomm.h>
#include <iostream>
#include <iomanip>	// setfill

extern const char ref_data_bits_24qam[1295];
extern const char ref_encoded_bits_24qam[2589];
extern const int ref_nonMarker_24qam[567];
extern const int ref_marker_24qam[81];

/*---------------------------------------------------------------------------------
 calcDiffRate:
 Compute the difference rate of two sequence
 Parameters:
 	 decoded	- Items to be compared with the reference items
 	 ref		- Reference items
 	 nItems		- Number of items to be compared
 Return
 	 None
 ---------------------------------------------------------------------------------*/
template<typename TYPE1, typename TYPE2>
double calcDiffRate(const TYPE1 & decoded, const TYPE2 & ref, int nItems, bool debug) {
	int nErr = 0;
	for(int i=0;i<nItems;i++) {
		if(debug) {
			std::cout << std::setw(4) << std::setfill('0') << i << ": decoded bit = " << decoded[i] << ", encoded bit = " << ref[i];
		}
		if(decoded[i] != ref[i] - '0') {
			nErr++;
			if(debug) {
				std::cout << ", error " << nErr << std::endl;
			}
		}
		else {
			if(debug) {
				std::cout << std::endl;
			}
		}
	}
	return 1.0*nErr/nItems;
}

double calcINTDiffRate(const int * decoded, const int * ref, int nItems, bool debug);

#endif
