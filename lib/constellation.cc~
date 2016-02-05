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
 *
 *
 * TODO: MERGE two classes "QAMDemodulate" and "VlcMod"
 */

#include "constellation.h"

#include <cassert>
#include <string>
#include <math.h>
#include <string.h>
#include <utils.h>
#include <algorithm>    // std::max

/*-------------- 10-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM10_D[10] = {
	std::complex<float>(-0.3780, +0.3780),  // 0
	std::complex<float>(-0.3780, +1.1339),  // 1
	std::complex<float>(+0.3780, +0.3780),  // 2
	std::complex<float>(+0.3780, +1.1339),  // 3
	std::complex<float>(-1.1339, +0.3780),  // 4
	std::complex<float>(-1.1339, -0.3780),  // 5
	std::complex<float>(-0.3780, -0.3780),  // 6
	std::complex<float>(+0.3780, -0.3780),  // 7
	std::complex<float>(+1.1339, +0.3780),  // 8
	std::complex<float>(+1.1339, -0.3780)   // 9
};

// codeword for different modulations
const std::string QAM10_code[10] = {
	"0100",    // 4-bit, 0
	"0101",    // 4-bit, 1
	"1000",    // 4-bit, 2
	"1001",    // 4-bit, 3
	"000",    // 3-bit, 4
	"001",    // 3-bit, 5
	"011",    // 3-bit, 6
	"101",    // 3-bit, 7
	"110",    // 3-bit, 8
	"111"     // 3-bit, 9
};

// codeword length for different modulations
const int QAM10_bitLen[10] = {
	4,    // 0
	4,    // 1
	4,    // 2
	4,    // 3
	3,    // 4
	3,    // 5
	3,    // 6
	3,    // 7
	3,    // 8
	3     // 9
};

// codeword length set for different modulations
const int QAM10_lenSet[2] = {
	4,    // 0
	3     // 1
};

// constellation used for marker
const std::complex<float> QAM10_2bitMarker_D[4] = {	// 2 bit
	std::complex<float>(-1.1339, -0.3780),  // 0
	std::complex<float>(+0.3780, -0.3780),  // 1
	std::complex<float>(+1.1339, +0.3780),  // 2
	std::complex<float>(-0.3780, +0.3780)   // 3
};

/*-------------- 12-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM12_D[12] = {
	std::complex<float>(-1.2247, +0.4082),  // 0
	std::complex<float>(-0.4082, +1.2247),  // 1
	std::complex<float>(-1.2247, -0.4082),  // 2
	std::complex<float>(-0.4082, -1.2247),  // 3
	std::complex<float>(+1.2247, +0.4082),  // 4
	std::complex<float>(+0.4082, +1.2247),  // 5
	std::complex<float>(+1.2247, -0.4082),  // 6
	std::complex<float>(+0.4082, -1.2247),  // 7
	std::complex<float>(-0.4082, +0.4082),  // 8
	std::complex<float>(-0.4082, -0.4082),  // 9
	std::complex<float>(+0.4082, +0.4082),  // 10
	std::complex<float>(+0.4082, -0.4082)   // 11
};

// codeword for different modulations
const std::string QAM12_code[12] = {
	"0000",    // 4-bit, 0
	"0001",    // 4-bit, 1
	"0010",    // 4-bit, 2
	"0011",    // 4-bit, 3
	"1100",    // 4-bit, 4
	"1101",    // 4-bit, 5
	"1110",    // 4-bit, 6
	"1111",    // 4-bit, 7
	"010",    // 3-bit, 8
	"011",    // 3-bit, 9
	"100",    // 3-bit, 10
	"101"     // 3-bit, 11
};

// codeword length for different modulations
const int QAM12_bitLen[12] = {
	4,    // 0
	4,    // 1
	4,    // 2
	4,    // 3
	4,    // 4
	4,    // 5
	4,    // 6
	4,    // 7
	3,    // 8
	3,    // 9
	3,    // 10
	3     // 11
};

// codeword length set for different modulations
const int QAM12_lenSet[2] = {
	4,    // 0
	3     // 1
};

// constellation used for marker
const std::complex<float> QAM12_2bitMarker_D[4] = {	// 2 bit
	std::complex<float>(-1.2247, -0.4082),  // 0
	std::complex<float>(+1.2247, +0.4082),  // 1
	std::complex<float>(+0.4082, -1.2247),  // 2
	std::complex<float>(-0.4082, +1.2247)   // 3
};

/*-------------- 14-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM14_D[14] = {
	std::complex<float>(-0.3536, +1.0607),  // 0
	std::complex<float>(-1.0607, +0.3536),  // 1
	std::complex<float>(-1.0607, -0.3536),  // 2
	std::complex<float>(-1.0607, -1.0607),  // 3
	std::complex<float>(-0.3536, -0.3536),  // 4
	std::complex<float>(-0.3536, -1.0607),  // 5
	std::complex<float>(+0.3536, -0.3536),  // 6
	std::complex<float>(+0.3536, -1.0607),  // 7
	std::complex<float>(+0.3536, +1.0607),  // 8
	std::complex<float>(+1.0607, +0.3536),  // 9
	std::complex<float>(+1.0607, -0.3536),  // 10
	std::complex<float>(+1.0607, -1.0607),  // 11
	std::complex<float>(-0.3536, +0.3536),  // 12
	std::complex<float>(+0.3536, +0.3536)   // 13
};

// codeword for different modulations
const std::string QAM14_code[14] = {
	"0000",    // 4-bit, 0
	"0001",    // 4-bit, 1
	"0010",    // 4-bit, 2
	"0011",    // 4-bit, 3
	"0110",    // 4-bit, 4
	"0111",    // 4-bit, 5
	"1010",    // 4-bit, 6
	"1011",    // 4-bit, 7
	"1100",    // 4-bit, 8
	"1101",    // 4-bit, 9
	"1110",    // 4-bit, 10
	"1111",    // 4-bit, 11
	"010",    // 3-bit, 12
	"100"     // 3-bit, 13
};

// codeword length for different modulations
const int QAM14_bitLen[14] = {
	4,    // 0
	4,    // 1
	4,    // 2
	4,    // 3
	4,    // 4
	4,    // 5
	4,    // 6
	4,    // 7
	4,    // 8
	4,    // 9
	4,    // 10
	4,    // 11
	3,    // 12
	3     // 13
};

// codeword length set for different modulations
const int QAM14_lenSet[2] = {
	4,    // 0
	3     // 1
};

// constellation used for marker
const std::complex<float> QAM14_2bitMarker_D[4] = {	// 2 bit
	std::complex<float>(-1.0607, -0.3536),  // 0
	std::complex<float>(+1.0607, +0.3536),  // 1
	std::complex<float>(+0.3536, -1.0607),  // 2
	std::complex<float>(-0.3536, +1.0607)   // 3
};

/*-------------- 20-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM20_D[20] = {
	std::complex<float>(-0.3015, +1.5076),  // 0
	std::complex<float>(-0.9045, +0.9045),  // 1
	std::complex<float>(-0.3015, -1.5076),  // 2
	std::complex<float>(-0.9045, -0.9045),  // 3
	std::complex<float>(+0.3015, +1.5076),  // 4
	std::complex<float>(+0.9045, +0.9045),  // 5
	std::complex<float>(+0.3015, -1.5076),  // 6
	std::complex<float>(+0.9045, -0.9045),  // 7
	std::complex<float>(-0.9045, +0.3015),  // 8
	std::complex<float>(-0.9045, -0.3015),  // 9
	std::complex<float>(-0.3015, +0.9045),  // 10
	std::complex<float>(-0.3015, +0.3015),  // 11
	std::complex<float>(-0.3015, -0.9045),  // 12
	std::complex<float>(-0.3015, -0.3015),  // 13
	std::complex<float>(+0.9045, +0.3015),  // 14
	std::complex<float>(+0.9045, -0.3015),  // 15
	std::complex<float>(+0.3015, +0.9045),  // 16
	std::complex<float>(+0.3015, +0.3015),  // 17
	std::complex<float>(+0.3015, -0.9045),  // 18
	std::complex<float>(+0.3015, -0.3015)   // 19
};

// codeword for different modulations
const std::string QAM20_code[20] = {
	"00000",    // 5-bit, 0
	"00001",    // 5-bit, 1
	"00100",    // 5-bit, 2
	"00101",    // 5-bit, 3
	"10000",    // 5-bit, 4
	"10001",    // 5-bit, 5
	"10100",    // 5-bit, 6
	"10101",    // 5-bit, 7
	"0001",    // 4-bit, 8
	"0011",    // 4-bit, 9
	"0100",    // 4-bit, 10
	"0101",    // 4-bit, 11
	"0110",    // 4-bit, 12
	"0111",    // 4-bit, 13
	"1001",    // 4-bit, 14
	"1011",    // 4-bit, 15
	"1100",    // 4-bit, 16
	"1101",    // 4-bit, 17
	"1110",    // 4-bit, 18
	"1111"     // 4-bit, 19
};

// codeword length for different modulations
const int QAM20_bitLen[20] = {
	5,    // 0
	5,    // 1
	5,    // 2
	5,    // 3
	5,    // 4
	5,    // 5
	5,    // 6
	5,    // 7
	4,    // 8
	4,    // 9
	4,    // 10
	4,    // 11
	4,    // 12
	4,    // 13
	4,    // 14
	4,    // 15
	4,    // 16
	4,    // 17
	4,    // 18
	4     // 19
};

// codeword length set for different modulations
const int QAM20_lenSet[2] = {
	5,    // 0
	4     // 1
};

// constellation used for marker
const std::complex<float> QAM20_3bitMarker_D[8] = {	// 3 bit
	std::complex<float>(+0.3015, -0.3015),  // 0
	std::complex<float>(-0.9045, -0.3015),  // 1
	std::complex<float>(+0.3015, +0.9045),  // 2
	std::complex<float>(-0.3015, +0.3015),  // 3
	std::complex<float>(+0.9045, +0.3015),  // 4
	std::complex<float>(-0.3015, -0.9045),  // 5
	std::complex<float>(-0.9045, +0.9045),  // 6
	std::complex<float>(+0.9045, -0.9045)   // 7
};

/*-------------- 24-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM24_D[24] = {
	std::complex<float>(-0.2774, +1.3868),  // 0
	std::complex<float>(-0.8321, +0.8321),  // 1
	std::complex<float>(-1.3868, +0.2774),  // 2
	std::complex<float>(-0.8321, +0.2774),  // 3
	std::complex<float>(-0.2774, -1.3868),  // 4
	std::complex<float>(-0.8321, -0.8321),  // 5
	std::complex<float>(-1.3868, -0.2774),  // 6
	std::complex<float>(-0.8321, -0.2774),  // 7
	std::complex<float>(+0.2774, +1.3868),  // 8
	std::complex<float>(+0.8321, +0.8321),  // 9
	std::complex<float>(+1.3868, +0.2774),  // 10
	std::complex<float>(+0.8321, +0.2774),  // 11
	std::complex<float>(+0.2774, -1.3868),  // 12
	std::complex<float>(+0.8321, -0.8321),  // 13
	std::complex<float>(+1.3868, -0.2774),  // 14
	std::complex<float>(+0.8321, -0.2774),  // 15
	std::complex<float>(-0.2774, +0.8321),  // 16
	std::complex<float>(-0.2774, +0.2774),  // 17
	std::complex<float>(-0.2774, -0.8321),  // 18
	std::complex<float>(-0.2774, -0.2774),  // 19
	std::complex<float>(+0.2774, +0.8321),  // 20
	std::complex<float>(+0.2774, +0.2774),  // 21
	std::complex<float>(+0.2774, -0.8321),  // 22
	std::complex<float>(+0.2774, -0.2774)   // 23
};

// codeword for different modulations
const std::string QAM24_code[24] = {
	"00000",    // 5-bit, 0
	"00001",    // 5-bit, 1
	"00010",    // 5-bit, 2
	"00011",    // 5-bit, 3
	"00100",    // 5-bit, 4
	"00101",    // 5-bit, 5
	"00110",    // 5-bit, 6
	"00111",    // 5-bit, 7
	"10000",    // 5-bit, 8
	"10001",    // 5-bit, 9
	"10010",    // 5-bit, 10
	"10011",    // 5-bit, 11
	"10100",    // 5-bit, 12
	"10101",    // 5-bit, 13
	"10110",    // 5-bit, 14
	"10111",    // 5-bit, 15
	"0100",    // 4-bit, 16
	"0101",    // 4-bit, 17
	"0110",    // 4-bit, 18
	"0111",    // 4-bit, 19
	"1100",    // 4-bit, 20
	"1101",    // 4-bit, 21
	"1110",    // 4-bit, 22
	"1111"     // 4-bit, 23
};

// codeword length for different modulations
const int QAM24_bitLen[24] = {
	5,    // 0
	5,    // 1
	5,    // 2
	5,    // 3
	5,    // 4
	5,    // 5
	5,    // 6
	5,    // 7
	5,    // 8
	5,    // 9
	5,    // 10
	5,    // 11
	5,    // 12
	5,    // 13
	5,    // 14
	5,    // 15
	4,    // 16
	4,    // 17
	4,    // 18
	4,    // 19
	4,    // 20
	4,    // 21
	4,    // 22
	4     // 23
};

// codeword length set for different modulations
const int QAM24_lenSet[2] = {
	5,    // 0
	4     // 1
};

// constellation used for marker
const std::complex<float> QAM24_3bitMarker_D[8] = {	// 3 bit
	std::complex<float>(+0.2774, -0.2774),  // 0
	std::complex<float>(-0.8321, -0.2774),  // 1
	std::complex<float>(+0.2774, +0.8321),  // 2
	std::complex<float>(-0.2774, +0.2774),  // 3
	std::complex<float>(+0.8321, +0.2774),  // 4
	std::complex<float>(-0.2774, -0.8321),  // 5
	std::complex<float>(-0.8321, +0.8321),  // 6
	std::complex<float>(+0.8321, -0.8321)   // 7
};

/*-------------- 28-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM28_D[28] = {
	std::complex<float>(-0.2500, +1.2500),  // 0
	std::complex<float>(-0.7500, +0.7500),  // 1
	std::complex<float>(-1.2500, +0.2500),  // 2
	std::complex<float>(-0.7500, +0.2500),  // 3
	std::complex<float>(-0.2500, -1.2500),  // 4
	std::complex<float>(-0.7500, -0.7500),  // 5
	std::complex<float>(-1.2500, -0.2500),  // 6
	std::complex<float>(-0.7500, -0.2500),  // 7
	std::complex<float>(-0.2500, +0.7500),  // 8
	std::complex<float>(-0.7500, +1.2500),  // 9
	std::complex<float>(-0.7500, -1.2500),  // 10
	std::complex<float>(-0.2500, -0.7500),  // 11
	std::complex<float>(+0.2500, +1.2500),  // 12
	std::complex<float>(+0.7500, +0.7500),  // 13
	std::complex<float>(+1.2500, +0.2500),  // 14
	std::complex<float>(+0.7500, +0.2500),  // 15
	std::complex<float>(+0.2500, -1.2500),  // 16
	std::complex<float>(+0.7500, -0.7500),  // 17
	std::complex<float>(+1.2500, -0.2500),  // 18
	std::complex<float>(+0.7500, -0.2500),  // 19
	std::complex<float>(+0.2500, +0.7500),  // 20
	std::complex<float>(+0.7500, +1.2500),  // 21
	std::complex<float>(+0.7500, -1.2500),  // 22
	std::complex<float>(+0.2500, -0.7500),  // 23
	std::complex<float>(-0.2500, +0.2500),  // 24
	std::complex<float>(-0.2500, -0.2500),  // 25
	std::complex<float>(+0.2500, +0.2500),  // 26
	std::complex<float>(+0.2500, -0.2500)   // 27
};

// codeword for different modulations
const std::string QAM28_code[28] = {
	"00000",    // 5-bit, 0
	"00001",    // 5-bit, 1
	"00010",    // 5-bit, 2
	"00011",    // 5-bit, 3
	"00100",    // 5-bit, 4
	"00101",    // 5-bit, 5
	"00110",    // 5-bit, 6
	"00111",    // 5-bit, 7
	"01000",    // 5-bit, 8
	"01001",    // 5-bit, 9
	"01100",    // 5-bit, 10
	"01101",    // 5-bit, 11
	"10000",    // 5-bit, 12
	"10001",    // 5-bit, 13
	"10010",    // 5-bit, 14
	"10011",    // 5-bit, 15
	"10100",    // 5-bit, 16
	"10101",    // 5-bit, 17
	"10110",    // 5-bit, 18
	"10111",    // 5-bit, 19
	"11000",    // 5-bit, 20
	"11001",    // 5-bit, 21
	"11100",    // 5-bit, 22
	"11101",    // 5-bit, 23
	"0101",    // 4-bit, 24
	"0111",    // 4-bit, 25
	"1101",    // 4-bit, 26
	"1111"     // 4-bit, 27
};

// codeword length for different modulations
const int QAM28_bitLen[28] = {
	5,    // 0
	5,    // 1
	5,    // 2
	5,    // 3
	5,    // 4
	5,    // 5
	5,    // 6
	5,    // 7
	5,    // 8
	5,    // 9
	5,    // 10
	5,    // 11
	5,    // 12
	5,    // 13
	5,    // 14
	5,    // 15
	5,    // 16
	5,    // 17
	5,    // 18
	5,    // 19
	5,    // 20
	5,    // 21
	5,    // 22
	5,    // 23
	4,    // 24
	4,    // 25
	4,    // 26
	4     // 27
};

// codeword length set for different modulations
const int QAM28_lenSet[2] = {
	5,    // 0
	4     // 1
};

// constellation used for marker
const std::complex<float> QAM28_3bitMarker_D[8] = {	// 3 bit
	std::complex<float>(+0.2500, -0.2500),  // 0
	std::complex<float>(-0.7500, -0.2500),  // 1
	std::complex<float>(+0.2500, +0.7500),  // 2
	std::complex<float>(-0.2500, +0.2500),  // 3
	std::complex<float>(+0.7500, +0.2500),  // 4
	std::complex<float>(-0.2500, -0.7500),  // 5
	std::complex<float>(-0.7500, +0.7500),  // 6
	std::complex<float>(+0.7500, -0.7500)   // 7
};

/*-------------- 32-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM32_D[32] = {
	std::complex<float>(-0.6708, +1.1180),  // 0
	std::complex<float>(-0.2236, +1.1180),  // 1
	std::complex<float>(-0.6708, -1.1180),  // 2
	std::complex<float>(-0.2236, -1.1180),  // 3
	std::complex<float>(-1.1180, +0.6708),  // 4
	std::complex<float>(-1.1180, +0.2236),  // 5
	std::complex<float>(-1.1180, -0.6708),  // 6
	std::complex<float>(-1.1180, -0.2236),  // 7
	std::complex<float>(-0.2236, +0.6708),  // 8
	std::complex<float>(-0.2236, +0.2236),  // 9
	std::complex<float>(-0.2236, -0.6708),  // 10
	std::complex<float>(-0.2236, -0.2236),  // 11
	std::complex<float>(-0.6708, +0.6708),  // 12
	std::complex<float>(-0.6708, +0.2236),  // 13
	std::complex<float>(-0.6708, -0.6708),  // 14
	std::complex<float>(-0.6708, -0.2236),  // 15
	std::complex<float>(+0.6708, +1.1180),  // 16
	std::complex<float>(+0.2236, +1.1180),  // 17
	std::complex<float>(+0.6708, -1.1180),  // 18
	std::complex<float>(+0.2236, -1.1180),  // 19
	std::complex<float>(+1.1180, +0.6708),  // 20
	std::complex<float>(+1.1180, +0.2236),  // 21
	std::complex<float>(+1.1180, -0.6708),  // 22
	std::complex<float>(+1.1180, -0.2236),  // 23
	std::complex<float>(+0.2236, +0.6708),  // 24
	std::complex<float>(+0.2236, +0.2236),  // 25
	std::complex<float>(+0.2236, -0.6708),  // 26
	std::complex<float>(+0.2236, -0.2236),  // 27
	std::complex<float>(+0.6708, +0.6708),  // 28
	std::complex<float>(+0.6708, +0.2236),  // 29
	std::complex<float>(+0.6708, -0.6708),  // 30
	std::complex<float>(+0.6708, -0.2236)   // 31
};

// codeword for different modulations
const std::string QAM32_code[32] = {
	"00000",    // 5-bit, 0
	"00001",    // 5-bit, 1
	"00010",    // 5-bit, 2
	"00011",    // 5-bit, 3
	"00100",    // 5-bit, 4
	"00101",    // 5-bit, 5
	"00110",    // 5-bit, 6
	"00111",    // 5-bit, 7
	"01000",    // 5-bit, 8
	"01001",    // 5-bit, 9
	"01010",    // 5-bit, 10
	"01011",    // 5-bit, 11
	"01100",    // 5-bit, 12
	"01101",    // 5-bit, 13
	"01110",    // 5-bit, 14
	"01111",    // 5-bit, 15
	"10000",    // 5-bit, 16
	"10001",    // 5-bit, 17
	"10010",    // 5-bit, 18
	"10011",    // 5-bit, 19
	"10100",    // 5-bit, 20
	"10101",    // 5-bit, 21
	"10110",    // 5-bit, 22
	"10111",    // 5-bit, 23
	"11000",    // 5-bit, 24
	"11001",    // 5-bit, 25
	"11010",    // 5-bit, 26
	"11011",    // 5-bit, 27
	"11100",    // 5-bit, 28
	"11101",    // 5-bit, 29
	"11110",    // 5-bit, 30
	"11111"     // 5-bit, 31
};

// codeword length for different modulations
const int QAM32_bitLen[32] = {
	5,    // 0
	5,    // 1
	5,    // 2
	5,    // 3
	5,    // 4
	5,    // 5
	5,    // 6
	5,    // 7
	5,    // 8
	5,    // 9
	5,    // 10
	5,    // 11
	5,    // 12
	5,    // 13
	5,    // 14
	5,    // 15
	5,    // 16
	5,    // 17
	5,    // 18
	5,    // 19
	5,    // 20
	5,    // 21
	5,    // 22
	5,    // 23
	5,    // 24
	5,    // 25
	5,    // 26
	5,    // 27
	5,    // 28
	5,    // 29
	5,    // 30
	5     // 31
};

// codeword length set for different modulations
const int QAM32_lenSet[2] = {
	5,     // 0
	5
};

// constellation used for marker
const std::complex<float> QAM32_3bitMarker_D[8] = {	// 3 bit
	std::complex<float>(-0.2236, -1.1180),  // 0
	std::complex<float>(+1.1180, -0.6708),  // 1
	std::complex<float>(-0.6708, +0.2236),  // 2
	std::complex<float>(+0.2236, -0.2236),  // 3
	std::complex<float>(+1.1180, +0.2236),  // 4
	std::complex<float>(+0.6708, +1.1180),  // 5
	std::complex<float>(-0.2236, +1.1180),  // 6
	std::complex<float>(-1.1180, -0.6708)   // 7
};

/*-------------- 40-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM40_D[40] = {
	std::complex<float>(-1.0660, +1.0660),  // 0
	std::complex<float>(-0.6396, +1.0660),  // 1
	std::complex<float>(-1.0660, -1.0660),  // 2
	std::complex<float>(-0.6396, -1.0660),  // 3
	std::complex<float>(-1.0660, +0.6396),  // 4
	std::complex<float>(-1.4924, +0.2132),  // 5
	std::complex<float>(-1.0660, -0.6396),  // 6
	std::complex<float>(-1.4924, -0.2132),  // 7
	std::complex<float>(+1.0660, +1.0660),  // 8
	std::complex<float>(+0.6396, +1.0660),  // 9
	std::complex<float>(+1.0660, -1.0660),  // 10
	std::complex<float>(+0.6396, -1.0660),  // 11
	std::complex<float>(+1.0660, +0.6396),  // 12
	std::complex<float>(+1.4924, +0.2132),  // 13
	std::complex<float>(+1.0660, -0.6396),  // 14
	std::complex<float>(+1.4924, -0.2132),  // 15
	std::complex<float>(-0.2132, +1.0660),  // 16
	std::complex<float>(-0.2132, -1.0660),  // 17
	std::complex<float>(-1.0660, +0.2132),  // 18
	std::complex<float>(-1.0660, -0.2132),  // 19
	std::complex<float>(-0.2132, +0.6396),  // 20
	std::complex<float>(-0.2132, +0.2132),  // 21
	std::complex<float>(-0.2132, -0.6396),  // 22
	std::complex<float>(-0.2132, -0.2132),  // 23
	std::complex<float>(-0.6396, +0.6396),  // 24
	std::complex<float>(-0.6396, +0.2132),  // 25
	std::complex<float>(-0.6396, -0.6396),  // 26
	std::complex<float>(-0.6396, -0.2132),  // 27
	std::complex<float>(+0.2132, +1.0660),  // 28
	std::complex<float>(+0.2132, -1.0660),  // 29
	std::complex<float>(+1.0660, +0.2132),  // 30
	std::complex<float>(+1.0660, -0.2132),  // 31
	std::complex<float>(+0.2132, +0.6396),  // 32
	std::complex<float>(+0.2132, +0.2132),  // 33
	std::complex<float>(+0.2132, -0.6396),  // 34
	std::complex<float>(+0.2132, -0.2132),  // 35
	std::complex<float>(+0.6396, +0.6396),  // 36
	std::complex<float>(+0.6396, +0.2132),  // 37
	std::complex<float>(+0.6396, -0.6396),  // 38
	std::complex<float>(+0.6396, -0.2132)   // 39
};

// codeword for different modulations
const std::string QAM40_code[40] = {
	"000000",    // 6-bit, 0
	"000001",    // 6-bit, 1
	"000100",    // 6-bit, 2
	"000101",    // 6-bit, 3
	"001000",    // 6-bit, 4
	"001001",    // 6-bit, 5
	"001100",    // 6-bit, 6
	"001101",    // 6-bit, 7
	"100000",    // 6-bit, 8
	"100001",    // 6-bit, 9
	"100100",    // 6-bit, 10
	"100101",    // 6-bit, 11
	"101000",    // 6-bit, 12
	"101001",    // 6-bit, 13
	"101100",    // 6-bit, 14
	"101101",    // 6-bit, 15
	"00001",    // 5-bit, 16
	"00011",    // 5-bit, 17
	"00101",    // 5-bit, 18
	"00111",    // 5-bit, 19
	"01000",    // 5-bit, 20
	"01001",    // 5-bit, 21
	"01010",    // 5-bit, 22
	"01011",    // 5-bit, 23
	"01100",    // 5-bit, 24
	"01101",    // 5-bit, 25
	"01110",    // 5-bit, 26
	"01111",    // 5-bit, 27
	"10001",    // 5-bit, 28
	"10011",    // 5-bit, 29
	"10101",    // 5-bit, 30
	"10111",    // 5-bit, 31
	"11000",    // 5-bit, 32
	"11001",    // 5-bit, 33
	"11010",    // 5-bit, 34
	"11011",    // 5-bit, 35
	"11100",    // 5-bit, 36
	"11101",    // 5-bit, 37
	"11110",    // 5-bit, 38
	"11111"     // 5-bit, 39
};

// codeword length for different modulations
const int QAM40_bitLen[40] = {
	6,    // 0
	6,    // 1
	6,    // 2
	6,    // 3
	6,    // 4
	6,    // 5
	6,    // 6
	6,    // 7
	6,    // 8
	6,    // 9
	6,    // 10
	6,    // 11
	6,    // 12
	6,    // 13
	6,    // 14
	6,    // 15
	5,    // 16
	5,    // 17
	5,    // 18
	5,    // 19
	5,    // 20
	5,    // 21
	5,    // 22
	5,    // 23
	5,    // 24
	5,    // 25
	5,    // 26
	5,    // 27
	5,    // 28
	5,    // 29
	5,    // 30
	5,    // 31
	5,    // 32
	5,    // 33
	5,    // 34
	5,    // 35
	5,    // 36
	5,    // 37
	5,    // 38
	5     // 39
};

// codeword length set for different modulations
const int QAM40_lenSet[2] = {
	6,    // 0
	5     // 1
};

// constellation used for marker
const std::complex<float> QAM40_3bitMarker_D[8] = {	// 3 bit
	std::complex<float>(-1.4924, -0.2132),  // 0
	std::complex<float>(-1.0660, -1.0660),  // 1
	std::complex<float>(-0.2132, -0.2132),  // 2
	std::complex<float>(+0.2132, -1.0660),  // 3
	std::complex<float>(+1.0660, -0.2132),  // 4
	std::complex<float>(+1.0660, +1.0660),  // 5
	std::complex<float>(+0.2132, +0.6396),  // 6
	std::complex<float>(-1.0660, +1.0660)   // 7
};

/*-------------- 48-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM48_D[48] = {
	std::complex<float>(-0.9901, +0.9901),  // 0
	std::complex<float>(-0.5941, +0.9901),  // 1
	std::complex<float>(-0.1980, +1.3862),  // 2
	std::complex<float>(-0.1980, +0.9901),  // 3
	std::complex<float>(-0.9901, -0.9901),  // 4
	std::complex<float>(-0.5941, -0.9901),  // 5
	std::complex<float>(-0.1980, -1.3862),  // 6
	std::complex<float>(-0.1980, -0.9901),  // 7
	std::complex<float>(-0.9901, +0.5941),  // 8
	std::complex<float>(-1.3862, +0.5941),  // 9
	std::complex<float>(-0.9901, +0.1980),  // 10
	std::complex<float>(-1.3862, +0.1980),  // 11
	std::complex<float>(-0.9901, -0.5941),  // 12
	std::complex<float>(-1.3862, -0.5941),  // 13
	std::complex<float>(-0.9901, -0.1980),  // 14
	std::complex<float>(-1.3862, -0.1980),  // 15
	std::complex<float>(+0.9901, +0.9901),  // 16
	std::complex<float>(+0.5941, +0.9901),  // 17
	std::complex<float>(+0.1980, +1.3862),  // 18
	std::complex<float>(+0.1980, +0.9901),  // 19
	std::complex<float>(+0.9901, -0.9901),  // 20
	std::complex<float>(+0.5941, -0.9901),  // 21
	std::complex<float>(+0.1980, -1.3862),  // 22
	std::complex<float>(+0.1980, -0.9901),  // 23
	std::complex<float>(+0.9901, +0.5941),  // 24
	std::complex<float>(+1.3862, +0.5941),  // 25
	std::complex<float>(+0.9901, +0.1980),  // 26
	std::complex<float>(+1.3862, +0.1980),  // 27
	std::complex<float>(+0.9901, -0.5941),  // 28
	std::complex<float>(+1.3862, -0.5941),  // 29
	std::complex<float>(+0.9901, -0.1980),  // 30
	std::complex<float>(+1.3862, -0.1980),  // 31
	std::complex<float>(-0.1980, +0.5941),  // 32
	std::complex<float>(-0.1980, +0.1980),  // 33
	std::complex<float>(-0.1980, -0.5941),  // 34
	std::complex<float>(-0.1980, -0.1980),  // 35
	std::complex<float>(-0.5941, +0.5941),  // 36
	std::complex<float>(-0.5941, +0.1980),  // 37
	std::complex<float>(-0.5941, -0.5941),  // 38
	std::complex<float>(-0.5941, -0.1980),  // 39
	std::complex<float>(+0.1980, +0.5941),  // 40
	std::complex<float>(+0.1980, +0.1980),  // 41
	std::complex<float>(+0.1980, -0.5941),  // 42
	std::complex<float>(+0.1980, -0.1980),  // 43
	std::complex<float>(+0.5941, +0.5941),  // 44
	std::complex<float>(+0.5941, +0.1980),  // 45
	std::complex<float>(+0.5941, -0.5941),  // 46
	std::complex<float>(+0.5941, -0.1980)   // 47
};

// codeword for different modulations
const std::string QAM48_code[48] = {
	"000000",    // 6-bit, 0
	"000001",    // 6-bit, 1
	"000010",    // 6-bit, 2
	"000011",    // 6-bit, 3
	"000100",    // 6-bit, 4
	"000101",    // 6-bit, 5
	"000110",    // 6-bit, 6
	"000111",    // 6-bit, 7
	"001000",    // 6-bit, 8
	"001001",    // 6-bit, 9
	"001010",    // 6-bit, 10
	"001011",    // 6-bit, 11
	"001100",    // 6-bit, 12
	"001101",    // 6-bit, 13
	"001110",    // 6-bit, 14
	"001111",    // 6-bit, 15
	"100000",    // 6-bit, 16
	"100001",    // 6-bit, 17
	"100010",    // 6-bit, 18
	"100011",    // 6-bit, 19
	"100100",    // 6-bit, 20
	"100101",    // 6-bit, 21
	"100110",    // 6-bit, 22
	"100111",    // 6-bit, 23
	"101000",    // 6-bit, 24
	"101001",    // 6-bit, 25
	"101010",    // 6-bit, 26
	"101011",    // 6-bit, 27
	"101100",    // 6-bit, 28
	"101101",    // 6-bit, 29
	"101110",    // 6-bit, 30
	"101111",    // 6-bit, 31
	"01000",    // 5-bit, 32
	"01001",    // 5-bit, 33
	"01010",    // 5-bit, 34
	"01011",    // 5-bit, 35
	"01100",    // 5-bit, 36
	"01101",    // 5-bit, 37
	"01110",    // 5-bit, 38
	"01111",    // 5-bit, 39
	"11000",    // 5-bit, 40
	"11001",    // 5-bit, 41
	"11010",    // 5-bit, 42
	"11011",    // 5-bit, 43
	"11100",    // 5-bit, 44
	"11101",    // 5-bit, 45
	"11110",    // 5-bit, 46
	"11111"     // 5-bit, 47
};

// codeword length for different modulations
const int QAM48_bitLen[48] = {
	6,    // 0
	6,    // 1
	6,    // 2
	6,    // 3
	6,    // 4
	6,    // 5
	6,    // 6
	6,    // 7
	6,    // 8
	6,    // 9
	6,    // 10
	6,    // 11
	6,    // 12
	6,    // 13
	6,    // 14
	6,    // 15
	6,    // 16
	6,    // 17
	6,    // 18
	6,    // 19
	6,    // 20
	6,    // 21
	6,    // 22
	6,    // 23
	6,    // 24
	6,    // 25
	6,    // 26
	6,    // 27
	6,    // 28
	6,    // 29
	6,    // 30
	6,    // 31
	5,    // 32
	5,    // 33
	5,    // 34
	5,    // 35
	5,    // 36
	5,    // 37
	5,    // 38
	5,    // 39
	5,    // 40
	5,    // 41
	5,    // 42
	5,    // 43
	5,    // 44
	5,    // 45
	5,    // 46
	5     // 47
};

// codeword length set for different modulations
const int QAM48_lenSet[2] = {
	6,    // 0
	5     // 1
};

// constellation used for marker
const std::complex<float> QAM48_3bitMarker_D[8] = {	// 3 bit
	std::complex<float>(-1.3862, -0.1980),  // 0
	std::complex<float>(-0.5941, -0.9901),  // 1
	std::complex<float>(+0.1980, -0.1980),  // 2
	std::complex<float>(+0.9901, -0.9901),  // 3
	std::complex<float>(+1.3862, -0.1980),  // 4
	std::complex<float>(+0.9901, +0.9901),  // 5
	std::complex<float>(-0.1980, +1.3862),  // 6
	std::complex<float>(-0.5941, +0.5941)   // 7
};

/*-------------- 56-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
const std::complex<float> QAM56_D[56] = {
	std::complex<float>(-0.5303, +0.8839),  // 0
	std::complex<float>(-0.5303, +1.2374),  // 1
	std::complex<float>(-0.1768, +0.8839),  // 2
	std::complex<float>(-0.1768, +1.2374),  // 3
	std::complex<float>(-0.5303, -0.8839),  // 4
	std::complex<float>(-0.5303, -1.2374),  // 5
	std::complex<float>(-0.1768, -0.8839),  // 6
	std::complex<float>(-0.1768, -1.2374),  // 7
	std::complex<float>(-0.5303, +0.5303),  // 8
	std::complex<float>(-0.8839, +0.8839),  // 9
	std::complex<float>(-0.5303, +0.1768),  // 10
	std::complex<float>(-0.8839, +0.5303),  // 11
	std::complex<float>(-0.5303, -0.5303),  // 12
	std::complex<float>(-0.8839, -0.8839),  // 13
	std::complex<float>(-0.5303, -0.1768),  // 14
	std::complex<float>(-0.8839, -0.5303),  // 15
	std::complex<float>(-1.2374, +0.5303),  // 16
	std::complex<float>(-1.2374, +0.8839),  // 17
	std::complex<float>(-1.2374, +0.1768),  // 18
	std::complex<float>(-0.8839, +0.1768),  // 19
	std::complex<float>(-1.2374, -0.5303),  // 20
	std::complex<float>(-1.2374, -0.8839),  // 21
	std::complex<float>(-1.2374, -0.1768),  // 22
	std::complex<float>(-0.8839, -0.1768),  // 23
	std::complex<float>(+0.5303, +0.8839),  // 24
	std::complex<float>(+0.5303, +1.2374),  // 25
	std::complex<float>(+0.1768, +0.8839),  // 26
	std::complex<float>(+0.1768, +1.2374),  // 27
	std::complex<float>(+0.5303, -0.8839),  // 28
	std::complex<float>(+0.5303, -1.2374),  // 29
	std::complex<float>(+0.1768, -0.8839),  // 30
	std::complex<float>(+0.1768, -1.2374),  // 31
	std::complex<float>(+0.5303, +0.5303),  // 32
	std::complex<float>(+0.8839, +0.8839),  // 33
	std::complex<float>(+0.5303, +0.1768),  // 34
	std::complex<float>(+0.8839, +0.5303),  // 35
	std::complex<float>(+0.5303, -0.5303),  // 36
	std::complex<float>(+0.8839, -0.8839),  // 37
	std::complex<float>(+0.5303, -0.1768),  // 38
	std::complex<float>(+0.8839, -0.5303),  // 39
	std::complex<float>(+1.2374, +0.5303),  // 40
	std::complex<float>(+1.2374, +0.8839),  // 41
	std::complex<float>(+1.2374, +0.1768),  // 42
	std::complex<float>(+0.8839, +0.1768),  // 43
	std::complex<float>(+1.2374, -0.5303),  // 44
	std::complex<float>(+1.2374, -0.8839),  // 45
	std::complex<float>(+1.2374, -0.1768),  // 46
	std::complex<float>(+0.8839, -0.1768),  // 47
	std::complex<float>(-0.1768, +0.5303),  // 48
	std::complex<float>(-0.1768, +0.1768),  // 49
	std::complex<float>(-0.1768, -0.5303),  // 50
	std::complex<float>(-0.1768, -0.1768),  // 51
	std::complex<float>(+0.1768, +0.5303),  // 52
	std::complex<float>(+0.1768, +0.1768),  // 53
	std::complex<float>(+0.1768, -0.5303),  // 54
	std::complex<float>(+0.1768, -0.1768)   // 55
};

// codeword for different modulations
const std::string QAM56_code[56] = {
	"000000",    // 6-bit, 0
	"000001",    // 6-bit, 1
	"000010",    // 6-bit, 2
	"000011",    // 6-bit, 3
	"000100",    // 6-bit, 4
	"000101",    // 6-bit, 5
	"000110",    // 6-bit, 6
	"000111",    // 6-bit, 7
	"001000",    // 6-bit, 8
	"001001",    // 6-bit, 9
	"001010",    // 6-bit, 10
	"001011",    // 6-bit, 11
	"001100",    // 6-bit, 12
	"001101",    // 6-bit, 13
	"001110",    // 6-bit, 14
	"001111",    // 6-bit, 15
	"011000",    // 6-bit, 16
	"011001",    // 6-bit, 17
	"011010",    // 6-bit, 18
	"011011",    // 6-bit, 19
	"011100",    // 6-bit, 20
	"011101",    // 6-bit, 21
	"011110",    // 6-bit, 22
	"011111",    // 6-bit, 23
	"100000",    // 6-bit, 24
	"100001",    // 6-bit, 25
	"100010",    // 6-bit, 26
	"100011",    // 6-bit, 27
	"100100",    // 6-bit, 28
	"100101",    // 6-bit, 29
	"100110",    // 6-bit, 30
	"100111",    // 6-bit, 31
	"101000",    // 6-bit, 32
	"101001",    // 6-bit, 33
	"101010",    // 6-bit, 34
	"101011",    // 6-bit, 35
	"101100",    // 6-bit, 36
	"101101",    // 6-bit, 37
	"101110",    // 6-bit, 38
	"101111",    // 6-bit, 39
	"111000",    // 6-bit, 40
	"111001",    // 6-bit, 41
	"111010",    // 6-bit, 42
	"111011",    // 6-bit, 43
	"111100",    // 6-bit, 44
	"111101",    // 6-bit, 45
	"111110",    // 6-bit, 46
	"111111",    // 6-bit, 47
	"01000",    // 5-bit, 48
	"01001",    // 5-bit, 49
	"01010",    // 5-bit, 50
	"01011",    // 5-bit, 51
	"11000",    // 5-bit, 52
	"11001",    // 5-bit, 53
	"11010",    // 5-bit, 54
	"11011"     // 5-bit, 55
};

// codeword length for different modulations
const int QAM56_bitLen[56] = {
	6,    // 0
	6,    // 1
	6,    // 2
	6,    // 3
	6,    // 4
	6,    // 5
	6,    // 6
	6,    // 7
	6,    // 8
	6,    // 9
	6,    // 10
	6,    // 11
	6,    // 12
	6,    // 13
	6,    // 14
	6,    // 15
	6,    // 16
	6,    // 17
	6,    // 18
	6,    // 19
	6,    // 20
	6,    // 21
	6,    // 22
	6,    // 23
	6,    // 24
	6,    // 25
	6,    // 26
	6,    // 27
	6,    // 28
	6,    // 29
	6,    // 30
	6,    // 31
	6,    // 32
	6,    // 33
	6,    // 34
	6,    // 35
	6,    // 36
	6,    // 37
	6,    // 38
	6,    // 39
	6,    // 40
	6,    // 41
	6,    // 42
	6,    // 43
	6,    // 44
	6,    // 45
	6,    // 46
	6,    // 47
	5,    // 48
	5,    // 49
	5,    // 50
	5,    // 51
	5,    // 52
	5,    // 53
	5,    // 54
	5     // 55
};

// codeword length set for different modulations
const int QAM56_lenSet[2] = {
	6,    // 0
	5     // 1
};

// constellation used for marker
const std::complex<float> QAM56_3bitMarker_D[8] = {	// 3 bit
	std::complex<float>(-1.2374, -0.5303),  // 0
	std::complex<float>(-0.5303, -1.2374),  // 1
	std::complex<float>(-0.1768, -0.1768),  // 2
	std::complex<float>(+0.5303, -1.2374),  // 3
	std::complex<float>(+1.2374, -0.1768),  // 4
	std::complex<float>(+0.8839, +0.8839),  // 5
	std::complex<float>(-0.1768, +1.2374),  // 6
	std::complex<float>(-1.2374, +0.8839)   // 7
};

/*-------------- 64-QAM --------------*/
// constellations for different modulations (normalized to avg power 1)
/*const std::complex<float> QAM64_D[64] = {
	std::complex<float>(-1.0801, +1.0801),  // 0
	std::complex<float>(-1.0801, +0.7715),  // 1
	std::complex<float>(-1.0801, +0.1543),  // 2
	std::complex<float>(-1.0801, +0.4629),  // 3
	std::complex<float>(-1.0801, -1.0801),  // 4
	std::complex<float>(-1.0801, -0.7715),  // 5
	std::complex<float>(-1.0801, -0.1543),  // 6
	std::complex<float>(-1.0801, -0.4629),  // 7
	std::complex<float>(-0.7715, +1.0801),  // 8
	std::complex<float>(-0.7715, +0.7715),  // 9
	std::complex<float>(-0.7715, +0.1543),  // 10
	std::complex<float>(-0.7715, +0.4629),  // 11
	std::complex<float>(-0.7715, -1.0801),  // 12
	std::complex<float>(-0.7715, -0.7715),  // 13
	std::complex<float>(-0.7715, -0.1543),  // 14
	std::complex<float>(-0.7715, -0.4629),  // 15
	std::complex<float>(-0.1543, +1.0801),  // 16
	std::complex<float>(-0.1543, +0.7715),  // 17
	std::complex<float>(-0.1543, +0.1543),  // 18
	std::complex<float>(-0.1543, +0.4629),  // 19
	std::complex<float>(-0.1543, -1.0801),  // 20
	std::complex<float>(-0.1543, -0.7715),  // 21
	std::complex<float>(-0.1543, -0.1543),  // 22
	std::complex<float>(-0.1543, -0.4629),  // 23
	std::complex<float>(-0.4629, +1.0801),  // 24
	std::complex<float>(-0.4629, +0.7715),  // 25
	std::complex<float>(-0.4629, +0.1543),  // 26
	std::complex<float>(-0.4629, +0.4629),  // 27
	std::complex<float>(-0.4629, -1.0801),  // 28
	std::complex<float>(-0.4629, -0.7715),  // 29
	std::complex<float>(-0.4629, -0.1543),  // 30
	std::complex<float>(-0.4629, -0.4629),  // 31
	std::complex<float>(+1.0801, +1.0801),  // 32
	std::complex<float>(+1.0801, +0.7715),  // 33
	std::complex<float>(+1.0801, +0.1543),  // 34
	std::complex<float>(+1.0801, +0.4629),  // 35
	std::complex<float>(+1.0801, -1.0801),  // 36
	std::complex<float>(+1.0801, -0.7715),  // 37
	std::complex<float>(+1.0801, -0.1543),  // 38
	std::complex<float>(+1.0801, -0.4629),  // 39
	std::complex<float>(+0.7715, +1.0801),  // 40
	std::complex<float>(+0.7715, +0.7715),  // 41
	std::complex<float>(+0.7715, +0.1543),  // 42
	std::complex<float>(+0.7715, +0.4629),  // 43
	std::complex<float>(+0.7715, -1.0801),  // 44
	std::complex<float>(+0.7715, -0.7715),  // 45
	std::complex<float>(+0.7715, -0.1543),  // 46
	std::complex<float>(+0.7715, -0.4629),  // 47
	std::complex<float>(+0.1543, +1.0801),  // 48
	std::complex<float>(+0.1543, +0.7715),  // 49
	std::complex<float>(+0.1543, +0.1543),  // 50
	std::complex<float>(+0.1543, +0.4629),  // 51
	std::complex<float>(+0.1543, -1.0801),  // 52
	std::complex<float>(+0.1543, -0.7715),  // 53
	std::complex<float>(+0.1543, -0.1543),  // 54
	std::complex<float>(+0.1543, -0.4629),  // 55
	std::complex<float>(+0.4629, +1.0801),  // 56
	std::complex<float>(+0.4629, +0.7715),  // 57
	std::complex<float>(+0.4629, +0.1543),  // 58
	std::complex<float>(+0.4629, +0.4629),  // 59
	std::complex<float>(+0.4629, -1.0801),  // 60
	std::complex<float>(+0.4629, -0.7715),  // 61
	std::complex<float>(+0.4629, -0.1543),  // 62
	std::complex<float>(+0.4629, -0.4629)   // 63
};
*/
// codeword for different modulations
const std::string QAM64_code[64] = {
	"000000",    // 6-bit, 0
	"000001",    // 6-bit, 1
	"000010",    // 6-bit, 2
	"000011",    // 6-bit, 3
	"000100",    // 6-bit, 4
	"000101",    // 6-bit, 5
	"000110",    // 6-bit, 6
	"000111",    // 6-bit, 7
	"001000",    // 6-bit, 8
	"001001",    // 6-bit, 9
	"001010",    // 6-bit, 10
	"001011",    // 6-bit, 11
	"001100",    // 6-bit, 12
	"001101",    // 6-bit, 13
	"001110",    // 6-bit, 14
	"001111",    // 6-bit, 15
	"010000",    // 6-bit, 16
	"010001",    // 6-bit, 17
	"010010",    // 6-bit, 18
	"010011",    // 6-bit, 19
	"010100",    // 6-bit, 20
	"010101",    // 6-bit, 21
	"010110",    // 6-bit, 22
	"010111",    // 6-bit, 23
	"011000",    // 6-bit, 24
	"011001",    // 6-bit, 25
	"011010",    // 6-bit, 26
	"011011",    // 6-bit, 27
	"011100",    // 6-bit, 28
	"011101",    // 6-bit, 29
	"011110",    // 6-bit, 30
	"011111",    // 6-bit, 31
	"100000",    // 6-bit, 32
	"100001",    // 6-bit, 33
	"100010",    // 6-bit, 34
	"100011",    // 6-bit, 35
	"100100",    // 6-bit, 36
	"100101",    // 6-bit, 37
	"100110",    // 6-bit, 38
	"100111",    // 6-bit, 39
	"101000",    // 6-bit, 40
	"101001",    // 6-bit, 41
	"101010",    // 6-bit, 42
	"101011",    // 6-bit, 43
	"101100",    // 6-bit, 44
	"101101",    // 6-bit, 45
	"101110",    // 6-bit, 46
	"101111",    // 6-bit, 47
	"110000",    // 6-bit, 48
	"110001",    // 6-bit, 49
	"110010",    // 6-bit, 50
	"110011",    // 6-bit, 51
	"110100",    // 6-bit, 52
	"110101",    // 6-bit, 53
	"110110",    // 6-bit, 54
	"110111",    // 6-bit, 55
	"111000",    // 6-bit, 56
	"111001",    // 6-bit, 57
	"111010",    // 6-bit, 58
	"111011",    // 6-bit, 59
	"111100",    // 6-bit, 60
	"111101",    // 6-bit, 61
	"111110",    // 6-bit, 62
	"111111"     // 6-bit, 63
};

