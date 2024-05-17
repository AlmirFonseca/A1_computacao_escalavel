from graph_user_flow import *
import simulation
import grpc
import sys
sys.path.append('../shared_proto')
import data_analytics_pb2_grpc

if __name__ == "__main__":

    print("Starting simulation...")

    params = simulation.SimulationParams(
        cycle_duration=0.1,
        num_initial_users=1010,
        num_initial_products=1000,
        qtd_stock_initial=2000,
        max_simultaneus_users=1000,
        num_new_users_per_cycle=100,
        num_new_products_per_cycle=100,
    )

    connection = grpc.insecure_channel('localhost:50051')
    stub = data_analytics_pb2_grpc.SimulationServiceStreamStub(connection)

    sim = simulation.Simulation(params, stub)

    
    sim.run()

    print("Simulation finished.")
