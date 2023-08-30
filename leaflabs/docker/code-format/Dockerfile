#
# Build a Docker image with clang-format and Black for checking formatting
# Update the container image by running:
# $ docker build -t registry.gitlab.leaflabs.com/bldg8/ovrsource/code-format .
#
# Update the container registry by running:
# $ docker push registry.gitlab.leaflabs.com/bldg8/ovrsource/code-format
#

# Start with a base image for Ubuntu 18.04 LTS
FROM ubuntu:18.04

# Install utilities from apt/pip
RUN apt update && \
    apt install -y clang-format-8 \
                   git-core \
                   python3-pip
                   
RUN pip3 install black
