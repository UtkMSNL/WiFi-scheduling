Hi,

This project evaluates the efficiency of wireless network scheduling. In this project, WiFi device notifies its neighbors with its new data rate after adapting its data rate to the instantaneous channel condition. The neighbors set their own backoff window time accordingly, in order to maximize the overall network throughput.

This project is built upon GNURadio/USRP platform. The project implements the complete IEEE802.11 protocol, which includes PHY (Based on 2), MAC (Based on 3)), and rate adaptation approaches. The rate adaptation approaches includes Minstrel and Adaptive Auto Rate Fallback (AARF).
 
To use this project, please cite the following literatures:

1) @inproceedings{lu2016scheduling,
  title={Scheduling Dynamic Wireless Networks with Limited Operations},
  author={Haoyang Lu and Wei Gao},
  booktitle={IEEE International Conference on Network Protocols},
  year={2016},
  organization={IEEE}
}

2) @inproceedings{bloessl2013ieee,
  title={An IEEE 802.11 a/g/p OFDM Receiver for GNU Radio},
  author={Bloessl, Bastian and Segata, Michele and Sommer, Christoph and Dressler, Falko},
  booktitle={Proceedings of the second workshop on Software radio implementation forum},
  pages={9--16},
  year={2013},
  organization={ACM}
}

3) @inproceedings{gutierrez2010ieee,
  title={An IEEE 802.11 MAC Software Defined Radio implementation for experimental wireless communications and networking research},
  author={Gutierrez-Agullo, Juan R and Coll-Perales, Baldomero and Gozalvez, Javier},
  booktitle={Wireless Days (WD), 2010 IFIP},
  pages={1--5},
  year={2010},
  organization={IEEE}
}
# Installation
For installation of GNURadio and library, please refer to (https://github.com/bastibl/gr-ieee802-11/).

# Further information
For details of the project, please refer to the paper 1). If you have any further questions, please contact
Haoyang Lu, (hlu9@utk.edu)
University of Tennessee
