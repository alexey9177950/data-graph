#include <algorithm>
#include <iostream>
#include <vector>

using std::vector;

vector<vector<float>> gen_2_clusters(int n, int d) {
    vector<vector<float>> ans(n, vector<float>(d));
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

float calc_dist(const vector<float> &v1, const vector<float> &v2) {
    double ans = 0.;
    int d = v1.size();
    for (int i = 0; i < d; ++i) {
        float delta = v1[i] - v2[i];
        ans += delta * delta;
    }
    return ans;
}

vector<vector<int>> nn_graph(const vector<vector<float>> &data) {
    int n = data.size();
    vector<vector<int>> ans(n);
    for (int i = 0; i < n; ++i) {
        int best_j = (i + 1) % n;
        float min_dist = 1e10;
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                continue;
            }
            float dist = calc_dist(data[i], data[j]);
            if (dist < min_dist) {
                best_j = j;
                min_dist = dist;
            }
        }
        if (ans[i].size() == 0 || ans[i][0] != best_j) {
            ans[i].push_back(best_j);
            ans[best_j].push_back(i);
        }
    }
    return ans;
}

struct Edge {
    int v1;
    int v2;
    float dist;
};

bool operator<(const Edge &e1, const Edge &e2) {
    return e1.dist < e2.dist;
}

vector<vector<int>> mst_graph(const vector<vector<float>> &data) {
    vector<vector<int>> ans(data.size());
    vector<int> comp;
    vector<Edge> edges;
    int n = data.size();
    for (int i = 0; i < n; ++i) {
        comp.push_back(i);
    }
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            edges.push_back(Edge{i, j, calc_dist(data[i], data[j])});
        }
    }
    std::sort(edges.begin(), edges.end());    
    for (const Edge &e : edges) {
        if (comp[e.v1] != comp[e.v2]) {
            ans[e.v1].push_back(e.v2);
            ans[e.v2].push_back(e.v1);
            int cmp_ind = comp[e.v2];
            for (int i = 0; i < n; ++i) {
                if (comp[i] == cmp_ind) {
                    comp[i] = comp[e.v1];
                }
            }
        }
    }
    return ans;
}


bool check_rng(size_t ind_1, size_t ind_2, const vector<vector<float>> &data) {
    double rad = calc_dist(data[ind_1], data[ind_2]);
    for (size_t i = 0; i < data.size(); ++i) {
        if (i == ind_1 || i == ind_2) {
            continue;
        }
        if (calc_dist(data[ind_1], data[i]) < rad && calc_dist(data[ind_2], data[i]) < rad) {
            return false;
        }
    }
    return true;
}


vector<vector<int>> rng_graph(const vector<vector<float>> &data) {
    vector<vector<int>> ans(data.size());
    int n = data.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (check_rng(i, j, data)) {
                ans[i].push_back(j);
                ans[j].push_back(i);
            }
        }
    }
    return ans;
}

bool check_gabriel(size_t ind_1, size_t ind_2, const vector<vector<float>> &data) {
    int d = data[0].size();
    vector<float> mid(d);
    for (int i = 0; i < d; ++i) {
        mid[i] = (data[ind_1][i] + data[ind_2][i]) / 2.;
    }
    float rad = calc_dist(data[ind_1], data[ind_2]) / 4.;
    for (size_t i = 0; i < data.size(); ++i) {
        if (i != ind_1 && i != ind_2 && calc_dist(mid, data[i]) < rad) {
            return false;
        }
    }
    return true;
}

vector<vector<int>> gabriel_graph(const vector<vector<float>> &data) {
    vector<vector<int>> ans(data.size());
    int n = data.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (check_gabriel(i, j, data)) {
                ans[i].push_back(j);
                ans[j].push_back(i);
            }
        }
    }
    return ans;
}


vector<vector<int>> triangulation_graph(const vector<vector<float>> &data) {
    // TODO
    return vector<vector<int>>();
}

void write_data(const vector<vector<float>> &data) {
    std::cout << std::fixed;
    std::cout.precision(7);
    for (const auto &vec : data) {
        for (const float &x : vec) {
            std::cout << x << ' ';
        }
        std::cout << std::endl;
    }
}

void write_graph(const vector<vector<int>> &graph) {
    int v_num = graph.size();
    for (int i = 0; i < v_num; ++i) {
        for (int j : graph[i]) {
            std::cout << j << ' ';
        }
        std::cout << std::endl;
    }
}

#define BUILD_WRITE(F) graph = F(data); write_graph(graph); std::cerr << "done" << std::endl;

int main(int argc, char **argv) {
    if (argc != 4) {
        return 1;
    }
    int n = atoi(argv[1]);
    int d = atoi(argv[2]);
    srand(atoi(argv[3]));
    vector<vector<float>> data = gen_2_clusters(n, d);
    write_data(data);
    std::cerr << "data written" << std::endl;

    vector<vector<int>> graph;
    BUILD_WRITE(nn_graph);
    BUILD_WRITE(mst_graph);
    BUILD_WRITE(rng_graph);
    BUILD_WRITE(gabriel_graph);
    //BUILD_WRITE(tr_gr);
}
