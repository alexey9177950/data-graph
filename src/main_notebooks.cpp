#include <iostream>
#include <vector>

#include "gen_data_graphs.cpp"

using std::vector;

FastMatrix gen_2_clusters(int n, int d) {
    FastMatrix ans(n, d);
    for (int i = 0; i < n; ++i) {
        float norm = 0;
        for (int j = 0; j < d; ++j) {
            ans[i][j] = -0.5 + float(rand()) / float(RAND_MAX);
            norm += ans[i][j] * ans[i][j];
        }
        if (norm > 1) {
            --i;
        } else {
            if (i < n / 2) {
                ans[i][0] += 1.;
            } else {
                ans[i][0] -= 1.;
            }
        }
    }
    return ans;
}

void write_data(const FastMatrix &data) {
    std::cout << std::fixed;
    std::cout.precision(7);
    for (size_t i = 0; i < data.n_rows_; ++i) {
        for (size_t j = 0; j < data.n_cols_; ++j) {
            std::cout << data[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

void write_graph(const vector<vector<int>> &graph, const FastMatrix &data, bool with_w = false) {
    int v_num = graph.size();
    for (int i = 0; i < v_num; ++i) {
        for (int j : graph[i]) {
            std::cout << j << ' ';
            if (with_w) {
                std::cout << calc_dist(data[i], data[j], data.n_rows_) << ' ';
            }
        }
        std::cout << std::endl;
    }
}

#define BUILD_WRITE(F) graph = F(data); write_graph(graph, data, with_w); std::cerr << "done" << std::endl;
#define BUILD_WRITE_1(F, arg) graph = F(data, arg); write_graph(graph, data, with_w); std::cerr << "done" << std::endl;

int main(int argc, char **argv) {
    if (argc != 6) {
        return 1;
    }
    int n = atoi(argv[1]);
    int d = atoi(argv[2]);
    srand(atoi(argv[3]));
    bool with_w = atoi(argv[4]);
    int n_nb = atoi(argv[5]);

    FastMatrix data = gen_2_clusters(n, d);
    write_data(data);
    std::cerr << "data written" << std::endl;

    vector<vector<int>> graph;
    BUILD_WRITE_1(knn_graph, n_nb);
    BUILD_WRITE_1(eps_graph, n_nb * data.n_rows_);
    BUILD_WRITE(influence_graph);
    BUILD_WRITE(rng_graph);
    BUILD_WRITE(gabriel_graph);
    //BUILD_WRITE(tr_graph);
    //BUILD_WRITE(mst_graph);
}
