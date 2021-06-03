# Changelog

## 21.04.2

### Bug fix

*TLS*

GnuTLS query was not understood on RedHat 8 and Centos8.

*BBDO serialized events*

Converts theses events into trace.

*tcp*

Connections can fail when many pollers establish connection to cbd. This should
be fixed with this new version.

When cbd is stopped, sometimes centengine cannot reconnect. This is fixed now.

### Build

repair the compilation for Raspberry PI

### Enhancements

*Parser*

Add unit test for the parser, while it is reading configuration file with loggers at null
value.

*Timeranges*

Allowing parsing \r or \n for the parser of timeranges.

## 21.04.1

`Date of the release`

### Bug fixes

*Factories*

If we want two outputs configured with tls but with different configurations,
this was impossible to configure, because the factory can only store one
configuration. This is fixed now.

*Build*

Since the bintray closure, we could not build cbd. Dependencies have been moved
to the conan-center and the build is back.

*Bam*

Meta-services are removed from broker. This code was not used and could produce

### Enhancements

*Storage*

We optimize queries by removing useless join. A join with hosts table is not
necessary in certain queries.

## 21.04.0

`2021 april 20th`

### Enhancements

*Broker exceptions*

They are all based on a small msg\_fmt exception that is lighter than the old
msg one.
