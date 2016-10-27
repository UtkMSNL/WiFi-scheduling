/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 * 15/09/08: Add redundancy to side channel coding
 */
#include <ieee802-11/side_channel_append_constellation.h>
#include "utils.h"
#include <gnuradio/io_signature.h>
//#include <gnuradio/tag_checker.h>

using namespace gr::ieee802_11;
class side_channel_append_constellation_impl : public side_channel_append_constellation {
	public:
		/*
		 * The private constructor
		 */
		side_channel_append_constellation_impl(std::vector<int> enc_vector, double alpha, Encoding e_side, bool debug)
		  : gr::tagged_stream_block("side_channel_append_constellation",
		          gr::io_signature::make(1, 1, sizeof(gr_complex)),
		          gr::io_signature::make(1, 1, sizeof(gr_complex)), "packet_len"),
		          d_debug(debug),
		          d_alpha(alpha),
				  d_e_side(e_side)
		{
		    int enc_subcarrier = enc_vector.size()/2;
			if(sideChan_len >= enc_subcarrier) {
				sideChan_len = enc_subcarrier;
			}
			else {
				std::cout<<"Vector length = "<<enc_vector.size()<<"(>"<<sideChan_len*2<<"): Only first "<<sideChan_len*2<<"elements are used"<<std::endl;
			}
			for(int i=0;i<sideChan_len;i++) {
				if((enc_vector[2*i] == 1 || enc_vector[2*i] == 0) && (enc_vector[2*i+1] == 1 || enc_vector[2*i+1] == 0))
					d_sideChan[i] = 2*enc_vector[2*i] + enc_vector[2*i+1];
				else
					throw std::runtime_error("Wrong numbers in mask inputs");
			}
			set_alpha(alpha);
			
			if(e_side == QPSK_1_2 || e_side == PSK8_1_2) {
				set_encoding_side(e_side);
			}
			else {
				std::cout << "The encoding of side channel should be either QPSK_1_2 or PSK8_1_2\n";
				throw std::runtime_error("Wrong side channel encoding type");
			}
		}

		/*
		 * Our virtual destructor.
		 */
		~side_channel_append_constellation_impl()
		{
		}

		int
		calculate_output_stream_length(const gr_vector_int &ninput_items)
		{
		  int noutput_items = ninput_items[0];
		  return noutput_items ;
		}

