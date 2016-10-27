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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "side_channel_mod_cc_impl.h"
#include "utils.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/tag_checker.h>
#include <iostream>

namespace gr {
  namespace ieee802_11 {

    side_channel_mod_cc::sptr
    side_channel_mod_cc::make(bool debug, bool enable, std::vector<float> mask)
    {
      return gnuradio::get_initial_sptr
        (new side_channel_mod_cc_impl(debug, enable, mask));
    }

    /*
     * The private constructor
     */
    side_channel_mod_cc_impl::side_channel_mod_cc_impl(bool debug, bool enable, std::vector<float> mask)
      : gr::tagged_stream_block("side_channel_mod_cc",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)), "packet_len"),
              d_debug(debug),
              d_enable(enable)
    {
    	int mask_length = mask.size();
    	if(d_mask_len >= mask_length) {
    		d_mask_len = mask_length;
    	}
    	else {
    		std::cout<<"mask length = "<<mask_length<<"(>"<<d_mask_len<<"): Only first "<<d_mask_len<<"element is used"<<std::endl;
    	}
    	for(int i=0;i<d_mask_len;i++) {
    		d_mask[i] = mask[i];
    	}
    }

    /*
     * Our virtual destructor.
     */
    side_channel_mod_cc_impl::~side_channel_mod_cc_impl()
    {
    }

    int
    side_channel_mod_cc_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items = ninput_items[0];
      return noutput_items ;
    }

    int
    side_channel_mod_cc_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const gr_complex *in = (const gr_complex *) input_items[0];
        gr_complex *out = (gr_complex *) output_items[0];
        
        // Read tags (encoding,psdu_length)
		std::vector<tag_t> tags;
		get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0],pmt::mp("encoding"));
		if(tags.size() != 1) {
			throw std::runtime_error("no encoding in input stream");
		}
		Encoding encoding = (Encoding)pmt::to_long(tags[0].value);
		
		get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0],pmt::mp("psdu_len"));
		if(tags.size() != 1) {
			throw std::runtime_error("no psdu_len in input stream");
		}
		
		int psdu_length = pmt::to_long(tags[0].value);
		float code_rate = 0.0;
		switch(encoding) {
			case BPSK_1_2: code_rate = 1/2; break;
			case BPSK_3_4: code_rate = 3/4; break;
			case QPSK_1_2: code_rate = 1/2; break;
			case QPSK_3_4: code_rate = 3/4; break;
			case QAM16_1_2: code_rate = 1/2; break;
			case QAM16_3_4: code_rate = 3/4; break;
			case QAM64_2_3: code_rate = 2/3; break;
			case QAM64_3_4: code_rate = 3/4; break;
			default:
				throw std::runtime_error("wrong encoding");
				break;
		}
		ofdm_param d_ofdm(encoding);
		tx_param tx(d_ofdm, psdu_length);
		int n_symbol = tx.n_sym; 
		std::cout<<"N_symbol is "<<n_symbol<<", mask length is "<<(sizeof(d_mask)/sizeof(float))<<std::endl;
		std::cout<<"encoding "<<encoding<<", padding"<<tx.n_pad<<", encoded bits :"<<tx.n_data-tx.n_pad<<std::endl;
		if((sizeof(d_mask)/sizeof(float)) < n_symbol) {
			dout<<"mask size should be larger than packet length!"<<std::endl;
			// throw std::runtime_error("mask size should be larger than packet length!");
		}
		int mod_length = std::min(d_mask_len,ninput_items[0]);
        if(d_enable) {
        	for(int i=0;i<mod_length;i++) {
        		out[i] = in[i] * d_mask[i];
        	}
        	for(int i=mod_length;i<ninput_items[0];i++) {
        		out[i] = in[i];
        	}
        }
        else {
        	for(int i=0;i<ninput_items[0];i++) {
        		out[i] = in[i];
        	}
        }
        for(int i=0;i<ninput_items[0];i++) {
        	if(i%48==0) {
        		dout<<std::endl<<i<<":";
        	}
        	dout<<out[i]<<',';
        }
        dout<<std::endl;
        
        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace ieee802-11 */
} /* namespace gr */

