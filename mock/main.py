import networkx as nx
import random
import time
import models
from graph_user_flow import *
import simulation


if __name__ == "__main__":

    print("Starting simulation...")

    params = simulation.SimulationParams(
        cycle_duration=1,
        num_initial_users=5,
        num_initial_products=2,
        qtd_stock_initial=100,
        max_simultaneus_users=10
    )
    sim = simulation.Simulation(params)

    sim.run()

    print("Simulation finished.")
