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
#ifndef INCLUDED_IEEE802_11_OFDM_UTILS_H
#define INCLUDED_IEEE802_11_OFDM_UTILS_H

#include <ieee802-11/api.h>
#include <ieee802-11/ofdm_mapper.h>
#include <gnuradio/config.h>
#include <cinttypes>
#include <iostream>

const int MAX_SYM = 520; // enough for 1500 byte BPSK 1/2
const int MAX_BITS = 1550 * 8 * 2;

#define dout d_debug && std::cout

#if defined(HAVE_LOG4CPP) && defined(ENABLE_GR_LOG)
#define mylog(msg) do { if(d_log) { *d_logger << log4cpp::Priority::INFO << msg << log4cpp::eol; }} while(0);
#else
#define mylog(msg) do { d_log && std::cout << msg << std::endl; } while(0);
#endif

// same mappings in double (itpp sucks)
extern const std::complex<double> BPSK_D[2];
extern const std::complex<double> QPSK_D[4];
extern const std::complex<double> QAM16_D[16];
extern const std::complex<double> QAM64_D[64];
extern const gr_complex ref_sym_enc0[1296];
extern const gr_complex ref_sym_enc1[1296];
extern const gr_complex ref_sym_enc2[1296];
extern const gr_complex ref_sym_enc3[1296];
extern const gr_complex ref_sym_enc4[1296];
extern const gr_complex ref_sym_enc5[1296];
extern const gr_complex ref_sym_enc6[1296];
extern const gr_complex ref_sym_enc7[1296];

struct mac_header {
	//protocol version, type, subtype, to_ds, from_ds, ...
	uint16_t frame_control;	// Frame Control - 2 bytes
/*	Frame Control 	
	15 - 14	Protocol version. Currently 0
  	13 - 12	Type
  	11 - 8	Subtype
  	7 	  	To DS. 1 = to the distribution system.
  	6 	  	From DS. 1 = exit from the Distribution System.
  	5 	  	More Frag. 1 = more fragment frames to follow (last or unfragmented frame = 0)
  	4 	  	Retry. 1 = this is a re-transmission.
  	3 	  	Power Mgt. 1 = station in power save mode, 1 = active mode.
  	2 	  	More Data. 1 = additional frames buffered for the destination address (address x).
  	1 	  	WEP. 1 = data processed with WEP algorithm. 0 = no WEP.
  	0 	  	Order. 1 = frames must be strictly ordered.*/
	uint16_t duration;		// 2 bytes
	uint8_t addr1[6];		// Source address - 6 bytes
	uint8_t addr2[6];		// Destination address - 6 bytes
	uint8_t addr3[6];		// Receiving station address (destination wireless station) - 6 bytes
	uint16_t seq_nr;		// Sequence control - 2 bytes
}__attribute__((packed));

/**
 * OFDM parameters
 */
class ofdm_param {
public:
	ofdm_param(Encoding e);

	// data rate
	Encoding encoding;
	// rate field of the SIGNAL header
	char     rate_field;
	// number of coded bits per sub carrier
	int      n_bpsc;
	// number of coded bits per OFDM symbol
	int      n_cbps;
	// number of data bits per OFDM symbol
	int      n_dbps;

	void print();
};

/**
 * packet specific parameters
 */
class tx_param {
public:
	tx_param(ofdm_param &ofdm, int psdu_length);
	// PSDU size in bytes
	int psdu_size;
	// number of OFDM symbols (17-11)
	int n_sym;
	// number of data bits in the DATA field, including service and padding (17-12)
	int n_data;
	// number of padding bits in the DATA field (17-13)
	int n_pad;
	int n_encoded_bits;

	void print();
};

/**
 * Given a payload, generates a MAC data frame (i.e., a PSDU) to be given
 * to the physical layer for encoding.
 *
 * \param msdu the payload for the MAC frame
 * \param msdu_size the size of the msdu in bytes
 * \param psdu pointer to a byte array where to store the MAC frame. Memory
 * will be alloced by the function
 * \param psdu_size pointer to an integer where the size of the psdu in bytes
 * will be stored
 * \param seq sequence number of the frame
 */
void generate_mac_data_frame(const char *msdu, int msdu_size, char **psdu, int *psdu_size, char seq);


void scramble(const char *input, char *out, tx_param &tx, char initial_state);

void reset_tail_bits(char *scrambled_data, tx_param &tx);

void convolutional_encoding(const char *input, char *out, tx_param &tx);

void puncturing(const char *input, char *out, tx_param &tx, ofdm_param &ofdm);

void interleave(const char *input, char *out, tx_param &tx, ofdm_param &ofdm, bool reverse = false);
void interleave(const double *input, double *out, tx_param &tx, ofdm_param &ofdm, bool reverse = false);

void split_symbols(const char *input, char *out, tx_param &tx, ofdm_param &ofdm);

void generate_bits(const char *psdu, char *data_bits, tx_param &tx);

int ones(int n);	// ADDED by Haoyang 

#endif /* INCLUDED_IEEE802_11_OFDM_UTILS_H */
