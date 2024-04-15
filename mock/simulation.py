import os
from dataclasses import dataclass
from random import choice, randint, random, choices, shuffle
from time import sleep, time
import models
import csv
from collections import deque
import threading
from graph_user_flow import *
import time
import threading

@dataclass
class SimulationParams:
    cycle_duration: float
    num_initial_users: int
    num_initial_products: int
    qtd_stock_initial: int
    max_simultaneus_users: int

class Simulation:
    params: SimulationParams
    cycle: int
    silent: bool = True

    def __init__(self, params: SimulationParams, silent: bool = True):
        self.cycle = 0
        self.params = params
        self.silent = silent
        self.users = []
        self.new_users = []
        self.products = []
        self.new_products = []
        self.stock = {}
        self.new_stock_decreases = {}
        self.purchase_orders = []
        self.new_purchase_orders = []
        self.waiting_users = deque()
        self.logged_users = deque()
        self.depuration = []
        self.log_flow = []
        self.user_flow_report = []
        self.lock_log_flow = threading.Lock()
        self.lock_user_flow_report = threading.Lock()

        self.G = G

        self.folder_name = "mock_files"
        self.subfolder_csv = "csv"
        self.subfolder_log = "log"
        self.subfolder_http_request = "request"
        self.log_filename = "log_simulation.txt"
        self.log_complete_path = f"{self.folder_name}/{self.subfolder_log}/{self.log_filename}"
        self.csv_file_names = ["users.csv", "products.csv", "stock.csv", "purchase_orders.csv"]
        self.csv_complete_path = [f"{self.folder_name}/{self.subfolder_csv}/{file_name}" for file_name in self.csv_file_names]
        self.request_file_name = "request"
        self.request_complete_path = f"{self.subfolder_http_request}/{self.request_file_name}"

        # if the folder exists, delete its contents
        if os.path.exists(self.folder_name):
            self.remove_folder_contents(self.folder_name)
        else:
            os.makedirs(self.folder_name)

        # createinside folder_name or delete other folder if they exist for the other subfolders
        csvs_folder = f"{self.folder_name}/{self.subfolder_csv}"
        logs_folder = f"{self.folder_name}/{self.subfolder_log}"
        http_request_folder = f"{self.folder_name}/{self.subfolder_http_request}"

        for folder in [csvs_folder, logs_folder, http_request_folder]:
            if os.path.exists(folder):
                self.remove_folder_contents(folder)
            else:
                os.makedirs(folder)
        

        # generate users at the start of the simulation
        for _ in range(self.params.num_initial_users):
            self.__generate_user()

        # generate products at the start of the simulation
        for _ in range(self.params.num_initial_products):
            self.__generate_product()

        # generate stock for the products
        for product in self.products:
            self.__generate_stock(product, self.params.qtd_stock_initial)

        self.__report_initial_cycle()
    
    def __report_initial_cycle(self):
        # report users, products and stocks created, creating the new csvs
        with open(self.csv_complete_path[0], 'w') as file:
            writer = csv.writer(file, delimiter=';', lineterminator='\n')
            first_line = ["id", "name", "email", "address", "registration_date", "birth_date"]
            writer.writerow(first_line)
            content = [[user.id, user.name, user.email, user.address, user.registration_date, user.birth_date] for user in self.new_users]
            writer.writerows(content)
        self.new_users = []
        
        with open(self.csv_complete_path[1], 'w') as file:
            writer = csv.writer(file, delimiter=';', lineterminator='\n')
            first_line = ["id", "name", "image", "description", "price"]
            writer.writerow(first_line)
            content = [[product.id, product.name, product.image, product.description, product.price] for product in self.new_products]
            writer.writerows(content)
        self.new_products = []

        with open(self.csv_complete_path[2], 'w') as file:
            writer = csv.writer(file, delimiter=';', lineterminator='\n')
            first_line = ["id_product", "quantity"]
            writer.writerow(first_line)
            content = [[product_id, quantity] for product_id, quantity in self.stock.items()]
            writer.writerows(content)
        self.new_products = []

        with open(self.csv_complete_path[3], 'w') as file:
            writer = csv.writer(file, delimiter=';', lineterminator='\n')
            first_line = ["user_id", "product_id", "quantity", "creation_date", "payment_date", "delivery_date"]
            writer.writerow(first_line)

    def remove_folder_contents(self, folder_path):
        for root, _, files in os.walk(folder_path):
            for file in files:
                file_path = os.path.join(root, file)
                os.remove(file_path)     
    
    def run(self):
        while True:
            sleep(self.params.cycle_duration)
            self.cycle += 1

            # CONTA VERDE
            self.__generate_users()
            self.__generate_products()
            self.__generate_stock_for_new_products()
            
            # allow users to enter and perform actions on the system, after they are loged
            # DATACAT & DataAnalytics
            self.__select_waiting_users()
            self.__select_users_to_login()    

            # CONTA VERDE
            self.__update_cycle_stock()

            # DATACAT
            self.__introduct_errors_for_log()

            self.__print_status()
            self.__report_cycle()
           
    def __introduct_errors_for_log(self):
        node = choice(nodes)
        message = f";Error;{node}.\n"
        self.__add_message_to_log(message)
        
    def __select_waiting_users(self):

        num_users = min(self.params.max_simultaneus_users, len(self.users))
        user_copy = self.users.copy()
        shuffle(user_copy)
        for i in range(num_users):
            self.waiting_users.append(user_copy[i])

        
    def __select_users_to_login(self):

        num_users = len(self.waiting_users)

        # creating a thread for each user
        threads = []
        for _ in self.waiting_users.copy():
            user = self.waiting_users.popleft()
            thread = threading.Thread(target=self.__user_flow, args=(user,))
            threads.append(thread)
            thread.start()
        for thread in threads:
            thread.join()

        

    def __user_flow(self, user):
        # let the user perform actions on the system until it reaches the EXIT node
        current_node = LOGIN
        sleep(random()/10)
        message = f";Audit;{user};{LOGIN}\n"
        self.__add_message_to_log(message)

        current_product_list = []
        current_product = None
        
        while current_node != EXIT:
            # select the next as the current node's successor, based on the probability of each neighbor in the edge

            current_node_neighbors = {}
            for u, v, d in G.edges(data=True):
                current_node_neighbors.setdefault(u, []).append((v, d["prob"]))
            next_node = choices(*zip(*current_node_neighbors[current_node]))[0]

            if next_node == HOME:
                self.__home(user)
                sleep(random()/10)

            elif next_node == VIEW_PRODUCT:
                products_in_stock = [product for product in self.products.copy() if self.stock[product] > 0]
                current_product = choice(products_in_stock)
                self.__view_product(user, current_product)
                sleep(random()/10)

            elif next_node == CART:
                current_product_list.append(current_product)
                self.__cart(user, current_product)
                sleep(random()/10)

            elif next_node == CHECKOUT:
                self.__checkout(user, current_product_list)
                sleep(random()/10)

            current_node = next_node
        sleep(random()/10)
        self.__exit(user)

    def get_timestamp_string(self):
        """Returns a high-resolution timestamp string."""
        return str(time.time_ns())  # Use nanoseconds for best resolution
    
    def __add_message_to_log(self, message):
        cur_time = self.get_timestamp_string()
        with self.lock_log_flow:
            self.log_flow.append(cur_time + message)

    def __add_message_to_user_flow_report(self, message):

        cur_time = self.get_timestamp_string()
        # add lock 
        with self.lock_user_flow_report:
            self.user_flow_report.append(cur_time + message)

    def __home(self, user):
        msg = f";User;{user};{STIMUL_SCROLLING};{HOME}.\n"
        self.__add_message_to_user_flow_report(msg)

    def __view_product(self, user, product):
        msg = f";User;{user};{STIMUL_ZOOM};{VIEW_PRODUCT} {product}.\n"
        self.__add_message_to_user_flow_report(msg)


    def __cart(self, user, product):
        msg = f";User;{user};{STIMUL_CLICK};{CART} with {product}.\n"
        self.__add_message_to_user_flow_report(msg)

    def __checkout(self, user, product_list):
        msg = f";User;{user};{STIMUL_CLICK};{CHECKOUT} with {product_list}.\n"
        self.__add_message_to_user_flow_report(msg)

        
        def add_purchase_order():
            dictionary_products = {}
            for product in product_list:
                dictionary_products[product] = dictionary_products.get(product, 0) + 1
            for product, quantity in dictionary_products.items():
                purchase_order = self.__generate_purchase_order(user, product, quantity)
                mesage = f";Audit;{user};created a purchase order for Product: {purchase_order.product_id} Quantity: {purchase_order.quantity}.\n"
                self.__add_message_to_log(mesage)
                self.__decrease_stock(product, quantity)
        add_purchase_order()
            
    def __exit(self, user):
        msg = f";User;{user};{STIMUL_CLICK};{EXIT}\n"
        self.__add_message_to_user_flow_report(msg)

        msg = f";Audit;{user};{EXIT}\n"
        self.__add_message_to_log(msg)



    def __generate_user(self):
        user = models.generate_user()
        self.users.append(user.id)
        self.new_users.append(user)

    def __generate_users(self):
        # choose a random number of users to generate, between 0 and 1, with 70% chance of generating 0 users
        num_users = 1 if random() > 0.3 else 0
        for _ in range(num_users):
            self.__generate_user()

    def __generate_stock(self, product_id, quantity):
        stock_product = models.generate_stock(product_id, quantity)
        self.stock[stock_product.id_product] = stock_product.quantity

    def __generate_product(self):
        product = models.generate_product()
        self.products.append(product.id)
        self.new_products.append(product)        

    def __generate_products(self):
        # choose a random number of products to generate, between 0 and 1, with 80% chance of generating 0 products
        num_products = 1 if random() > 0.2 else 0
        for _ in range(num_products):
            self.__generate_product()

    def __generate_stock_for_new_products(self):
        for product in self.new_products:
            self.__generate_stock(product.id, randint(1, 100))

    def __decrease_stock(self, product_id, quantity):
        self.new_stock_decreases[product_id] = self.new_stock_decreases.get(product_id, 0) + quantity

    def __update_cycle_stock(self):
        for product_id, quantity in self.new_stock_decreases.items():
            self.stock[product_id] -= quantity

    def __generate_purchase_order(self, user_id, product_id, quantity):
        purchase_order = models.generate_purchase_order(user_id, product_id, quantity)
        self.purchase_orders.append(purchase_order)
        self.new_purchase_orders.append(purchase_order)
        return purchase_order
    
    def __print_status(self):
        if self.silent:
            return

    def __report_cycle(self):


        # add cycle to the beginning of the base name of the log file
        log_cycle = f"{self.folder_name}/{self.subfolder_log}/{self.cycle}{self.log_filename}"

        # add cycle to the beginning of the base name of the http request file
        request_cycle = f"{self.folder_name}/{self.subfolder_http_request}/{self.cycle}{self.request_file_name}"

        log_flow_colunms = ["timestamp", "type", "content", "extra_1", "extra_2"]
        self.log_flow.insert(0, ";".join(log_flow_colunms) + "\n")
        # add use_flow_report list to the beginning of the base name of the log file
        self.log_flow.extend(self.user_flow_report)

        if self.log_flow:
            with open(log_cycle, "a") as f:
                    f.writelines(self.log_flow)
        self.log_flow = []

        if self.user_flow_report:
            with open(request_cycle, "a") as f:
                    f.writelines(self.user_flow_report)
        self.user_flow_report = []

        # update (create if not exists) the new_users in a .csv file, keeping existent users
        if self.new_users:
            with open(self.csv_complete_path[0], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                content = [[user.id, user.name, user.email, user.address, user.registration_date, user.birth_date] for user in self.new_users]
                writer.writerows(content)
        self.new_users = []

        if self.new_products:
            with open(self.csv_complete_path[1], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                content = [[product.id, product.name, product.image, product.description, product.price] for product in self.new_products]
                writer.writerows(content)
        self.new_products = []

        if self.new_stock_decreases:
            with open(self.csv_complete_path[2], 'w', newline='') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                first_line = ["id_product", "quantity"]
                writer.writerow(first_line)
                content = [[product_id, quantity] for product_id, quantity in self.stock.items()]
                writer.writerows(content)
        self.new_stock_decreases = {}

        if self.new_purchase_orders:
            with open(self.csv_complete_path[3], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                content = [[purchase_order.user_id, purchase_order.product_id, purchase_order.quantity, purchase_order.creation_date, purchase_order.payment_date, purchase_order.delivery_date] for purchase_order in self.new_purchase_orders]
                writer.writerows(content)

        self.new_purchase_orders = []
