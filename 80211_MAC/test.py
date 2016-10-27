from ieee802_11 import RateAdaptation as rate
from mapRegime import v2r, r2v
#The following is the setup for AARF
R = rate(1,29,4)

#The following is the setup for Minstrel
#R = rate(2,8,4,10)

main_regime = 8
sub_regime = 0
tx_regime = r2v(main_regime, sub_regime)

for i in range(100):
	print "######################################################"	
	print "No.%d:" %i
	print "main_regime: %d" %main_regime
	print "sub_regime: %d" %sub_regime
	print "tx_regime: %d" %tx_regime	
	#x = R.minstrel_update_stats(x, 0)
	#R.show_aarf_rate()	
	tx_regime = R.data_sel(tx_regime, False)
	main_regime, sub_regime = v2r(tx_regime)	
	print "######################################################"
