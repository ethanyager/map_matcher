FROM python:3.9-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    openjdk-17-jdk \
    && rm -rf /var/lib/apt/lists/*

# Get pulsar
RUN wget https://archive.apache.org/dist/pulsar/pulsar-2.11.1/apache-pulsar-2.11.1-bin.tar.gz -O /tmp/pulsar.tar.gz && \
    tar -xzf /tmp/pulsar.tar.gz -C /opt && \
    mv /opt/apache-pulsar-2.11.1 /opt/pulsar && \
    rm /tmp/pulsar.tar.gz

WORKDIR /opt/pulsar

# Pulsar (6650 for broker, 8080 for admin)
EXPOSE 6650 8080

#CMD ["/opt/pulsar/bin/pulsar", "standalone"]
#ENTRYPOINT ["tail", "-f", "/dev/null"]
ENTRYPOINT ["/opt/pulsar/bin/pulsar", "standalone"]