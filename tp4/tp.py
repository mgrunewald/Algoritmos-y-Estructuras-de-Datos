import pickle
from graph import Graph
from collections import deque
import random
from tqdm import tqdm
import networkx as nx 
import heapq
import time


"""
Lista de productos

Ejemplo de producto:
{'id': 2,
 'title': 'Candlemas: Feast of Flames',
 'group': 'Book',
 'categories': ['Books[283155]->Subjects[1000]->Religion & Spirituality[22]->Earth-Based Religions[12472]->Wicca[12484]',
  'Books[283155]->Subjects[1000]->Religion & Spirituality[22]->Earth-Based Religions[12472]->Witchcraft[12486]'],
 'reviewers': [('A11NCO6YTE4BTJ', 5),
  ('A9CQ3PLRNIR83', 4),
  ('A13SG9ACZ9O5IM', 5),
  ('A1BDAI6VEYMAZA', 5),
  ('A2P6KAWXJ16234', 4),
  ('AMACWC3M7PQFR', 4),
  ('A3GO7UV9XX14D8', 4),
  ('A1GIL64QK68WKL', 5),
  ('AEOBOF2ONQJWV', 5),
  ('A3IGHTES8ME05L', 5),
  ('A1CP26N8RHYVVO', 1),
  ('ANEIANH0WAT9D', 5)]}
"""
with open('products.pickle', 'rb') as file:
    products = pickle.load(file)

grafo = Graph()

print("Loading")
for p in tqdm(products):
    grafo.add_vertex(str(p["id"]), data={'title': p['title'],
                                         'group': p['group'],
                                         'categories': p['categories']})
    for reviewer, score in p['reviewers']:
        if not grafo.vertex_exists(reviewer):
            grafo.add_vertex(reviewer)
        grafo.add_edge(reviewer, str(p["id"]), score)
        grafo.add_edge(str(p["id"]), reviewer, score)

#grafo.print_graph()

def dfs(v, visitados, grafo):
    """
    Realiza un recorrido en profundidad (DFS) en un grafo a partir de un vértice inicial.

    Parámetros:
    - v: Vértice inicial para comenzar el recorrido.
    - visitados: Conjunto que almacena los vértices visitados durante el recorrido.
    - grafo: Grafo sobre el cual se realiza el recorrido.

    Modifica:
    - visitados: Agrega los vértices visitados durante el recorrido.

    Notas:
    - Utiliza una pila para llevar un seguimiento de los vértices a visitar.
    - Actualiza el conjunto de vértices visitados a medida que avanza en el recorrido.
    """
    pila = [v]

    while pila:
        actual = pila.pop()
        if actual not in visitados:
            visitados.add(actual)
            pila.extend(grafo.get_neighbors(actual))


def contar_componentes_conexas(grafo):
    """
    Calcula y devuelve el número de componentes conexas en un grafo.

    Parámetros:
    - grafo: Grafo sobre el cual se realizará el conteo de componentes conexas.

    Devuelve:
    - componentes: Número de componentes conexas en el grafo.

    Notas:
    - Utiliza el algoritmo de recorrido en profundidad (DFS) para explorar el grafo.
    - Cada llamada a DFS desde un vértice no visitado indica una nueva componente conexa.
    - El resultado es el número total de componentes conexas en el grafo.
    """
    vertices = list(grafo._graph.keys())
    visitados = set()
    componentes = 0

    for vertice in vertices:
        if vertice not in visitados:
            dfs(vertice, visitados, grafo)
            componentes += 1
    return componentes

def es_conexo(grafo):
    """
    Verifica si un grafo es conexo.

    Parámetros:
    - grafo: Grafo que se desea verificar.

    Devuelve:
    - True si el grafo es conexo, False en caso contrario.

    Notas:
    - Utiliza la función contar_componentes_conexas para determinar el número de componentes conexas.
    - Un grafo se considera conexo si tiene exactamente una componente conexa.
    """
    if (contar_componentes_conexas(grafo) == 1):
        return True
    return False

def tamano_y_componente_mas_grande(grafo):
    """
    Calcula el tamaño y devuelve la componente más grande de un grafo.

    Parámetros:
    - grafo: Grafo sobre el cual se realizarán los cálculos.

    Devuelve:
    - Un par (tamaño, componente) donde:
        - tamaño: Número de vértices en la componente más grande.
        - componente: Lista de vértices que forman la componente más grande.

    Notas:
    - Utiliza el algoritmo de recorrido en profundidad (DFS) para explorar el grafo.
    - Calcula el tamaño y la componente de cada componente conexa, actualizando el resultado si es necesario.
    """
    def dfs(v, visitados, componente_actual):
        pila = [v]

        while pila:
            actual = pila.pop()
            if actual not in visitados:
                visitados.add(actual)
                componente_actual.append(actual)
                pila.extend(grafo.get_neighbors(actual))

    vertices = list(grafo._graph.keys())
    visitados = set()
    tamano_mas_grande = 0
    componente_mas_grande = []

    for vertice in vertices:
        if vertice not in visitados:
            componente_actual = []
            dfs(vertice, visitados, componente_actual)
            tamano_actual = len(componente_actual)
            if tamano_actual > tamano_mas_grande:
                tamano_mas_grande = tamano_actual
                componente_mas_grande = componente_actual

    return tamano_mas_grande, componente_mas_grande

