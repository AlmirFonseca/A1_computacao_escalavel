import os
from dataclasses import dataclass
from random import choice, randint, random
from time import sleep, time
from typing import Optional
import models
import csv

@dataclass
class SimulationParams:
    cycle_duration: float
    num_initial_users: int
    num_initial_products: int

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

        self.files_names = ["simulation.log", "users.csv", "products.csv"]
        # delete the files if they exist
        for file_name in self.files_names:
            if os.path.exists(file_name):
                os.remove(file_name)

        # generate users at the start of the simulation
        for _ in range(self.params.num_initial_users):
            self.users.append(self.__generate_users())

        # generate products at the start of the simulation
        for _ in range(self.params.num_initial_products):
            self.products.append(self.__generate_products())


    def run(self):
        while True:
            self.__generate_users()
            self.__print_status()
            self.__report_cycle()
            sleep(self.params.cycle_duration)
            self.cycle += 1

    def __generate_users(self):
        # choose a random number of users to generate, between 0 and 1, with 70% chance of generating 0 users
        num_users = 1 if random() > 0.3 else 0
        for _ in range(num_users):
            user = models.generate_user()
            self.users.append(user)
            self.new_users.append(user)

    def __generate_products(self):
        # choose a random number of products to generate, between 0 and 1, with 80% chance of generating 0 products
        num_products = 1 if random() > 0.2 else 0
        for _ in range(num_products):
            product = models.generate_product()
            self.products.append(product)
            self.new_products.append(product)


    def __print_status(self):
        if self.silent:
            return

    def __report_cycle(self):
        # write the current cycle to the log file as csv
        with open(self.files_names[0], "a") as f:
            f.write(f"{self.cycle}\n")

        # update (create if not exists) the new_users in a .csv file, keeping existent users
        if self.new_users:
            with open(self.files_names[1], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                for user in self.new_users:
                    writer.writerow([user.id, user.name, user.email, user.address, user.registration_date, user.birth_date])
        self.new_users = []

        # update (create if not exists) the new_products in a .csv file, keeping existent products
        if self.new_products:
            with open(self.files_names[2], 'a') as file:
                writer = csv.writer(file, delimiter=';', lineterminator='\n')
                for product in self.new_products:
                    writer.writerow([product.id, product.name, product.image, product.description, product.price])
        self.new_products = []