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
RUN apt install -y libboost-all-dev
RUN apt install -y libgmp-dev
RUN apt install -y libsqlite3-dev
RUN apt install -y gdb
RUN apt install -y git
RUN apt-add-repository ppa:deadsnakes/ppa 
RUN apt update
RUN apt install -y python3.11 
RUN apt install -y python3-pip 

RUN ln -s /usr/bin/python3.11 /usr/bin/python
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 1

# add paths
ENV HOME=/home
ENV BUILD=$HOME/build

# copy source files
COPY . $HOME/

# create build directory
RUN cd $HOME && mkdir build

WORKDIR $HOME

RUN chmod +x run_benchmarks.sh

# RUN cd $HOME && pip install -r requirements.txt

#Sylvan
ENV SYLVAN=$HOME/sylvan
ENV ADDMC_SYLVAN=$ADDMC/libraries/sylvan/

# --------- DPMC ---------
ENV DMPC=$HOME/dpmc
ENV ADDMC=$DMPC/addmc
ENV DMC=$DMPC/dmc
ENV ADDMC_COLAMD=$ADDMC/libraries/colamd
ENV ADDMC_LIBS=$ADDMC/libraries
ENV SUITE_SPARSE=$ADDMC_LIBS/SuiteSparse
ENV DMPC_WEIGHTED_TESTS=$DMPC/tests/weighted
ENV DMPC_UNWEIGHTED_TESTS=$DMPC/tests/unweighted
ENV LG=$DMPC/lg
ENV LG_SOLVERS=$LG/solvers
ENV HTD_SOLVER=$LG_SOLVERS/htd-master
ENV DPMC_BUILD=$HOME/build/dpmc
RUN cd $BUILD && mkdir dpmc

# replace sylvan (uncomment when sylvan contains the missing functions added in dpmc)
# RUN rm -r $ADDMC_SYLVAN
# RUN cp -r $SYLVAN $ADDMC_SYLVAN

# compile the colamd library and copy it into the addmc library directory
RUN cd $SUITE_SPARSE/SuiteSparse_config/ && make -s
RUN rm -f $ADDMC_COLAMD/libcolamd.a
RUN cd $SUITE_SPARSE/COLAMD/ && make -s
RUN cp $SUITE_SPARSE/COLAMD/Lib/libcolamd.a $ADDMC_COLAMD/

# prepare the dmc executable
RUN cd $DMC/ && make dmc -j 8
RUN cp $DMC/dmc $DPMC_BUILD

# prepare the lg executable
RUN cd $LG/ && make -j 8
RUN cp $LG/build/lg $DPMC_BUILD

# prepare the htd_main-1.2.0 executable
RUN cd $HTD_SOLVER && mkdir build
RUN cd $HTD_SOLVER/build && cmake .. && make -j 8
RUN cp $HTD_SOLVER/build/bin/htd_main-1.2.0 $DPMC_BUILD

RUN cp $DMPC_WEIGHTED_TESTS/mcc21__wff.3.75.315.cnf $DPMC_BUILD

# --------- STORM ---------
ENV STORM_BUILD=$HOME/build/storm
RUN cd $BUILD && mkdir storm

# TODO:
# replace the STORM Sylvan source with the one in the sylvan folder
# compile the STORM executable
# copy the input data into the STORM_BUILD folder

# --------- Safety Synthesis ---------
ENV SAFETY_SYNT=$HOME/safety_synthesis
ENV SAFETY_SYNT_BUILD=$HOME/build/safety_synthesis

RUN cd $BUILD && mkdir safety_synthesis
RUN cd $SAFETY_SYNT && mkdir build

# prepare the safety_synthesis executable
RUN cd $SAFETY_SYNT/build && cmake ../../ && make -j 8
RUN cp $SAFETY_SYNT/build/safety_synthesis/aiger_synt $SAFETY_SYNT_BUILD
RUN cp $SAFETY_SYNT/models/add10n.aag $SAFETY_SYNT_BUILD