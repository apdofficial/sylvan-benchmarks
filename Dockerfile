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

# add paths
ENV HOME=/home

# DMPC
ENV DMPC=$HOME/dpmc
ENV ADDMC=$DMPC/addmc
ENV DMC=$DMPC/dmc
ENV ADDMC_COLAMD=$ADDMC/libraries/colamd
ENV ADDMC_LIBS=$ADDMC/libraries
ENV SUITE_SPARSE=$ADDMC_LIBS/SuiteSparse

#Sylvan
ENV SYLVAN=$HOME/sylvan
ENV ADDMC_SYLVAN=$ADDMC/libraries/sylvan/

#build
ENV BUILD=$HOME/build

# copy source files
COPY . $HOME/

#replace sylvan
# RUN rm -r $ADDMC_SYLVAN
# RUN cp -r $SYLVAN $ADDMC_SYLVAN

# compile the colamd library and copy it into the addmc library directory
RUN cd $SUITE_SPARSE/SuiteSparse_config/ && make -s
RUN rm -f $ADDMC_COLAMD/libcolamd.a
RUN cd $SUITE_SPARSE/COLAMD/ && make -s
RUN cp $SUITE_SPARSE/COLAMD/Lib/libcolamd.a $ADDMC_COLAMD/

# create build directory
RUN cd $HOME && mkdir build

# set build directory as working directory
WORKDIR $BUILD

# compile the dmc executable
RUN cd $DMC/ && make dmc

# copy the dmc executable to the build directory
RUN cp $DMC/dmc $BUILD