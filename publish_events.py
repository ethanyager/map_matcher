import json
import pulsar

client = pulsar.Client('pulsar://localhost:55001')

producer = client.create_producer('gps-events')

# TODO Load GPS events from file
sample_events = [
    {
        "journey_id": "test_journey_1",
        "data_point_id": "dp1",
        "timestamp": "2025-02-08T12:00:00Z",
        "lat": 37.176278,
        "lon": -113.333206,
        "speed": 100.0,
        "heading": 90
    },
    {
        "journey_id": "test_journey_2",
        "data_point_id": "dp2",
        "timestamp": "2025-02-08T12:00:05Z",
        "lat": 37.172349,
        "lon": -113.323695,
        "speed": 0.0,
        "heading": 269
    }
]

# Publish each event
for event in sample_events:
    message = json.dumps(event)
    producer.send(message.encode('utf-8'))
    print(f"Published event: {event['data_point_id']}")

client.close()
