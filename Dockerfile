FROM ubuntu:20.04

# this is for timezone config
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Amsterdam
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# install packages
RUN apt update
RUN apt install -y apt-utils
RUN apt install -y build-essential
RUN apt install -y software-properties-common
RUN apt install -y liblapack-dev libblas-dev
RUN apt install -y automake
RUN apt install -y cmake-data
RUN apt install -y cmake
RUN apt install -y make 
RUN apt install -y g++-10
RUN apt install -y gcc-10
RUN apt install -y libboost-all-dev
RUN apt install -y libgmp-dev
RUN apt install -y libsqlite3-dev
RUN apt install -y gdb
RUN apt install -y git
RUN apt install -y texlive texlive-latex-extra
RUN apt-add-repository ppa:deadsnakes/ppa 
RUN apt update
RUN apt install -y python3.11 
RUN apt install -y python3-pip 

RUN ln -s /usr/bin/python3.11 /usr/bin/python
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 1
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 1

# add paths
ENV HOME=/home
ENV BUILD=$HOME/build
ENV SCRIPTS=$HOME/scripts

# copy source files
COPY . $HOME/

WORKDIR $HOME

RUN mkdir build

#RUN pip install -r requirements.txt

RUN cd $SCRIPTS && chmod +x run_all.sh
RUN cd $SCRIPTS && chmod +x build_dmc.sh
RUN cd $SCRIPTS && chmod +x run_dpmc.sh
RUN cd $SCRIPTS && chmod +x build_safety_synth.sh
RUN cd $SCRIPTS && chmod +x run_safety_synth.sh
RUN cd $SCRIPTS && chmod +x run_storm.sh
RUN cd $SCRIPTS && chmod +x run_cmp_hashmap.sh

ENV CUDD=$HOME/external/cudd
ENV DPMC=$HOME/external/dpmc
ENV SYLVAN=$HOME/external/sylvan
ENV STORM=$HOME/external/storm

# --------- STORM ---------
ENV STORM_BUILD=$HOME/build/storm
RUN cd $BUILD && mkdir storm

# --------- DPMC ---------
ENV DMPC=$HOME/dpmc
ENV ADDMC=$DPMC/addmc
ENV DMC=$DPMC/dmc
ENV ADDMC_COLAMD=$ADDMC/libraries/colamd
ENV ADDMC_LIBS=$ADDMC/libraries
ENV ADDMC_SYLVAN=$ADDMC/libraries/sylvan/
ENV SUITE_SPARSE=$ADDMC_LIBS/SuiteSparse
ENV DMPC_TESTS=$DPMC/tests
ENV DMPC_WEIGHTED_TESTS=$DMPC_TESTS/weighted
ENV DMPC_UNWEIGHTED_TESTS=$DMPC_TESTS/unweighted
ENV LG=$DPMC/lg
ENV LG_SOLVERS=$LG/solvers
ENV HTD_SOLVER=$LG_SOLVERS/htd-master
ENV DPMC_BUILD=$HOME/build/dpmc
RUN cd $BUILD && mkdir dpmc

# compile the colamd library and copy it into the addmc library directory
RUN cd $SUITE_SPARSE/SuiteSparse_config/ && make -s
RUN rm -f $ADDMC_COLAMD/libcolamd.a
RUN cd $SUITE_SPARSE/COLAMD/ && make -s
RUN cp $SUITE_SPARSE/COLAMD/Lib/libcolamd.a $ADDMC_COLAMD/

# --------- Safety Synthesis ---------
ENV SAFETY_SYNT=$HOME/safety_synthesis
ENV SAFETY_SYNT_BUILD=$HOME/build/safety_synthesis
ENV SAFETY_SYNT_MODELS=$SAFETY_SYNT/models
RUN cd $BUILD && mkdir safety_synthesis

# --------- cmp hashmap ---------
ENV CMP_HASHMAP=$HOME/cmp_hashmap
ENV CMP_HASHMAP_BUILD=$HOME/build/cmp_hashmap
RUN cd $BUILD && mkdir cmp_hashmap
