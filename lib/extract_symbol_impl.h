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

#ifndef INCLUDED_IEEE802_11_EXTRACT_SYMBOL_IMPL_H
#define INCLUDED_IEEE802_11_EXTRACT_SYMBOL_IMPL_H

#include <ieee802-11/extract_symbol.h>

namespace gr {
  namespace ieee802_11 {

    class extract_symbol_impl : public extract_symbol
    {
     private:
      // Nothing to declare in this block.
      bool			d_debug;
      gr_complex* 	d_symbols;
      int 			d_symbols_offset;
      int 			d_symbols_len;

     public:
      extract_symbol_impl(bool debug);
      ~extract_symbol_impl();
	  const int COMPLEX_SIZE = sizeof(gr_complex);
      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802-11_EXTRACT_SYMBOL_IMPL_H */

