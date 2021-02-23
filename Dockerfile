FROM ubuntu:16.04
WORKDIR /ardupilot

RUN useradd -U -d /ardupilot ardupilot && \
    usermod -G users ardupilot

RUN apt-get update -y
RUN apt-get upgrade -y
RUN apt-get install sudo -y
RUN apt-get install python -y
RUN apt-get install python-pip -y
RUN apt-get install wget -y
RUN apt-get install lsb-release -y

# Add gcc-arm-non-eabi
ADD install_gcc_arm_none_eabi.sh /ardupilot
RUN /ardupilot/install_gcc_arm_none_eabi.sh

ENV PATH="/opt/gcc-arm-none-eabi-5_4-2016q2/bin:/usr/lib/ccache:/ardupilot/Tools:${PATH}"
ENV USER=ardupilot

# Add other prereqs
RUN chown -R ardupilot:ardupilot /ardupilot
ADD Tools/scripts/install-prereqs-ubuntu.sh /ardupilot
RUN /ardupilot/install-prereqs-ubuntu.sh -y

RUN apt-get clean
RUN rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

CMD