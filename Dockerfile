FROM centos:latest as base
MAINTAINER Centreon "support@centreon.com"

RUN curl https://downloads.mariadb.com/MariaDB/mariadb-10.1.19/yum/centos7-amd64/rpms/MariaDB-10.1.19-centos7-x86_64-devel.rpm --output /tmp/MariaDB-10.1.19-centos7-x86_64-devel.rpm
RUN curl https://downloads.mariadb.com/MariaDB/mariadb-10.1.19/yum/centos7-amd64/rpms/MariaDB-10.1.19-centos7-x86_64-common.rpm --output /tmp/MariaDB-10.1.19-centos7-x86_64-common.rpm
RUN rpm -ivh /tmp/MariaDB-10.1.19-centos7-x86_64-*.rpm
RUN yum -y install gcc-c++ cmake make lua-devel qt-devel gnutls-devel rrdtool-devel zlib-devel openssl-devel

FROM base as centreon-broker
COPY . /usr/src/centreon-broker
WORKDIR /usr/src/centreon-broker/build
RUN cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_MODULE_SIMU=On .
RUN make -j5
ENTRYPOINT ["top", "-b"]
