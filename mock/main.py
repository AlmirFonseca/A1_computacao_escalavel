import networkx as nx
import random
from multiprocessing import Process, Queue, Manager
import time


users = {
    "User 1": 1,
    "User 2": 2,
    "User 3": 3,
    "User 4": 4,
}

products = {
    1: "Product 1",
    2: "Product 2",
}

# Define nodes in the user journey
nodes = ["Login", "Home", "View Product", "Cart", "Checkout", "Exit"]

edges = [
    ("Login", "Home", {"prob": 1}),
    ("Home", "View Product", {"prob": 0.9}),
    ("Home", "Exit", {"prob": 0.1}),  # Exit from Home
    ("View Product", "Cart", {"prob": 0.3}),
    ("View Product", "Home", {"prob": 0.5}),
    ("View Product", "Exit", {"prob": 0.2}),  # Exit from View Product
    ("Cart", "Home", {"prob": 0.4}),
    ("Cart", "Checkout", {"prob": 0.5}),
    ("Cart", "Exit", {"prob": 0.1}),  # Exit from Cart
    ("Checkout", "Exit", {"prob": 0.9}),  # Exit after Purchase
    ("Checkout", "Home", {"prob": 0.1}),  # Exit after Purchase
]

graph = nx.DiGraph()

# Add nodes and edges
graph.add_nodes_from(nodes)
graph.add_edges_from(edges)

def get_timestamp_string():
    """Returns a high-resolution timestamp string."""
    return str(time.time_ns())  # Use nanoseconds for best resolution


def user_journey(usuario_id, tarefas_fila):
    user_name = list(users.keys())[list(users.values()).index(usuario_id)]
    current = "Login"

    while True:
        # Create neighbors with probabilities dictionary
        vizinhos_com_prob = {}
        for u, v, d in graph.edges(data=True):
            vizinhos_com_prob.setdefault(u, []).append((v, d["prob"]))

        # Select next action (excluding self-loops)
        proximo = random.choices(*zip(*vizinhos_com_prob[current]))[0]
        if proximo == current:
            continue

        current = proximo
        timestamp_atual = get_timestamp_string()  # Capture current timestamp

        if current == "Exit":
            log_message = f"User {user_name} ({usuario_id}) exit system."
            tarefas_fila.put((usuario_id, log_message, current))
            #print(log_message)
            break

        if current == "Home":
            log_message = f"{timestamp_atual} - User {user_name} ({usuario_id}) is in homepage."
            #print(log_message)
        elif current == "View Product":
            # Select a random product
            produto_id = random.choice(list(products.keys()))
            log_message = f"{timestamp_atual} - User {user_name} ({usuario_id}) is viewing product {products[produto_id]} ({produto_id})."
            #print(log_message)
        elif current == "Cart":
            log_message = f"{timestamp_atual} - User {user_name} ({usuario_id}) is in the shopping cart."
        elif current == "Checkout":
            log_message = f"{timestamp_atual} - User {user_name} ({usuario_id}) completed the checkout."
            #print(log_message)

        # Add log message and current node to task queue
        tarefas_fila.put((usuario_id, log_message, current))
        #print('size:',tarefas_fila.qsize())


if __name__ == "__main__":
    num_users = len(users)

    taks_queue = Queue()

    proccesses = []
    for user_id in users.values():
        proccess = Process(target=user_journey, args=(user_id, taks_queue))
        proccesses.append(proccess)
        proccess.start()

    for proccess in proccesses:
        proccess.join()

    log_file = open("users_journey.log", "w")  # Open log file for writing
    while not taks_queue.empty():
        #time.sleep(0.1)  # Add a small delay to allow queue population
        user_id, log_message, node = taks_queue.get()
        log_file.write(f"{log_message}\n")
        log_file.flush()  # Flush buffer to write to file

    log_file.close()  # Close log file

    print("All the journeys are completed.")