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


#ifndef INCLUDED_IEEE802_11_SIDE_CHANNEL_MOD_CC_H
#define INCLUDED_IEEE802_11_SIDE_CHANNEL_MOD_CC_H

#include <ieee802-11/api.h>
#include <gnuradio/tagged_stream_block.h>

namespace gr {
  namespace ieee802_11 {

    /*!
     * \brief <+description of block+>
     * \ingroup ieee802-11
     *
     */
     
    class IEEE802_11_API side_channel_mod_cc : virtual public gr::tagged_stream_block
    {
     public:
      typedef boost::shared_ptr<side_channel_mod_cc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ieee802-11::side_channel_mod_cc.
       *
       * To avoid accidental use of raw pointers, ieee802-11::side_channel_mod_cc's
       * constructor is in a private implementation
       * class. ieee802-11::side_channel_mod_cc::make is the public interface for
       * creating new instances.
       */
      static sptr make(bool debug, bool enable, std::vector<float> mask);
    };

  } // namespace ieee802-11
} // namespace gr

#endif /* INCLUDED_IEEE802-11_SIDE_CHANNEL_MOD_CC_H */