// codeword length for different modulations
const int QAM64_bitLen[64] = {
	6,    // 0
	6,    // 1
	6,    // 2
	6,    // 3
	6,    // 4
	6,    // 5
	6,    // 6
	6,    // 7
	6,    // 8
	6,    // 9
	6,    // 10
	6,    // 11
	6,    // 12
	6,    // 13
	6,    // 14
	6,    // 15
	6,    // 16
	6,    // 17
	6,    // 18
	6,    // 19
	6,    // 20
	6,    // 21
	6,    // 22
	6,    // 23
	6,    // 24
	6,    // 25
	6,    // 26
	6,    // 27
	6,    // 28
	6,    // 29
	6,    // 30
	6,    // 31
	6,    // 32
	6,    // 33
	6,    // 34
	6,    // 35
	6,    // 36
	6,    // 37
	6,    // 38
	6,    // 39
	6,    // 40
	6,    // 41
	6,    // 42
	6,    // 43
	6,    // 44
	6,    // 45
	6,    // 46
	6,    // 47
	6,    // 48
	6,    // 49
	6,    // 50
	6,    // 51
	6,    // 52
	6,    // 53
	6,    // 54
	6,    // 55
	6,    // 56
	6,    // 57
	6,    // 58
	6,    // 59
	6,    // 60
	6,    // 61
	6,    // 62
	6     // 63
};

