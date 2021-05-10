# Changelog

## 21.04.1

`Date of the release`

### Bug fixes

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
