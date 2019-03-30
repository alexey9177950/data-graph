import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt
from random import random
from scipy.stats import pearsonr
from collections import Counter
from os import system


# строит граф связей по датасету "6 degrees of Francis Bacon"
def parse_6dfb():
    # nodes
    graph = nx.Graph()
    node_ids = pd.read_csv('../datasets/6dfb/SDFB_people.csv')['SDFB Person ID']
    id_to_ind = dict(zip(node_ids, range(len(node_ids))))
    for i in id_to_ind.values():
        graph.add_node(i)
    
    # edges
    fnames = ['SDFB_relationships_100120001_100140000.csv',
              'SDFB_relationships_100100001_100120000.csv',
              'SDFB_relationships_100000000_100020000.csv',
              'SDFB_relationships_100080001_100100000.csv',
              'SDFB_relationships_100040001_100060000.csv',
              'SDFB_relationships_100020001_100040000.csv',
              'SDFB_relationships_greater_than_100180000.csv',
              'SDFB_relationships_100160001_100180000.csv',
              'SDFB_relationships_100140001_100160000.csv',
              'SDFB_relationships_100060001_100080000.csv']
    
    for fname in fnames:
        edges = pd.read_csv('../datasets/6dfb/' + fname)[['Person 1 ID', 'Person 2 ID']]
        for ind, edge in edges.iterrows():
            v1, v2 = edge['Person 1 ID'], edge['Person 2 ID']
            try:
                v1, v2 = id_to_ind[v1], id_to_ind[v2]
                graph.add_edge(v1, v2)
            except:
                continue
    return graph


# строит граф связей по датасету твиттера
def parse_twitter():
    # nodes
    graph = nx.Graph()
    node_ids = list(map(int, open('../datasets/twitter_dataset/user_list.txt', "r").readlines()))
    for i in range(len(node_ids)):
        graph.add_node(i)

    # edges
    for line in open('../datasets/twitter_dataset/graph_cb.txt', "r"):
        v1, v2, unused = map(int, line.split())
        try:
            graph.add_edge(v1, v2)
        except BaseException as exp:
            print("Exception:", exp)
            continue
    return graph


# строит график зависимости средней степени соседей от степени вершины
def assort_plot(graph):
    deg_sum = dict()
    deg_cnt = dict()
    for v1 in graph.nodes:
        nb_degs = [graph.degree(v2) for v2 in graph.neighbors(v1)]
        if len(nb_degs) == 0:
            continue
        cur_deg = graph.degree(v1)
        mean_deg = sum(nb_degs) / len(nb_degs)
        try:
            deg_sum[cur_deg] += mean_deg
            deg_cnt[cur_deg] += 1
        except:
            deg_sum[cur_deg] = mean_deg
            deg_cnt[cur_deg] = 1
    degs = sorted(list(deg_cnt.keys()))
    nb_degs = [deg_sum[i] / deg_cnt[i] for i in degs]
    plt.plot(degs, nb_degs)


# вспомогательная функция для label propagation
def find_components(graph, labels):
    n_nodes = graph.number_of_nodes()
    answer = [-1 for i in range(n_nodes)]
    n_components = 0
    for i in range(n_nodes):
        if answer[i] != -1:
            continue
        stack = [i]
        answer[i] = n_components
        while len(stack) > 0:
            v_1 = stack.pop()
            for v_2 in graph.neighbors(v_1):
                if answer[v_2] == -1 and labels[v_1] == labels[v_2]:
                    answer[v_2] = n_components
                    stack.append(v_2)
        n_components += 1

    return answer


