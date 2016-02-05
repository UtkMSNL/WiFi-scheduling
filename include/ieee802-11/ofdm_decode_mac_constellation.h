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


#ifndef INCLUDED_IEEE802_11_OFDM_DECODE_MAC_CONSTELLATION_H
#define INCLUDED_IEEE802_11_OFDM_DECODE_MAC_CONSTELLATION_H

#include <ieee802-11/api.h>
#include <gnuradio/block.h>
#include <ieee802-11/ofdm_mapper.h>

namespace gr {
  namespace ieee802_11 {

    /*!
     * \brief <+description of block+>
     * \ingroup ieee802-11
     *
     */
    class IEEE802_11_API ofdm_decode_mac_constellation : virtual public block
    {
     public:
      typedef boost::shared_ptr<ofdm_decode_mac_constellation> sptr;
      static sptr make(double alpha, bool debug = false, Encoding e_side = QPSK_1_2);
      virtual void set_encoding_side(Encoding e_side) = 0;
    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802-11_OFDM_DECODE_MAC_CONSTELLATION_H */

