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
Centreon Engine or Nagios). It uses mostly raw binary values which
allows it to consume very few memory.

.. _dev_bbdo_types:

*****
Types
*****

As a binary protocol, BBDO uses data types to serialize data. They are
written in a Big Endian format and described in the following table.

============= =========================== ============
Type          Representation              Size (bytes)
============= =========================== ============
integer       binary                      4
short integer binary                      2
long integer  binary                      8
time          binary (timestamp)          8
boolean       binary (0 is false,         1
              everything else is true)
string        nul-terminated UTF-8 string variable
real          nul-terminated UTF-8 string variable
              (either in fixed (2013) or
              scientific (2.013e+3)
              format)
============= =========================== ============

.. _dev_bbdo_packet_format:

*************
Packet format
*************

The packets format of Centreon Broker introduce only 8 bytes of header
to transmit each monitoring event (usually about 100-200 bytes each).
Fields are provided in the big endian format.

========= ====================== =====================================
Field     Type                   Description
========= ====================== =====================================
checksum  unsigned short integer CRC-16-CCITT X.25 of size and id.
                                 The checksum can be used to recover
                                 from an incomplete data packet sent
                                 in the stream by dropping bytes one
                                 by one.
size      unsigned short integer Size of the packet, excluding header.
id        unsigned integer       ID of the event.
data                             Payload data.
========= ====================== =====================================

Packet ID
=========

As seen in the previous paragraph, every packet holds an ID that express
by itself how data is encoded. This ID can be splitted in two 16-bits
components. The 16 most significant bits are the event category and the
16 least significant bits the event type.

The event categories serialize events properties one after the other, so
order is very important to not loose track when unserializing events.

.. _dev_bbdo_event_categories:

****************
Event categories
****************

The current available categories are described in the table below.

=========== ===================== ===== ================================
Category    API macro             Value Description
=========== ===================== ===== ================================
NEB         BBDO_NEB_TYPE         1     Classical monitoring events
                                        (hosts, services, notifications,
                                        event handlers, plugin
                                        execution, ...).
Storage     BBDO_STORAGE_TYPE     2     Category related to RRD graph
                                        building.
Correlation BBDO_CORRELATION_TYPE 3     Status correlation.
Internal    BBDO_INTERNAL_TYPE    65535 Reserved for internal protocol
                                        use.
=========== ===================== ===== ================================

NEB
===

The table below lists event types available in the NEB category. They
have to be mixed with the BBDO_NEB_TYPE category to get a BBDO event ID.

====================== =====
Type                   Value
====================== =====
Acknowledgement        1
Comment                2
Custom variable        3
Custom variable status 4
Downtime               5
Event handler          6
Flapping status        7
Host                   8
Host check             9
Host dependency        10
Host group             11
Host group member      12
Host parent            13
Host status            14
Instance               15
Instance status        16
Log entry              17
Module                 18
Notification           19
Service                20
Service check          21
Service dependency     22
Service group          23
Service group member   24
Service status         25
====================== =====

Storage
=======

The table below lists event types available in the Storage category.
They have to be mixed with the BBDO_STORAGE_TYPE category to get a BBDO
event ID.

============ =====
Type         Value
============ =====
metric       1
rebuild      2
remove_graph 3
status       4
============ =====

Correlation
===========

The table below lists event types available in the Correlation category.
They have to be mixed with the BBDO_CORRELATION_TYPE category to get a
BBDO event ID.

============= =====
Type          Value
============= =====
engine_state  1
host_state    2
issue         3
issue_parent  4
service_state 5
============= =====

Internal
========

The table below lists event types available in the Internal category.
They have to be mixed with the BBDO_INTERNAL_TYPE category to get a BBDO
event ID.

================ =====
Type             Value
================ =====
version_response 1
================ =====

*******************
Event serialization
*******************

