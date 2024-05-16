# implent the client side of the mock server
import grpc
import sys

sys.path.append('../shared_proto')

import data_analytics_pb2, data_analytics_pb2_grpc


def send_log_data():
    with grpc.insecure_channel('localhost:123456') as channel:
        stub = data_analytics_pb2_grpc.SimulationServiceStub(channel)
        log_data = data_analytics_pb2.logdataanalytics(
            timestamp='2024-05-16 12:00:00',
            type='INFO',
            content='Log message content',
            extra_1='Extra data 1',
            extra_2='Extra data 2'
        )
        response = stub.ReportCycle(log_data)
        print("Received response:", response)

if __name__ == '__main__':
    send_log_data()
# Run the client
# python mock/client.py