// codeword length set for different modulations
const int QAM64_lenSet[2] = {
	6,     // 0
	6
};

// constellation used for marker
const std::complex<float> QAM64_3bitMarker_D[8] = {	// 3 bit
	std::complex<float>(-1.0801, -1.0801),  // 0
	std::complex<float>(+1.0801, -1.0801),  // 1
	std::complex<float>(+1.0801, +1.0801),  // 2
	std::complex<float>(-1.0801, +1.0801),  // 3
	std::complex<float>(-0.4629, +0.4629),  // 4
	std::complex<float>(+0.4629, +0.4629),  // 5
	std::complex<float>(+0.4629, -0.4629),  // 6
	std::complex<float>(-0.4629, -0.4629)   // 7
};

// Currently only supports nBackup=2.
// TODO: Can be simply support every nBackup according to markerDecode
void VlcMod::QAMDemod_nonMarker(const std::complex<float> *sigRec, int nSig, int *nonMarker, double *nonMarkerDist, int nBackup) {

	if(nBackup !=2) {
		std::cerr << "VlcMod::QAMDemod_nonMarker only supports nBackup = 2, which is now" << nBackup << std::endl;
	}
	for(int i=0;i<nSig;i++) {
		double distL = 1000.0, distS = 1000.0;
		int indexL = 0, indexS = 0;

		for(int j=0;j<nQAM; j++) {
			double distTmp = std::abs(sigRec[i]-constellation_nonMarker[j]);
			if(bitLen[j] == lenSet[0]) {
				if(distTmp <= distL) {
					indexL = j;
					distL = distTmp;
				}
			}
			else {
				if(distTmp <= distS) {
					indexS = j;
					distS = distTmp;
				}
			}
		}
		
		nonMarker[nBackup*i] = indexL;
		nonMarkerDist[nBackup*i] = distL;
		nonMarker[nBackup*i+1] = indexS;
		nonMarkerDist[nBackup*i+1] = distS;
		
	}
}

