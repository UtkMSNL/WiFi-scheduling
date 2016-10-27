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
#include <ieee802-11/ofdm_decode_mac_constellation.h>

#include "utils.h"
#include "ber_side_valid.h"
#include <boost/crc.hpp>
#include <gnuradio/io_signature.h>
#include <itpp/itcomm.h>
#include <iomanip>
#include <cmath>

#define PI 3.14159265358979

using namespace gr::ieee802_11;
using namespace itpp;

class ofdm_decode_mac_constellation_impl : public ofdm_decode_mac_constellation {

	public:

	const unsigned char mac_header_uchar[24] = {
								0x00,0x00,0x08,0x00,0x00,0x00,
								0x42,0x42,0x42,0x42,0x42,0x42,
								0x23,0x23,0x23,0x23,0x23,0x23, 
								0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
							};
							
	const char* data_side = "XzBmIi18F8Ep1bzO5XDyDcfgQ87q36q4yOcO5BPtNyVLOqhFGua7wbHcA6LD7LTHfINWFhzlVSY8DVnxCvmLQIW09soYGHFvON78gD5gcqVv8UQw06teqQhLMxHA1LPeGwHFUJD9XGVJ47SUIqih8W7sODUbgpkPfwbaEmhkTBLfXvSFDuR22Hm7jGId7h3fXO8IIhl1gSmahuBzCACJ0avFUAsDupEqp7RiFumTdpjuFPoQ";

