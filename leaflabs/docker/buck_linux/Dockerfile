#
# Build a Docker image with buck for building under linux
# Update the container image by running:
# $ docker build -t registry.gitlab.leaflabs.com/bldg8/ovrsource/buck-linux:[buck_release] .
#
# Update the container registry by running:
# $ docker push registry.gitlab.leaflabs.com/bldg8/ovrsource/buck-linux:[buck_release]
#

FROM ubuntu:18.04

ARG buck_release=2021.01.12.01

# Download & install buck package & its depends + recommends 
RUN apt-get update \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y -q curl \
  && curl -LO https://github.com/facebook/buck/releases/download/v${buck_release}/buck.${buck_release}_all.deb \
  && DEBIAN_FRONTEND=noninteractive apt-get install -y -q ./buck.${buck_release}_all.deb \
  && rm -rf buck.${buck_release}_all.deb /var/lib/apt/lists/*

# Install our build tools & locale-gen
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y -q \
  build-essential \
  python3 \
  python3-pip \
  clang \
  locales \
  doxygen \
  && rm -rf /var/lib/apt/lists/*

# Install documentation generation tools
RUN pip3 install sphinx sphinx_rtd_theme breathe

# Set locale
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8

# Login as non-root user (uid & gid are 1000)
RUN useradd -m user
WORKDIR /home/user
USER user