void VlcMod::markerDecode(const std::complex<float> *sigRec, int nSig, int *marker, double *markerDist) {
//	std::cout << "No. of signals = " << nSig << std::endl;
	for(int i=0;i<nSig;i++) {
//		std::cout << "Signal " << std::setw(2) << i << std::endl;
		double dist[nMarker];
		for(int j=0;j<nMarker; j++) {
			dist[j] = std::abs(sigRec[i]-constellation_marker[j]);
//			std::cout << std::setw(4) << dist[j] << ",";
//			if(j == nMarker-1)
//				std::cout << std::endl;
		}
		int index[nMarker];
		for(int j=0;j<nMarker;j++)
			index[j] = j;
		IndexSort(index, dist, nMarker);
		for(int j=0;j<nMarker;j++) {
//			std::cout << index[j] << ",";			
//			if(j == nMarker-1)
//				std::cout << std::endl;
		}
		
		for(int j=0;j<nMarkerBackup;j++) {
//			std::cout << index[j] << "-" << dist[index[j]] << std::endl;
			marker[nMarkerBackup*i+j] = index[j];
			markerDist[nMarkerBackup*i+j] = dist[index[j]];
		}
	}
}

/*---------------------------------------------------------------------------------
 IndexSort.
 Return the index of the nMarkerBackup least index in the buffer
 Precondition: None
 Postcondition: A VlcMod decoding tree has been created with root node
 pointed to by myRoot. The inserted nodes are based on member variable "codeword".
 ---------------------------------------------------------------------------------*/
