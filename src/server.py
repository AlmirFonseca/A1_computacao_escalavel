import grpc
import sys
sys.path.append('../shared_proto')
import data_analytics_pb2
import data_analytics_pb2_grpc
from concurrent import futures


class SimulationServiceStreamServicer(data_analytics_pb2_grpc.SimulationServiceStreamServicer):
    def ReportCycle(self, request, context):
        print("Received log data:")
        print("Timestamp:", request.timestamp)
        return data_analytics_pb2.Empty()

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    data_analytics_pb2_grpc.add_SimulationServiceStreamServicer_to_server(
        SimulationServiceStreamServicer(), server)
    server.add_insecure_port('[::]:123456')
    server.start()
    print("Server started. Listening on port 123456...")
    server.wait_for_termination()

if __name__ == '__main__':
    serve()
