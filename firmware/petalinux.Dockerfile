FROM ubuntu:18.04 as petalinux_base

# Inspired by https://github.com/z4yx/petalinux-docker and https://github.com/matthieu-labas/docker-petalinux
# build with docker build -f petalinux.Dockerfile -t petalinux:2019.2 .

#install dependences:
RUN dpkg --add-architecture i386 && apt-get update &&  DEBIAN_FRONTEND=noninteractive apt-get install -y -q \
  sudo \
  locales \
  gawk \
  python \
  build-essential \
  gcc \
  git \
  make \
  net-tools \
  libncurses5-dev \
  tftpd \
  zlib1g:i386 \
  libssl-dev \
  flex \
  bison \
  libselinux1 \
  gnupg \
  wget \
  diffstat \
  chrpath \
  socat \
  xterm \
  autoconf \
  libtool \
  tar \
  unzip \
  texinfo \
  zlib1g-dev \
  gcc-multilib \
  build-essential \
  libsdl1.2-dev \
  libglib2.0-dev \
  screen \
  pax \
  gzip \
  cpio \
  rsync \
  lsb-release \
  xvfb \
  && rm -rf /var/lib/apt/lists/*  

# Set locale
RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8

# Add user 'user' with password 'user'
RUN useradd -m -G sudo,dialout -p '$6$zVcRjbcN1hLM4f0z$L.Pl85Anx4owhIBBf1HgpcSLLl3N6JqlYVDvIN1QmIkGJIhaddkE0uXjPbiQ01T68dmhQVfJoV57m7Y4cLjGD.' user

# make /bin/sh symlink to bash instead of dash:
RUN echo "dash dash/sh boolean false" | debconf-set-selections && DEBIAN_FRONTEND=noninteractive dpkg-reconfigure dash

# Install under /home/user as user 'user'
WORKDIR /home/user
USER user

FROM petalinux_base as installer

ARG PETA_RUN_FILE=petalinux-v2019.2-final-installer.run
COPY --chown=user:user ${PETA_RUN_FILE} .

RUN chmod +x ${PETA_RUN_FILE} && SKIP_LICENSE=y ./${PETA_RUN_FILE} ./petalinux

FROM petalinux_base

COPY --from=installer /home/user/petalinux ./petalinux/
RUN echo "source $HOME/petalinux/settings.sh" >> .bashrc