void VlcMod::IndexSort(int *pIndex, double *buffer, int buffer_len) {
	double f_tmp;
	int i_tmp = 0;
	bool swap = false;

	for(int j=0;j<nMarkerBackup;j++) {	// if change nMarkerBackup to buffer_len, sort thoroughly
		swap = false;
		f_tmp = buffer[pIndex[j]];
		for(int i=j+1;i<buffer_len;i++) {	// find the element smaller the reference
			if(f_tmp > buffer[pIndex[i]]) {
				i_tmp = i;
				f_tmp = buffer[pIndex[i]];
				swap = true;
			}
		}
		if(swap) {
			int iswap;
			// exchange the index
			iswap = pIndex[i_tmp];
			pIndex[i_tmp] = pIndex[j];
			pIndex[j] = iswap;
		}
	}
}

/*------------------------------------------------------------------
					From VlcMod.cpp
------------------------------------------------------------------*/

//--- Definition of buildDecodingTree()
void VlcMod::buildDecodingTree() {
	for (int i=0;i<nQAM;i++) {
		insert(i, (std::string) codeword[i]);
	}
}

//--- Definition of insert()
void VlcMod::insert(int ch, std::string code) {
	VlcMod::BinNodePointer p = myRoot; // pointer to move down the tree
	for (int i = 0, codeLen = code.length(); i < codeLen; i++) {
		switch (code[i]) {
		case '0': // descend left
			if (p->left == 0) // create node along path
				p->left = new VlcMod::BinNode(257);
			p = p->left;
			break;
		case '1': // descend right
			if (p->right == 0) // create node along path
				p->right = new VlcMod::BinNode(257);
			p = p->right;
			break;
		default:
			std::cerr << "*** Illegal character in code ***\n";
			return;
		}
	}
	p->data = ch;
}

