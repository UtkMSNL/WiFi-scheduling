/* -*- c++ -*- */
/* 
 * Copyright 2015 <Haoyang Lu>.
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
 * 15/10/01: Created
 * Input:
 *		1) std::vector<int> side_dat
 *		2) double alpha
 *		3) std::vector<int> sectors - such as [3,3,2]
 * 		4) bool debug 
 */
#include <ieee802-11/side_channel_varrate.h>
#include "utils.h"
#include <gnuradio/io_signature.h>

using namespace gr::ieee802_11;
class side_channel_varrate_impl : public side_channel_varrate {
	public:
		/*
		 * The private constructor
		 */
		side_channel_varrate_impl(std::vector<int> side_dat, double alpha, std::vector<int> sector, bool debug)
		  : gr::tagged_stream_block("side_channel_varrate",
		          gr::io_signature::make(1, 1, sizeof(gr_complex)),
		          gr::io_signature::make(1, 1, sizeof(gr_complex)), "packet_len"),
		          d_alpha(alpha),
				  d_sector(sector),
		          d_debug(debug)
		{
			set_alpha(alpha);
			set_sector(sector);
			init_offsetRef();
			dout << "init_offsetRef() completed!\n";
		}

		/*
		 * Our virtual destructor.
		 */
		~side_channel_varrate_impl()
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
			
			ofdm_param d_ofdm(encoding);
			tx_param tx(d_ofdm, psdu_length);
			
			gr::thread::scoped_lock lock(d_mutex);
			// offsetGain = half_minDist[encoding] * d_alpha;	// in magnitude
			offsetGain = 1;
			dout << "offsetGain = " << offsetGain << "\n";
			
/*			int psdu_length_side = 28;
			if(d_e_side == QPSK_1_2)
				psdu_length_side += 131;
			else if(d_e_side == PSK8_1_2)
				psdu_length_side += 212;
*/			

			int nGroup = tx.n_sym * 48 / nSubPerGroup;				// number of integral groups in the frame
			dout << "nGroup = "	<< nGroup << "\n";
			int psdu_length_side = calc_psduLen_side(tx.n_sym, nGroup, bitsPerGroup, cr_side);
			dout << "psdu_length_side = " << psdu_length_side << "\n";
			char *psdu_side = (char*)calloc(psdu_length_side, sizeof(char));
			
			// ############ INSERT MAC STUFF
			for(int i=0; i<psdu_length_side; i++) {
				*(psdu_side+i) = data_sc[i];
			}
		
			ofdm_param ofdm_side(BPSK_1_2);
			tx_param tx_side(ofdm_side, psdu_length_side);
			tx_side.n_sym = tx.n_sym;
			tx_side.n_data = nGroup * bitsPerGroup * cr_side;
			tx_side.n_encoded_bits = tx_side.n_data / cr_side;
			tx_side.n_pad = (tx_side.n_data - 16 - 6) % 8;
			
			dout << "tx_side.n_encoded_bits = " << tx_side.n_encoded_bits
				 << ", tx_side.n_data = " << tx_side.n_data
				 << ", tx_side.n_pad = " << tx_side.n_pad
				 << "\n";

			
			//alloc memory for modulation steps
			char *data_bits_side        = (char*)calloc(tx_side.n_data, sizeof(char));
			char *scrambled_data_side   = (char*)calloc(tx_side.n_data, sizeof(char));
			char *encoded_data_side     = (char*)calloc(tx_side.n_data * 2, sizeof(char));
			char *punctured_data_side   = (char*)calloc(tx_side.n_encoded_bits, sizeof(char));
			char *interleaved_data_side = (char*)calloc(tx_side.n_encoded_bits, sizeof(char));
			char *symbols_side          = (char*)calloc(nGroup, sizeof(char));
			
			if(nGroup * nSubPerGroup > tx.n_sym * 48) {
				std::cout << "Something goes wrong about the side channel length:\n"
				<< "n_sym*48 = " << tx.n_sym << "*48 = " << tx.n_sym *48 
				<< ", tx_side.n_encoded_bits =" << tx_side.n_encoded_bits 
				<< ", bitsPerGroup = " << bitsPerGroup 
				<< ", nSubPerGroup = " << nSubPerGroup <<"\n";
			}
			
