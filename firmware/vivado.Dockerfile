FROM ubuntu:18.04 as installer

# build with docker build -f vivado.Dockerfile -t vivado:2019.2 .

ARG VIVADO_TAR_GZ_FILE=Xilinx_Vivado_2019.2_1106_2127
ARG UPDATE_TAR_GZ_FILE=Xilinx_Vivado_Vitis_Update_2019.2.1_1205_0436
ARG VIVADO_INSTALL_CONFIG_FILE=vivado_install_config.txt

COPY ${VIVADO_INSTALL_CONFIG_FILE} .

# Install Vivado
ADD ${VIVADO_TAR_GZ_FILE}.tar.gz .
RUN ${VIVADO_TAR_GZ_FILE}/xsetup --agree XilinxEULA,3rdPartyEULA,WebTalkTerms --batch Install --config ${VIVADO_INSTALL_CONFIG_FILE}

# Install Vivado update
ADD ${UPDATE_TAR_GZ_FILE}.tar.gz .
RUN ${UPDATE_TAR_GZ_FILE}/xsetup --agree XilinxEULA,3rdPartyEULA,WebTalkTerms --batch Update --config ${VIVADO_INSTALL_CONFIG_FILE}

FROM registry.gitlab.leaflabs.com/bldg8/pando_box-platform/petalinux:2019.2

COPY --from=installer /opt/Xilinx /opt/Xilinx/

RUN echo "source /opt/Xilinx/Vivado/2019.2/settings64.sh" >> .bashrc
