import networkx as nx

STIMUL_SCROLLING = "SCROLLING"
STIMUL_CLICK = "CLICK"
STIMUL_ZOOM = "ZOOM"

LOGIN = "LOGIN"
HOME = "HOME"
VIEW_PRODUCT = "VIEW_PRODUCT"
CART = "CART"
CHECKOUT = "CHECKOUT"
EXIT = "EXIT"

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


G = nx.DiGraph()
G.add_nodes_from(nodes)
G.add_edges_from(edges)