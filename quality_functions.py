from collections import Counter
from random import shuffle

def global_density(partition, graph): 
    comm_sizes = Counter(partition)
    internal, external = len(partition), 0
    for v_1, v_2, data in graph.edges(data=True):
        if 'weight' in data.keys():
            w = data['weight']
        else:
            w = 1
        if partition[v_1] == partition[v_2]:
            internal += 2 * w
        else:
            external += 2 * w
    int_norm, ext_norm = 0, 0
    for i in comm_sizes.values():
        int_norm += i * i
        ext_norm += i * (len(partition) - i)
    internal /= int_norm
    external /= ext_norm
    return 0.5 * (internal + 1 - external)


def local_density(partition, graph):
    comm_sizes = Counter(partition)
    internal = dict((i, 0) for i in comm_sizes.keys())
    external = internal.copy()
    for v_1, v_2, data in graph.edges(data=True):
        if 'weight' in data.keys():
            w = data['weight']
        else:
            w = 1
        c_1, c_2 = partition[v_1], partition[v_2]
        if c_1 == c_2:
            internal[c_1] += 2 * w
        else:
            external[c_1] += w
            external[c_2] += w
    for i, cnt in comm_sizes.items():
        internal[i] += cnt
        internal[i] /= cnt
        external[i] /= len(partition) - cnt
    return 0.5 + (sum(internal.values()) - sum(external.values())) / (2 * len(partition))


def distance_based(partition, graph):
    ans = sum(i ** 2 for i in Counter(partition).values())
    for v_1, v_2 in graph.edges():
        c_1, c_2 = partition[v_1], partition[v_2]
        if c_1 == c_2:
            ans -= 2
        else:
            ans += 2
    ans -= len(partition)
    ans /= len(partition) ** 2
    return 1 - ans


def node_membership(partition, graph):
    comm_sizes = Counter(partition)
    internal = dict((i, 0) for i in range(len(partition)))
    external = internal.copy()
    for v_1, v_2 in graph.edges():
        c_1, c_2 = partition[v_1], partition[v_2]
        if c_1 == c_2:
            internal[v_1] += 1
            internal[v_2] += 1
        else:
            external[v_1] += 1
            external[v_2] += 1
    int_sum, ext_sum = 0, 0
    for i in range(len(partition)):
        comm_s = comm_sizes[partition[i]]
        int_sum += (internal[i] + 1) / comm_s
        if comm_s < len(partition):
            ext_sum += external[i] / (len(partition) - comm_s)
        
    return 0.5 + (int_sum - ext_sum) / (2* len(partition))
 

def eval_functions(partition, graph):
    functions = [global_density, local_density, distance_based, node_membership]
    print("UNSHUFFLED")
    for f in functions:
        print(f.__name__, ":", f(partition, graph))
    print("SHUFFLED")
    s_partition = partition.copy()
    shuffle(s_partition)
    for f in functions:
        print(f.__name__, ":", f(s_partition, graph))


from random import randint, choice

def minimize(graph, f=global_density, max_iter=5 * 10**3, max_wait=20):
    n = graph.number_of_nodes()
    cur_wait = 0
    partition = [i for i in range(n)]
    max_ind = len(partition)
    history = []
    for iter_n in range(max_iter):
        if cur_wait > max_wait:
            break
        node = randint(0, n - 1)
        old_c = partition[node]
        new_c = choice(list(set(partition)))
        if new_c == old_c:
            new_c = max_ind
            max_ind += 1
        old_f = f(partition, graph)
        partition[node] = new_c
        new_f = f(partition, graph)
        if new_f > old_f:
            cur_wait += 1
            partition[node] = old_c
        else:
            cur_wait = 0
        history.append(old_f)
    return partition, history