			//generate the OFDM data field, adding service field and pad bits
			generate_bits(psdu_side, data_bits_side, tx_side);
			dout << "generate_bits completed\n";
			
			scramble(data_bits_side, scrambled_data_side, tx_side, 23);
			dout << "scramble completed\n";
			
			reset_tail_bits(scrambled_data_side, tx_side);
			dout << "reset_tail_bits completed\n";
			
			convolutional_encoding(scrambled_data_side, encoded_data_side, tx_side);
			dout << "convolutional_encoding completed\n";
			
			puncturing(encoded_data_side, punctured_data_side, tx_side, ofdm_side);
			dout << "puncturing completed\n"; 

			//interleave(punctured_data_side, interleaved_data_side, tx_side, ofdm_side);
			memcpy(interleaved_data_side, punctured_data_side, tx_side.n_encoded_bits); // Without interleaving
			dout << "memcpy (interleave) completed\n";
			
			split_symbols_side(interleaved_data_side, symbols_side, tx_side, bitsPerGroup);
			dout << "split_symbols_side completed\n";
			
			free(psdu_side);
			free(data_bits_side);
			free(scrambled_data_side);
			free(encoded_data_side);
			free(punctured_data_side);
			free(interleaved_data_side);
			
			dout << "free completed\n";
			/*
			memset(d_sideChan, 0, tx.n_sym * 48);	// set all offset to none
			for(int i=0,len = sizeof(d_sideChan)/sizeof(int); i < len; i++) {
				d_sideChan[i] = 0;
			}
			for(int i=0; i<nGroup; i++) {
				int enc = (int)symbols_side[i];
				dout << i << ", enc = " << enc <<"\n";
				
				int sec_idx = 0;
				while(sec_idx < nSubPerGroup-1) {
					if(enc >= d_sector[sec_idx]) {
						enc -= d_sector[sec_idx];
						sec_idx ++;
					}
					else
						break;
				}
				dout << i << ", enc = " << enc << ", enc_idx = " << sec_idx << "\n";
				d_sideChan[nSubPerGroup*i + sec_idx] = enc + 1;
			}
			*/
			
			dout << "Group no. - weight:\n";
			for(int i=0; i<nGroup; i++) {
				int enc = (int)symbols_side[i];
				
				int ded = 0;
				for(int j=nSubPerGroup-1; j>=0; j--) {
					enc = enc - ded;
					int p = enc / weight[j];
					d_sideChan[nSubPerGroup*(i+1)-1-j] = p;
					ded = p * weight[j];
					dout<< p << ",";
				}
				dout << "\n";
			}
			
			dout << "set subcarrier offset completed\n";
			free(symbols_side);

			int n_symbol = tx.n_sym; 
			dout <<"N_symbol is " << n_symbol
				 <<", encoding "  << encoding
				 <<", padding "    <<tx.n_pad
				 <<", encoded bits :"<<tx.n_data-tx.n_pad<<std::endl;

			int mod_length = nSubPerGroup * nGroup;
			
			dout << "mod_length = " << mod_length << "\n";
			if(mod_length > ninput_items[0]) {
				std::cout << "Error! mod_length=" << mod_length
						  << ", ninput_items[0]=" << ninput_items[0] <<std::endl;
			}
			// Do <+signal processing+>
			for(int i=0;i<mod_length;i++) {
				dout << d_sideChan[i] << ",";
				if(i%48 == 47)
					dout << "\n"; 
				int idx = d_sideChan[i];
				out[i] = offsetRef[d_sector[i%nSubPerGroup]-2][idx] * offsetGain;
			}
			
			for(int i=mod_length;i<ninput_items[0];i++) {
				out[i] = 0;
			}
			
