#!/usr/bin/env python


# This Python file contains the classes 'ofdm_mod' and 'ftw_transmit_path' necessary to create
# the 802.11a/g/p OFDM encoder flow-graph. The original PHY code was developed by FTW 
# (Forschungszentrum Telekommunikation Wien / Telecommunications Research Center Vienna, 
# http://www.ftw.at). The code was first presented and described in the following publication:

# The Uwicore Laboratory at the University Miguel Hernandez of Elche has added additional 
# functionalities to the FTW PHY code, in particular: Carrier Sensing functions, and 
# reconfigurability of the transmission GNU radio graph to allow for the possibility to 
# transmit different MAC frames (also of varying size). In addition, the Uwicore PHY 
# contribution communicates with the MAC layer, and is capable to process requests from 
# the MAC to sense or to transmit a packet to the wireless medium. 


# The FTW OFMD code triggers the encoding procedures and sends the complex baseband 
# signal to the USRP2 sink. The Uwicore carrier sensing function, based on the example 
# 'USRP_spectrum_sense' provided by GNU Radio, estimates the power of the signal using
# the signal's Power Spectrum Density (PSD).

import copy, math, sys, time
from gnuradio import gr, gru, uhd, eng_notation  #, ftw
#import gnuradio.ieee802_11 as gr_ieee802_11
import gnuradio.gr.gr_threading as _threading
#from gnuradio.blks2impl import psk, qam
import ftw_packet_utils as ofdm_packet_utils
from gnuradio.digital import digital_swig
import uwicore_mpif as plcp
from gnuradio import blocks
from gnuradio import digital
from gnuradio import fft
from gnuradio import gr
from gnuradio.fft import window
from gnuradio.filter import firdes
#from gnuradio import gr
import ieee802_11
import pmt
import foo
from gnuradio.eng_option import eng_option
from optparse import OptionParser
from mapRegime import v2r

