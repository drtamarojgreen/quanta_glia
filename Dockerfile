# Use an official Python runtime as a parent image
FROM python:3.10-slim

# Set the maintainer label
LABEL maintainer="Jules"

# Install git, which is required for cloning repositories
RUN apt-get update && apt-get install -y --no-install-recommends git && rm -rf /var/lib/apt/lists/*

# Set the working directory in the container
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . .

# Add the app directory to the python path
ENV PYTHONPATH=/app

# Create the workspace directory and clone all required sibling repositories
# as defined in the project's bootstrap.sh script.
RUN GH_USER="drtamarojgreen" && \
    REPOS="prismquanta quanta_dorsa quanta_ethos quanta_porto quanta_sensa quanta_synapse quanta_tissu quanta_memora quanta_serene quanta_cogno quanta_retina quanta_pulsa quanta_lista quanta_alarma quanta_cerebra" && \
    mkdir -p workspace && \
    cd workspace && \
    for repo in $REPOS; do \
        echo "Cloning ${repo}..." && \
        git clone --depth 1 "https://github.com/${GH_USER}/${repo}.git"; \
    done

# Sets the default command for the container.
# The script can be run with arguments pointing to local paths inside the container.
# For example:
# python3 scripts/quanta_glia.py workspace/quanta_ethos
CMD ["python3", "scripts/quanta_glia.py"]
