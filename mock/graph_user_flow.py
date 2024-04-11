LOGIN = "Login"
HOME = "Home"
VIEW_PRODUCT = "View Product"
CART = "Cart"
CHECKOUT = "Checkout"
EXIT = "Exit"

nodes = [LOGIN, HOME, VIEW_PRODUCT, CART, CHECKOUT, EXIT]

edges = [
    (LOGIN, HOME, {"prob": 1}),
    (HOME, VIEW_PRODUCT, {"prob": 0.9}),
    (HOME, EXIT, {"prob": 0.1}),
    (VIEW_PRODUCT, CART, {"prob": 0.3}),
    (VIEW_PRODUCT, HOME, {"prob": 0.5}),
    (VIEW_PRODUCT, EXIT, {"prob": 0.2}),
    (CART, HOME, {"prob": 0.4}),
    (CART, CHECKOUT, {"prob": 0.5}),
    (CART, EXIT, {"prob": 0.1}),
    (CHECKOUT, EXIT, {"prob": 0.9}),
    (CHECKOUT, HOME, {"prob": 0.1}),
]
