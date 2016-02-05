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
#include "chunks_to_symbols_vlc_impl.h"
#include "utils.h"
#include "constellation.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/tag_checker.h>
#include <assert.h>

using namespace gr::ieee802_11;

chunks_to_symbols_vlc_impl::chunks_to_symbols_vlc_impl(bool debug)
	: tagged_stream_block("wifi_chunks_to_symbols_vlc",
			   io_signature::make(1, 1, sizeof(char)),
			   io_signature::make(1, 1, sizeof(gr_complex)), "packet_len"),
			   d_debug(debug) {}

chunks_to_symbols_vlc_impl::~chunks_to_symbols_vlc_impl() { }

int
chunks_to_symbols_vlc_impl::work(int noutput_items,
		gr_vector_int &ninput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {

	dout << "====================== Entering chunks_to_symbols_vlc ======================" << std::endl;
	
	const unsigned char *in = (unsigned char*)input_items[0];
	gr_complex *out = (gr_complex*)output_items[0];

	// retrieve the tag "nSymbol" which implies the number of valid non-marker symbols
	std::vector<tag_t> tags;
	get_tags_in_range(tags, 0, nitems_read(0),
			nitems_read(0) + ninput_items[0],
			pmt::mp("nSymbol"));
	if(tags.size() != 1) {
		throw std::runtime_error("no nSymbol in input stream");
	}

	int symbols_len = (int)pmt::to_long(tags[0].value);
	dout << "symbols_len = " << symbols_len << std::endl;
	
	if(ninput_items[0] != symbols_len) {
		std::cerr << "Error: ninput_items[0] != symbols_len! in chunks_to_symbols_vlc()" << std::endl;
	}
	
	// retrieve the tag "nQAM" which implies the number of QAM used
	get_tags_in_range(tags, 0, nitems_read(0),
		nitems_read(0) + ninput_items[0],
		pmt::mp("nQAM"));
	
	int nQAM = (int)pmt::to_long(tags[0].value);
	dout << "nQAM = " << nQAM << std::endl;
	
	// Configure the modulator
	VlcMod *d_vlcModulator;
	switch(nQAM) {
		case 20: d_vlcModulator = &vlcMod20; break;
		case 24: d_vlcModulator = &vlcMod24; break;
		case 28: d_vlcModulator = &vlcMod28; break;
		default: std::cerr << "Invalid nQAM in chunks_to_symbols_vlc() - "
			<< nQAM << std::endl;
	}
	
	// Calculate the number of total symbols, including padding symbols and markers
	int nSymOut = d_vlcModulator->calcTotSymbol(symbols_len);
	
	gr_complex symbolsOut[nSymOut];
	int symOutInt[ninput_items[0]];
	
	for(int i = 0; i < ninput_items[0]; i++) {
		symOutInt[i] = static_cast<int>(in[i]);
	}
	
	dout << "Total symbols out: nSymOut = " << nSymOut << std::endl;
	d_vlcModulator->modulate(symOutInt, out, ninput_items[0], nSymOut);
	
	dout << "====================== Symbols out (gr_complex) ======================" << std::endl;
	for(int i=0;i<nSymOut;i++) {
		std::string markerStr = "";
		if(i%d_vlcModulator->nMarker == d_vlcModulator->nMarker - 1) {
			markerStr = " - Marker ("+ std::to_string(i/d_vlcModulator->nMarker) + ")";
		}
		dout << std::setw(3) << i << "," << out[i] << markerStr << std::endl;
	}
	dout << std::endl;
	dout << "====================== Quit chunks_to_symbols_vlc ======================" << std::endl;

	return nSymOut;		// return the number of output produced
}

chunks_to_symbols_vlc::sptr chunks_to_symbols_vlc::make(bool debug)
{
	return gnuradio::get_initial_sptr(new chunks_to_symbols_vlc_impl(debug));
}
