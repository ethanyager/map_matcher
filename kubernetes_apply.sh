kubectl apply -f statefulset_postgres.yaml
kubectl apply -f service_postgres.yaml
kubectl apply -f pod_pulsar.yaml
kubectl apply -f service_pulsar.yaml
kubectl apply -f job_map_matcher.yaml