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
#include <ieee802-11/ofdm_decode_mac.h>

#include "utils.h"

#include <boost/crc.hpp>
#include <gnuradio/io_signature.h>
#include <itpp/itcomm.h>
#include <iomanip>

using namespace gr::ieee802_11;
using namespace itpp;

class ofdm_decode_mac_impl : public ofdm_decode_mac {

	public:
	
	const gr_complex Complex_One = gr_complex(1,0);
	const gr_complex Complex_Zero = gr_complex(0,0);
	const int sym_mem_size = 48 * sizeof(gr_complex);
	int side_coding1[48*27] = {
		1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 0
		1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 1
		1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 2
		1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 3
		1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 4
		1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 5
		1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 6
		1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 7
		1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 8
		1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 9
		1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 10
		1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 11
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 12
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 13
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 14
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 15
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 16
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 17
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 18
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 19
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 20
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 21
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 22
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 23
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 24
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 25
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,	// Symbol 26
	};

	int side_coding2[48*27] = {
		1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
	};
	const unsigned char mac_header_uchar[24] = {
								0x00,0x00,0x08,0x00,0x00,0x00,
								0x42,0x42,0x42,0x42,0x42,0x42,
								0x23,0x23,0x23,0x23,0x23,0x23, 
								0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
							};
	const char *mac_header_char = (const char*)mac_header_uchar;					
	const char data_char = 'Y';
	ofdm_decode_mac_impl(bool log, bool debug, int n_zeros) : block("ofdm_decode_mac",
				gr::io_signature::make(1, 1, 48 * sizeof(gr_complex)),
				gr::io_signature::make(0, 0, 0)),
				d_log(log),
				d_debug(debug),
				d_n_zeros(n_zeros),
				d_ofdm(BPSK_1_2),
				d_tx(d_ofdm, 0),
				d_frame_complete(true)
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
		
		if(d_n_zeros==0) {	// process regular WiFi packets (side channel disabled)
			std::cout<<"side channel disabled"<<std::endl;
		}
		else {				// process WiFi packets with side channel
			if(d_n_zeros==1) {
				side_coding = side_coding1;
				std::cout<<"Single 0:";
			}
			else if(d_n_zeros==2){
				side_coding = side_coding2;
				std::cout<<"Dual 0:";
			}
			else {
				std::cout<<"?????????????????????"<<std::endl;
			}
			for(int i=0;i<48;i++)
				std::cout<<side_coding[i]<<",";
			std::cout<<std::endl;
		}
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
	
	// compute the power of a complex
	float compute_complex_pow(gr_complex comp) {
		return std::pow(comp.real(),2) + std::pow(comp.imag(),2);
	}
	float compute_complex_pow(gr_complexd comp) {
		return std::pow(comp.real(),2) + std::pow(comp.imag(),2);
	}

	// return the index of the nleast least index in the buffer 
	void IndexSort(int *pIndex, float *buffer, int buffer_len, int nleast) {
		float f_tmp;
		int i_tmp = 0;
		bool swap = false;
		
		for(int j=0;j<nleast;j++) {	// if change nleast to buffer_len, sort thoroughly
			swap = false;
			f_tmp = buffer[pIndex[j]];
			for(int i=j+1;i<buffer_len;i++) {	// find the element larger than the reference
				if(f_tmp > buffer[pIndex[i]]) {
					i_tmp = i;
					f_tmp = buffer[pIndex[i]];
					swap = true;
				}
			}
			if(swap) {
				int iswap;
				// exchange the index
				iswap = pIndex[i_tmp];
				pIndex[i_tmp] = pIndex[j];
				pIndex[j] = iswap;
			}
		}
	}

/*-------- Added by Haoyang: decode one symbol of the side channel using minimum-energy detection ---------*/	
	void decode_one_symbol(gr_complex *in, gr_complex *out, int n_zeros) {
		float power[48];
		int idx[48];
		int i;
		
		// compute the power of each subcarrier
		for(i=0;i<48;i++) {
			idx[i] = i;	// initialize the index
			power[i] = compute_complex_pow(in[i]);
		}
		
		IndexSort(idx, power, 48, n_zeros);
		
		for(i=0;i<48;i++) {
			out[i] = Complex_One;
		}
		
		for(i=0;i<n_zeros;i++) {
			out[idx[i]] = Complex_Zero;
		}
	}
	
