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


#ifndef INCLUDED_IEEE802_11_SIDE_CHANNEL_VARRATE_H
#define INCLUDED_IEEE802_11_SIDE_CHANNEL_VARRATE_H

#include <ieee802-11/api.h>
#include <gnuradio/tagged_stream_block.h>

#define PI 3.141592654
#define MAX_SECTOR 8	// maximum sectors (PSK) being supported

namespace gr {
  namespace ieee802_11 {

    class IEEE802_11_API side_channel_varrate : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<side_channel_varrate> sptr;
      static sptr make(std::vector<int> enc_vector, double alpha, std::vector<int> sector, bool debug);
	  virtual void set_alpha(double alp) = 0;
	  virtual void set_sector(std::vector<int> sector) = 0;
    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802-11_SIDE_CHANNEL_APPEND_CONSTELLATION_H */

