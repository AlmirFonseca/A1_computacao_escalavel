import os
from dataclasses import dataclass
from random import choice, randint, random, choices, shuffle
import models
import csv
from collections import deque
from graph_user_flow import *
import time
import msvcrt
import rpc
import time

@dataclass
class SimulationParams:
    cycle_duration: float
    num_initial_users: int
    num_initial_products: int
    qtd_stock_initial: int
    max_simultaneus_users: int
    num_new_users_per_cycle: int
    num_new_products_per_cycle: int

class Simulation:
    params: SimulationParams
    cycle: int
    silent: bool = True
    stub: rpc.pb2_grpc.SimulationServiceStreamStub

    def __init__(self, params: SimulationParams, stub: rpc.pb2_grpc.SimulationServiceStreamStub, silent: bool = True):
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
        self.stub = stub

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

        # create inside folder_name or delete other folder if they exist for the other subfolders
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
            self.cycle += 1

            # CONTA VERDE
            for _ in range(self.params.num_new_users_per_cycle):
                self.__generate_user()
            for _ in range(self.params.num_new_products_per_cycle):
                self.__generate_product()
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
            time.sleep(self.params.cycle_duration)
           
    def __introduct_errors_for_log(self):
        
        # choose a random number int from 0 to 5
        component_error = randint(0, 5)
        message = f";Error;{component_error}\n"
        self.__add_message_to_log(message)
        
    def __select_waiting_users(self):

        num_users = min(self.params.max_simultaneus_users, len(self.users))
        user_copy = self.users.copy()
        shuffle(user_copy)
        for i in range(num_users):
            self.waiting_users.append(user_copy[i])

        
    def __select_users_to_login(self):

        for _ in self.waiting_users.copy():
            user = self.waiting_users.popleft()
            # apply userflow to the user, without using threads
            self.__user_flow(user)


    def __user_flow(self, user):
        # let the user perform actions on the system until it reaches the EXIT node
        current_node = LOGIN
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

            elif next_node == VIEW_PRODUCT:
                products_in_stock = [product for product in self.products.copy() if self.stock[product] > 0]
                if not products_in_stock:
                    next_node = EXIT
                else:
                    current_product = choice(products_in_stock)
                    self.__view_product(user, current_product)

            elif next_node == CART:
                current_product_list.append(current_product)
                self.__cart(user, current_product)

            elif next_node == CHECKOUT:
                self.__checkout(user, current_product_list)

            current_node = next_node
        self.__exit(user)

    def get_timestamp_string(self):
        """Returns a high-resolution timestamp string."""
        return str(time.time_ns())  # Use nanoseconds for best resolution
    
    def __add_message_to_log(self, message):
        cur_time = self.get_timestamp_string()
        self.log_flow.append(cur_time + message)

    def __add_message_to_user_flow_report(self, message):
        cur_time = self.get_timestamp_string()
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
                
                for _ in range(quantity):
                    mesage = f";Audit;{user};BUY;{purchase_order.product_id}\n"
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

        # self.log_flow.insert(0, ";".join(log_flow_colunms) + "\n")
        
        first_half = self.user_flow_report[len(self.user_flow_report)//2:]
        self.user_flow_report = self.user_flow_report[:len(self.user_flow_report)//2]

        log_flow_colunms = ["timestamp", "type", "content", "extra_1", "extra_2"]
        self.user_flow_report.insert(0, ";".join(log_flow_colunms) + "\n")


        first_half.insert(0, ";".join(log_flow_colunms) + "\n")
        # add to the beginning of the log_flow the first half of the user_flow_report
        self.log_flow = first_half + self.log_flow

        self.write_log(log_cycle)
        self.log_flow = []

        self.write_log_dataAnalytics(request_cycle)
        self.user_flow_report = []

        # update (create if not exists) the new_users in a .csv file, keeping existent users
        self.add_new_users_to_csv()
        self.new_users = []

        self.new_method()
        self.new_products = []

        self.rewrite_full_stock_to_csv()
        self.new_stock_decreases = {}

        self.add_new_purchase_orders_to_csv()
        self.new_purchase_orders = []

    def add_new_purchase_orders_to_csv(self):
        if self.new_purchase_orders:
            content = [[purchase_order.user_id, purchase_order.product_id, purchase_order.quantity, purchase_order.creation_date, purchase_order.payment_date, purchase_order.delivery_date] for purchase_order in self.new_purchase_orders]
            with open(self.csv_complete_path[3], 'a') as file:
                # acquire lock and write to the file, then release the lock
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                # if self.acquire_lock(self.csv_complete_path[3]):
                writer.writerows(content)
                # self.release_lock(self.csv_complete_path[3])

    def rewrite_full_stock_to_csv(self):
        if self.new_stock_decreases:
            first_line = ["id_product", "quantity"]
            content = [[product_id, quantity] for product_id, quantity in self.stock.items()]
            # add the first line to the content beggining
            content.insert(0, first_line)
            with open(self.csv_complete_path[2], 'w', newline='') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                # if self.acquire_lock(self.csv_complete_path[2]):
                writer.writerows(content)
                # self.release_lock(self.csv_complete_path[2])


    def new_method(self):
        if self.new_products:
            content = [[product.id, product.name, product.image, product.description, product.price] for product in self.new_products]
            with open(self.csv_complete_path[1], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                # acquire lock and write to the file, then release the lock
                # if self.acquire_lock(self.csv_complete_path[1]):
                writer.writerows(content)
                # self.release_lock(self.csv_complete_path[1])

    def add_new_users_to_csv(self):
        if self.new_users:
            content = [[user.id, user.name, user.email, user.address, user.registration_date, user.birth_date] for user in self.new_users]
            with open(self.csv_complete_path[0], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                # acquire lock and write to the file, then release the lock
                # if self.acquire_lock(self.csv_complete_path[0]):
                writer.writerows(content)
                # self.release_lock(self.csv_complete_path[0])

    def write_log_dataAnalytics(self, request_cycle):
        rpc.report_cycle(self.stub, self.user_flow_report)

    def write_log(self, log_cycle):
        if self.log_flow:
            with open(log_cycle, "a") as f:
                # acquire lock and write to the file, then release the lock
                # if self.acquire_lock(log_cycle):
                f.writelines(self.log_flow)
                # self.release_lock(log_cycle)

    def acquire_lock(self, file_path):
        try:
            msvcrt.locking(open(file_path, "rb").fileno(), msvcrt.LK_NBLCK, 1)
            return True
        except IOError:
            return False

    def release_lock(self, file_path):
        try:
            msvcrt.locking(open(file_path, "rb").fileno(), msvcrt.LK_UNLCK, 1)
        except IOError:
            pass

