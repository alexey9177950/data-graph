import pandas as pd
import networkx as nx
import networkx
from os import system
from tqdm import tqdm

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
    
    for fname in tqdm(fnames):
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

# чтение датасета амазона
def parse_amazon(path='../datasets/amazon-meta.txt'):
    fhr = open(path, 'r', encoding='utf-8', errors='ignore')
    amazonProducts = {}
    (Id, ASIN, Copurchased) = ("", "", "")
    for line in tqdm(fhr):
        line = line.strip()
        # a product block started
        if(line.startswith("Id")):
            Id = line[3:].strip()
        elif(line.startswith("ASIN")):
            ASIN = line[5:].strip()
        elif(line.startswith("similar")):
            ls = line.split()
            Copurchased = ' '.join([c for c in ls[2:]])
        elif (line==""):
            try:
                MetaData = {}
                if (ASIN != ""):
                    amazonProducts[ASIN]=MetaData
                MetaData['Copurchased'] = Copurchased
            except NameError:
                continue
            (Id, ASIN, Copurchased) = ("", "", "")
    fhr.close()
    for asin, metadata in tqdm(amazonProducts.items()):
        amazonProducts[asin]['Copurchased'] = \
        ' '.join([cp for cp in metadata['Copurchased'].split() \
            if cp in amazonProducts.keys()])
    copurchaseGraph = networkx.Graph()
    asin_to_ind = dict()
    for asin,metadata in tqdm(amazonProducts.items()):
        if asin not in asin_to_ind:
            asin_to_ind[asin] = len(asin_to_ind)
        copurchaseGraph.add_node(asin_to_ind[asin])
        for a in metadata['Copurchased'].split():
            if a.strip() not in asin_to_ind:
                asin_to_ind[a.strip()] = len(asin_to_ind)
            copurchaseGraph.add_node(asin_to_ind[a.strip()])
            copurchaseGraph.add_edge(asin_to_ind[asin], asin_to_ind[a.strip()])
    return copurchaseGraph