		int
		work (int noutput_items,
		                   gr_vector_int &ninput_items,
		                   gr_vector_const_void_star &input_items,
		                   gr_vector_void_star &output_items)
		{
		    const gr_complex *in = (const gr_complex *) input_items[0];
		    gr_complex *out = (gr_complex *) output_items[0];

		    // Read tags (encoding, psdu_length)
			std::vector<gr::tag_t> tags;
			get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0],pmt::mp("encoding"));
			if(tags.size() != 1) {
				std::cout<<"no encoding in input stream"<<std::endl;
				throw std::runtime_error("no encoding in input stream");
			}
			Encoding encoding = (Encoding)pmt::to_long(tags[0].value);
		
			get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0],pmt::mp("psdu_len"));
			if(tags.size() != 1) {
				std::cout<<"no psdu_len in input stream"<<std::endl;
				throw std::runtime_error("no psdu_len in input stream");
			}
			int psdu_length = pmt::to_long(tags[0].value);
		
			gr::thread::scoped_lock lock(d_mutex);
		
			switch(encoding) {
				case BPSK_1_2: 	
				case BPSK_3_4: 	half_minDist = 1; break;
				case QPSK_1_2:
				case QPSK_3_4: 	half_minDist = 0.707106781; break;	// 1/sqrt(2)
				case QAM16_1_2:
				case QAM16_3_4: half_minDist = 0.316227766; break;	// 1/sqrt(10)
				case QAM64_2_3:
				case QAM64_3_4: half_minDist = 0.15430335; 	break;	// 1/sqrt(42)
				default:
					throw std::runtime_error("wrong encoding");
					break;
			}
			offset = half_minDist * d_alpha;	// horizontal and vertical offset
			
			if(d_e_side == QPSK_1_2) {
				for(int i=0;i<4;i++) {
					gr_offset[i] = gr_std_offset[i] * offset;
					dout << gr_offset[i] << ",";
				}
			}
			else {
				for(int i=0;i<8;i++) {	// PSK8
					gr_offset[i] = gr_std_offset_8PSK[i] * offset;
					dout << gr_offset[i] << ",";
				}
			}
			dout << std::endl;
			
			ofdm_param ofdm_side(d_e_side);
			
			int psdu_length_side = 28;
			if(d_e_side == QPSK_1_2)
				psdu_length_side += 131;
			else if(d_e_side == PSK8_1_2)
				psdu_length_side += 212;
				
			char *psdu_side = (char*)calloc(psdu_length_side, sizeof(char));
			
			// ############ INSERT MAC STUFF
			for(int i=0; i<psdu_length_side; i++) {
				*(psdu_side+i) = data_sc[i];
			}
		
			tx_param tx_side(ofdm_side, psdu_length_side);
			if(tx_side.n_sym > MAX_SYM) {
				std::cout << "packet too large, maximum number of symbols is " << MAX_SYM << std::endl;
				return 0;
			}

			//alloc memory for modulation steps
			char *data_bits_side        = (char*)calloc(tx_side.n_data, sizeof(char));
			char *scrambled_data_side   = (char*)calloc(tx_side.n_data, sizeof(char));
			char *encoded_data_side     = (char*)calloc(tx_side.n_data * 2, sizeof(char));
			char *punctured_data_side   = (char*)calloc(tx_side.n_encoded_bits, sizeof(char));
			char *interleaved_data_side = (char*)calloc(tx_side.n_encoded_bits, sizeof(char));
			char *symbols_side          = (char*)calloc((tx_side.n_encoded_bits / ofdm_side.n_bpsc), sizeof(char));

			//generate the OFDM data field, adding service field and pad bits
			generate_bits(psdu_side, data_bits_side, tx_side);
			//print_output(data_bits_side, tx_side.n_data);
			//print_hex_array(data_bits, tx.n_data);

			// first step, scrambling
			scramble(data_bits_side, scrambled_data_side, tx_side, 23);
			//print_hex_array(scrambled_data, tx.n_data);
			// reset tail bits
			reset_tail_bits(scrambled_data_side, tx_side);
			//print_hex_array(scrambled_data, tx.n_data);
			// encoding
			convolutional_encoding(scrambled_data_side, encoded_data_side, tx_side);
			//print_hex_array(encoded_data, tx.n_data * 2);
			// puncturing
			puncturing(encoded_data_side, punctured_data_side, tx_side, ofdm_side);
			//std::cout << "punctured" << std::endl;
			// interleaving
			interleave(punctured_data_side, interleaved_data_side, tx_side, ofdm_side);
			// memcpy(interleaved_data_side, punctured_data_side, tx_side.n_encoded_bits); // Without interleaving
			//std::cout << "interleaved" << std::endl;

	                    // one byte per symbol
			split_symbols(interleaved_data_side, symbols_side, tx_side, ofdm_side);

			int symbols_len = tx_side.n_sym * 48;
		
			for(int i=0; i<symbols_len; i++) {
				d_sideChan[i] = (int)symbols_side[i];
			}
		
			free(psdu_side);
			free(data_bits_side);
			free(scrambled_data_side);
			free(encoded_data_side);
			free(punctured_data_side);
			free(interleaved_data_side);
			free(symbols_side);

			ofdm_param d_ofdm(encoding);
			tx_param tx(d_ofdm, psdu_length);
			int n_symbol = tx.n_sym; 
			dout<<"N_symbol is "<<n_symbol<<", mask length is "<<(sizeof(d_sideChan)/sizeof(int))<<std::endl;
			dout<<"encoding "<<encoding<<", padding"<<tx.n_pad<<", encoded bits :"<<tx.n_data-tx.n_pad<<std::endl;

			//int mod_length = std::min(sideChan_len,ninput_items[0]);
			int mod_length = std::min(symbols_len, ninput_items[0]);
			dout<<"mod_length="<<mod_length<<std::endl;
	
			// Do <+signal processing+>
			for(int i=0;i<mod_length;i++) {
				int idx = d_sideChan[i];
				out[i] = gr_complex(std::real(in[i]) + std::real(gr_offset[idx]),std::imag(in[i]) + std::imag(gr_offset[idx]));
			}
			for(int i=mod_length;i<ninput_items[0];i++) {
				out[i] = in[i];
			}
		
		
			for(int i=0;i<ninput_items[0];i++) {
				if(i%48==0) {
					dout<<std::endl<<i<<":";
				}
				dout<<out[i]<<',';
			}
			dout<<std::endl;
				// Tell runtime system how many input items we consumed on
				// each input stream.
				//consume_each (noutput_items);

		    return noutput_items;
		}
		
		void set_alpha(double alp) {
			std::cout << "side_channel_append_constellation: alpha: " << alp << std::endl;
			gr::thread::scoped_lock lock(d_mutex);

			d_alpha = alp;
		}
		
		void set_encoding_side(Encoding e_side) {
			std::cout << "side_channel_append_constellation: e_side: " << e_side << std::endl;
			gr::thread::scoped_lock lock(d_mutex);

			d_e_side = e_side;
		}
	
	private:
      // Nothing to declare in this block.
      bool d_debug;
      double d_alpha;
      Encoding d_e_side = QPSK_1_2;
      int d_sideChan[10000];
      int sideChan_len = 10000;
      double half_minDist = 1;
      double offset = 0.0;
      const char* data_sc = "XzBmIi18F8Ep1bzO5XDyDcfgQ87q36q4yOcO5BPtNyVLOqhFGua7wbHcA6LD7LTHfINWFhzlVSY8DVnxCvmLQIW09soYGHFvON78gD5gcqVv8UQw06teqQhLMxHA1LPeGwHFUJD9XGVJ47SUIqih8W7sODUbgpkPfwbaEmhkTBLfXvSFDuR22Hm7jGId7h3fXO8IIhl1gSmahuBzCACJ0avFUAsDupEqp7RiFumTdpjuFPoQ";
      std::complex<double> gr_offset[8];
		
      std::complex<double> gr_std_offset[4] = {
		std::complex<double>(-1, -1),	// 00
		std::complex<double>(-1, +1),	// 01
		std::complex<double>(+1, -1),	// 10
		std::complex<double>(+1, +1)	// 11
		};
	  
	  std::complex<double> gr_std_offset_8PSK[8] = {	// wiki 8-psk
		std::complex<double>(-0.707106781, -0.707106781),	// 000
		std::complex<double>(-1, 0),						// 001
		std::complex<double>(0, +1),						// 010
		std::complex<double>(-0.707106781, 0.707106781),	// 011
		std::complex<double>(0, -1),						// 100
		std::complex<double>(0.707106781, -0.707106781),	// 101
		std::complex<double>(0.707106781, 0.707106781),		// 110
		std::complex<double>(1, 0),							// 111
		};
	  
	  gr::thread::mutex d_mutex;
};

side_channel_append_constellation::sptr
side_channel_append_constellation::make(std::vector<int> enc_vector, double alpha, Encoding e_side, bool debug)
{
  return gnuradio::get_initial_sptr
    (new side_channel_append_constellation_impl(enc_vector, alpha, e_side, debug));
}
