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

#ifndef INCLUDED_IEEE802_11_SIDE_CHANNEL_MOD_CC_IMPL_H
#define INCLUDED_IEEE802_11_SIDE_CHANNEL_MOD_CC_IMPL_H

#include <ieee802-11/side_channel_mod_cc.h>

namespace gr {
  namespace ieee802_11 {

    class side_channel_mod_cc_impl : public side_channel_mod_cc
    {
     private:
      // Nothing to declare in this block.		
      bool d_debug;
      bool d_enable;
      //TODO: SET THE SIZE OF MASK BY (MAXIMUM SYMBOL NUMBER * 48)
      float d_mask[100000];
      int d_mask_len = 100000;

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      side_channel_mod_cc_impl(bool debug, bool enable, std::vector<float> mask);
      ~side_channel_mod_cc_impl();

      // Where all the action really happens
      int work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802-11_SIDE_CHANNEL_MOD_CC_IMPL_H */

