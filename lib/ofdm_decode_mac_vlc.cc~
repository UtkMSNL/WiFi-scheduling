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
 *
 * Notes By Haoyang:
 * 	This version is a clean version which is not related to side channel.
 * 	This file is used to analyze the regular WiFi traffic, by comparing 
 *	it with the frames captured by WiFi adapter.
 * 	The module name is changed from ofdm_decode_mac to ofdm_decode_mac_vlc
 *
 * New features added (10/15/2015):
 *	1. Extract frame type (subtype), sequence number, retransmission properties;
 *	2. Compute the SNR in frequency domain.
 */
#include <ieee802-11/ofdm_decode_mac_vlc.h>

#include "utils.h"
#include <boost/crc.hpp>
#include <gnuradio/io_signature.h>
#include <itpp/itcomm.h>
#include <iomanip>
#include "constellation.h"
#include "verify.h"

using namespace gr::ieee802_11;
using namespace itpp;

class ofdm_decode_mac_vlc_impl : public ofdm_decode_mac_vlc {

public:
static const int DATA_CARRIERS = 48;

ofdm_decode_mac_vlc_impl(bool log, bool debug) : block("ofdm_decode_mac_vlc",
			gr::io_signature::make(1, 1, 48 * sizeof(gr_complex)),
			gr::io_signature::make(0, 0, 0)),
			d_log(log),
			d_debug(debug),
			d_ofdm(BPSK_1_2),
			d_tx(d_ofdm, 0),
			d_frame_complete(true) {

	message_port_register_out(pmt::mp("out"));

	// bpsk
	int bpsk_bits[] = {0, 1};
	bpsk.set(cvec(BPSK_D, 2), ivec(bpsk_bits, 2));

	// qpsk
	int qpsk_bits[] = {0, 1, 2, 3};
	qpsk.set(cvec(QPSK_D, 4), ivec(qpsk_bits, 4));

	// qam16
	int qam16_bits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	qam16.set(cvec(QAM16_D, 16), ivec(qam16_bits, 16));

	// qam64
	int qam64_bits[] = {
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
		40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
		50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
		60, 61, 62, 63};
	qam64.set(cvec(QAM64_D, 64), ivec(qam64_bits, 64));
}

int general_work (int noutput_items, gr_vector_int& ninput_items,
		gr_vector_const_void_star& input_items,
		gr_vector_void_star& output_items) {

	const gr_complex *in = (const gr_complex*)input_items[0];

	int i = 0;

	std::vector<gr::tag_t> tags;
	const uint64_t nread = this->nitems_read(0);

	dout << "Decode MAC: input " << ninput_items[0] << std::endl;

	while(i < ninput_items[0]) {

		get_tags_in_range(tags, 0, nread + i, nread + i + 1,
			pmt::string_to_symbol("ofdm_start"));

		if(tags.size()) {
			if (d_frame_complete == false) {
				dout << "Warning: starting to receive new frame before old frame was complete" << std::endl;
				dout << "Already copied " << copied << " out of " << d_tx.n_sym << " symbols of last frame" << std::endl;
			}
			d_frame_complete = false;

			pmt::pmt_t tuple = tags[0].value;
			int len_data = pmt::to_uint64(pmt::car(tuple));
			int encoding = pmt::to_uint64(pmt::cdr(tuple));

			ofdm_param ofdm = ofdm_param((Encoding)encoding);
			tx_param tx = tx_param(ofdm, len_data);
			
			// sanity check, more than MAX_SYM may indicate garbage
			if(tx.n_sym <= MAX_SYM && tx.n_encoded_bits <= MAX_BITS) {
				d_ofdm = ofdm;
				// HERE I MANUALLY SET d_tx.n_sym to nSymTot * 48
				dout << "Received a complete frame\n";
				tx.n_sym = nSymTot;
				d_tx = tx;
				copied = 0;
				dout << "Decode MAC: frame start -- len " << len_data
					<< "  symbols " << tx.n_sym << "  encoding "
					<< encoding << std::endl;
			} else {
				dout << "Dropping frame which is too large (symbols or bits)" << std::endl;
			}
		}

		if(copied < d_tx.n_sym) {
			dout << "copy one symbol, copied " << copied << " out of " << d_tx.n_sym << std::endl;
			std::memcpy(sym + (copied * 48), in, 48 * sizeof(gr_complex));
			copied++;

			if(copied == d_tx.n_sym) {
				dout << "received complete frame - decoding" << std::endl;
				// decode();
				decode_vlc();
				in += 48;
				i++;
				d_frame_complete = true;
				break;
			}
		}

		in += 48;
		i++;
	}

	consume(0, i);
	return 0;
}

void decode() {
	demodulate();
	deinterleave();
	decode_conv();
	descramble();
	print_output();
	if(print_info() == 0) {

		// skip service field
		boost::crc_32_type result;
		result.process_bytes(out_bytes + 2, d_tx.psdu_size);
		if(result.checksum() != 558161692) {
			dout << "checksum wrong -- dropping" << std::endl;
			std::cout << ",failed\n";
			return;
		}
		else {
			std::cout <<"\n";
		}
	}
	
	mylog(boost::format("encoding: %1% - length: %2% - symbols: %3%")
			% d_ofdm.encoding % d_tx.psdu_size % d_tx.n_sym);

	// create PDU
	pmt::pmt_t blob = pmt::make_blob(out_bytes + 2, d_tx.psdu_size - 4);
	pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
	pmt::pmt_t dict = pmt::make_dict();
	dict = pmt::dict_add(dict, pmt::mp("encoding"), enc);
	message_port_pub(pmt::mp("out"), pmt::cons(dict, blob));
}

void demodulate() {

	cvec symbols;
	symbols.set_length(d_tx.n_sym * 48);
	for(int i = 0; i < d_tx.n_sym * 48; i++) {
		symbols[i] = std::complex<double>(sym[i]);
	}

	switch(d_ofdm.encoding) {
	case BPSK_1_2:
	case BPSK_3_4:

		bits = to_vec(bpsk.demodulate_bits(symbols));
		break;

	case QPSK_1_2:
	case QPSK_3_4:

		bits = to_vec(qpsk.demodulate_bits(symbols));
		break;

	case QAM16_1_2:
	case QAM16_3_4:
		bits = to_vec(qam16.demodulate_bits(symbols));
		break;
	case QAM64_2_3:
	case QAM64_3_4:
		bits = to_vec(qam64.demodulate_bits(symbols));
		break;
	}

	// I hate the guy who wrote itpp
	bits = bits * (-2) + 1;
}

void deinterleave() {

	int n_cbps = d_ofdm.n_cbps;
	int first[n_cbps];
	int second[n_cbps];
	int s = std::max(d_ofdm.n_bpsc / 2, 1);

	for(int j = 0; j < n_cbps; j++) {
		first[j] = s * (j / s) + ((j + int(floor(16.0 * j / n_cbps))) % s);
	}

	for(int i = 0; i < n_cbps; i++) {
		second[i] = 16 * i - (n_cbps - 1) * int(floor(16.0 * i / n_cbps));
	}

	for(int i = 0; i < d_tx.n_sym; i++) {
		for(int k = 0; k < n_cbps; k++) {
			deinter[i * n_cbps + second[first[k]]] = bits[i * n_cbps + k];
		}
	}
}

void decode_conv() {
	Punctured_Convolutional_Code code;
	ivec generator(2);
	generator(0)=0133;
	generator(1)=0171;
	code.set_generator_polynomials(generator, 7);

	bmat puncture_matrix;
	switch(d_ofdm.encoding) {
	case BPSK_1_2:
	case QPSK_1_2:
	case QAM16_1_2:
		puncture_matrix = "1 1; 1 1";
		break;
	case BPSK_3_4:
	case QPSK_3_4:
	case QAM16_3_4:
	case QAM64_3_4:
		puncture_matrix = "1 1 0; 1 0 1;";
		break;
	case QAM64_2_3:
		puncture_matrix = "1 1 1 1 1 1; 1 0 1 0 1 0;";
		break;
	}
	code.set_puncture_matrix(puncture_matrix);
	code.set_truncation_length(30);

	dout << "coding rate " << code.get_rate() << std::endl;
	dout << d_tx.n_encoded_bits << std::endl;

	vec rx_signal(deinter, d_tx.n_encoded_bits);

	code.reset();
	decoded_bits.set_length(d_tx.n_encoded_bits);
	code.decode_tail(rx_signal, decoded_bits);

	//dout << "length decoded " << decoded_bits.size() << std::endl;
	//std::cout << decoded_bits << std::endl;
}

void descramble () {
	int state = 0;
	for(int i = 0; i < 7; i++) {
		if(decoded_bits(i)) {
			state |= 1 << (6 - i);
		}
	}

	int feedback;

	for(int i = 7; i < decoded_bits.size(); i++) {
		feedback = ((!!(state & 64))) ^ (!!(state & 8));
		out_bits[i] = feedback ^ (int)decoded_bits(i);
		state = ((state << 1) & 0x7e) | feedback;
	}

	for(int i = 0; i < decoded_bits.size(); i++) {
		int bit = i % 8;
		int byte = i / 8;
		if(bit == 0) {
			out_bytes[byte] = 0;
		}

		if(out_bits[i]) {
			out_bytes[byte] |= (1 << bit);
		}
	}
}

void print_output() {

	dout << std::endl;
	for(int i = 0; i < decoded_bits.size() / 8; i++) {
		dout << std::setfill('0') << std::setw(2) << std::hex << ((unsigned int)out_bytes[i] & 0xFF) << std::dec << " ";
		if(i % 16 == 15) {
			dout << std::endl;
		}
	}
	dout << std::endl;
	for(int i = 0; i < decoded_bits.size() / 8; i++) {
		if((out_bytes[i] > 31) && (out_bytes[i] < 127)) {
			dout << ((char) out_bytes[i]);
		} else {
			dout << ".";
		}
	}
	dout << std::endl;
}

/* 
	func name: print_info
	function : print details of the frame received
	print format: 	new,
					type 	- 0: multicast (pda=FF:FF:FF:FF:FF:FF) 1: others
				  	pda	 	- destination address (12 hex)
				  	psa	 	- source address (12 hex)
				  	seq_num - sequence number
				  	len	 	- frame length
				  	*mcs_rate 		- data rate
				  	crc_err - 0: correct 1: error
				  	SNR1	- estimated SNR
				  	RSSI1	- ignore
				  	SNR2	- ignore
				  	RSSI2	- ignore
				  	frame_type_hex	- frame type
				  	subtype_hex		- frame subtype
	WiFi frame format:
	SERVICE | Frame Ctrl | Duration ID | Addr1 | Addr2 | Addr3 | Sequence Ctrl | Addr4 | Frame body | FCS
		2	|	   2	 | 		2	   |   6   |   6   |   6   |   	   2	   |   6   |    0~2312	|  4
	return: 1 - not for wifi adapter
			0 - to wifi adapter or multicast
	
*/
int print_info() {
	// First determine the frame type since frames of different types have different frame format
	unsigned int frame_type = (unsigned int) out_bytes[2] & 0x0C;
	unsigned int frame_subtype = (unsigned int) out_bytes[2] & 0xFC;
	
	unsigned int to_from_ds = (unsigned int) out_bytes[3] & 0x3; // bit 1 - From_DS, bit 0 - To_DS
	
	unsigned int pda[6];	// destination addr. (6 byte)
	unsigned int psa[6];	// source addr. (6 byte)
	
	unsigned int pda_ini_addr = 0;
	unsigned int psa_ini_addr = 0;

	if(frame_type == 0x4)	{	// control frame (short frame length)
		std::cout << "ctrl," << d_ofdm.encoding << "\n";
		return 2;
	}
	
	switch(to_from_ds) {
		case 0:	// To_DS = 0, From_DS = 0: DA - Addr1, SA - Addr2
			pda_ini_addr = 6; psa_ini_addr = 12; break;
		case 1:	// To_DS = 1, From_DS = 0: DA - Addr3, SA - Addr2
			pda_ini_addr = 18; psa_ini_addr = 12; break;
		case 2:	// To_DS = 0, From_DS = 1: DA - Addr1, SA - Addr3
			pda_ini_addr = 6; psa_ini_addr = 18; break;
		case 3:	// To_DS = 1, From_DS = 1: DA - Addr3, SA - Addr4
			pda_ini_addr = 18; psa_ini_addr = 26; break;
	}
	// The positions of pda and psa are determined by to_ds and from_ds
	
	
	unsigned int is_multicast = 1;	// not multicast (default)
	unsigned int is_toAdapter = 1;
	
	for(int i=0;i<6;i++) {
		pda[i] = (unsigned int) out_bytes[pda_ini_addr+i] & 0xFF;	// Addr1
		if(pda[i] != wifi_multicast_addr[i])
			is_multicast = 0;
		if(pda[i] != wifi_adapter_addr[i])
			is_toAdapter = 0;
		psa[i] = (unsigned int) out_bytes[psa_ini_addr+i] & 0xFF;	// Addr2
	}
	
	if(is_multicast != 1 && is_toAdapter != 1) { // only multicast frames or frames sent to WiFi adapter are displayed
		return 1;
	}
	unsigned int seq_ctrl_sn = // sequence number subfield - 12 bits; Each MSDU has a sequence number and it's constant.
		((((unsigned int)out_bytes[25] & 0xFF) << 8) | ((unsigned int)out_bytes[24] & 0xFF)) >> 4;	
	unsigned int seq_ctrl_fn = // fragment number subfield - 4 bits; Assigned to each fragment of an MSDU.
		(unsigned int)out_bytes[24] & 0x0F;
	
	unsigned int bytes_len = d_tx.psdu_size - 4;	// WHY minus 4??
	
	printf("new,type: %d, pda: %02X%02X%02X%02X%02X%02X, psa: %02X%02X%02X%02X%02X%02X, Seq_num: %04d,  len: %04d,  mcs_rate: %02d, crc_err: %d, SNR1: %d, RSSI1: %d, SNR2: %d, RSSI2: %d,  frame_type_hex: %02X, subtype_hex: %02X", !is_multicast, *pda,*(pda+1), *(pda+2),*(pda+3),*(pda+4),*(pda+5),*psa,*(psa+1), *(psa+2),*(psa+3),*(psa+4),*(psa+5), seq_ctrl_sn, bytes_len, d_ofdm.encoding, 0, (int)est_snr(), 0, 0, 0, frame_type, frame_subtype);
	
	return 0;
}
	
