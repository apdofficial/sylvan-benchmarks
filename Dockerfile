FROM ubuntu:23.04

# this is for timezone config
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Amsterdam
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# install packages
RUN apt-get update
RUN apt-get install -y git
RUN apt-get install -y apt-utils
RUN apt-get install -y build-essential
RUN apt-get install -y libboost-all-dev
RUN apt-get install -y liblapack-dev libblas-dev
RUN apt-get install -y automake
RUN apt-get install -y cmake
RUN apt-get install -y g++ m4 zlib1g-dev make p7zip
RUN apt-get install -y libgmp-dev
RUN apt-get install -y libsqlite3-dev

# add paths
ENV HOME=/home

ENV DMPC=$HOME/dmpc
ENV ADDMC=$DMPC/addmc
ENV DMC=$DMPC/dmc

ENV ADDMC_COLAMD=$ADDMC/libraries/colamd
ENV SUITE_SPARSE=$ADDMC/libraries/SuiteSparse-4.0.2

# copy source files
COPY ./ $HOME/

# compile the colamd library and copy it into the addmc library directory
RUN cd $SUITE_SPARSE/SuiteSparse_config/ && make -s
RUN rm -f $ADDMC_COLAMD/libcolamd.a
RUN cd $SUITE_SPARSE/COLAMD/ && make -s
RUN cp $SUITE_SPARSE/COLAMD/Lib/libcolamd.a $ADDMC_COLAMD/

WORKDIR $DMC

# compile the dmc executable
RUN make -C $ADDMC/ dmc opt=-Ofast link=-static