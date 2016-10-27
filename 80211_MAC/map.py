import math

def r2v(regime, sub):
	"""
	Return the value of the (regime, sub) pair
	"""
	value = 4*regime-3+sub
	return value


def v2r(value):
	"""
	Return the (regime, sub) pair of a value
	"""
	if value%4 != 0: 
		regime = value/4+1
	else:
		regime = value/4
	sub = 3-(4*regime-value)
	return regime, sub
