import grpc
import sys
sys.path.append('../shared_proto')
import data_analytics_pb2
import data_analytics_pb2_grpc
from concurrent import futures


class SimulationServiceServicer(data_analytics_pb2_grpc.SimulationServiceServicer):
    def ReportCycle(self, request, context):
        print("Received log data:")
        print("Timestamp:", request.timestamp)
        print("Type:", request.type)
        print("Content:", request.content)
        print("Extra 1:", request.extra_1)
        print("Extra 2:", request.extra_2)
        return data_analytics_pb2.Empty()

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    data_analytics_pb2_grpc.add_SimulationServiceServicer_to_server(
        SimulationServiceServicer(), server)
    server.add_insecure_port('[::]:123456')
    server.start()
    print("Server started. Listening on port 123456...")
    server.wait_for_termination()

if __name__ == '__main__':
    serve()
