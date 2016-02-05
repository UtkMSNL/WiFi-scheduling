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
#include <ieee802-11/ofdm_mapper_vlc.h>
#include <math.h>
#include "constellation.h"
#include "utils.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/tag_checker.h>
#include <assert.h>

using namespace gr::ieee802_11;

class ofdm_mapper_vlc_impl: public ofdm_mapper_vlc {
public:

	static const int DATA_CARRIERS = 48;

	ofdm_mapper_vlc_impl(int nQAM, bool debug) :
			block("ofdm_mapper_vlc",
					gr::io_signature::make(0, 0, 0),
					gr::io_signature::make(1, 1, sizeof(char))), 
					d_symbols_offset(0),
					d_symbols(NULL),
					d_debug(debug),
					d_nQAM(nQAM),
					d_vlcModulator(VlcMod(d_nQAM,3,2)),
					d_ofdm((Encoding) QPSK_1_2) // for convolutional encoding use only (1/2)
	{
		message_port_register_in(pmt::mp("in"));
		set_encoding(nQAM);
	}

	~ofdm_mapper_vlc_impl() {
		free(d_symbols);
	}

	int general_work(int noutput, gr_vector_int& ninput_items,
			gr_vector_const_void_star& input_items,
			gr_vector_void_star& output_items) {

		char *out = (char*) output_items[0];
		dout << "OFDM MAPPER called offset: " << d_symbols_offset
				<< "   length: " << d_symbols_len << std::endl;

		while (!d_symbols_offset) {
			pmt::pmt_t msg(delete_head_blocking(pmt::intern("in")));

			if (pmt::is_eof_object(msg)) {
				dout << "MAPPER: exiting" << std::endl;
				return -1;
			}

			if (pmt::is_pair(msg)) {
				dout << "OFDM MAPPER: received new message" << std::endl;
				gr::thread::scoped_lock lock(d_mutex);

				int psdu_length = pmt::blob_length(pmt::cdr(msg));
				const char *psdu = static_cast<const char*>(pmt::blob_data(
						pmt::cdr(msg)));
						
				// valid bit length including SERVICE (16 bit) and Tail （6 bit)
				int bitLength_raw = 16 + 8 * psdu_length + 6;
				std::cout << "psdu_length = " << psdu_length << std::endl;
				char *data_bits 	= (char*)calloc(bitLength_raw, sizeof(char));
				char *encoded_data 	= (char*)calloc(bitLength_raw*2, sizeof(char));
				int *symOut 		= (int*)calloc((int) (bitLength_raw*2/d_vlcModulator.lenSet[1]), sizeof(int));

				generate_bits_vlc(psdu, data_bits, psdu_length);
				char data_bits_char[bitLength_raw];
				std::cout << "data_bits (char) in ofdm_mapper_vlc:" << std::endl;
				for(int i=0;i<bitLength_raw;i++) {
					data_bits_char[i] = data_bits[i] + '0';
				}
				print_char(data_bits_char, bitLength_raw);
				
				convolutional_encoding_vlc(data_bits, encoded_data, bitLength_raw);
				// Print the encoded_data (char)
				for(int i=0;i<bitLength_raw*2;i++) {
					encoded_data[i] += '0';
				}
				std::cout << "encoded_data (char) in ofdm_mapper_vlc:" << std::endl;
				print_char(encoded_data, bitLength_raw*2);			
	
				int nPadBitsLastSym = 
					d_vlcModulator.mapper(encoded_data, bitLength_raw*2, symOut, &d_symbols_len);

				std::cout << "mapper in ofdm_mapper_vlc is done, output symbol length (excluding marker and padding symbols) is "
						<< d_symbols_len << std::endl;
				
				// compute total subcarriers needed, including padding symbols and markers		
				int nSubcarTot = d_vlcModulator.calcTotSymbol(d_symbols_len);
				// compute the number of markers
				int nMarker;
				if(d_symbols_len%(d_vlcModulator.nMarker-1) == 0)
					nMarker = d_symbols_len / (d_vlcModulator.nMarker-1);
				else
					nMarker = ceil(d_symbols_len / (1.0*(d_vlcModulator.nMarker-1) ));
				// compute padding symbols
				int nSubcarPadding = nSubcarTot - nMarker - d_symbols_len;
				int nSubcarPaddingLastSym = nSubcarPadding%d_vlcModulator.nMarker;
				// Print padding and symbol numbers
				std::cout << "Padding bits in last symbol = " << nPadBitsLastSym << std::endl
						 << "Encoded symbols =  " << d_symbols_len << std::endl
						 << "Marker symbols = " << nMarker << std::endl
 						 << "Total symbols = " << nSubcarTot << std::endl
						 << "Padding symbols = " << nSubcarPadding << std::endl
						 << "Padding symbols for last group = " << nSubcarPaddingLastSym << std::endl;
		
				for(int i=0;i<d_symbols_len;i++) {
					std::cout << std::setw(2) << std::setfill('0') << symOut[i] << ",";
				}
				std::cout << std::endl;
				d_symbols = (char*)calloc(d_symbols_len, sizeof(char));

				for(int i=0;i<d_symbols_len;i++) {
					d_symbols[i] = static_cast<char>(symOut[i]);
				}

				/*
				 // ############ INSERT MAC STUFF
				 tx_param tx(d_ofdm, psdu_length);
				 if(tx.n_sym > MAX_SYM) {
				 std::cout << "packet too large, maximum number of symbols is " << MAX_SYM << std::endl;
				 return 0;
				 }

				 //alloc memory for modulation steps
				 char *data_bits        = (char*)calloc(tx.n_data, sizeof(char));
				 char *scrambled_data   = (char*)calloc(tx.n_data, sizeof(char));
				 char *encoded_data     = (char*)calloc(tx.n_data * 2, sizeof(char));
				 char *punctured_data   = (char*)calloc(tx.n_encoded_bits, sizeof(char));
				 char *interleaved_data = (char*)calloc(tx.n_encoded_bits, sizeof(char));
				 char *symbols          = (char*)calloc((tx.n_encoded_bits / d_ofdm.n_bpsc), sizeof(char));

				 //generate the OFDM data field, adding service field and pad bits
				 generate_bits(psdu, data_bits, tx);
				 print_output(data_bits, tx.n_data);
				 //print_hex_array(data_bits, tx.n_data);

				 // first step, scrambling
				 scramble(data_bits, scrambled_data, tx, 23);
				 //print_hex_array(scrambled_data, tx.n_data);
				 // reset tail bits
				 reset_tail_bits(scrambled_data, tx);
				 //print_hex_array(scrambled_data, tx.n_data);
				 // encoding
				 convolutional_encoding(scrambled_data, encoded_data, tx);
				 //print_hex_array(encoded_data, tx.n_data * 2);
				 // puncturing
				 puncturing(encoded_data, punctured_data, tx, d_ofdm);
				 //std::cout << "punctured" << std::endl;
				 // interleaving
				 interleave(punctured_data, interleaved_data, tx, d_ofdm);
				 //std::cout << "interleaved" << std::endl;

				 // one byte per symbol
				 split_symbols(interleaved_data, symbols, tx, d_ofdm);

				 d_symbols_len = tx.n_sym * 48;

				 d_symbols = (char*)calloc(d_symbols_len, 1);
				 std::memcpy(d_symbols, symbols, d_symbols_len);

				 */
				 
				std::cout << "Before memory free()\n";
				free(data_bits);
				free(encoded_data);
				free(symOut);
				std::cout << "After memory free()\n";
				
				// add tags
				pmt::pmt_t key = pmt::string_to_symbol("packet_len");
				pmt::pmt_t value = pmt::from_long(d_symbols_len);
				pmt::pmt_t srcid = pmt::string_to_symbol(alias());
				add_item_tag(0, nitems_written(0), key, value, srcid);

				pmt::pmt_t psdu_bytes = pmt::from_long(psdu_length);
				add_item_tag(0, nitems_written(0), pmt::mp("psdu_len"),
						psdu_bytes, srcid);

				pmt::pmt_t encoding = pmt::from_long(d_ofdm.encoding);
				add_item_tag(0, nitems_written(0), pmt::mp("encoding"),
						encoding, srcid);
				
				pmt::pmt_t nSymbol = pmt::from_long(d_symbols_len);
				add_item_tag(0, nitems_written(0), pmt::mp("nSymbol"),
						nSymbol, srcid);
						
				pmt::pmt_t nQAM = pmt::from_long(d_nQAM);
				add_item_tag(0, nitems_written(0), pmt::mp("nQAM"),
						nQAM, srcid);
				std::cout << "After adding tags\n";
							
/*
				free(data_bits);
				free(scrambled_data);
				free(encoded_data);
				free(punctured_data);
				free(interleaved_data);
				free(symbols);
*/
				break;
			}
		}

		int i = std::min(noutput, d_symbols_len - d_symbols_offset);
		std::memcpy(out, d_symbols + d_symbols_offset, i);
		d_symbols_offset += i;
		
		std::cout << "d_symbols_offset = " << d_symbols_offset << std::endl;
		
		if (d_symbols_offset == d_symbols_len) {
			d_symbols_offset = 0;
			free(d_symbols);
			d_symbols = 0;
			std::cout << "Free d_symbols" << std::endl;
		}

		return i;
	}

