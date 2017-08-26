# -*- coding: utf-8 -*-
"""
pydot example 2
@author: Federico Cáceres
@url: http://pythonhaven.wordpress.com/2009/12/09/generating_graphs_with_pydot
"""
import pydot

# this time, in graph_type we specify we want a DIrected GRAPH
graph = pydot.Dot(graph_type='digraph')

# MQTT Broker (central node)
node_a = pydot.Node("Mosquitto MQTT Broker", style="filled", fillcolor="red")

# Home assistant
node_b = pydot.Node("Home Assistant", style="filled", fillcolor="turquoise")

# ESP Bare Metal
node_c = pydot.Node("Outside Light", style="filled", fillcolor="yellow")
node_d = pydot.Node("Outside Temperature", style="filled", fillcolor="yellow")
# node_ef = pydot.Node("Gate", style="filled", fillcolor="yellow")


node_e = pydot.Node("Gate Light", style="filled", fillcolor="yellow")
node_f = pydot.Node("Gate Control", style="filled", fillcolor="yellow")

# Python
node_g = pydot.Node("UPS Power Monitor", style="filled", fillcolor="green")

# Micropython
node_h = pydot.Node("Solar Battery Monitor",
                    style="filled", fillcolor="purple")

# Internet
node_i = pydot.Node("The Internet",
                    style="filled", fillcolor="white")

# Universal Remote Control
node_j = pydot.Node("Universal Remote", style="filled", fillcolor="green")

# Alarm system interface
node_k = pydot.Node("Paradox Alarm Interface", style="filled", fillcolor="yellow")

# now we add the nodes to the graph
graph.add_node(node_a)
graph.add_node(node_b)
graph.add_node(node_c)
graph.add_node(node_d)

cluster_esp = pydot.Cluster("Gate_ESP", label="Gate ESP")
cluster_esp.add_node(node_e)
cluster_esp.add_node(node_f)
graph.add_subgraph(cluster_esp)

graph.add_node(node_g)
graph.add_node(node_h)

graph.add_node(node_i)

graph.add_node(node_j)

graph.add_node(node_k)

graph.set_edge_defaults(color="black", arrowhead="vee", weight="0")
graph.add_edge(pydot.Edge(node_a, node_b))
graph.add_edge(pydot.Edge(node_b, node_a))

graph.add_edge(pydot.Edge(node_a, node_c))
graph.add_edge(pydot.Edge(node_c, node_a))

graph.add_edge(pydot.Edge(node_d, node_a))

graph.add_edge(pydot.Edge(node_g, node_a))

graph.add_edge(pydot.Edge(node_h, node_a))

graph.add_edge(pydot.Edge(node_a, node_e))
graph.add_edge(pydot.Edge(node_e, node_a))

graph.add_edge(pydot.Edge(node_a, node_f))

graph.add_edge(pydot.Edge(node_k, node_a))

graph.set_edge_defaults(color="blue", arrowhead="vee", weight="0")
graph.add_edge(pydot.Edge(node_i, node_b))
graph.add_edge(pydot.Edge(node_b, node_i))

graph.set_edge_defaults(color="black", arrowhead="vee", weight="0")
graph.add_edge(pydot.Edge(node_a, node_j))

graph.write_raw('kydhome_img/overview.dot')
# and we are done
graph.write_png('kydhome_img/overview.png')

# this is too good to be true!