/*---------------------------------------------------------------------------------
 VlcMod::mapper
 Map the encoded 0/1 bits to variable-length symbols. The last few bits may not form
 a complete symbol. In this case, padding the input stream so that each bits are mapped
 to a complete symbol.

 Parameters:
 	 bitsIn		- '0'/'1' bit streams to be mapped
 	 nBitsIn	- Length of input bit stream
 	 symOut		- Mapped symbol index
 	 nSymOut   	- Number of mapped symbols
 Return
 	 Number of Padding bits.
 ---------------------------------------------------------------------------------*/
int VlcMod::mapper(const char * bitsIn, const int nBitsIn, int * symOut, int *nSymOut) {
	char bit; // next message bit
	VlcMod::BinNodePointer p; // pointer to trace path in decoding tree
	int bitIdx = 0;
	int symIdx = 0;
	for (;;) {
		p = myRoot;
		while (p->left != 0 || p->right != 0) {
			bit = bitsIn[bitIdx];
			if (bitIdx == nBitsIn - 1) {
				while (p->left != 0 || p->right != 0) {
					bit = '0';
					bitIdx ++;
					std::cout << bit;
					if (bit == '0')
						p = p->left;
					else if (bit == '1')
						p = p->right;
					else
						std::cerr << "Illegal bit: " << bit << " -- ignored\n";
				}
				std::cout << "--" << p->data << "," << symIdx << std::endl;
				symOut[symIdx] = p->data;
				nSymOut[0] = symIdx + 1;
				std::cout << "mapper is done \n";
				return bitIdx - nBitsIn;
			}

			bitIdx ++;
			std::cout << bit;
			if (bit == '0')
				p = p->left;
			else if (bit == '1')
				p = p->right;
			else
				std::cerr << "Illegal bit: " << bit << " -- ignored\n";
		}

		std::cout << "--" << p->data << "," << symIdx << std::endl;
		symOut[symIdx] = p->data;
		symIdx ++;
	}
	std::cout << "mapper exits unexpectedly" << std::endl;
	return -1;
}
/*---------------------------------------------------------------------------------
 VlcMod::modulate
 Modulation with marker appended

 Parameters:
 	 symIn 	 - Index of symbols
 	 sigOut  - complex symbols out
 	 nSymIn	 - Number of mapped symbols
 	 nSigOut - Number of complex out symbols
 Return
 	 Number of output bits.
 TODO: Replace 7 with nMarker
 ---------------------------------------------------------------------------------*/