	// compute the power of a complex
	float compute_complex_pow(gr_complex comp) {
		return std::pow(comp.real(),2) + std::pow(comp.imag(),2);
	}
	float compute_complex_pow(gr_complexd comp) {
		return std::pow(comp.real(),2) + std::pow(comp.imag(),2);
	}
	// SNR estimation using decoding results
	float est_snr() {
		int sym_tot = d_tx.n_sym * 48;
		cvec symbols;
		symbols.set_length(sym_tot);
		for(int i = 0; i < sym_tot; i++) {
			symbols[i] = std::complex<double>(sym[i]);
		}
		int sym_ref[sym_tot];
		for(int i=0;i<sym_tot;i++) {
			int tmp = 0;
			int offset = d_ofdm.n_bpsc * i;
			for(int j=0;j<d_ofdm.n_bpsc;j++) {
				if(bits[offset + j] == -1)
					tmp = (tmp<<1) | 1;
				else
					tmp = tmp<<1;
			}
			sym_ref[i] = tmp;
		}
		const std::complex<double>* ref_cons;	// constellation
		switch(d_ofdm.encoding) {
			case BPSK_1_2:
			case BPSK_3_4:
				ref_cons = BPSK_D;
				break;
			case QPSK_1_2:
			case QPSK_3_4:
				ref_cons = QPSK_D;
				break;
			case QAM16_1_2:
			case QAM16_3_4:
				ref_cons = QAM16_D;
				break;
			case QAM64_2_3:
			case QAM64_3_4:
				ref_cons = QAM64_D;
				break;
		}	
		float noise_pow = 0;
		
		for(int i=0;i<sym_tot;i++) {
			noise_pow += compute_complex_pow(symbols[i]-ref_cons[sym_ref[i]]);
		}
		return -10*std::log10(noise_pow/sym_tot);
	}

int decode_vlc() {
	/* Parameters should be given until now
	 nSymTot 		- Number of symbols (48 subcarriers) used - 12-bit
	 psduSize 		- PSDU size in bytes
	 nSubcarPadding - Number of padding subcarriers to assemble a full group
	 nBitsPadding	- Number of padding bits to assemble a full symbol
	*/
	dout << "entering decode_vlc\n";
	/*
	int psduSize = 159;
	int nSubcarPadding = 24;
	int nBitsPadding = 0;
	VlcMod &vlcMod = vlcMod24;
	*/
	int psduSize = 159;
	int nSubcarPadding = 4;
	int nBitsPadding = 1;
	VlcMod &vlcMod = vlcMod12;
	int numOfMarkers = calc_nMarker(nSymTot, nSubcarPadding, vlcMod.nMarker);
	int numOfNonMarkers = calc_nNonMarker(nSymTot, numOfMarkers, nSubcarPadding);
	int nBitsTot = calc_totBits(psduSize, nBitsPadding, 0.5);
	int nLongerSubcar = nBitsTot - vlcMod.lenSet[1] * numOfNonMarkers;
	int nSym_valid = numOfMarkers + numOfNonMarkers;
	dout << "Number of total symbols (48 subcarriers) used = " << nSymTot << std::endl
		<< "PSDU size in bytes = " << psduSize << std::endl
		<< "Number of padding subcarriers to assemble a full symbol = " << nSubcarPadding << std::endl
		<< "Number of padding bits to assemble a full subcarrier = " << nBitsPadding << std::endl
		<< "Number of markers = " << numOfMarkers << std::endl
		<< "Number of non-markers = " << numOfNonMarkers << std::endl
		<< "Number of valid bits (including nBitsPadding) = " << nBitsTot << std::endl
		<< "Number of longer-code symbols = " << nLongerSubcar << std::endl;
/*
	// PRINT all received signals
	dout << "============ Print received signals ============" << std::endl;
	for(int i=0;i<14*48;i++) {
		dout << std::setw(3) << i << "," << sym[i] << std::endl;
	}
	dout << "============ End printing Received signals ============" << std::endl;
*/	
	gr_complex *sym_nonMarker 	= (gr_complex*) calloc(numOfNonMarkers, sizeof(gr_complex));
	gr_complex *sym_marker 		= (gr_complex*) calloc(numOfMarkers, sizeof(gr_complex));
	
	// Extract nonMarker symbol and marker symbol
	int nonMarkerIdx = 0;
	int markerIdx = 0;
	for(int i=0;i<nSym_valid; i++) {
		if(i!=0 && (i+1)%vlcMod.nMarker == 0) {
			sym_marker[markerIdx++] = sym[i];
		}
		else {
			sym_nonMarker[nonMarkerIdx++] = sym[i];
		}
	}
	/*
	if(nonMarkerIdx == numOfNonMarkers && markerIdx == numOfMarkers) {
		dout << "NonMarker symbol and marker symbol extraction successful" << std::endl;
	}
	else {
		dout << "NonMarker symbol and marker symbol extraction failed" 
			<< ", nonMarkerIdx = " << nonMarkerIdx
			<< ", numOfNonMarkers = " << numOfNonMarkers
			<< ", markerIdx = " << markerIdx
			<< ", numOfMarkers = " << numOfMarkers << std::endl;
		return 1;
	}
	*/
	// marker symbol demodulation
	int marker[numOfMarkers*vlcMod.nMarkerBackup];
	double dist_marker[numOfMarkers*vlcMod.nMarkerBackup];
	int markerOut[numOfMarkers];
	vlcMod.markerDecode(sym_marker, numOfMarkers, marker, dist_marker);
	/*
	dout << "============ Marker decoded using markerDecode ============" << std::endl;
	for(int i=0;i<numOfMarkers;i++) {
		dout << "Marker " << std::setw(2) << i << " = " << marker[vlcMod.nMarkerBackup*i] << std::endl;
	}
	dout << "============ End of markerDecode results ============\n";	
	*/
	if(dp_demod(nLongerSubcar, marker, dist_marker, numOfMarkers, vlcMod.nMarkerBackup, markerOut, true) == 0) {
		dout << "============ Marker decoded in ofdm_decode_mac_vlc ============" << std::endl;
		for(int i=0;i<numOfMarkers;i++) {
			dout << "Marker " << std::setw(2) << i << " = " << markerOut[i] << std::endl;
		}
		dout << "dp_demod for marker succeeded\n";
	}
	else {
		dout << "dp_demod for marker failed\n";
		return 1;
	}
	bool printFlag = false;
	double markerErrRate = calcINTDiffRate(markerOut, ref_marker_24qam, numOfMarkers, printFlag);
	std::cout << "marker error rate is " << std::setprecision(3) << markerErrRate << ", ";
	
	// nonMarker symbol demodulation
	int nonMarker[numOfNonMarkers*2];	// TODO: Replace 2 with the number of codeword lengths, QAMDemod_nonMarker needs change as well
	double dist_nonMarker[numOfNonMarkers*2];
	
//	std::cout << "==================== Entering QAMDemod_nonMarker ====================" << std::endl;

	vlcMod.QAMDemod_nonMarker(sym_nonMarker, numOfNonMarkers, nonMarker, dist_nonMarker, 2);
/*	
	for(int i=0;i<numOfNonMarkers;i++) {
		if(i%(vlcMod.nMarker-1) == 0) {
			int mIdx = i/(vlcMod.nMarker - 1);
			std::cout << "Marker " << mIdx << " = " << markerOut[mIdx] << std::endl;
		}
		std::cout << std::setw(3) << i << ": "<< nonMarker[2*i] << "(" << dist_nonMarker[2*i] << "), "
			<< nonMarker[2*i+1] << "(" << dist_nonMarker[2*i]+1 << ")" << std::endl;
	}
*/
//	std::cout << "==================== Exit QAMDemod_nonMarker ====================" << std::endl;
	
	// group-wise symbol length error correction
	int nDataSymPerMarker = vlcMod.nMarker - 1;
	int nonMarkerOutIdx[nDataSymPerMarker*2];
	int lenSetRep[nDataSymPerMarker*2];
	for(int i=0;i<nDataSymPerMarker;i++) {
		lenSetRep[2*i] = vlcMod.lenSet[0];
		lenSetRep[2*i+1] = vlcMod.lenSet[1];
	}
	int nonMarkerOut[numOfNonMarkers];
	
	// determine the nonMarker symbol demodulation
	for(int i=0;i<numOfMarkers;i++) {
		// compute the number of valid nonMarker symbols corresponding to each marker (The last marker needs attention)
		int nDataSymPerMarker_tmp = nDataSymPerMarker;
		if(i == numOfMarkers-1) {
			nDataSymPerMarker_tmp = nDataSymPerMarker - (numOfMarkers * nDataSymPerMarker - numOfNonMarkers);
		}
		
		//std::cout << "Marker " << i << ": " << markerOut[i] << ", nDataSymPerMarker_tmp = " << nDataSymPerMarker_tmp << std::endl;
		dp_demod(markerOut[i]+nDataSymPerMarker_tmp*vlcMod.lenSet[1], lenSetRep, dist_nonMarker+i*nDataSymPerMarker*2, nDataSymPerMarker_tmp, 2, nonMarkerOutIdx, false);
		for(int j=0;j<nDataSymPerMarker_tmp;j++) {
			nonMarkerOut[i*nDataSymPerMarker+j] = nonMarker[2*(i*nDataSymPerMarker+j)+nonMarkerOutIdx[j]];
			// std::cout << i*nDataSymPerMarker+j << " - " << nonMarkerOut[i*nDataSymPerMarker+j] << ", " << std::endl;
		}
	}
	
	double nonMarkerErrRate = calcINTDiffRate(nonMarkerOut, ref_nonMarker_24qam, numOfNonMarkers, printFlag);
	std::cout << "nonMarker error rate is " << std::setprecision(3) << nonMarkerErrRate << ", ";
	
	int nBitLen = vlcMod.demapper(nonMarkerOut, out_bits, numOfNonMarkers);
	if(nBitLen != nBitsTot) {
		std::cout << "nBitLen (" << nBitLen << ") is unequal to nBitsTot(" << nBitsTot << ")" << std::endl;
		return 2;
	}

	// Viterbi decoding
	decode_conv_vlc(out_bits, nBitLen - nBitsPadding);
	
	double ber = calcDiffRate(decoded_bits_vlc, ref_data_bits_24qam, (nBitLen - nBitsPadding)/2, printFlag);
	
	std::cout << "total Bits is " << (nBitLen - nBitsPadding)/2
		<< ", BER = " << std::setprecision(3) << ber << std::endl;
	return 0;
}

int calc_nMarker(int nSymTot, int nSubcarPadding, int nSubcarPerGroup) {
	int nSubcarTot = nSymTot * 48 - nSubcarPadding;
	if(nSubcarTot % nSubcarPerGroup == 0) {
		return nSubcarTot/nSubcarPerGroup;
	}
	else {
		return ceil(nSubcarTot / (nSubcarPerGroup*1.0));
	}
}
int calc_nNonMarker(int nSymTot, int nMarker, int nSubcarPadding) {
	return nSymTot * 48 - nMarker - nSubcarPadding;
}

int calc_totBits(int psdu_size, int nBitsPadding, double code_rate) {
	return (16 + 8*psdu_size + 6)/code_rate + nBitsPadding;
}

void decode_conv_vlc(char *encoded_bits, int n_encoded_bits) {
	Punctured_Convolutional_Code code;
	ivec generator(2);
	generator(0)=0133;
	generator(1)=0171;
	code.set_generator_polynomials(generator, 7);

	bmat puncture_matrix;
	puncture_matrix = "1 1; 1 1";
	code.set_puncture_matrix(puncture_matrix);
	code.set_truncation_length(30);

	dout << "VLC coding rate " << code.get_rate() << std::endl;
	dout << "VLC encoded bit length " << n_encoded_bits << std::endl;

	double encoded_bits_double[n_encoded_bits];
	for(int i=0;i<n_encoded_bits;i++) {
		encoded_bits_double[i] = (encoded_bits[i] -'0') * (-2) + 1;
	}
	vec rx_signal(encoded_bits_double, n_encoded_bits);

	code.reset();
	decoded_bits_vlc.set_length(n_encoded_bits*code.get_rate());
	code.decode_tail(rx_signal, decoded_bits_vlc);
	/*
	// Print decoded bits
	for(int i=0;i<n_encoded_bits;i++) {
		dout << std::setw(4) << i << ": " << decoded_bits_vlc[i] << std::endl;
	}
	*/
}

private:
	gr_complex sym[1000 * 48 * 100];
	vec bits;
	double deinter[1000 * 48];
	char out_bits[40000];
	char out_bytes[40000];
	bvec decoded_bits;
	bvec decoded_bits_vlc;

