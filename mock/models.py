from dataclasses import dataclass
from faker import Faker
from functools import wraps
from dataclasses import dataclass, field
from random import choice
from typing import Optional
import time

@dataclass
class User:
    id: str
    name: str
    email: str
    address: str
    registration_date: str
    birth_date: str

@dataclass
class Product:
    id: str
    name: str
    image: str
    description: str
    price: float

@dataclass
class Stock:
    id_product: str
    quantity: int

@dataclass
class Purchase_Order:
    user_id: str
    product_id: str
    quantity: int
    creation_date: str
    payment_date: str
    delivery_date: str

# Singleton decorator
def singleton(cls):
    @wraps(cls)
    def wrapper(*args, **kwargs):
        if not hasattr(cls, "_instance"):
            cls._instance = cls(*args, **kwargs)
        return cls._instance
    return wrapper

@singleton
class FakerSingleton:
    def __init__(self):
        self._faker = None

    def get_faker(self):
        if not self._faker:
            self._faker = Faker()
        return self._faker


def generate_user():
    """Generates a User dataclass instance with unique data."""
    faker = FakerSingleton().get_faker()
    return User(
        id=faker.unique.numerify(text="1########"),
        name=faker.name(),
        email=faker.email(),
        address= f"{faker.street_address()}, {faker.city()}, {faker.state()}, {faker.zipcode()}, {faker.country()}",
        registration_date = str(time.time_ns()),
        birth_date=faker.date_of_birth(minimum_age=18, maximum_age=100).isoformat(),
    )

def generate_product():
    """Generates a Product dataclass instance with unique data."""
    faker = FakerSingleton().get_faker()

    return Product(
        id=faker.unique.numerify(text="2######"),
        name=faker.word(),
        image=faker.image_url(),
        description=faker.sentence(),
        price=faker.random_int(min=1, max=1000),
    )

def generate_stock(product_id: str, quantity: int):
    """Generates a Stock dataclass instance with unique data."""
    faker = FakerSingleton().get_faker()
    return Stock(
        id_product=product_id,
        quantity=quantity,
    )

def generate_purchase_order(user_id: str, product_id: str, quantity: int):
    """Generates a Purchase_Order dataclass instance with unique data."""
    faker = FakerSingleton().get_faker()
    return Purchase_Order(
        user_id=user_id,
        product_id=product_id,
        quantity=quantity,
        creation_date=str(time.time_ns()),
        payment_date=str(time.time_ns()),
        delivery_date=str(time.time_ns()),
    )

    