		    return noutput_items;
		}
		
		void set_alpha(double alp) {
			std::cout << "side_channel_varrate: alpha: " << alp << std::endl;
			gr::thread::scoped_lock lock(d_mutex);

			d_alpha = alp;
		}
		
		void init_offsetRef(void) {
			for(int i=0; i<MAX_SECTOR-1; i++) {
				double angle_dif = 2*PI / (i+2);
				dout << i << "th row (sector number=" << i+2 << "): ";
				
				for(int j=0; j<i+2; j++) {
					offsetRef[i][j] = std::complex<double>(cos(j*angle_dif), sin(j*angle_dif));
					dout << offsetRef[i][j] << ",";
				}
				dout << "\n";
			}
		}
		
		int calc_psduLen_side(int n_sym, int ngroup, int bitspergroup, double cr) {			
			// number of characters (bytes) can be encoded in side channel, excluding the first 16-bit 0s and padding
			return (int) ((bitspergroup * ngroup * cr - 16 - 6) / 8);	
		}

		void split_symbols_side(const char *in, char *out, tx_param &tx, int bitspergroup) {

			int symbols = tx.n_encoded_bits / bitspergroup;

			for (int i = 0; i < symbols; i++) {
				out[i] = 0;
				for(int k = 0; k < bitspergroup; k++) {
					assert(*in == 1 || *in == 0);
					out[i] |= (*in << (bitspergroup - k - 1));
					in++;
				}
			}
		}
		
		void set_sector(std::vector<int> sector) {
			int vec_len = sector.size();
			gr::thread::scoped_lock lock(d_mutex);
			
			d_sector.resize(vec_len);
			for(int i=0;i<vec_len;i++) {
				d_sector[i] = sector[i];
			}
			
			nSubPerGroup = vec_len;
			weight.resize(nSubPerGroup);
			bitsPerGroup = 1;
			
			// Computes the bitsPerGroup, and print the sector vector
			std::cout << "Change sector to [";
			for(int i=nSubPerGroup-1;i>=0; i--) {
				std::cout << d_sector[nSubPerGroup-1-i] << ",";
				if(d_sector[i] > MAX_SECTOR) {
					std::cout << "The maximum number of sectors (PSK) being supported is " << MAX_SECTOR << "\n!";
				}
				bitsPerGroup *= d_sector[i];
				if(i == nSubPerGroup-1) 
					weight[nSubPerGroup-1-i] = 1;
				else
					weight[nSubPerGroup-1-i] = weight[nSubPerGroup-2-i] * d_sector[i+1];
			}
			
			std::cout << "]; Weight vector is : [";
			
			for(int i=nSubPerGroup-1;i>=0; i--) {
				std::cout << weight[i] << ",";
			}
			std::cout << "]\n";
			
			bitsPerGroup = floor(log2(bitsPerGroup));
			std::cout << "Side channel data rate is " << bitsPerGroup/nSubPerGroup * 48 * cr_side / 4 << "Mbps\n";
		}

	private:
      // Nothing to declare in this block.
      bool d_debug;
      double d_alpha;
      std::vector<int> d_sector;
      std::vector<int> weight;
      int nSubPerGroup;
      int bitsPerGroup;
      int d_sideChan[4800];
      double offsetGain = 1.0;
      double cr_side = 0.5;
      
	  gr::thread::mutex d_mutex;
      
      const double half_minDist[8] = {	1.000000000, 1.000000000,	// BPSK 
			  							0.707106781, 0.707106781,	// QPSK 	1/sqrt(2)
			  							0.316227766, 0.316227766,	// 16-QAM 	1/sqrt(10)
			  							0.15430335,  0.15430335};	// 64-QAM 	1/sqrt(42)
      const char* data_sc = "XzBmIi18F8Ep1bzO5XDyDcfgQ87q36q4yOcO5BPtNyVLOqhFGua7wbHcA6LD7LTHfINWFhzlVSY8DVnxCvmLQIW09soYGHFvON78gD5gcqVv8UQw06teqQhLMxHA1LPeGwHFUJD9XGVJ47SUIqih8W7sODUbgpkPfwbaEmhkTBLfXvSFDuR22Hm7jGId7h3fXO8IIhl1gSmahuBzCACJ0avFUAsDupEqp7RiFumTdpjuFPoQ";
	  std::complex<double> offsetRef[MAX_SECTOR-1][MAX_SECTOR];
};

side_channel_varrate::sptr
side_channel_varrate::make(std::vector<int> side_dat, double alpha, std::vector<int> sector, bool debug)
{
  return gnuradio::get_initial_sptr
    (new side_channel_varrate_impl(side_dat, alpha, sector, debug));
}