void VlcMod::modulate(const int *symIn, std::complex<float> *sigOut, int nSymIn, int nSigOut) {
	// compute how many symbols to be transmitted

	int nMarkerSym;
	if(nSymIn%7 == 0)
		nMarkerSym = nSymIn/7;
	else
		nMarkerSym = ceil(nSymIn/7.0);
	int nPadSymsLastMarker = nMarkerSym * 7 - nSymIn;
	for(int i=0;i<nMarkerSym-1;i++) {
		int nSymLong = 0;
		for(int j=0;j<7;j++) {
			int symIdx = symIn[7*i+j];
			sigOut[8*i+j] = constellation_nonMarker[symIdx];
			if(bitLen[symIdx] == lenSet[0])
				nSymLong ++;
		}
		sigOut[8*i+7] = constellation_marker[nSymLong];
		std::cout<< "Marker " << std::setw(2) << i << " = " << nSymLong <<", " << sigOut[8*i+7] << std::endl;
	}
	for(int i=nMarkerSym-1;i<nMarkerSym;i++) {
		int nSymLong = 0;
		for(int j=0;j<7-nPadSymsLastMarker;j++) {
			int symIdx = symIn[7*i+j];
			sigOut[8*i+j] = constellation_nonMarker[symIdx];
			if(bitLen[symIdx] == lenSet[0])
				nSymLong ++;
		}
		for(int j=7-nPadSymsLastMarker;j<7;j++) {	// set the padding symbols in the last group
			sigOut[8*i+j] = constellation_marker[2];
		}
		sigOut[8*i+7] = constellation_marker[nSymLong];
		std::cout<< "Marker " << std::setw(2) << i << " = " << nSymLong <<", " << sigOut[8*i+7] << std::endl;
	}
	for(int i=8*nMarkerSym;i<nSigOut;i++) {	// set the padding symbols and their markers
		sigOut[i] = constellation_marker[2];
	}
}
/*---------------------------------------------------------------------------------
 VlcMod::calcTotSymbol
 Compute how many symbols are to be used

 Parameters:
 	 nSymIn	- Number of mapped symbols (excluding markers)
 Return
 	 Number of total symbols out, including marker and padding symbols.
 TODO: change 7 to the vlcMod.nMarker - 1
 ---------------------------------------------------------------------------------*/
