#include <iostream>
#include <vector>
#include <utility>
#include <unordered_map>

struct Edge {
    int ind;
    float w;
};

using std::vector;
using Graph = vector<vector<Edge>>;
using LabelPropRes = std::pair<vector<int>, vector<int>>;

Graph input() {
    int n_nodes, n_edges;
    std::cin >> n_nodes >> n_edges;
    Graph ans(n_nodes);
    for (int i = 0; i < n_edges; ++i) {
        int v_1, v_2;
        float w;
        std::cin >> v_1 >> v_2 >> w;
        ans[v_1].push_back(Edge{v_2, w});
        ans[v_2].push_back(Edge{v_1, w});
    }
    return ans;
}

LabelPropRes label_prop(const Graph &graph, int max_iter, int min_delta) {
    vector<int> labels(graph.size());;
    for (size_t i = 0; i < graph.size(); ++i) {
        labels[i] = i;
    }
    vector<int> deltas;
    int iter_num;
    for (iter_num = 1; iter_num <= max_iter; ++iter_num) {
        vector<int> new_labels(graph.size());
        int delta = 0;
        for (size_t i = 0; i < graph.size(); ++i) {
            if (graph[i].empty()) {
                continue;
            }
            std::unordered_map<int, float> comm_counts;
            for (const Edge &e : graph[i]) {
                comm_counts[labels[e.ind]] += e.w;
            }
            int max_ind = labels[i];
            float max_val = -1.;
            for (const std::pair<int, float> key_val : comm_counts) {
                if (key_val.second > max_val) {
                    max_val = key_val.second;
                    max_ind = key_val.first;
                }
            }
            if (rand() % 10 == 0) {
                max_ind = labels[i];
            }
            new_labels[i] = max_ind;
            if (labels[i] != new_labels[i]) {
                delta += 1;
            }
        }
        std::cerr << iter_num << " iters" << std::endl;
        labels = new_labels;
        deltas.push_back(delta);
        if (delta < min_delta) {
            break;
        }
    }
    return std::make_pair(labels, deltas);
}


void output(const LabelPropRes &res) {
    for (int i : res.first) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    for (int i : res.second) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        return 1;
    }
    int max_iter = atoi(argv[1]);
    int min_delta = atoi(argv[2]);
    output(label_prop(input(), max_iter, min_delta));
    return 0;
}
