import grpc
import sys
sys.path.append('../shared_proto')
import data_analytics_pb2 as pb2, data_analytics_pb2_grpc as pb2_grpc

from typing import List, Tuple
from time import time
from models import User
import socket


def connect() -> pb2_grpc.SimulationServiceStreamStub:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    result = sock.connect_ex(("localhost", 123456))
    if result != 0:
        return None

    channel = grpc.insecure_channel("localhost:123456")
    stub = pb2_grpc.SimulationServiceStreamStub(channel)

    return stub


def report_cycle(
    stub: pb2_grpc.SimulationServiceStreamStub,
    user_flow_report: List[str]
):
    request = pb2.logdataanalyticsWithTime(
        timestamp=int(time() * 1000),
        log=user_flow_report
    )
    response = stub.ReportCycle(request)
    return
