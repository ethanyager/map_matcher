# Use a slim Python image as the base.
FROM python:3.9-slim

# Install system dependencies needed for building C++ extensions.
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
 && rm -rf /var/lib/apt/lists/*

# Set the working directory.
WORKDIR /app

# Copy your application code into the container.
COPY . /app

# Upgrade pip and install only the Python dependencies required by your script.
# (Exclude dependencies related to running services like Pulsar or PostgreSQL here.)
RUN pip install --upgrade pip && \
    pip install pybind11 psycopg2-binary pulsar-client

# Build your C++ extension module (assuming your setup.py is set up for this).
RUN python setup.py build_ext --inplace

# Set the command to run your map matcher script.
# Replace 'map_matcher.py' with the actual entrypoint script for your application.
CMD ["python", "run_map_matcher.py"]
#ENTRYPOINT ["tail", "-f", "/dev/null"]