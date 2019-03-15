#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

using std::vector;
using std::min;

float calc_dist(const vector<float> &v1, const vector<float> &v2) {
    float ans = 0.;
    int d = v1.size();
    for (int i = 0; i < d; ++i) {
        float delta = v1[i] - v2[i];
        ans += delta * delta;
    }
    return ans;
}

vector<vector<int>> eps_graph(const vector<vector<float>> &data, double max_dist) {
    int n = data.size();
    vector<vector<int>> ans(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (calc_dist(data[i], data[j]) < max_dist) {
                ans[i].push_back(j);
                ans[j].push_back(i);
            }
        }
    }
    return ans;
}

vector<vector<int>> influence_graph(const vector<vector<float>> &data) {
    vector<float> min_dist(data.size(), 1e10);
    vector<vector<int>> ans(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < i; ++j) {
            float cur_dist = calc_dist(data[i], data[j]);
            min_dist[i] = std::min(min_dist[i], cur_dist);
            min_dist[j] = std::min(min_dist[j], cur_dist);
        }
    }
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < i; ++j) {
            if (calc_dist(data[i], data[j]) < min_dist[i] + min_dist[j]) {
                ans[i].push_back(j);
                ans[j].push_back(i);
            }
        }
    }
    return ans;
}

vector<vector<float>> read_data(size_t n, size_t d, const char *path) {
    std::ifstream in(path);
    vector<vector<float>> data(n, vector<float>(d));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < d; ++j) {
            in >> data[i][j];
        }
    }
    return data;
}

void write_graph(const vector<vector<int>> &graph, const vector<vector<float>> &data, bool with_w = false) {
    int v_num = graph.size();
    for (int i = 0; i < v_num; ++i) {
        for (int j : graph[i]) {
            std::cout << j << ' ';
            if (with_w) {
                std::cout << calc_dist(data[i], data[j]) << ' ';
            }
        }
        std::cout << std::endl;
    }
}

#define BUILD_WRITE(F) graph = F(data); write_graph(graph, data, with_w); std::cerr << "done" << std::endl;

int main(int argc, char **argv) {
    if (argc != 4 && argc != 5) {
        return 1;
    }
    int n = atoi(argv[1]);
    int d = atoi(argv[2]);
    char *path = argv[3];
    double with_w = false;

    vector<vector<float>> data = read_data(n, d, path);
    vector<vector<int>> graph;
    BUILD_WRITE(influence_graph);
}
