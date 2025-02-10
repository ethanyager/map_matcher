FROM python:3.9-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    wget \
    supervisor \
    postgresql \
    postgresql-contrib \
    openjdk-17-jdk \
    && rm -rf /var/lib/apt/lists/*

# Add PostgreSQL binaries to PATH.
ENV PATH="/usr/lib/postgresql/15/bin:${PATH}"

# Set environment variables for PostgreSQL
ENV PGDATA=/var/lib/postgresql/data

# Create PostgreSQL data directory, fix permissions
RUN mkdir -p /var/lib/postgresql/data && chown -R postgres:postgres /var/lib/postgresql/data

# Switch to the postgres user to initialize the database
USER postgres
RUN initdb -D /var/lib/postgresql/data
RUN pg_ctl -D /var/lib/postgresql/data -l /var/lib/postgresql/data/logfile start && \
    psql --command "ALTER USER postgres WITH PASSWORD 'postgres';" && \
    psql --command "CREATE DATABASE mapmatcher;" && \
    pg_ctl -D /var/lib/postgresql/data stop

USER root



# Get pulsar
RUN wget https://archive.apache.org/dist/pulsar/pulsar-2.11.1/apache-pulsar-2.11.1-bin.tar.gz -O /tmp/pulsar.tar.gz && \
    tar -xzf /tmp/pulsar.tar.gz -C /opt && \
    mv /opt/apache-pulsar-2.11.1 /opt/pulsar && \
    rm /tmp/pulsar.tar.gz

WORKDIR /app

COPY . /app

RUN pip install --upgrade pip
RUN pip install pybind11 pulsar-client psycopg2-binary apache-airflow

RUN airflow db init && \
    airflow users create --username admin --firstname Admin --lastname User --role Admin --email admin@example.com --password admin

# Build the C++ extension module (map_matcher)
RUN python setup.py build_ext --inplace

# PostgreSQL (5432), Pulsar (6650 for broker, 8080 for admin), Airflow server (8081)
EXPOSE 5432 6650 8080 8081

COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf

# Use supervisord to start all services
CMD ["/usr/bin/supervisord", "-n"]