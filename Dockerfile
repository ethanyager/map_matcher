FROM python:3.9-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . /app

RUN pip install --upgrade pip && \
    pip install pybind11 psycopg2-binary pulsar-client

# Build C++
RUN python setup.py build_ext --inplace

CMD ["python", "run_map_matcher.py"]
#ENTRYPOINT ["tail", "-f", "/dev/null"]