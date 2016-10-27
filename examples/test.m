packet_full = read_complex_binary("full_packet.txt");
packet_header = read_complex_binary("packet_header.txt");
packet_data = read_complex_binary("packet_data.txt");
size(packet_full)
size(packet_header)
size(packet_data)

figure
plot(packet_data,'b')
hold on
plot(packet_full,'r')
x = packet_data-packet_full;