int VlcMod::calcTotSymbol(int nSymIn) {
	// compute how many symbols to be transmitted
	int nMarkerSym;
	if(nSymIn%7 == 0)
		nMarkerSym = nSymIn/7;
	else
		nMarkerSym = ceil(nSymIn/7.0);

	int nPadSymsLastMarker = nMarkerSym * 7 - nSymIn;

	int nSymValid = nMarkerSym * 8;
	int nSymTot;
	if(nSymValid%48==0)
		nSymTot = nSymValid;
	else
		nSymTot = ceil(nSymValid/48.0) * 48;
/*
	std::cout << "Entering VlcMod::calcTotSymbol" 
			<< "\nEncoded symbols = " << nSymIn
			<< "\nMarker symbols = " << nMarkerSym
			<< "\nPadding symbols for last marker = " << nPadSymsLastMarker
			<< "\nTotal symbols = " << nSymTot << std::endl;
*/
	return nSymTot;
}

/*---------------------------------------------------------------------------------
 VlcMod::demapper
 De-map the variable-length symbols into encoded 0/1 bits.

 Parameters:
 	 symIn 	 - Index of symbols
 	 bitsOut - '0'/'1' bit out
 	 nSymOut - Number of mapped symbols
 Return
 	 Number of output bits.
 ---------------------------------------------------------------------------------*/
int VlcMod::demapper(const int * symIn, char * bitsOut, int nSymOut) {
	std::string strConcat= "";
	for(int i=0;i<nSymOut;i++) {
		strConcat += codeword[(int) symIn[i]];
	}
	strcpy(bitsOut, strConcat.c_str());
	return strConcat.length();
}

//--- Definition of printTree()
void VlcMod::printTree(std::ostream & out, VlcMod::BinNodePointer root,
		int indent) {
	if (root != 0) {
		printTree(out, root->right, indent + 8);
		out << std::setw(indent) << " " << root->data << std::endl;
		printTree(out, root->left, indent + 8);
	}
}


/*---------------------------------------------------------------------------------
 						Self-defined functions
 ---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------
 generate_bits_vlc
 (in accordance to generate_bits() & reset_tail_bits() in utils.cc)
 
 Append 16-bit all-zero SERVICE field and 6-bit all-zero TAIL bits to the given psdu.
 This function doesn't consider padding bits.

 Parameters:
 	 psdu 	 	- psdu (char). One PSDU byte includes 8 bits.
 	 data_bits  - output array (char). Each byte represents a single bit 0/1
 	 psduSize 	- Number of PSDU size (in byte)
 Return
 	 None
 ---------------------------------------------------------------------------------*/
void generate_bits_vlc(const char *psdu, char *data_bits, int psduSize) {

	// first 16 bits are zero (SERVICE/DATA field)
	memset(data_bits, 0, 16);
	data_bits += 16;

	for(int i = 0; i < psduSize; i++) {
		for(int b = 0; b < 8; b++) {
			data_bits[i * 8 + b] = !!(psdu[i] & (1 << b));
		}
	}
	memset(data_bits+psduSize*8-6, 0, 6 * sizeof(char));	// reset tail bits
}

void convolutional_encoding_vlc(const char *in, char *out, int nBits) {

	int state = 0;

	for(int i = 0; i < nBits; i++) {
		assert(in[i] == 0 || in[i] == 1);
		state = ((state << 1) & 0x7e) | in[i];
		out[i * 2]     = ones(state & 0155) % 2;
		out[i * 2 + 1] = ones(state & 0117) % 2;
	}
}

/*---------------------------------------------------------------------------------
 dp_demod:
 MAP-based demodulation using dynamic programming
 Parameters:
 	 objIndex 	- objective summation of srcIndex
 	 srcIndex	- (1-D array) Index of symbols
 	 	 	  	  [2*i] stores the longer-code index of symbol i,
 	 	 	  	  [2*i+1] stores the shorter-code index of symbol i
 	 dist 		- (1-D array) Distance of each symbols
 	 	 	  	  [2*i] stores the error vector of longer-code symbol,
 	 	 	  	  [2*i+1] stores the error vector of shorter-code symbol
 	 nSym		- number of symbols
 	 nBackup 	- number of backup symbols per signal
 	 out		- (1-nSym array) decoded symbol or its index, which is depends on outFmt.
 	 outFmt		- 0: output the index of decoded symbol, 1: output decoded symbol
 Return
 	 Flag		- 0: success, 1: failed
 ---------------------------------------------------------------------------------*/
int dp_demod(int objIndex, const int *srcIndex, const double *dist, int nSym, int nBackup, int *out, bool outFmt) {
	// Create metric matrix and trace-back matrix
	double metricMat[nSym][objIndex+1];
	int tbMat[nSym][objIndex+1];

	double *metricMatptr = metricMat[0];
	int *tbMatptr = tbMat[0];
	int idxMin, idxMax;
	// std::cout << "Enter dp_demod" << std::endl;
	max_min(srcIndex, nBackup, idxMax, idxMin);
	// set the metric matrix to MAX
	for(int i=0;i<nSym;i++) {
		for(int j=0;j<objIndex+1;j++) {
			metricMat[i][j] = 10000;
			tbMat[i][j] = 0;
		}
	}
	// First symbol
	for(int i=0;i<nBackup;i++) {
		metricMat[0][srcIndex[i]] = dist[i];
	}
/*
	int iter = 0;
	std::cout << "\nIteration " << iter++ << std::endl;
	std::cout << "metricMat\n";
	print2DArray(metricMatptr, nSym, objIndex+1);
	std::cout << "tbMat\n";
	print2DArray(tbMatptr, nSym, objIndex+1);
*/
	int idx;
	double q;
	for(int i=1;i<nSym;i++) {
		int idxMinTemp, idxMaxTemp;
		max_min(srcIndex+i*nBackup, nBackup, idxMaxTemp, idxMinTemp);
		for(int j=idxMin+idxMinTemp; j<=std::min(idxMax+idxMaxTemp,objIndex);j++) {
			for(int k=0;k<nBackup;k++) {
				idx = i*nBackup + k;
				if(j>=srcIndex[idx]+1 && j-srcIndex[idx] <= objIndex && metricMat[i-1][j-srcIndex[idx]] != 10000) {
					q = dist[idx] + metricMat[i-1][j-srcIndex[idx]];
					if(q < metricMat[i][j]) {
						int tb = j-srcIndex[idx];
						metricMat[i][j] = q;
						tbMat[i][j] = tb;
					}
				}
			}
		}
		idxMin += idxMinTemp;
		idxMax += idxMaxTemp;
/*
		// print metricMat and tbMat
		std::cout << "\nIteration " << iter++ << std::endl;
		std::cout << "metricMat\n";
		print2DArray(metricMatptr, nSym, objIndex+1);
		std::cout << "tbMat\n";
		print2DArray(tbMatptr, nSym, objIndex+1);
*/

	}
	//Judge if the optimal solution is achievable --%%
	if(metricMat[nSym-1][objIndex] == 10000) {
		std::cerr <<"dp approach failed (noise is too strong) (dp_demod func)\n";
		std::cout << "Exit dp_demod without correct output" << std::endl;
		return 1;
	}
	int nOut[nSym];
	// traceback stage
	int tbLen = objIndex;
	for(int i=nSym-1;i>0;i--) {
		nOut[i] = tbLen - tbMat[i][tbLen];
		tbLen = tbMat[i][tbLen];
	}
	nOut[0] = tbLen;
	if(outFmt) {	// output the decoded symbol directly, otherwise, output its index
		for(int i=0;i<nSym;i++) {
			out[i] = nOut[i];
			// std::cout << out[i] << std::endl;
		}
		// std::cout << std::endl;
		// std::cout << "Exit dp_demod normally" << std::endl;
		return 0;
	}
	/*
	for(int i=0;i<nSym;i++) {
		std::cout << std::setw(3) << i << " - " << nOut[i] << std::endl;
	}
	std::cout << std::endl;
	*/
	// Decide the index of the decoded symbol in the given choices
	for(int i=0;i<nSym;i++) {
		for(int j=0;j<nBackup;j++) {
			int idxTmp = i*nBackup+j;
			if(nOut[i] == srcIndex[idxTmp]) {
				out[i] = j;
				break;
			}
		}
	}
	/*
	for(int i=0;i<nSym;i++) {
		std::cout << nOut[i] << " - " << out[i] << std::endl;
	}
	std::cout << std::endl;
	*/
	// std::cout << "Exit dp_demod normally" << std::endl;
	return 0;
}

/*---------------------------------------------------------------------------------
 max_min:
 Return the maximum and the minimum of the first n elements of an integer array
 Parameters:
 	 numbers 	- pointer to the integers to be compared
 	 n			- length of the integer array
 	 max 		- pointer to store the maximum
 	 min	 	- pointer to store the minimum
 Return
 	 None
 ---------------------------------------------------------------------------------*/
void max_min(const int *numbers, int n, int &max, int &min) {
	int max_tmp = 0;
	int min_tmp = 65535;

	for(int i=0;i<n;i++) {
		if(*(numbers+i) > max_tmp)
			max_tmp = numbers[i];
		if(*(numbers+i) < min_tmp)
			min_tmp = numbers[i];
	}
	max = max_tmp;
	min = min_tmp;
}
