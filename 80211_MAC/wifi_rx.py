#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Wifi Rx
# Generated: Sun Feb 28 20:29:51 2016
##################################################
"""
if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"
"""
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import fft
from gnuradio import gr
from gnuradio import uhd
from gnuradio import wxgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.wxgui import forms
from gnuradio.wxgui import numbersink2
from gnuradio.wxgui import scopesink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import ieee802_11
import time
import wx


class wifi_rx(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Wifi Rx")
        #_icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        #self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.window_size = window_size = 48
        self.sync_length = sync_length = 320
        self.samp_rate = samp_rate = 20e6
        self.lo_offset = lo_offset = 0e6
        self.gain = gain = 20
        self.freq = freq = 5.89e9
        self.chan_est = chan_est = 0

        ##################################################
        # Blocks
        ##################################################
        self._samp_rate_chooser = forms.radio_buttons(
        	parent=self.GetWin(),
        	value=self.samp_rate,
        	callback=self.set_samp_rate,
        	label="Sample Rate",
        	choices=[5e6, 10e6, 20e6],
        	labels=["5 MHz", "10 MHz", "20 MHz"],
        	style=wx.RA_HORIZONTAL,
        )
        self.Add(self._samp_rate_chooser)
        self.nb = self.nb = wx.Notebook(self.GetWin(), style=wx.NB_TOP)
        self.nb.AddPage(grc_wxgui.Panel(self.nb), "constellation")
        self.nb.AddPage(grc_wxgui.Panel(self.nb), "autocorrelation")
        self.Add(self.nb)
        self._lo_offset_chooser = forms.drop_down(
        	parent=self.GetWin(),
        	value=self.lo_offset,
        	callback=self.set_lo_offset,
        	label="LO Offset",
        	choices=[0, 6e6, 11e6],
        	labels=['0 MHz', '6 MHz', '11 MHz'],
        )
        self.Add(self._lo_offset_chooser)
        _gain_sizer = wx.BoxSizer(wx.VERTICAL)
        self._gain_text_box = forms.text_box(
        	parent=self.GetWin(),
        	sizer=_gain_sizer,
        	value=self.gain,
        	callback=self.set_gain,
        	label='gain',
        	converter=forms.int_converter(),
        	proportion=0,
        )
        self._gain_slider = forms.slider(
        	parent=self.GetWin(),
        	sizer=_gain_sizer,
        	value=self.gain,
        	callback=self.set_gain,
        	minimum=0,
        	maximum=100,
        	num_steps=100,
        	style=wx.SL_HORIZONTAL,
        	cast=int,
        	proportion=1,
        )
        self.Add(_gain_sizer)
        self._freq_chooser = forms.drop_down(
        	parent=self.GetWin(),
        	value=self.freq,
        	callback=self.set_freq,
        	label="Channel",
        	choices=[2412000000.0, 2417000000.0, 2422000000.0, 2427000000.0, 2432000000.0, 2437000000.0, 2442000000.0, 2447000000.0, 2452000000.0, 2457000000.0, 2462000000.0, 2467000000.0, 2472000000.0, 2484000000.0, 5170000000.0, 5180000000.0, 5190000000.0, 5200000000.0, 5210000000.0, 5220000000.0, 5230000000.0, 5240000000.0, 5260000000.0, 5280000000.0, 5300000000.0, 5320000000.0, 5500000000.0, 5520000000.0, 5540000000.0, 5560000000.0, 5580000000.0, 5600000000.0, 5620000000.0, 5640000000.0, 5660000000.0, 5680000000.0, 5700000000.0, 5745000000.0, 5765000000.0, 5785000000.0, 5805000000.0, 5825000000.0, 5860000000.0, 5870000000.0, 5880000000.0, 5890000000.0, 5900000000.0, 5910000000.0, 5920000000.0],
        	labels=['  1 | 2412.0 | 11g', '  2 | 2417.0 | 11g', '  3 | 2422.0 | 11g', '  4 | 2427.0 | 11g', '  5 | 2432.0 | 11g', '  6 | 2437.0 | 11g', '  7 | 2442.0 | 11g', '  8 | 2447.0 | 11g', '  9 | 2452.0 | 11g', ' 10 | 2457.0 | 11g', ' 11 | 2462.0 | 11g', ' 12 | 2467.0 | 11g', ' 13 | 2472.0 | 11g', ' 14 | 2484.0 | 11g', ' 34 | 5170.0 | 11a', ' 36 | 5180.0 | 11a', ' 38 | 5190.0 | 11a', ' 40 | 5200.0 | 11a', ' 42 | 5210.0 | 11a', ' 44 | 5220.0 | 11a', ' 46 | 5230.0 | 11a', ' 48 | 5240.0 | 11a', ' 52 | 5260.0 | 11a', ' 56 | 5280.0 | 11a', ' 58 | 5300.0 | 11a', ' 60 | 5320.0 | 11a', '100 | 5500.0 | 11a', '104 | 5520.0 | 11a', '108 | 5540.0 | 11a', '112 | 5560.0 | 11a', '116 | 5580.0 | 11a', '120 | 5600.0 | 11a', '124 | 5620.0 | 11a', '128 | 5640.0 | 11a', '132 | 5660.0 | 11a', '136 | 5680.0 | 11a', '140 | 5700.0 | 11a', '149 | 5745.0 | 11a', '153 | 5765.0 | 11a', '157 | 5785.0 | 11a', '161 | 5805.0 | 11a', '165 | 5825.0 | 11a', '172 | 5860.0 | 11p', '174 | 5870.0 | 11p', '176 | 5880.0 | 11p', '178 | 5890.0 | 11p', '180 | 5900.0 | 11p', '182 | 5910.0 | 11p', '184 | 5920.0 | 11p'],
        )
        self.Add(self._freq_chooser)
        self._chan_est_chooser = forms.radio_buttons(
        	parent=self.GetWin(),
        	value=self.chan_est,
        	callback=self.set_chan_est,
        	label='chan_est',
        	choices=[0, 1],
        	labels=["LMS", "Linear Comb"],
        	style=wx.RA_HORIZONTAL,
        )
        self.Add(self._chan_est_chooser)
        self.wxgui_scopesink2_0 = scopesink2.scope_sink_f(
        	self.nb.GetPage(1).GetWin(),
        	title="Scope Plot",
        	sample_rate=samp_rate,
        	v_scale=0,
        	v_offset=0,
        	t_scale=0,
        	ac_couple=False,
        	xy_mode=False,
        	num_inputs=1,
        	trig_mode=wxgui.TRIG_MODE_AUTO,
        	y_axis_label="Counts",
        )
        self.nb.GetPage(1).Add(self.wxgui_scopesink2_0.win)
        self.wxgui_scopesink1 = scopesink2.scope_sink_c(
        	self.nb.GetPage(0).GetWin(),
        	title="Scope Plot",
        	sample_rate=12500,
        	v_scale=0,
        	v_offset=0,
        	t_scale=0,
        	ac_couple=False,
        	xy_mode=True,
        	num_inputs=1,
        	trig_mode=wxgui.TRIG_MODE_AUTO,
        	y_axis_label="Counts",
        )
        self.nb.GetPage(0).Add(self.wxgui_scopesink1.win)
        self.wxgui_numbersink2_0 = numbersink2.number_sink_f(
        	self.GetWin(),
        	unit="%",
        	minval=0,
        	maxval=100,
        	factor=1.0,
        	decimal_places=2,
        	ref_level=0,
        	sample_rate=1,
        	number_rate=15,
        	average=True,
        	avg_alpha=0.02,
        	label="Frame Error Rate",
        	peak_hold=False,
        	show_gauge=True,
        )
        self.Add(self.wxgui_numbersink2_0.win)
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(samp_rate)
        self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(freq, rf_freq = freq - lo_offset, rf_freq_policy=uhd.tune_request.POLICY_MANUAL), 0)
        self.uhd_usrp_source_0.set_gain(gain, 0)
        self.ieee802_11_ofdm_sync_short_0 = ieee802_11.ofdm_sync_short(0.56, 2, False, False)
        self.ieee802_11_ofdm_sync_long_0 = ieee802_11.ofdm_sync_long(sync_length, True, False)
        self.ieee802_11_ofdm_parse_mac_0 = ieee802_11.ofdm_parse_mac(False, True)
        self.ieee802_11_ofdm_equalize_symbols_0 = ieee802_11.ofdm_equalize_symbols(chan_est, False)
        self.ieee802_11_ofdm_decode_signal_0 = ieee802_11.ofdm_decode_signal(True, False)
        self.ieee802_11_ofdm_decode_mac_0 = ieee802_11.ofdm_decode_mac(True, False)
        self.ieee802_11_moving_average_xx_1 = ieee802_11.moving_average_ff(window_size + 16)
        self.ieee802_11_moving_average_xx_0 = ieee802_11.moving_average_cc(window_size)
        self.fft_vxx_0 = fft.fft_vcc(64, True, (window.rectangular(64)), True, 1)
        self.blocks_vector_to_stream_0 = blocks.vector_to_stream(gr.sizeof_gr_complex*1, 48)
        self.blocks_stream_to_vector_0 = blocks.stream_to_vector(gr.sizeof_gr_complex*1, 64)
        self.blocks_socket_pdu_0 = blocks.socket_pdu("UDP_SERVER", "", "52001", 10000, False)
        self.blocks_pdu_to_tagged_stream_0 = blocks.pdu_to_tagged_stream(blocks.float_t, "packet_len")
        self.blocks_multiply_xx_0 = blocks.multiply_vcc(1)
        self.blocks_divide_xx_0 = blocks.divide_ff(1)
        self.blocks_delay_0_0 = blocks.delay(gr.sizeof_gr_complex*1, 16)
        self.blocks_delay_0 = blocks.delay(gr.sizeof_gr_complex*1, sync_length)
        self.blocks_conjugate_cc_0 = blocks.conjugate_cc()
        self.blocks_complex_to_mag_squared_0 = blocks.complex_to_mag_squared(1)
        self.blocks_complex_to_mag_0 = blocks.complex_to_mag(1)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.ieee802_11_ofdm_decode_mac_0, 'out'), (self.ieee802_11_ofdm_parse_mac_0, 'in'))    
        self.msg_connect((self.ieee802_11_ofdm_parse_mac_0, 'fer'), (self.blocks_pdu_to_tagged_stream_0, 'pdus'))    
        self.connect((self.blocks_complex_to_mag_0, 0), (self.blocks_divide_xx_0, 0))    
        self.connect((self.blocks_complex_to_mag_squared_0, 0), (self.ieee802_11_moving_average_xx_1, 0))    
        self.connect((self.blocks_conjugate_cc_0, 0), (self.blocks_multiply_xx_0, 1))    
        self.connect((self.blocks_delay_0, 0), (self.ieee802_11_ofdm_sync_long_0, 1))    
        self.connect((self.blocks_delay_0_0, 0), (self.blocks_conjugate_cc_0, 0))    
        self.connect((self.blocks_delay_0_0, 0), (self.ieee802_11_ofdm_sync_short_0, 0))    
        self.connect((self.blocks_divide_xx_0, 0), (self.ieee802_11_ofdm_sync_short_0, 2))    
        self.connect((self.blocks_divide_xx_0, 0), (self.wxgui_scopesink2_0, 0))    
        self.connect((self.blocks_multiply_xx_0, 0), (self.ieee802_11_moving_average_xx_0, 0))    
        self.connect((self.blocks_pdu_to_tagged_stream_0, 0), (self.wxgui_numbersink2_0, 0))    
        self.connect((self.blocks_stream_to_vector_0, 0), (self.fft_vxx_0, 0))    
        self.connect((self.blocks_vector_to_stream_0, 0), (self.wxgui_scopesink1, 0))    
        self.connect((self.fft_vxx_0, 0), (self.ieee802_11_ofdm_equalize_symbols_0, 0))    
        self.connect((self.ieee802_11_moving_average_xx_0, 0), (self.blocks_complex_to_mag_0, 0))    
        self.connect((self.ieee802_11_moving_average_xx_0, 0), (self.ieee802_11_ofdm_sync_short_0, 1))    
        self.connect((self.ieee802_11_moving_average_xx_1, 0), (self.blocks_divide_xx_0, 1))    
        self.connect((self.ieee802_11_ofdm_decode_signal_0, 0), (self.blocks_vector_to_stream_0, 0))    
        self.connect((self.ieee802_11_ofdm_decode_signal_0, 0), (self.ieee802_11_ofdm_decode_mac_0, 0))    
        self.connect((self.ieee802_11_ofdm_equalize_symbols_0, 0), (self.ieee802_11_ofdm_decode_signal_0, 0))    
        self.connect((self.ieee802_11_ofdm_sync_long_0, 0), (self.blocks_stream_to_vector_0, 0))    
        self.connect((self.ieee802_11_ofdm_sync_short_0, 0), (self.blocks_delay_0, 0))    
        self.connect((self.ieee802_11_ofdm_sync_short_0, 0), (self.ieee802_11_ofdm_sync_long_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.blocks_complex_to_mag_squared_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.blocks_delay_0_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.blocks_multiply_xx_0, 0))    

    def get_window_size(self):
        return self.window_size

    def set_window_size(self, window_size):
        self.window_size = window_size
        self.ieee802_11_moving_average_xx_0.set_length(self.window_size)
        self.ieee802_11_moving_average_xx_1.set_length(self.window_size + 16)

    def get_sync_length(self):
        return self.sync_length

    def set_sync_length(self, sync_length):
        self.sync_length = sync_length
        self.blocks_delay_0.set_dly(self.sync_length)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self._samp_rate_chooser.set_value(self.samp_rate)
        self.uhd_usrp_source_0.set_samp_rate(self.samp_rate)
        self.wxgui_scopesink2_0.set_sample_rate(self.samp_rate)

    def get_lo_offset(self):
        return self.lo_offset

    def set_lo_offset(self, lo_offset):
        self.lo_offset = lo_offset
        self._lo_offset_chooser.set_value(self.lo_offset)
        self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(self.freq, rf_freq = self.freq - self.lo_offset, rf_freq_policy=uhd.tune_request.POLICY_MANUAL), 0)

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self._gain_slider.set_value(self.gain)
        self._gain_text_box.set_value(self.gain)
        self.uhd_usrp_source_0.set_gain(self.gain, 0)
        	

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self._freq_chooser.set_value(self.freq)
        self.uhd_usrp_source_0.set_center_freq(uhd.tune_request(self.freq, rf_freq = self.freq - self.lo_offset, rf_freq_policy=uhd.tune_request.POLICY_MANUAL), 0)

    def get_chan_est(self):
        return self.chan_est

    def set_chan_est(self, chan_est):
        self.chan_est = chan_est
        self._chan_est_chooser.set_value(self.chan_est)
        self.ieee802_11_ofdm_equalize_symbols_0.set_algorithm(self.chan_est)


def main(top_block_cls=wifi_rx, options=None):

    tb = top_block_cls()
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