	/* validate the decoding of side channel
		Input:	n	- symbol index
				fn	- false negative
				fp	- false positive
		Return:	True - if match
				Fasle - Mismatch
	*/
	bool check_side_symbol(int n) {
		int addr_offset = 48 * n;
		bool is_correct = true;
		for(int i=0;i<48;i++) {
			if(out_sym_side[addr_offset + i].real() != side_coding[addr_offset + i]) {
				is_correct = false;
				break;
			}
		}
		return is_correct;
	}

	// compute the bits difference between two char
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
	
	int calc_ber() {
		int sum = 0;
		// compare header
		for(int i=0;i<24;i++) {
			sum += compare_bytes(out_bytes[i], mac_header_char[i]);
		}
		// compare data block
		for(int i=26,len = 26+d_tx.psdu_size-28; i<len; i++) {
			sum += compare_bytes(out_bytes[i], data_char);
		}
		return sum;
	}	
	/*
		Usage: Decode the side channel, compute the statistics, e.g. detection rate of the side channel
		Parameters: n_zeros - number of zeros per symbol
	*/
	void decode_side_channel(int n_0s) {
		float power[48] = {0.0};
		gr_complex *dec_tmp = (gr_complex*)calloc(48, sizeof(gr_complex));
		int addr_offset = 0;
		int n_err = 0;
		for(int i=0; i<d_tx.n_sym; i++) {
			// find the subcarrier with the smallest energy and decode it as '0' in side channel
			addr_offset = 48 * i;
			decode_one_symbol(sym + addr_offset, dec_tmp, n_0s);
			memcpy(out_sym_side + addr_offset, dec_tmp, sym_mem_size);
			if(check_side_symbol(i) == false) {	// the side channel decoding is wrong
				n_err++;
			}
		}
		unsigned int SN = ((((unsigned int)out_bytes[25] & 0xFF) << 8) | ((unsigned int)out_bytes[24] & 0xFF)) >> 4;
		std::cout<<"Encoding "<<d_ofdm.encoding<<",";
		std::cout<<" SN "<<std::setfill('0') << std::setw(5) << SN<<",";
		std::cout<<" Symbol length "<<d_tx.n_sym<<", Symbol error number " << std::setfill(' ') << std::setw(3)<<n_err<<",";
		
		int biterror = calc_ber();
		float ber = biterror * 1.0/((d_tx.psdu_size-4) * 8);	// not include sequence number (2 bytes) and crc (4 bytes)
		std::cout<<" Bit error number " << std::setfill(' ') << std::setw(6) << biterror <<", BER "<<std::setw(5) << std::fixed << std::setprecision(2)<<ber*100<<",";
		
		std::cout<<" SNR "<< std::setw(8) << std::fixed << std::setprecision(4) <<est_snr()<<" dB,"<<std::endl;
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
		if(d_tx.n_sym == 27 && d_n_zeros) {	// remove filter "mac_src_addr[0] == 0x23 && "
			print_output();
			//contain_side_chan = true;
			decode_side_channel(d_n_zeros);
		}
		else {
			if(d_n_zeros == 0) {
				print_output();
			//	contain_side_chan = false;
			}
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

		mylog(boost::format("encoding: %1% - length: %2% - symbols: %3%")
				% d_ofdm.encoding % d_tx.psdu_size % d_tx.n_sym);

		// create PDU
		pmt::pmt_t blob = pmt::make_blob(out_bytes + 2, d_tx.psdu_size - 4);
		pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
		pmt::pmt_t dict = pmt::make_dict();
		dict = pmt::dict_add(dict, pmt::mp("encoding"), enc);
		message_port_pub(pmt::mp("out"), pmt::cons(dict, blob));
		
		// Added by Haoyang, output the received symbols and the side channel decoding
		if(contain_side_chan) {
			if(d_tx.n_sym != 27) {
				std::cout<<"Symbol number ("<<d_tx.n_sym<<") is incorrect!"<<std::endl;
			}
			else {
				// create message of symbols
				pmt::pmt_t blob = pmt::make_blob(sym, d_tx.n_sym * 48 * sizeof(gr_complex));
				pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
				pmt::pmt_t dict = pmt::make_dict();
				dict = pmt::dict_add(dict, pmt::mp("encoding"), enc);
				message_port_pub(pmt::mp("symbol out"), pmt::cons(dict, blob));


				// create message of side channel decoding
				pmt::pmt_t blob1 = pmt::make_blob(out_sym_side, d_tx.n_sym * 48 * sizeof(gr_complex));
				//pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
				pmt::pmt_t dict1 = pmt::make_dict();
				dict = pmt::dict_add(dict1, pmt::mp("encoding"), enc);
				message_port_pub(pmt::mp("side out"), pmt::cons(dict1, blob1));
			}
		}
		else {
			if(d_ofdm.encoding == 2 ||d_ofdm.encoding == 3)	{
				// create message of symbols
				pmt::pmt_t blob = pmt::make_blob(sym, d_tx.n_sym * 48 * sizeof(gr_complex));
				pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
				pmt::pmt_t dict = pmt::make_dict();
				dict = pmt::dict_add(dict, pmt::mp("encoding"), enc);
				message_port_pub(pmt::mp("symbol out"), pmt::cons(dict, blob));
			}
			else if(d_ofdm.encoding == 4 || d_ofdm.encoding == 5)	{
				// create message of side channel decoding
				pmt::pmt_t blob1 = pmt::make_blob(sym, d_tx.n_sym * 48 * sizeof(gr_complex));
				pmt::pmt_t enc = pmt::from_uint64(d_ofdm.encoding);
				pmt::pmt_t dict1 = pmt::make_dict();
				dict = pmt::dict_add(dict1, pmt::mp("encoding"), enc);
				message_port_pub(pmt::mp("side out"), pmt::cons(dict1, blob1));
			}
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
		for(int i = 0, nbyte; i < nbytes; i++) {
			if((out_bytes[i] > 31) && (out_bytes[i] < 127)) {
				dout << ((char) out_bytes[i]);
			} else {
				dout << ".";
			}
		}
		dout << std::endl;
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
			if(side_coding[i])
				noise_pow += compute_complex_pow(symbols[i]-ref_cons[sym_ref[i]]);
			else
				noise_pow += compute_complex_pow(symbols[i]);
		}
		return -10*std::log10(noise_pow/sym_tot);
	}
	
	
	private:
		gr_complex sym[1000 * 48 * 100];
		gr_complex out_sym_side[1000 * 48 * 100];	// Added by Haoyang: store the side channel decoding
		bool contain_side_chan = false;
		vec bits;
		double deinter[1000 * 48];
		char out_bits[40000];
		char out_bytes[40000];
		bvec decoded_bits;
		int* side_coding;
		bool d_debug;
		bool d_log;
		int d_n_zeros;
		tx_param d_tx;
		ofdm_param d_ofdm;
		int copied;
		bool d_frame_complete;

		Modulator<std::complex<double> > bpsk;
		Modulator<std::complex<double> > qpsk;
		Modulator<std::complex<double> > qam16;
		Modulator<std::complex<double> > qam64;
};

ofdm_decode_mac::sptr
ofdm_decode_mac::make(bool log, bool debug, int n_zeros) {
	return gnuradio::get_initial_sptr(new ofdm_decode_mac_impl(log, debug, n_zeros));
}

