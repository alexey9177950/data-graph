#include <algorithm>
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

vector<int> CNM(const Graph &graph) {
    // TODO
}

int assign_label(const Graph &graph, const vector<int> &labels, int ind) {
    if (graph[ind].empty()) {
        return labels[ind];
    }
    std::unordered_map<int, float> comm_counts;
    for (const Edge &e : graph[ind]) {
        comm_counts[labels[e.ind]] += e.w;
    }
    int max_ind = labels[ind];
    float max_val = -1.;
    int max_cnt = 0;
    for (const std::pair<int, float> key_val : comm_counts) {
        if (key_val.second > max_val - 1e-5) {
            if (key_val.second > max_val + 1e-5) {
                max_val = key_val.second;
                max_ind = key_val.first;
                max_cnt = 1;
            } else {
                max_cnt += 1;
                if (rand() % max_cnt == 0) {
                    max_ind = key_val.first;
                }
            }
        }
    }
    return max_ind;
}

LabelPropRes label_prop(const Graph &graph, int max_iter, int min_delta, int async=false) {
    srand(time(NULL));
    vector<int> labels(graph.size());;
    for (size_t i = 0; i < graph.size(); ++i) {
        labels[i] = i;
    }
    vector<int> deltas;
    int iter_num;
    std::vector<int> shuffle;
    std::vector<int> new_labels;
    if (async) {
        shuffle.resize(graph.size());
        for (size_t i = 0; i < graph.size(); ++i) {
            shuffle[i] = i;
        }
    } else {
        new_labels.resize(graph.size());
    }
    for (iter_num = 1; iter_num <= max_iter; ++iter_num) {
        int delta = 0;
        if (async) {
            std::random_shuffle(shuffle.begin(), shuffle.end());
            for (int i : shuffle) {
                int new_label = assign_label(graph, labels, i);
                if (new_label != labels[i]) {
                    ++delta;
                    labels[i] = new_label;
                }
            }
        } else {
            std::cerr << "here I am" << std::endl;
            for (size_t i = 0; i < graph.size(); ++i) {
                int new_label = assign_label(graph, labels, i);
                if (new_label != labels[i]) {
                    ++delta;
                }
                new_labels[i] = new_label;
            }
            std::swap(labels, new_labels);
        }
        std::cerr << iter_num << " iters" << std::endl;
        deltas.push_back(delta);
        if (delta < min_delta) {
            break;
        }
    }
    return std::make_pair(labels, deltas);
}