def componente_mas_chica(grafo):
    """
    Calcula el tamaño y devuelve un vértice de la componente más pequeña en un grafo.

    Parámetros:
    - grafo: Grafo sobre el cual se realizarán los cálculos.

    Devuelve:
    - Un par (tamaño, vertice) donde:
        - tamaño: Número de vértices en la componente más pequeña.
        - vertice: Vértice que pertenece a la componente más pequeña.

    Notas:
    - Utiliza el algoritmo de recorrido en profundidad (DFS) para explorar el grafo.
    - Calcula el tamaño y selecciona un vértice de la componente más pequeña.
    """
    def dfs(v, visitados, componente_actual):
        pila = [v]

        while pila:
            actual = pila.pop()
            if actual not in visitados:
                visitados.add(actual)
                componente_actual.append(actual)
                pila.extend(grafo.get_neighbors(actual))

    vertices = list(grafo._graph.keys())
    visitados = set()
    tamano_mas_chico = float('inf')
    vertice_mas_chico = None

    for vertice in vertices:
        if vertice not in visitados:
            componente_actual = []
            dfs(vertice, visitados, componente_actual)
            tamano_actual = len(componente_actual)
            if tamano_actual < tamano_mas_chico:
                tamano_mas_chico = tamano_actual
                vertice_mas_chico = componente_actual[0]

    return tamano_mas_chico, vertice_mas_chico


def find_shortest_path(grafo, node1, node2):
    """
    Encuentra el camino más corto entre dos nodos en un grafo utilizando BFS.

    Parámetros:
    - grafo: Grafo sobre el cual se realizará la búsqueda.
    - node1: Nodo de inicio.
    - node2: Nodo de destino.

    Devuelve:
    - Una lista que representa el camino más corto desde node1 hasta node2.
    - None si no hay un camino entre los nodos.

    Lanza:
    - ValueError si node1 o node2 no existen en el grafo.

    Notas:
    - Utiliza el algoritmo de búsqueda en amplitud (BFS) para encontrar el camino más corto.
    - El grafo debe ser no dirigido.
    - El resultado es una lista de nodos que forman el camino más corto.
    - Se utiliza una cola para explorar los nodos en el orden adecuado.
    """
    if not grafo.vertex_exists(node1) or not grafo.vertex_exists(node2):
        raise ValueError("Ambos nodos deben existir en el grafo.")
    queue = deque([(node1, [node1])])
    visited_nodes = set()
    while queue:
        current_node, path = queue.popleft()
        if current_node in visited_nodes:
            continue
        visited_nodes.add(current_node)
        if current_node == node2:
            return path
        neighbors = [
            neighbor for neighbor in grafo.get_neighbors(current_node)
        ]
        for neighbor in neighbors:
            new_path = path + [neighbor]
            queue.append((neighbor, new_path))
    return None

def shortest_path_but_just_the_products(shortest_path):
    """
    Extrae y devuelve solo los productos de un camino más corto.

    Parámetros:
    - shortest_path: Lista que representa el camino más corto, donde cada elemento es un vértice.

    Devuelve:
    - Lista que contiene solo los vértices que representan productos en el camino más corto.

    Notas:
    - Se espera que el formato de los vértices sea una cadena de caracteres.
    - Se asume que los productos están identificados por vértices cuyos primeros caracteres no son 'A'.
    """
    products_path = []
    for vertix in shortest_path:
        if vertix[0] != 'A':
            products_path.append(vertix)
    return products_path


def get_vertexes(graph):
    """
    Obtiene y devuelve una lista de vértices presentes en el grafo.

    Parámetros:
    - graph: Grafo del cual se extraerán los vértices.

    Devuelve:
    - Lista de vértices presentes en el grafo.

    Notas:
    - La función accede directamente al atributo '_graph' del grafo para obtener los vértices.
    - La lista devuelta puede contener duplicados si hay vértices conectados a múltiples aristas.
    """
    return list(graph._graph.keys())

