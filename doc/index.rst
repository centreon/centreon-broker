.. _top:

Welcome to Centreon Broker's documentation!
===========================================

Centreon Broker offers a new effective way to store your Nagios events
in a database. With its flexible core engine, you may now decide exactly
how your data stream over your network. These transfers are also
extremly fast.
You will be able to set up an auxiliary database which will only monitor
a part of your IT infrastructure.
By setting up fail-overs that will be activated when the primary Nagios
database is down, you will be in a position to guarantee your Centreon
users with no data loss.

Centreon Broker is software designed to convey monitoring events from a
point A to a point B. This abstract definition is due to the very
versatile nature of Centreon Broker which merely acts as a data
multiplexer. However on most cases, users will use Centreon Broker as a
tool to push in a database monitoring events originating from their
monitoring engine (Centreon Engine, Nagios ...).

The standard Centreon Broker comes with a set of modules that provide
basic communication features such as TCP streams or database insertion.

Contents:

.. toctree::
   :maxdepth: 2
   
   release_notes/index
   installation/index
   user/index
   exploit/index
   dev/index
