import json
import pulsar

client = pulsar.Client('pulsar://127.0.0.1:61819')

producer = client.create_producer('gps-events')

with open('events.json', 'r') as f:
    events = json.load(f)

for event in events:
    message = json.dumps(event)
    producer.send(message.encode('utf-8'))
    print(f"Published event: {event['data_point_id']}")

client.close()