def dijkstra_between_books(graph, start, end):
    """
    Encuentra y devuelve el camino más corto entre dos vértices en un grafo ponderado.

    Parámetros:
    - graph: Grafo ponderado sobre el cual se realizará la búsqueda.
    - start: Vértice de inicio.
    - end: Vértice de destino.

    Devuelve:
    - Lista que representa el camino más corto desde el vértice de inicio hasta el vértice de destino.
      La lista contiene los vértices en orden, incluyendo los extremos.

    Notas:
    - Utiliza el algoritmo de Dijkstra para encontrar el camino más corto en un grafo ponderado.
    - El grafo debe ser dirigido y ponderado.
    - Los pesos de las aristas representan la distancia entre los vértices.
    - La lista devuelta puede estar vacía si no hay un camino entre los vértices.
    """
    vertices = get_vertexes(grafo)
    distances = {vertex: float('infinity') for vertex in vertices}
    distances[start] = 0
    priority_queue = [(0,start)]
    previous_nodes = {vertex: None for vertex in graph._graph}
    while priority_queue:
        current_distance, current_vertex = heapq.heappop(priority_queue)
        if current_distance > distances[current_vertex]:
            continue
        for neighbor, weigth in graph._graph[current_vertex]['neighbors'].items():
            distance = current_distance + 1/weigth
            if distance < distances[neighbor]:
                distances[neighbor] = distance
                previous_nodes[neighbor] = current_vertex
                heapq.heappush(priority_queue, (distance,neighbor))
    path = []
    current_node = end
    while current_node is not None:
        path.insert(0,current_node)
        current_node = previous_nodes[current_node]

    return path

def bfs_longest_path(graph, start):
    """
    Encuentra y devuelve el camino más largo desde un vértice utilizando BFS.

    Parámetros:
    - graph: Grafo sobre el cual se realizará la búsqueda.
    - start: Vértice de inicio.

    Devuelve:
    - Lista que representa el camino más largo desde el vértice de inicio.
      La lista contiene los vértices en orden.

    Notas:
    - Utiliza el algoritmo de búsqueda en amplitud (BFS) para encontrar el camino más largo.
    - La longitud del camino se mide por la cantidad de vértices visitados.
    - La lista devuelta puede estar vacía si el vértice de inicio no está en el grafo.

    """
    visited = set()
    queue = deque([(start, [start])])
    longest_path = []

    while queue:
        node, path = queue.popleft()

        if node not in visited:
            visited.add(node)
            neighbors = list(graph.get_neighbors(node))  

            for neighbor in neighbors:
                if neighbor not in path:
                    queue.append((neighbor, path + [neighbor]))

            if len(path) > len(longest_path):
                longest_path = path

    return longest_path

def approximate_diameter(graph, componente_max, n):
    """
    Aproxima el diámetro de un grafo mediante la selección aleatoria de nodos y la búsqueda del camino más largo desde cada uno.

    Parámetros:
    - graph: Grafo sobre el cual se realizará la aproximación del diámetro.
    - componente_max: Lista de vértices que forman la componente más grande del grafo.
    - n: Número de nodos a seleccionar aleatoriamente para encontrar caminos más largos.

    Devuelve:
    - Lista que representa un camino que se aproxima al diámetro del grafo.

    Notas:
    - Utiliza el algoritmo de búsqueda en amplitud (BFS) para encontrar caminos más largos desde nodos seleccionados aleatoriamente.
    - La aproximación del diámetro se basa en la selección de múltiples nodos y la elección del camino más largo entre ellos.

    """
    selected_nodes = random.sample(componente_max, n)
    longest_paths = []

    for node in selected_nodes:
        longest_path = bfs_longest_path(graph, node)
        longest_paths.append(longest_path)

    diameter_candidate = max(longest_paths, key=len)

    return diameter_candidate

#Preguntas (Con correr el código aparecen las respuestas)

#1
print("PARTE 1")
conexo = es_conexo(grafo)
print("El grafo es conexo:", conexo)
num_componentes = contar_componentes_conexas(grafo)
print("Número de componentes conexas:", num_componentes)
tamano_max, componente_max = tamano_y_componente_mas_grande(grafo)
print("Tamaño de la componente más grande:", tamano_max)
tamano_mas_chico, vertice_mas_chico = componente_mas_chica(grafo)
print("Tamaño de la componente más pequeña:", tamano_mas_chico)
print("Id del vértice de la componente más pequeña:", vertice_mas_chico)

#2
print("PARTE 2")
book_1 = "41742"  # Popper
book_2 = "2065"  # Garfield
shortest_path = find_shortest_path(grafo, book_1, book_2)
print("Camino más corto:", shortest_path)
product_path = shortest_path_but_just_the_products(shortest_path)
print("El camino más corto considerando únicamente los productos:", product_path)

#3
print("PARTE 3")
shortest_and_best_path_between_books = dijkstra_between_books(grafo, book_1, book_2)
print("Camino más corto pasando por los artículos con mayor rating:", shortest_and_best_path_between_books)

#4
print("PARTE 4")
start_time = time.time()
diametro_aproximado = approximate_diameter(grafo, componente_max, 5)
numero_minimo_de_la_estimacion = len(diametro_aproximado)
print("Mínimo diámetro estimado:", numero_minimo_de_la_estimacion)
end_time = time.time()
elapsed_time = end_time - start_time
time_in_minutes = elapsed_time / 60
print(f"La función tardó {time_in_minutes} minutos en ejecutarse.")
si_fuera_con_todos_los_vertices = (time_in_minutes / 5) * tamano_max
print(f"La función tardaría {si_fuera_con_todos_los_vertices} minutos en ejecutarse para poder saber con certeza cuál es el diámetro de la mayor componente del grafo.")