	bool d_debug;
	bool d_log;
	tx_param d_tx;
	ofdm_param d_ofdm;
	int copied;
	bool d_frame_complete;

	Modulator<std::complex<double> > bpsk;
	Modulator<std::complex<double> > qpsk;
	Modulator<std::complex<double> > qam16;
	Modulator<std::complex<double> > qam64;

	int nSymTot = 13;	// total symbols (48 subcarriers) to be captured

	VlcMod vlcMod10 = VlcMod(10,2,2);
	VlcMod vlcMod12 = VlcMod(12,2,2);
	VlcMod vlcMod14 = VlcMod(14,2,2);
	VlcMod vlcMod20 = VlcMod(20,3,2);
	VlcMod vlcMod24 = VlcMod(24,3,2);
	VlcMod vlcMod28 = VlcMod(28,3,2);
	VlcMod vlcMod40 = VlcMod(40,3,2);
	VlcMod vlcMod48 = VlcMod(48,3,2);
	VlcMod vlcMod56 = VlcMod(56,3,2);
	
	unsigned int wifi_adapter_addr[6] = 	{0x8C,0xAE,0x4C,0xF8,0x07,0x23};
	unsigned int wifi_multicast_addr[6] = 	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
};

ofdm_decode_mac_vlc::sptr
ofdm_decode_mac_vlc::make(bool log, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_decode_mac_vlc_impl(log, debug));
}