# sets up the transmit path
class ftw_transmit_path(gr.hier_block2): 
	def __init__(self, options):
		gr.hier_block2.__init__(self, "transmit_path",
				gr.io_signature(0, 0, 0), # Input signature
				gr.io_signature(1, 1, gr.sizeof_gr_complex)) # Output signature

		##################################################
		# Parameters
		##################################################
		self.encoding = int(options.regime) - 1	# data rate index !!
		encoding = int(options.regime) - 1  
		self.sub_encoding = 2 ######new added
		sub_encoding = 2
		#encoding = 0

		##################################################
		# Variables
		##################################################
		self.mult = mult = .4
		self.out_buf_size = out_buf_size = 96000
		self.header_formatter = header_formatter = ieee802_11.wifi_signal_field()

		##################################################
		# Blocks
		##################################################
		self.ieee802_11_ofdm_mapper_0 = ieee802_11.ofdm_mapper(encoding, sub_encoding,True)
		self.digital_packet_headergenerator_bb_0 = digital.packet_headergenerator_bb(header_formatter.formatter(), "packet_len")
		self.digital_chunks_to_symbols_xx_0 = digital.chunks_to_symbols_bc(([-1, 1]), 1)
		self.ieee802_11_chunks_to_symbols_xx_0 = ieee802_11.chunks_to_symbols(False) #####Add a new parameter
		(self.ieee802_11_chunks_to_symbols_xx_0).set_min_output_buffer(96000)
		self.blocks_tagged_stream_mux_0 = blocks.tagged_stream_mux(gr.sizeof_gr_complex*1, "packet_len", 1)
		(self.blocks_tagged_stream_mux_0).set_min_output_buffer(96000)
		self.digital_ofdm_carrier_allocator_cvc_0_0_0 = digital.ofdm_carrier_allocator_cvc(64, (range(-26, -21) + range(-20, -7) + range(-6, 0) + range(1, 7) + range(8, 21) + range(22, 27),), ((-21, -7, 7, 21), ), ((1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (1, 1, 1, -1), (1, 1, 1, -1), (1, 1, 1, -1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1), (-1, -1, -1, 1)), ((0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0), (0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (-1.4719601443879746-1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, (1.4719601443879746+1.4719601443879746j), 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0), (0, 0j, 0, 0j, 0, 0j, -1, 1j, -1, 1j, -1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, (-0-1j), 1, -1j, -1, 1j, 0, -1j, 1, (-0-1j), 1, -1j, 1, 1j, -1, -1j, 1, (-0-1j), -1, 1j, 1, 1j, 1, 1j, 1, 1j, -1, -1j, 1, 1j, 1, -1j, -1, 0j, 0, 0j, 0, 0j), (0, 0, 0, 0, 0, 0, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1, 1, -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 1, -1, -1, 1, 1, -1, 1, -1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1, 1, -1, 1, 1, 1, 1, 0, 0, 0, 0, 0)), "packet_len")
		(self.digital_ofdm_carrier_allocator_cvc_0_0_0).set_min_output_buffer(96000)
		self.fft_vxx_0_0 = fft.fft_vcc(64, False, (tuple([1/52**.5] * 64)), True, 1)
		(self.fft_vxx_0_0).set_min_output_buffer(96000)
		self.digital_ofdm_cyclic_prefixer_0_0 = digital.ofdm_cyclic_prefixer(64, 64+16, 2, "packet_len")
		(self.digital_ofdm_cyclic_prefixer_0_0).set_min_output_buffer(96000)
		self.blocks_multiply_const_vxx_0 = blocks.multiply_const_vcc((mult, ))
		(self.blocks_multiply_const_vxx_0).set_min_output_buffer(100000)
		self.foo_packet_pad2_0 = foo.packet_pad2(False, False, 0.01, 100, 1000)
		(self.foo_packet_pad2_0).set_min_output_buffer(96000)
		#self.blocks_socket_pdu_0 = blocks.socket_pdu("UDP_SERVER", "", "52001", 10000, False)
		#self.blocks_null_source_0 = blocks.null_source(gr.sizeof_gr_complex*1)
		#self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)

		##################################################
		# Connections
		##################################################
		self.connect((self.ieee802_11_ofdm_mapper_0, 0), (self.digital_packet_headergenerator_bb_0, 0))
		self.connect((self.digital_packet_headergenerator_bb_0, 0), (self.digital_chunks_to_symbols_xx_0, 0))
		self.connect((self.ieee802_11_ofdm_mapper_0, 0), (self.ieee802_11_chunks_to_symbols_xx_0, 0))
		self.connect((self.digital_chunks_to_symbols_xx_0, 0), (self.blocks_tagged_stream_mux_0, 0))
		self.connect((self.ieee802_11_chunks_to_symbols_xx_0, 0), (self.blocks_tagged_stream_mux_0, 1))
		self.connect((self.blocks_tagged_stream_mux_0, 0), (self.digital_ofdm_carrier_allocator_cvc_0_0_0, 0))
		self.connect((self.digital_ofdm_carrier_allocator_cvc_0_0_0, 0), (self.fft_vxx_0_0, 0))
		self.connect((self.fft_vxx_0_0, 0), (self.digital_ofdm_cyclic_prefixer_0_0, 0))
		self.connect((self.digital_ofdm_cyclic_prefixer_0_0, 0), (self.blocks_multiply_const_vxx_0, 0))
		self.connect((self.blocks_multiply_const_vxx_0, 0), (self.foo_packet_pad2_0, 0))
		self.connect((self.foo_packet_pad2_0, 0), (self))

	def get_header_formatter(self):
		return self.header_formatter

	def set_header_formatter(self, header_formatter):
		self.header_formatter = header_formatter

	def send_pkt(self, mac_msg, main_regime, sub_regime, eof=False):
		port = pmt.intern("in")				# OFDM_mapper input port name
		if eof:
			msg = gr.message(1)             # tell self._pkt_input we're not sending any more packets
			self.ieee802_11_ofdm_mapper_0.to_basic_block()._post(port, pmt.PMT_EOF)

		else:
			info	= mac_msg["INFO"]       # this dictionary vector contains tx information that will be used during the encoding process                     
			N_cbps	= info["N_cbps"]     	# Upload the number of Coded bits per OFDM Symbol
			N_bpsc	= info["N_bpsc"]     	# Upload the number of Coded bits per sub-carrier
			N_rate	= info["rate"]   		# Upload the data rate code
			N_sym	= info["N_sym"]      	# Upload the OFDM Symbols' number
			print info
			print N_cbps
			print N_bpsc
			print N_rate
			print N_sym

			# MODIFIED FROM ORIGINAL
			# It is necessary to update the payload and length field on every packet transmitted
			#(pkt,Length) = mac_msg["MPDU"], mac_msg["LENGTH"]
			
			pkt = info["packet"]	# pkt is a string
			if type(pkt) is list:
				print 'a list'
			elif type(pkt) is tuple:
				print 'a tuple'
			elif type(pkt) == str:
				print 'a string'
			else:
				print 'neither a tuple or a list'
			print ','.join(x for x in pkt)
			print ','.join(x.encode('hex') for x in pkt)
			# dict1 - indicate if crc is included
			dict1 = pmt.make_dict();
			dict1 = pmt.dict_add(dict1, pmt.string_to_symbol("crc_included"), pmt.PMT_F)

			# dict mac - carry the u8vector and its length
			dict_mac = pmt.make_dict();
			dict_mac = pmt.dict_add(dict_mac, pmt.string_to_symbol("psdu_len"), pmt.to_pmt(len(pkt)))

			p = pmt.make_u8vector(len(pkt), 0)
			pkt1 = list(pkt)
			for i in range(len(pkt1)):
				pmt.u8vector_set(p,i,ord(pkt1[i]))

			mac = pmt.cons(dict_mac, p)
			# blob
			#mac = pmt.make_blob(psdu, int(info["packet_len"])-4);
			#mac = p

	
			##self.ieee802_11_ofdm_mapper_0.set_encoding(main_regime)
			self.ieee802_11_ofdm_mapper_0.set_encoding(3)
			##self.ieee802_11_ofdm_mapper_0.set_sub_encoding(sub_regime) #####new added
			self.ieee802_11_ofdm_mapper_0.set_sub_encoding(0)

			self.ieee802_11_ofdm_mapper_0.to_basic_block()._post(port, pmt.cons(dict1, mac))