	void set_encoding(int nQAM) {

		std::cout << "OFDM MAPPER VLC: nQAM: " << nQAM << std::endl;
		gr::thread::scoped_lock lock(d_mutex);

		//	d_ofdm = ofdm_param(encoding);
		d_nQAM = nQAM;
		d_vlcModulator = VlcMod(nQAM,3,2);
		d_vlcModulator.displayDecodingTree(std::cout);
	}

	void print_message(const char *msg, size_t len) {

		dout << std::hex << "OFDM MAPPER input symbols" << std::endl
				<< "===========================" << std::endl;

		for (int i = 0; i < len; i++) {
			dout << std::hex << (int) msg[i] << " ";
		}

		dout << std::dec << std::endl;
	}

	void print_output(const char *psdu, int len) {

		dout << std::endl;
		for (int i = 0; i < len; i++) {
			dout << "0x" << std::setfill('0') << std::setw(2) << std::hex
					<< ((unsigned int) psdu[i] & 0xFF) << std::dec << " ";
			if (i % 16 == 15) {
				dout << std::endl;
			}
		}
		dout << std::endl;
		for (int i = 0; i < len; i++) {
			if ((psdu[i] > 31) && (psdu[i] < 127)) {
				dout << ((char) psdu[i]);
			} else {
				dout << ".";
			}
		}
		dout << std::endl;
	}
	
	void print_char(const char *psdu, int len) {
		for (int i = 0; i < len; i++) {
			if ((psdu[i] > 31) && (psdu[i] < 127)) {
				std::cout << ((char) psdu[i]);
			} else {
				std::cout << ".";
			}
		}
		std::cout << std::endl;
	}
	
	

private:
	bool d_debug;
	char* d_symbols;
	int d_symbols_offset;
	int d_symbols_len;
	ofdm_param d_ofdm;
	gr::thread::mutex d_mutex;
	/* for vlc modulation use only */
	int d_nQAM;	// number of constellation points
	VlcMod d_vlcModulator;
};

ofdm_mapper_vlc::sptr ofdm_mapper_vlc::make(int nQAM, bool debug) {
	return gnuradio::get_initial_sptr(new ofdm_mapper_vlc_impl(nQAM, debug));
}
