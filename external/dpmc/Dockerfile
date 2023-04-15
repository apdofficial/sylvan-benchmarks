FROM ubuntu:20.04

# this is for timezone config
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Amsterdam
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# install packages
RUN apt-get update
RUN apt-get install -y apt-utils
RUN apt-get install -y build-essential
RUN apt-get install -y libboost-all-dev
RUN apt-get install -y liblapack-dev libblas-dev
RUN apt-get install -y automake
RUN apt-get install -y cmake
RUN apt-get install -y make 
RUN apt-get install -y g++-10 
RUN apt-get install -y libgmp-dev
RUN apt-get install -y libsqlite3-dev
RUN apt-get install -y gdb
RUN apt-get install -y git

RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 1

# set environment variables
ENV HOME=/home

ENV DPMC=$HOME
ENV ADDMC=$DPMC/addmc
ENV DMC=$DPMC/dmc

ENV ADDMC_LIBS=$ADDMC/libraries
ENV SUITE_SPARSE=$ADDMC_LIBS/SuiteSparse

# copy source files
COPY ./ $HOME/

# compile the colamd library and copy it into the addmc library directory
RUN cd $SUITE_SPARSE/SuiteSparse_config/ && make -s
RUN rm -f $ADDMC_LIBS/colamd/libcolamd.a
RUN cd $SUITE_SPARSE/COLAMD/ && make -s
RUN cp $SUITE_SPARSE/COLAMD/Lib/libcolamd.a $ADDMC_LIBS/colamd/

# compile the dmc executable
RUN cd $DMC/ && make dmc

# set the working directory to the dmc directory
WORKDIR $DMC

