#include <iostream>
#include <vector>

#include "gen_data_graphs.cpp"

FastMatrix read_data(size_t n_rows, size_t n_cols) {
    FastMatrix ans(n_rows, n_cols);
    for (size_t i = 0; i < n_rows; ++i) {
        for (size_t j = 0; j < n_cols; ++j) {
            std::cin >> ans[i][j];
        }
    }
    return ans;
}

void write_hist(const vector<int> &data) {
    for (int x : data) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;
}

void write_graph(const vector<vector<int>> &graph, const FastMatrix &data, bool with_w = false) {
    int v_num = graph.size();
    for (int i = 0; i < v_num; ++i) {
        for (int j : graph[i]) {
            std::cout << j << ' ';
            if (with_w) {
                std::cout << calc_dist(data[i], data[j], data.n_cols_) << ' ';
            }
        }
        std::cout << std::endl;
    }
}

/*
1 -- eps
2 -- KNN
3 -- RNG
4 -- Gabriel
5 -- influence
*/

int main(int argc, char **argv) {
    // parse arguments
    if (argc != 4 && argc != 5 && argc != 6) {
        return 1;
    }
    int n = atoi(argv[1]);
    int d = atoi(argv[2]);
    int task_type = atoi(argv[3]);
    size_t k;
    if (argc > 4) {
        k = atoi(argv[4]);
    }
    if (argc > 5) {
        LOG_PERIOD = atoi(argv[5]);
    } else {
        LOG_PERIOD = 100;
    }

    // commit task
    FastMatrix data = read_data(n, d);
    vector<vector<int>> graph;
    vector<int> hist;
    switch (task_type) {
    case 1:
        graph = eps_graph_n_edges(data, data.n_rows_ * k);
        break;
    case 2:
        graph = knn_graph(data, k);
        break;
    case 3:
        graph = rng_graph(data);
        break;
    case 4:
        graph = gabriel_graph(data);
        break;
    case 5:
        graph = influence_graph(data);
        break;
    }
    if (task_type == 0) {
        write_hist(hist);
    } else {
        write_graph(graph, data);
    }
    return 0;
}
