import os
from dataclasses import dataclass
from random import choice, randint, random, choices, shuffle
from time import sleep, time
from typing import Optional
import models
import csv
from collections import deque
import threading
from graph_user_flow import *
import time


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
        self.log_flow = []

        self.G = G

        # TODO: create one file per cycle
        # Delete the folder where they are
        # add a constant to the class with the file names

        self.files_names = ["log_simulation.txt", "users.csv", "products.csv", "stock.csv", "purchase_orders.csv"]
        # delete the files if they exist
        for file_name in self.files_names:
            if os.path.exists(file_name):
                os.remove(file_name)

        # generate users at the start of the simulation
        for _ in range(self.params.num_initial_users):
            self.__generate_user()

        # generate products at the start of the simulation
        for _ in range(self.params.num_initial_products):
            self.__generate_product()

        # generate stock for the products
        for product in self.products:
            self.__generate_stock(product, self.params.qtd_stock_initial)

        # TODO: generate initial report with the users, products and stock and sleep for cycle_duration
    
    def run(self):
        while True:
            self.__generate_users()
            self.__generate_products()
            self.__generate_stock_for_new_products()
            
            # allow users to enter and perform actions on the system, if they are not yet loged
            self.__select_waiting_users()
            self.__select_users_to_login()    

            self.__update_cycle_stock()
            self.__print_status()
            self.__report_cycle()
            sleep(self.params.cycle_duration)
            self.cycle += 1
    
    
    def __select_waiting_users(self):
        # select max_simultaneus_users to enter the waiting queue
        for _ in range(self.params.max_simultaneus_users):
            self.waiting_users.append(choice(self.users))

    def __select_users_to_login(self):
        # select users from the waiting queue to login
        num_users = randint(0, len(self.waiting_users))
        for _ in range(num_users):
            user = self.waiting_users.popleft()
            self.logged_users.append(user)

        # create a threead for each user to perform actions on the system (user_flow actions that ends with EXIT)
        # apply __user_flow method to users in the logged_users queue
        threads = []
        for user in self.logged_users:
            t = threading.Thread(target=self.__user_flow, args=(user,))
            threads.append(t)
            t.start()

        for t in threads:
            t.join()

        # remove users from the logged_users queue
        self.logged_users.clear()

    
            
    def __user_flow(self, user):
        # let the user perform actions on the system until it reaches the EXIT node
        current_node = LOGIN
        # TO-DO: Make it thread-safe
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
                # TO DO: check if access to product is thread-safe
                # show just products that are in stock
                products_in_stock = [product for product in self.products if self.stock[product] > 0]
                current_product = choice(products_in_stock)
                self.__view_product(user, current_product)
            elif next_node == CART:
                current_product_list.append(current_product)
                self.__cart(user, current_product)
            elif next_node == CHECKOUT:
                self.__checkout(user, current_product_list)
            current_node = next_node
        # if user reached the EXIT node, log a message and remove it from the logged_users list
        self.__exit(user)

    def get_timestamp_string(self):
        """Returns a high-resolution timestamp string."""
        return str(time.time_ns())  # Use nanoseconds for best resolution


    def __home(self, user):
        # TO-DO: make it thread-safe
        self.log_flow.append(f"{self.get_timestamp_string()}-User-{user}- is {HOME}.\n")

    def __view_product(self, user, product):
        # choose a random product to view
        self.log_flow.append(f"{self.get_timestamp_string()}-User-{user}- is {VIEW_PRODUCT} {product}.\n")

    def __cart(self, user, product):
        self.log_flow.append(f"{self.get_timestamp_string()}-User-{user}- is {CART} with {product}.\n")

    def __checkout(self, user, product_list):
        self.log_flow.append(f"{self.get_timestamp_string()}-User-{user}- is {CHECKOUT} with {product_list}.\n")
        # create a order for the user
        
        def add_purchase_order():
            dictionary_products = {}
            for product in product_list:
                dictionary_products[product] = dictionary_products.get(product, 0) + 1
            for product, quantity in dictionary_products.items():
                # TO-DO: make it thread-safe, AND do not allow to create a purchase order if the stock is not enough
                purchase_order = self.__generate_purchase_order(user, product, quantity)
                self.log_flow.append(f"{self.get_timestamp_string()}-Audit-{user}- created a purchase order for Product: {purchase_order.product_id} Quantity: {purchase_order.quantity}.\n")
                # update the stock of the products
                self.__decrease_stock(product, quantity)

        add_purchase_order()
            
    def __exit(self, user):
        self.log_flow.append(f"{self.get_timestamp_string()}-User-{user}- is {EXIT}.\n")

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
        # add self.log_flow to the log file as csv
        with open(self.files_names[0], "a") as f:
            # write at once all the log flow (no csv writerow method, because the log_flow is already a list of strings)
            f.writelines(self.log_flow)
        self.log_flow = []

        # update (create if not exists) the new_users in a .csv file, keeping existent users
        if self.new_users:
            with open(self.files_names[1], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                # write at once all the new users
                content = [[user.id, user.name, user.email, user.address, user.registration_date, user.birth_date] for user in self.new_users]
                writer.writerows(content)
        self.new_users = []

        # update (create if not exists) the new_products in a .csv file, keeping existent products
        if self.new_products:
            with open(self.files_names[2], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                # write at once all the products
                content = [[product.id, product.name, product.image, product.description, product.price] for product in self.new_products]
                writer.writerows(content)
        self.new_products = []

        # update (create if not exists) the new_stock_decreases in a .csv file, keeping existent stock
        # TODO: create a database to update the stock
        # meanwhile, work with csv files
        if self.new_stock_decreases:
            # delete the file content and write the new stock
            with open(self.files_names[3], 'w') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                # writ at once all the stock
                content = [[product_id, quantity] for product_id, quantity in self.stock.items()]
                writer.writerows(content)

        self.new_stock_decreases = {}

        # update (create if not exists) the new_purchase_orders in a .csv file, keeping existent purchase orders
        if self.new_purchase_orders:
            with open(self.files_names[4], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                # write at once all the purchase orders
                content = [[purchase_order.user_id, purchase_order.product_id, purchase_order.quantity, purchase_order.creation_date, purchase_order.payment_date, purchase_order.delivery_date] for purchase_order in self.new_purchase_orders]
                writer.writerows(content)

        self.new_purchase_orders = []

        
