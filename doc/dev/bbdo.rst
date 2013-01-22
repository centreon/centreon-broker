#################
The BBDO protocol
#################

The BBDO protocol has been created to be the default protocol of
Centreon Broker. It is lightweight on the wire and easy to decode. It is
especially designed the for monitoring area of Centreon Broker.

************
Introduction
************

BBDO stands for Broker Binary Data Object. BBDO is designed to transfer
"data packets" from a node to another. These "data packets" are most of
the time monitoring information provided by the monitoring engine (eg.
Centreon Engine or Nagios). It uses raw binary values which allows it to
consume very few memory.

*************
Packet format
*************

The packets format of Centreon Broker introduce only 8 bytes of header
to transmit each monitoring event (usually about 100-200 bytes each).
Fields are provided in the big endian format.

========= ============ =====================================
Field     Size (bytes) Description
========= ============ =====================================
checksum  2            CRC-16-CCITT of size and id.
size      2            Size of the packet, including header.
id        4            ID of the event.
data      size         Payload data.
========= ============ =====================================