Most events listed in each
:ref:`event category <dev_bbdo_event_categories>` have a mapping used to
serialize their content. Indeed their content is directly serialized in
the :ref:`packet payload data <dev_bbdo_packet_format>`, one field after
the other in the order described in the
:ref:`mapping tables <dev_mapping>`. They are encoded following rules
described in the :ref:`types paragraph <dev_bbdo_types>`.

*******
Example
*******

Let's take an example and see how an *host check event* gets sent in a
packet. Its mapping is as follow :

===================== ================ =================================
Property              Type             Value in example
===================== ================ =================================
active_checks_enabled boolean          True.
check_type            short integer    0 (active host check).
host_id               unsigned integer 42
next_check            time             1365080225
command_line          string           ./my_plugin -H 127.0.0.1
===================== ================ =================================

And gives the following packet with values in hexadecimal.

::

  +-----------------+-----------------+-----------------------------------+
  |      CRC16      |      SIZE       |                ID                 |
  +========+========+========+========+========+========+========+========+
  |   27   |   33   |   00   |   28   |   00   |   01   |   00   |   09   |
  +--------+--------+--------+--------+--------+--------+--------+--------+

  +--------+-----------------+-----------------------------------+--------
  | active_|                 |                                   |
  | checks_|    check_type   |              host_id              |    =>
  | enabled|                 |                                   |
  +========+========+========+========+==========================+========+
  |   01   |   00   |   00   |   00   |   00   |   00   |   2A   |   00   |
  +--------+--------+--------+--------+--------+--------+--------+--------+

   --------------------------+--------------------------------------------
                             =>  next_check                      |    =>
  +========+========+========+========+========+========+========+========+
  |   00   |   00   |   00   |   51   |   5D   |   78   |   A1   |   2E   |
  +--------+--------+--------+--------+--------+--------+--------+--------+

   -----------------------------------------------------------------------
                             => command_line =>
  +========+========+========+========+========+========+========+========+
  |   2F   |   6D   |   79   |   5F   |   70   |   6C   |   75   |   67   |
  +--------+--------+--------+--------+--------+--------+--------+--------+

   -----------------------------------------------------------------------
                             => command_line =>
  +========+========+========+========+========+========+========+========+
  |   69   |   6E   |   20   |   2D   |   48   |   20   |   31   |   32   |
  +--------+--------+--------+--------+--------+--------+--------+--------+

   -----------------------------------------------------------------------+
                             => command_line                              |
  +========+========+========+========+========+========+========+========+
  |   37   |   2E   |   30   |   2E   |   30   |   2E   |   31   |   00   |
  +--------+--------+--------+--------+--------+--------+--------+--------+

************************
Connection establishment
************************

BBDO is a protocol which can negociate features. When establishing a
connection, a *version_response* packet is sent by the client. It
provides its supported BBDO protocol version and extensions. The server
replies to this message with another *version_response* packet
containing its own supported protocol version and extensions. If
protocol versions match, then starts the extensions negociation.

Currently two extensions are supported : *TLS* and *compression*. Right
after the *version_response* packet, each peer search in the other
peer's extension list the extensions it supports. When one is found, it
is enabled (ie. it immediately starts).

You can find more details in the :ref:`TLS module documentation <user_modules_tls>`
and the :ref:`compression module documentation <user_modules_compression>`.

Example
=======

Let's have C the client and S the server. The following steps are
performed sequentially.

  - C initiates a TCP connection with S and connection gets established
  - C sends a *version_response* packet with the following attributes
    - protocol major : 1
    - protocol minor : 0
    - protocol patch : 0
    - extensions : "TLS compression"
  - S sends its own *version_response* packet in reply to C's
    - protocol major : 1
    - protocol minor : 0
    - protocol patch : 0
    - extensions : "TLS compression"
  - C and S determines which extensions they have in common (here TLS
    and compression)
  - if order is important, extensions are applied in the order provided
    by the server
  - TLS connection is initiated, handshake performed, ...
  - compression connection is opened
  - now data transmitted between C and S is both encrypted and
    compressed !
