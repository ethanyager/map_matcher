from airflow import DAG
from airflow.providers.cncf.kubernetes.operators.kubernetes import KubernetesJobOperator
from datetime import datetime

with DAG("map_matcher_dag", start_date=datetime(2025, 1, 1), schedule_interval="0 12 * * *", catchup=False) as dag:
    run_job = KubernetesJobOperator(
        task_id="run_map_matcher",
        name="map-matcher-job",
        namespace="default",
        image="map-matcher:latest",
        env_vars={
            "PULSAR_SERVICE_URL": "pulsar://pulsar-service:6650",
            "POSTGRES_CONN": "dbname=mapmatcher user=postgres password=postgres host=postgres-service port=5432",
            "PULSAR_TOPIC": "gps-events"
        }
    )