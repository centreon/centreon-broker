# Centreon Broker

Centreon Broker is an extensible open-source monitoring event
transmitter (broker). It is a low-level component of the
[Centreon software suite](https://www.centreon.com).

Centreon Broker is released under the Apache License, Version 2.0
and is endorsed by the [Centreon company](https://www.centreon.com).

Centreon Broker is the communication backbone of the Centreon software
suite so most events are processed by one or more of its module.
Centreon Broker has multiple modules that perform specific tasks. The
list below describes the most common of them.

- SQL: store real-time monitoring events in a SQL database
- storage: parse and store performance data in a SQL database
- RRD: write RRD graph files from monitoring performance data
- BAM: compute Business Activity status and availability
- Graphite: write monitoring performance data to Graphite
- InfluxDB: write monitoring performance data to InfluxDB

Centreon Broker is extremely fast and is a credible alternative to the
old NDOutils. It is also extremly modular and can fit most network
security requirements. Just give it a try!

## Documentation

*Coming soon on https://docs.centreon.com*

## Installing from binaries

> Centreon Broker is a low-level component of the Centreon
> software suite. If this is your first installation you would probably
> want to [install it entirely](https://docs.centreon.com/current/en/installation/installation-of-a-central-server/using-sources.html).

Centreon ([the company behind the Centreon software suite](http://www.centreon.com))
provides binary packages for RedHat / CentOS. They are available either
as part of the [Centreon Platform](https://www.centreon.com/en/platform/)
or as individual packages on [our RPM repository](https://docs.centreon.com/current/en/installation/installation-of-a-poller/using-packages.html).

Once the repository installed a simple command will be needed to install
Centreon Broker.

```shell
yum install centreon-broker
```

## Fetching sources

Beware that the repository hosts in-development sources and that it
might not work at all.

Stable releases are available as gziped tarballs on [Centreon's
download site](https://download.centreon.com).

## Compilation

This paragraph is only a quickstart guide for the compilation of
Centreon Broker.

### CentOS / Debian / Raspbian

Compilation of these distributions is pretty straightforward.

You'll need to download the project and launch the *cmake.sh* script
to prepare the compilation environment:

```shell
git clone https://github.com/centreon/centreon-broker
cd centreon-broker
./cmake.sh
```

Now launch the compilation using the *make* command and then install the
software by running *make install* as priviledged user:

```shell
cd build
make
make install
```

### Other distributions

If you are on another distribution, then follow the steps below.

Check if you have these packages installed (Note that packages names
come from CentOS distributions, so if some packages names don't match
on your distribution try to find their equivalent names): git, make,
cmake, gcc-c++, python3, libgnutls-devel, liblua-devel, librrd-devel.

For the projet compilation you need to have conan installed. Try to use
the package manager given by your OS to install conan. ('apt' for
Debian, 'rpm' for Red Hat, 'pacman' for Arch Linux, ...). It is prefered
to install gcc before conan.

Example :

```shell
apt install conan
```

If it does not work, conan can be installed with pip3:

```shell
pip3 install conan
```

> All the dependencies pulled by conan are located in conanfile.txt. If
> you want to use a dependency from your package manager instead of conan,
> you need to remove it from conanfile.txt.

Then you have to add a remote conan repository, for that enter the command:

```shell
conan remote add centreon https://api.bintray.com/conan/centreon/centreon
```

Now, the list command should list two repositories:

```shell
conan remote list
conan-center: https://conan.bintray.com [Verify SSL: True]
centreon: https://api.bintray.com/conan/centreon/centreon [Verify SSL: True]
```

You can now prepare the compilation environment:

```shell
git clone https://github.com/centreon/centreon-broker
mkdir -p centreon-broker/build
cd centreon-broker/build
conan install --remote centreon --build missing ..
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios ..
```

This will look for required dependencies and print a summary of the
compilation parameters if everything went fine.

Now launch the compilation using the *make* command and then install the
software by running *make install* as priviledged user:

```shell
make
make install
```

Normally if all compiles, you have finished installing broker. But if
you want, you can also check it. Always from the *build* directory you
can execute this command:

```shell
test/ut
```

You're done!

## Bug reports / Feature requests

The best way to report a bug or to request a feature is to open an issue
in GitHub's [issue tracker](https://github.com/centreon/centreon-broker/issues/).

Please note that Centreon Broker follows the
[same workflow as Centreon](https://github.com/centreon/centreon/issues/new/choose)
to process issues.

For a quick resolution of a bug your message should contain:

- The problem description
- Precise steps on how to reproduce the issue (if you're using Centreon
  web UI tell us where you click)
- The expected behavior
- The Centreon product**s** version**s**
- The operating system you're using (name and version)
- If possible configuration, log and debug files

## Contributing

Contributions are much welcome! If possible provide them as
pull-requests on GitHub. If not, patches will do but describe against
which version/commit they apply.

For any question or remark feel free to send a mail to the project
maintainers:

<a href="https://github.com/bouda1"><img src="https://avatars1.githubusercontent.com/u/6324413?s=400&v=4" title="David Boucher" width="80" height="80"></a> &nbsp;
<a href="https://github.com/rem31"><img src="https://avatars.githubusercontent.com/u/73845199?s=460&v=4" title="Rémi Gres" width="80" height="80"></a> &nbsp;
<a href="https://github.com/centreonpm"><img src="https://avatars.githubusercontent.com/u/73105891?s=460&v=4" title="Pierre Margale" width="80" height="80"></a> &nbsp;
