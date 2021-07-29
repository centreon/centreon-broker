FROM centos:7
RUN yum -y update && \
    yum -y install cmake && \
    yum clean all
WORKDIR /src
ENTRYPOINT [ "/src/centreon-collect/cmake.sh"]