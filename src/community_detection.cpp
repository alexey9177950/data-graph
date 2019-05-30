#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>
#include <unordered_map>
#include <map>
#include <set>

struct Edge {
    int ind;
    float w;
};

using std::pair;
using std::vector;
using std::map;
using std::set;
using Graph = vector<vector<Edge>>;
using LabelPropRes = std::pair<vector<int>, vector<int>>;

vector<int> get_labels(vector<pair<int, int>> &unions, int n_vert) {
    vector<int> labels(n_vert, -1);
    for (int i = 0; i < n_vert; ++i) {
        labels[i] = i;
    }
    for (int i = unions.size() - 1; i >= 0; --i) {
        labels[unions[i].second] = labels[unions[i].first];
    }
    return labels;
}

void delete_verticle(vector<map<int, double>> &comm_graph, int i_delete) {
    for (auto pr : comm_graph[i_delete]) {
        comm_graph[pr.second].erase(i_delete);
    }
    comm_graph[i_delete].clear();
}

vector<int> CNM(const Graph &graph, int max_iter=2'000'000) {
    int n_vert, n_edge;
    // fractions of edges of each community, multiplied by 2*m
    vector<int> fractions;
    // weighted graph of communities
    vector<map<int, double>> comm_graph;
    // vector of maps j -> delta_Q_ij (multiplied by 4*m^2)
    vector<map<int, double>> delta_Q;
    // heap of pairs: (delta_Q_ij, (i, j)), j = argmax k delta_Q_ik
    set<pair<double, pair<int, int>>> max_elems;
    // pairs of united communities
    vector<pair<int, int>> unions;

    // INIT
    n_vert = graph.size();
    n_edge = 0;
    for (const auto &vec : graph) {
        n_edge += vec.size();
    }
    n_edge /= 2;

    for (int i = 0; i < n_vert; ++i) {
        fractions[i] = graph[i].size();
        if (!graph[i].empty()) {
            int j = graph[i][0].ind;
            max_elems.insert({graph[i][0].w, {i, j}});
        }
        for (auto pr : graph[i]) {
            int j = pr.ind;
            comm_graph[i][j] = pr.w;
            delta_Q[i][j] = 2 * n_edge - graph[i].size() * graph[j].size();
        }
    }

    // STEPS

    for (int i_iter = 1; i_iter < max_iter; ++i_iter) {
        while (max_elems.size() > 0) {
            auto iter = max_elems.begin();
            int i = iter->second.first, j = iter->second.second;
            if (iter->first <= 0 || comm_graph[i].empty() || comm_graph[j].empty()) {
                max_elems.erase(max_elems.begin());
            } else {
                break;
            }
        }
        if (max_elems.empty()) {
            break;
        }
        pair<int, pair<int, int>> max_elem = *max_elems.begin();
        int i_delete = max_elem.second.first;
        int i_expand = max_elem.second.second;
        if (comm_graph[i_delete].size() > comm_graph[i_expand].size()) {
            std::swap(i_delete, i_expand);
        }
        set<int> affected_inds;
        for (pair<int, int> pr : comm_graph[i_expand]) {
            affected_inds.insert(pr.first);
        }
        for (pair<int, int> pr : comm_graph[i_delete]) {
            affected_inds.insert(pr.first);
        }
        for (int ind : affected_inds) {
            bool conn_delete = (comm_graph[i_delete].find(ind) != comm_graph[i_delete].end());
            bool conn_expand = (comm_graph[i_expand].find(ind) != comm_graph[i_expand].end());
            int delta;
            if (conn_delete && conn_expand) {
                delta = delta_Q[i_delete][ind] + delta_Q[i_expand][ind];
            } else if (conn_delete) {
                delta = delta_Q[i_delete][ind] - 2 * fractions[i_expand] * fractions[ind];
            } else {
                delta = delta_Q[i_expand][ind] - 2 * fractions[i_delete] * fractions[ind];
            }
            delta_Q[i_expand][ind] += delta;
            delta_Q[ind][i_expand] += delta;
        }
        delete_verticle(comm_graph, i_delete);
        fractions[i_expand] += fractions[i_delete];

        comm_graph[i_delete].clear();
    }
    return get_labels(unions, n_vert);
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
