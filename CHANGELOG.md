# Changelog
## 21.04.2

### Bam

*meta-services*

Meta-services are removed from broker. This code was not used and could produce

## 21.04.1

`Date of the release`

### Enhancements

*Storage*

We optimize queries by removing useless join. A join with hosts table is not necessary in 
certain queries.

## 21.04.0

`2021 april 20th`

### Enhancements

*Broker exceptions*

They are all based on a small msg\_fmt exception that is lighter than the old
msg one.
