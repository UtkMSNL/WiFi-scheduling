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

#include <gnuradio/io_signature.h>
#include "extract_symbol_impl.h"
#include <iostream>

#define dout d_debug && std::cout
namespace gr {
  namespace ieee802_11 {

    extract_symbol::sptr
    extract_symbol::make(bool debug)
    {
      return gnuradio::get_initial_sptr
        (new extract_symbol_impl(debug));
    }

    /*
     * The private constructor
     */
    extract_symbol_impl::extract_symbol_impl(bool debug)
      : gr::block("extract_symbol",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
              d_symbols_offset(0),
              d_symbols(NULL),
    		  d_debug(debug)
    {
    	message_port_register_in(pmt::mp("in"));
    }

    /*
     * Our virtual destructor.
     */
    extract_symbol_impl::~extract_symbol_impl()
    {
    	free(d_symbols);
    }

    int
    extract_symbol_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        gr_complex *out = (gr_complex *) output_items[0];

        while(!d_symbols_offset) {
        	pmt::pmt_t msg(delete_head_blocking(pmt::mp("in")));
        	
        	if(pmt::is_eof_object(msg)) {
        		dout<<"EXTRACT SYMBOL: existing"<<std::endl;
        		return -1;
        	}
        	
        	if(pmt::is_pair(msg)) {
        		int symbol_length = pmt::blob_length(pmt::cdr(msg));
        		const gr_complex *symbols = static_cast<const gr_complex*>(pmt::blob_data(pmt::cdr(msg)));
        		if(symbol_length%COMPLEX_SIZE != 0) {
        			std::cout<<"Error in extract symbol module: The symbols length should be a multiple of 8"<<std::endl;
        			break;
        		}
        		d_symbols_len = symbol_length;
        		d_symbols = (gr_complex*)calloc(d_symbols_len, COMPLEX_SIZE);
        		std::memcpy(d_symbols, symbols, d_symbols_len);
        		dout<<"Extract_symbol: "<<d_symbols_len<<" symbols received"<<std::endl;
 
        		for(int i=0,len=symbol_length/COMPLEX_SIZE;i<len;i++) {
        			dout<<symbols[i].real()<<",";
        			if(i%48==47) {
        				dout<<std::endl;
        			}
        		}
        		break;
        	}
        	else
        		throw std::invalid_argument("invalid inputs for extract_symbol");
        }
        
		int i = std::min(noutput_items, int(d_symbols_len/COMPLEX_SIZE) - d_symbols_offset);
		std::memcpy(out, d_symbols + d_symbols_offset * COMPLEX_SIZE, i * COMPLEX_SIZE);			
		
		d_symbols_offset += i;
		
		if(d_symbols_offset == d_symbols_len/COMPLEX_SIZE) {
			d_symbols_offset = 0;
			free(d_symbols);
			d_symbols = 0;
			dout<<"One frame is successfully sent!"<<std::endl;
		}
		/*
		if(i!=0)
			dout<<"Produce "<<i<<std::endl;
		*/
        // Tell runtime system how many output items we produced.
        return i;
    }

  } /* namespace ieee802-11 */
} /* namespace gr */

