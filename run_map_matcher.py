# run_map_matcher.py
import json
import os
import time

import pulsar
import psycopg2
from map_matcher import HMMMapMatcher

PULSAR_SERVICE_URL = os.getenv("PULSAR_SERVICE_URL", "pulsar://localhost:6650")
POSTGRES_CONN = os.getenv("POSTGRES_CONN",
                          "dbname=mapmatcher user=postgres password=postgres host=localhost port=5432")
PULSAR_TOPIC = os.getenv("PULSAR_TOPIC", "gps-events")

def process_event(event, matcher):
    """
    Process a single event by extracting the GPS coordinate and running the matcher.
    """
    gps_point = (event["lat"], event["lon"])
    # match_trace expects a list of points; for simplicity, we pass a one-element list.
    matched_edges = matcher.match_trace([gps_point])
    return matched_edges[0] if matched_edges else None

def insert_into_postgres(conn, event, matched_edge):
    """
    Insert the matched result.
    """
    cur = conn.cursor()
    cur.execute(
        """
        INSERT INTO map_matches (journey_id, data_point_id, timestamp, matched_edge)
        VALUES (%s, %s, %s, %s)
        """,
        (event["journey_id"], event["data_point_id"], event["timestamp"], matched_edge)
    )
    conn.commit()
    cur.close()

def main():
    # Connect / create table if it doesn't exist
    conn = psycopg2.connect(POSTGRES_CONN)
    cur = conn.cursor()
    cur.execute("""
    CREATE TABLE IF NOT EXISTS map_matches (
        id SERIAL PRIMARY KEY,
        journey_id VARCHAR(100),
        data_point_id VARCHAR(100),
        timestamp TIMESTAMP,
        matched_edge VARCHAR(100)
    )
    """)
    conn.commit()
    cur.close()

    # Instantiate map matcher
    matcher = HMMMapMatcher("edges.json")

    client = pulsar.Client(PULSAR_SERVICE_URL)
    consumer = client.subscribe(PULSAR_TOPIC, subscription_name="map-matcher-subscription")

    try:
        while True:
            try:
                msg = consumer.receive(timeout_millis=5000)
            except Exception as ex:
                # Uncomment and remove break for long running task that constantly drains topic.
                #print("No message received within timeout; sleeping for 5 seconds.")
                #time.sleep(5)
                #continue
                break

            try:
                event = json.loads(msg.data().decode('utf-8'))
                matched_edge = process_event(event, matcher)
                insert_into_postgres(conn, event, matched_edge)
                consumer.acknowledge(msg)
                print(f"Processed event {event['data_point_id']}: matched edge {matched_edge}")
            except Exception as e:
                consumer.negative_acknowledge(msg)
                print(f"Error processing message: {e}")
    finally:
        consumer.close()
        client.close()
        conn.close()

        

if __name__ == '__main__':
    main()