	const char *mac_header_char = (const char*)mac_header_uchar;					
	const char data_char = 'Y';
	ofdm_decode_mac_constellation_impl(double alpha, bool debug, Encoding e_side) : block("ofdm_decode_mac_constellation",
				gr::io_signature::make(1, 1, 48 * sizeof(gr_complex)),
				gr::io_signature::make(0, 0, 0)),
				d_alpha(alpha),
				d_debug(debug),
				d_ofdm(BPSK_1_2),
				d_tx(d_ofdm, 0),
				d_frame_complete(true),
				d_e_side(e_side),
				ofdm_side(e_side),
				tx_side(ofdm_side, 0)
	{
		message_port_register_out(pmt::mp("out"));
		
		// Added by Haoyang
		message_port_register_out(pmt::mp("symbol out"));
		message_port_register_out(pmt::mp("side out"));
		
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
		
		set_encoding_side(e_side);	// configure the code rate of side channel
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
					decode();
					
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

	// compute the bits difference between two characters
	int compare_bytes(char byte1, const char byte2) {
		int x = (int)((byte1 & 0xFF) ^ (byte2 & 0xFF));
		int sum = 0;
		for(int i = 0; i < 8; i++) {
			if(x & (1 << i)) {
				sum++;
			}
		}
		return sum;
	}

	// return the main channel BER in percentage (ignore the Sequence number and CRC)		
	double calc_ber_main() {
		int sum = 0;
		// compare header
		for(int i=0;i<24;i++) {
			sum += compare_bytes(out_bytes[i], mac_header_char[i]);
		}
		// compare data block
		for(int i=26,len = 26+d_tx.psdu_size-28; i<len; i++) {
			sum += compare_bytes(out_bytes[i], data_char);
		}
		
		return sum * 100.0/((d_tx.psdu_size-4) * 8);
	}

	// return the side channel BER in percentage 
	double calc_ber_side() {
		int sum = 0;
		for(int i=0,len = tx_side.psdu_size; i<len; i++) {
			sum += compare_bytes(out_bytes_side[i+2], data_side[i]);
		}
		
		return sum * 100.0/((tx_side.psdu_size) * 8);
	}
	
	/*
		Usage: Decode the side channel, compute the statistics, e.g. detection rate of the side channel
	*/
	void demodulate_side_constellation(cvec &signal, vec &out) {
		int tot_subcarriers = d_tx.n_sym * 48;	// total number of subcarriers
		
		out.set_length(ofdm_side.n_bpsc * signal.size());	// store the bits in side channel: 2 bits / subcarrier
		
		double abs_real, abs_imag;
		int sign_real, sign_imag;
		double central_real, central_imag;
		int bit_real, bit_imag;
		int max_offset;
		double half_minDist;
		
		switch(d_ofdm.encoding) {
			case BPSK_1_2:
			case BPSK_3_4:
				half_minDist = 1;
				max_offset = 1;
				break;

			case QPSK_1_2:
			case QPSK_3_4:
				half_minDist = 0.707106781;
				max_offset = 1;
				break;

			case QAM16_1_2:
			case QAM16_3_4:
				half_minDist = 0.316227766;
				max_offset = 3;
				break;
				
			case QAM64_2_3:
			case QAM64_3_4:
				half_minDist = 0.15430335;
				max_offset = 7;
				break;
		}
		
		for(int i=0; i<signal.size(); i++) {
			abs_real = std::abs(signal(i).real());		// the abs part of the real part
			abs_imag = std::abs(signal(i).imag());		// the abs part of the imag part
			sign_real = signal(i).real() > 0 ? 1 : -1;	// the sign of the real part
			sign_imag = signal(i).imag() > 0 ? 1 : -1;	// the sign of the imag part
			central_real = sign_real * (abs_real - std::min(max_offset, (2*int(abs_real/(2*half_minDist))+1)) * half_minDist);
			bit_real = central_real > 0 ? 1 : 0;
			
			if(d_ofdm.encoding != BPSK_1_2 && d_ofdm.encoding != BPSK_3_4) {
				central_imag = sign_imag * (abs_imag - std::min(max_offset, (2*int(abs_imag/(2*half_minDist))+1)) * half_minDist);
			}
			else {
				central_imag = sign_imag * abs_imag;
			}
			
			bit_imag = central_imag > 0 ? 1 : 0;
			
			if(d_e_side == QPSK_1_2) {
				out.replace_mid(2*i, side_codebook_QPSK[bit_real*2 + bit_imag]);
			}
			else if(d_e_side == PSK8_1_2) {
				double ang = std::atan2(central_imag, central_real);

				if(ang < 0)	// switch the range of angle from -PI~PI to 0~2PI
					ang += 2*PI;
				
				int zone_index = 7;
				if(ang < PI/8 || ang > 15*PI/8)
					zone_index = 7;
				else {
					zone_index = (ang - PI/8) / (PI/4);
				}
				out.replace_mid(3*i, side_codebook_PSK8[zone_index]);
			}
		}
		
		out = out * (-2) + 1;
	}

	void decode_side() {
		cvec symbols;
		symbols.set_length(d_tx.n_sym * 48);
		
		for(int i = 0; i < d_tx.n_sym * 48; i++) {
			symbols[i] = std::complex<double>(sym[i]);
		}
		
		demodulate_side_constellation(symbols, bits_sideChan);
		
		int n_cbps = ofdm_side.n_cbps;
		deinterleave_side();
/*		for(int i = 0; i < tx_side.n_sym; i++) {
			for(int k = 0; k < n_cbps; k++) {
				deinter_side[i * n_cbps + k] = bits_sideChan[i * n_cbps + k];
			}
		}
*/		
		decode_conv_side();
		descramble_side();
	}

	void deinterleave_side() {

		int n_cbps = ofdm_side.n_cbps;
		int first[n_cbps];
		int second[n_cbps];
		int s = std::max(ofdm_side.n_bpsc / 2, 1);
		
		for(int j = 0; j < n_cbps; j++) {
			first[j] = s * (j / s) + ((j + int(floor(16.0 * j / n_cbps))) % s);
		}

		for(int i = 0; i < n_cbps; i++) {
			second[i] = 16 * i - (n_cbps - 1) * int(floor(16.0 * i / n_cbps));
		}

		for(int i = 0; i < tx_side.n_sym; i++) {
			for(int k = 0; k < n_cbps; k++) {
				deinter_side[i * n_cbps + second[first[k]]] = bits_sideChan[i * n_cbps + k];
			}
		}
	}
	
	void decode_conv_side() {
		Punctured_Convolutional_Code code;
		ivec generator(2);
		generator(0)=0133;
		generator(1)=0171;
		code.set_generator_polynomials(generator, 7);

		bmat puncture_matrix;
		switch(ofdm_side.encoding) {
		case BPSK_1_2:
		case QPSK_1_2:
		case QAM16_1_2:
		case PSK8_1_2:	// for side channel use only
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
		dout << tx_side.n_encoded_bits << std::endl;

		vec rx_signal(deinter_side, tx_side.n_encoded_bits);

		code.reset();
		decoded_bits_side.set_length(tx_side.n_encoded_bits);
		code.decode_tail(rx_signal, decoded_bits_side);
		//dout << "length decoded " << decoded_bits.size() << std::endl;
		//std::cout << decoded_bits << std::endl;
	}

	void descramble_side() {
		int state = 0;
		for(int i = 0; i < 7; i++) {
			if(decoded_bits_side(i)) {
				state |= 1 << (6 - i);
			}
		}

		int feedback;

		for(int i = 7; i < decoded_bits_side.size(); i++) {
			feedback = ((!!(state & 64))) ^ (!!(state & 8));
			out_bits_side[i] = feedback ^ (int)decoded_bits_side(i);
			state = ((state << 1) & 0x7e) | feedback;
		}

		for(int i = 0; i < decoded_bits_side.size(); i++) {
			int bit = i % 8;
			int byte = i / 8;
			if(bit == 0) {
				out_bytes_side[byte] = 0;
			}

			if(out_bits_side[i]) {
				out_bytes_side[byte] |= (1 << bit);
			}
		}
	}
	void decode() {
		demodulate();
		deinterleave();
		decode_conv();
		descramble();
		
		unsigned int mac_src_addr[6];
		for(int i=0;i<6;i++) {
			mac_src_addr[i] = ((unsigned int)out_bytes[i+12] & 0xFF);
		}
		print_output();
		
		// create PDU
		/*
		pmt::pmt_t blob = pmt::make_blob(out_bytes + 2, d_tx.psdu_size - 4);
		pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
		std::cout << "create pdu 1 completes\n";
		pmt::pmt_t dict = pmt::make_dict();
		std::cout << "create pdu 2 completes\n";
		dict = pmt::dict_add(dict, pmt::mp("encoding"), enc);
		std::cout << "create pdu 3 completes\n";
		message_port_pub(pmt::mp("out"), pmt::cons(dict, blob));
		
		std::cout << "create pdu completes\n";
		*/
		// Added by Haoyang, output the received symbols and the side channel decoding
		
		if(mac_src_addr[0] == 0x23 && d_tx.n_sym == 27) {	// sent from another USRP running side channel
			unsigned int SN = ((((unsigned int)out_bytes[25] & 0xFF) << 8) | ((unsigned int)out_bytes[24] & 0xFF)) >> 4;

			gr::thread::scoped_lock lock(d_mutex);
			decode_side();
			// print_output_side();
			std::cout<<"Encoding "<<d_ofdm.encoding<<", ";
			std::cout<<"SN "<<std::setfill('0') << std::setw(5) << SN<<", ";
			std::cout<<"Symbol length "<<d_tx.n_sym<<", ";// Symbol error number " << std::setfill(' ') << std::setw(3)<<n_err<<",";

			double ber_main = calc_ber_main();	// not include sequence number (2 bytes) and crc (2 bytes)
			double ber_side = calc_ber_side();
			std::cout << "Main channel BER "<<std::setw(5) << std::fixed << std::setprecision(2) << ber_main << ", ";
			std::cout << "Side channel BER "<<std::setw(5) << std::fixed << std::setprecision(2) << ber_side << ", ";
			std::cout << "SNR " << est_snr() <<" dB"<<std::endl;
/*		
			// create message of side channel decoding
			pmt::pmt_t blob1 = pmt::make_blob(out_sym_side, d_tx.n_sym * 48 * sizeof(gr_complex));
			//pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
			pmt::pmt_t dict1 = pmt::make_dict();
			dict = pmt::dict_add(dict1, pmt::mp("encoding"), enc);
			message_port_pub(pmt::mp("symbol out"), pmt::cons(dict1, blob1));
*/
		}

		// skip service field
		boost::crc_32_type result;
		result.process_bytes(out_bytes + 2, d_tx.psdu_size);
		if(result.checksum() != 558161692) {
			dout << "checksum wrong -- dropping" << std::endl;
			// ADD LOG TO DEBUG
			dout<<"encoding: "<<d_ofdm.encoding<<",PSDU length: "<<d_tx.psdu_size<<",symbols: "<<d_tx.n_sym<<std::endl;
			return;
		}
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
		int nbytes = decoded_bits.size() / 8;
		for(int i = 0; i < nbytes; i++) {
			dout << std::setfill('0') << std::setw(2) << std::hex << ((unsigned int)out_bytes[i] & 0xFF) << std::dec << " ";
			if(i % 16 == 15) {
				dout << std::endl;
			}
		}
		dout << std::endl;
		for(int i = 0; i < nbytes; i++) {
			if((out_bytes[i] > 31) && (out_bytes[i] < 127)) {
				dout << ((char) out_bytes[i]);
			} else {
				dout << ".";
			}
		}
		dout << std::endl;
	}
	
	void print_output_side() {
		std::cout << std::endl;
		int nbytes = decoded_bits_side.size() / 8;
		for(int i = 0; i < std::min(500, decoded_bits_side.size()); i++) {
			std::cout << !!out_bits_side[i];
			if(i % 100 == 99) {
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;

		for(int i = 0; i < std::min(50, nbytes); i++) {
			if((out_bytes_side[i] > 31) && (out_bytes_side[i] < 127)) {
				std::cout << ((char) out_bytes_side[i]);
			} else {
				std::cout << ".";
			}
		}
		std::cout << std::endl;
	}
	
	
	// compute the power of a complex
	double compute_complex_pow(gr_complex comp) {
		return std::pow(comp.real(),2) + std::pow(comp.imag(),2);
	}
	
	double compute_complex_pow(gr_complexd comp) {
		return std::pow(comp.real(),2) + std::pow(comp.imag(),2);
	}
	
	// SNR estimation using decoding results
	double est_snr() {
		int sym_tot = d_tx.n_sym * 48;

		const gr_complex* ref_cons;	// constellation
		switch(d_ofdm.encoding) {
			case BPSK_1_2:
				ref_cons = ref_enc_const0;
				break;
			case BPSK_3_4:
				ref_cons = ref_enc_const1;
				break;
			case QPSK_1_2:
				ref_cons = ref_enc_const2;
				break;
			case QPSK_3_4:
				ref_cons = ref_enc_const3;
				break;
			case QAM16_1_2:
				ref_cons = ref_enc_const4;
				break;
			case QAM16_3_4:
				ref_cons = ref_enc_const5;
				break;
			case QAM64_2_3:
				ref_cons = ref_enc_const6;
				break;
			case QAM64_3_4:
				ref_cons = ref_enc_const7;
				break;
		}	
		double noise_pow = 0;
		
		for(int i=0;i<sym_tot;i++) {
			noise_pow += compute_complex_pow(sym[i]-ref_cons[i]);
		}
		return -10*std::log10(noise_pow/sym_tot);
	}

	void set_encoding_side(Encoding e_side) {
		std::cout << "ofdm_decode_mac_constellation : encoding_side" << e_side << std::endl;
		gr::thread::scoped_lock lock(d_mutex);
		d_e_side = e_side;
		ofdm_side = ofdm_param(e_side);
		switch(e_side) {
			case QPSK_1_2: tx_side = tx_param(ofdm_side, 159); break;
			case PSK8_1_2: tx_side = tx_param(ofdm_side, 240); break;
			default: std::cout << "Side channel encoding should be QPSK_1_2 or PSK8_1_2" << std::endl;
		}
	}	
	private:
		gr_complex sym[1000 * 48 * 100];
		// gr_complex out_sym_side[1000 * 48 * 100];	// Added by Haoyang: store the side channel decoding
		vec bits;
		double deinter[1000 * 48];
		char out_bits[40000];
		char out_bytes[40000];
		bvec decoded_bits;
		double d_alpha;
		bool d_debug;
		tx_param d_tx;
		ofdm_param d_ofdm;
		
		Encoding d_e_side;
		ofdm_param ofdm_side;		// for code rate of side channel
		tx_param tx_side;
		vec bits_sideChan;	// Added by Haoyang: store the side channel decoding bits (constellation)
		double deinter_side[1000 * 48];
		char out_bits_side[40000];
		char out_bytes_side[40000];
		bvec decoded_bits_side;
		int copied;
		bool d_frame_complete;
		vec side_codebook_QPSK[4] = {"0,0", "0,1", "1,0", "1,1"};
		vec side_codebook_PSK8[8] = {"1,1,0", "0,1,0", "0,1,1", "0,0,1","0,0,0", "1,0,0", "1,0,1", "1,1,1"};
		gr::thread::mutex d_mutex;

		Modulator<std::complex<double> > bpsk;
		Modulator<std::complex<double> > qpsk;
		Modulator<std::complex<double> > qam16;
		Modulator<std::complex<double> > qam64;
};

ofdm_decode_mac_constellation::sptr
ofdm_decode_mac_constellation::make(double alpha, bool debug, Encoding e_side) {
	return gnuradio::get_initial_sptr(new ofdm_decode_mac_constellation_impl(alpha, debug, e_side));
}