# алгоритм label propagation
def label_propagation(graph, max_iter=100, min_delta=1, output=False):
    nodes_n = graph.number_of_nodes()
    labels = list(range(nodes_n))
    deltas = []
    for iter_n in range(max_iter):
        new_labels = labels.copy()
        delta = 0
        for v in range(nodes_n):
            cnt = Counter(labels[u] for u in graph.neighbors(v))
            try:
                new_labels[v] = cnt.most_common(1)[0][0]
            except:
                continue
            if new_labels[v] != labels[v]:
                delta += 1
        labels = new_labels
        deltas.append(delta)
        if delta < min_delta:
            break
        if iter_n % 5 == 4 and output:
            print("iteration number", iter_n + 1, "done")
    if output:
        print(iter_n + 1, 'iterations')
    labels = find_components(graph, labels)
    return labels, deltas


# генерация графа с сообществами через SBM
def gen_sbm_graph(sizes, prob_community, prob_other):
    block_ids = []
    n_blocks = len(sizes)
    n_nodes = sum(sizes)
    graph = nx.Graph()
    for i in range(n_nodes):
        graph.add_node(i)
    for i in range(n_blocks):
        for shift in range(sizes[i]):
            block_ids.append(i)
    for i in range(n_nodes):
        for j in range(i + 1, n_nodes):
            i1, j1 = block_ids[i], block_ids[j]
            if i1 == j1:
                prob = prob_community
            else:
                prob = prob_other
            if random() < prob:
                graph.add_edge(i, j)
    return graph


def sizes_to_partition(sizes):
    ans = []
    for ind, size in enumerate(sizes):
        ans += [ind] * size
    return ans


def get_sets(partition):
    groups = set(partition)
    new_ind = dict(zip(groups, range(len(groups))))
    ans = dict(zip(range(len(groups)), [set() for i in groups]))
    for i, ind in enumerate(partition):
        ans[new_ind[ind]].add(i)
    return ans


# label propagation, но на с++ и с весами
def w_label_prop(graph, max_iter = 100, min_delta = 1, change_w = None):
    with open("../tmp_files/in.txt", "w") as f:
        print(graph.number_of_nodes(), graph.number_of_edges(), file=f)
        for edge in graph.edges(data=True):
            v_1, v_2, e_data = edge
            try:
                w = e_data['weight']
            except:
                w = 1
            if change_w is not None:
                w = change_w(w)
            print(v_1, v_2, w, file=f)
    system("../exe/label_prop %d %d <../tmp_files/in.txt >../tmp_files/out.txt 2>../tmp_files/err.txt" % (max_iter, min_delta))
    with open("../tmp_files/out.txt", "r") as f:
        labels, deltas = f.readlines()
        labels = list(map(int, labels.split()))
        deltas = list(map(int, deltas.split()))
    return labels, deltas


# метрика качества разбиения
def quality_of_partition(true_partition, partition):
    sets_1 = get_sets(true_partition)
    sets_2 = get_sets(partition)
    graph = nx.Graph()
    n_nodes = len(partition)
    for i in range(2 * n_nodes):
        graph.add_node(i)
    for i in sets_1:
        for j in sets_2:
            w = len(sets_1[i].intersection(sets_2[j]))
            graph.add_edge(i, j + n_nodes, weight=w)
    matching = nx.algorithms.matching.max_weight_matching(graph)
    ans = 0
    for i, j in matching:
        i, j = min(i, j), max(i, j)
        ans += len(sets_1[i].intersection(sets_2[j - n_nodes]))
    return ans / len(partition)


def gen_data_graphs(n, dim, random_state=0, with_w=False):
    print(int(with_w))
    system("../exe/gen_data_graphs %d %d %d %d >../tmp_files/out.txt" % (n, dim, random_state, int(with_w)))
    lines = open('../tmp_files/out.txt').readlines()

    data = []
    for i in range(n):
        data.append(list(map(float, lines[i].split())))
    graphs = []
    for gr_i in range(4):
        graph = nx.Graph()
        for i in range(n):
            array = lines[n * (gr_i + 1) + i].split()
            if with_w:
                nodes = map(int, array[::2])
                weights = map(float, array[1::2])
                for j, w in zip(nodes, weights):
                    graph.add_edge(i, j, weight=w)
            else:
                for j in map(int, array):
                    graph.add_edge(i, j)
        graphs.append(graph)
    return data, graphs
