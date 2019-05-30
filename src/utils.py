import networkx as nx
import matplotlib.pyplot as plt
from random import random
from scipy.stats import pearsonr
from collections import Counter
from os import system


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


def w_label_prop(graph, max_iter = 100, min_delta = 1, change_w = None, async_f = 0):
    async_f = int(async_f)
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
    system("../exe/comm_det 1 %d %d %d <../tmp_files/in.txt >../tmp_files/out.txt 2>../tmp_files/err.txt"
            % (max_iter, min_delta, async_f))
    with open("../tmp_files/out.txt", "r") as f:
        labels, deltas = f.readlines()
        labels = list(map(int, labels.split()))
        deltas = list(map(int, deltas.split()))
    return labels, deltas

def CNM_alg(graph, max_iter = 1000, change_w = None):
    async_f = int(async_f)
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
    system("../exe/comm_det 2 %d <../tmp_files/in.txt >../tmp_files/out.txt 2>../tmp_files/err.txt"
            % max_iter)
    return open("../tmp_files/out.txt", "r").readlines()[0]


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


def gen_data_graphs(n, dim, random_state=0, with_w=False, n_nb=5):
    print(int(with_w))
    system("../exe/gen_dg_nb %d %d %d %d %d >../tmp_files/out.txt 2>../tmp_files/err.txt"\
            % (n, dim, random_state, int(with_w), n_nb))
    lines = open('../tmp_files/out.txt').readlines()

    data = []
    for i in range(n):
        data.append(list(map(float, lines[i].split())))
    graphs = []
    for gr_i in range(5):
        graph = nx.Graph()
        for i in range(n):
            graph.add_node(i)
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
