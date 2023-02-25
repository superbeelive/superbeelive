From ubuntu:latest
LABEL maintainer="Sebastien Druon (sebastien.druon@umontpellier.fr)"
RUN apt-get update \
	&& apt-get install -y libaravis-dev libaravis-0.8 aravis-tools-cli\
	&& apt-get install -y make \
	&& apt-get install -y gcc \
	&& apt-get install -y binutils \
	&& apt-get install -y autoconf \
	&& apt-get install -y automake \
	&& apt-get install -y libtool \
	&& apt-get install -y pkg-config \
	&& apt-get install -y git \
	&& apt-get install -y file \
	&& apt-get install -y patch \
	&& rm -fr /var/lib/apt/lists/*
WORKDIR /root
CMD /bin/bash
