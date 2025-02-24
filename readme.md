# Map Matcher Pipeline

This project implements a Hidden Markov Model (HMM) map matcher that processes GPS traces using C++ and Python, integrates with Apache Pulsar and PostgreSQL, and is deployed via Kubernetes with daily scheduling through Apache Airflow.

## Prerequisites
- Minikube (https://minikube.sigs.k8s.io/docs/start/) installed
- kubectl (https://kubernetes.io/docs/tasks/tools/) configured
- Docker (https://docs.docker.com/get-docker/) installed
- Python 3.9+ (for local testing, optional)

## Build
1. Start Minikube:
   minikube start
   This creates a local Kubernetes cluster.
2. Set Docker environment to use Minikube’s Docker daemon:
   eval $(minikube docker-env)
3. Build the Docker image:
   docker build -t map-matcher:latest .


## Deploy
1. Apply Kubernetes resources:
   kubectl apply -f postgres-deployment.yaml
   kubectl apply -f pulsar-deployment.yaml
   kubectl apply -f job_map_matcher.yaml
2. Verify deployments:
   kubectl get pods
   Look for postgres, pulsar, and map-matcher-job pods with Running or Completed status.

## Test the Pipeline
1. Check Job completion:
   kubectl get jobs
   Ensure map-matcher-job shows 1/1 completions.
2. Inspect logs:
   kubectl logs -l job-name=map-matcher-job
   Confirm events were processed.
3. Verify PostgreSQL results:
   Port-forward the PostgreSQL service:
   kubectl port-forward service/postgres-service 5432:5432 &
   Connect locally (e.g., with psql):
   psql -h localhost -U postgres -d mapmatcher
   Password: postgres
   Query the results:
   SELECT * FROM map_matches;
   Expect matched edge IDs from events.json.
   
## Publish Events
1. Ensure Pulsar is accessible by port-forwarding (run in the background):
   kubectl port-forward service/pulsar-service 6650:6650 &
2. Run the publisher:
   docker run map-matcher:latest python publish_events.py
   This sends events from events.json to the gps-events topic.
   Note: Alternatively, create a separate Kubernetes Job (e.g., publish-events-job.yaml) to automate this step.


## Notes
- Requires: edges.json, events.json, map_matcher.cpp, setup.py, run_map_matcher.py, publish_events.py, nlohmann/json.hpp.
- Environment variables set in job_map_matcher.yaml:
  - PULSAR_SERVICE_URL: pulsar://pulsar-service:6650
  - POSTGRES_CONN: dbname=mapmatcher user=postgres password=postgres host=postgres-service port=5432
  - PULSAR_TOPIC: gps-events
- Airflow: To schedule daily at 12:00 UTC, copy dag_map_matcher.py to your Airflow DAGs folder and configure Airflow to use Minikube’s Kubernetes cluster.