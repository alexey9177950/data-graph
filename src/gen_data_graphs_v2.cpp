#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>

#include <math.h>
#include <time.h>

using std::vector;

size_t LOG_PERIOD;

// матрица, хранящая данные в одном массиве
class FastMatrix {
    vector<float> data_;
public:
    const size_t n_rows_, n_cols_;
    FastMatrix(size_t n_rows, size_t n_cols)
        : data_(n_rows * n_cols),
          n_rows_(n_rows),
          n_cols_(n_cols) {
    }

    const float *operator[](size_t row_ind) const {
        return &data_[row_ind * n_cols_];
    }

    float *operator[](size_t row_ind) {
        return &data_[row_ind * n_cols_];
    }
};

FastMatrix read_data(size_t n_rows, size_t n_cols) {
    FastMatrix ans(n_rows, n_cols);
    for (size_t i = 0; i < n_rows; ++i) {
        for (size_t j = 0; j < n_cols; ++j) {
            std::cin >> ans[i][j];
        }
    }
    return ans;
}

// расстояние между двумя векторами размера dim
float calc_dist(const float *v_1, const float *v_2, size_t dim) {
    float ans = 0.;
    for (size_t i = 0; i < dim; ++i) {
        float delta = v_1[i] - v_2[i];
        ans += delta * delta;
    }
    return ans;
}

#define F_BEGIN \
        time_t t_begin = time(NULL); \
        size_t n = data.n_rows_; \
        size_t d = data.n_cols_; \
        vector<vector<int>> ans(n); \
        for (size_t i = 0; i < d; ++i); 

#define WRITE_LOG \
        if ((i + 1) % LOG_PERIOD == 0) { \
            std::cerr << i + 1 << ' ' << time(NULL) - t_begin << std::endl; \
        }
 

// граф k ближайших соседей
// переписать функцию
// + время, логирование и 
vector<vector<int>> knn_graph(const FastMatrix &data, size_t k) {
    F_BEGIN;
    for (size_t i = 0; i < n; ++i) {
        WRITE_LOG;
        std::priority_queue<std::pair<float, size_t>> best_k;
        for (size_t j = 0; j < n; ++j) {
            if (i == j) {
                continue;
            }
            float dist = calc_dist(data[i], data[j], d);
            best_k.push({dist, j});
            if (best_k.size() > k) {
                best_k.pop();
            }
        }
        while (best_k.size()) {
            ans[i].push_back(best_k.top().second);
            best_k.pop();
        }
    }
    return ans;
}

bool check_rng(size_t ind_1, size_t ind_2, const FastMatrix &data) {
    size_t d = data.n_cols_;
    float rad = calc_dist(data[ind_1], data[ind_2], d);
    for (size_t i = 0; i < data.n_rows_; ++i) {
        if (i == ind_1 || i == ind_2) {
            continue;
        }
        if (calc_dist(data[ind_1], data[i], d) < rad && calc_dist(data[ind_2], data[i], d) < rad) {
            return false;
        }
    }
    return true;
}

vector<vector<int>> rng_graph(const FastMatrix &data) {
    F_BEGIN;
    // disable warning
    for (size_t i = 0; i < n; ++i) {
        WRITE_LOG;
        for (size_t j = i + 1; j < n; ++j) {
            if (check_rng(i, j, data)) {
                ans[i].push_back(j);
            }
        }
    }
    return ans;
}

bool check_gabriel(size_t ind_1, size_t ind_2, const FastMatrix &data) {
    int d = data.n_cols_;
    vector<float> mid(d);
    for (int i = 0; i < d; ++i) {
        mid[i] = (data[ind_1][i] + data[ind_2][i]) / 2.;
    }
    float rad = calc_dist(data[ind_1], data[ind_2], d) / 4.;
    for (size_t i = 0; i < data.n_rows_; ++i) {
        if (i != ind_1 && i != ind_2 && calc_dist(&mid[0], data[i], d) < rad) {
            return false;
        }
    }
    return true;
}

vector<vector<int>> gabriel_graph(const FastMatrix &data) {
    F_BEGIN;
    for (size_t i = 0; i < n; ++i) {
        WRITE_LOG;
        for (size_t j = i + 1; j < n; ++j) {
            if (check_gabriel(i, j, data)) {
                ans[i].push_back(j);
            }
        }
    }
    return ans;
}

vector<vector<int>> eps_graph(const FastMatrix &data, float max_dist) {
    F_BEGIN;
    max_dist *= max_dist;
    for (size_t i = 0; i < n; ++i) {
        WRITE_LOG;
        for (size_t j = i + 1; j < n; ++j) {
            if (calc_dist(data[i], data[j], d) < max_dist) {
                ans[i].push_back(j);
            }
        }
    }
    return ans;
}

vector<vector<int>> influence_graph(const FastMatrix &data) {
    F_BEGIN;
    vector<float> min_dist(n, 1e10);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < i; ++j) {
            float cur_dist = calc_dist(data[i], data[j], d);
            min_dist[i] = std::min(min_dist[i], cur_dist);
            min_dist[j] = std::min(min_dist[j], cur_dist);
        }
    }
    for (size_t i = 0; i < n; ++i) {
        WRITE_LOG;
        for (size_t j = i + 1; j < n; ++j) {
            if (calc_dist(data[i], data[j], d) < min_dist[i] + min_dist[j]) {
                ans[i].push_back(j);
            }
        }
    }
    return ans;
}


vector<int> dist_histogram(const FastMatrix &data, size_t n_bins = 50'000, float l_border=-5, float r_border=5) {
    std::vector<int> ans(n_bins, 0);
    size_t n = data.n_rows_;
    size_t d = data.n_cols_;
    time_t t_begin = time(NULL);
    for (size_t i = 0; i < n; ++i) {
        WRITE_LOG;
        for (size_t j = i + 1; j < n; ++j) {
            float dist = log(calc_dist(data[i], data[j], d)) / 2.;
            int64_t bin_ind = round(static_cast<float>(n_bins) * (dist - l_border) / (r_border - l_border));
            if (bin_ind < 0) {
                bin_ind = 0;
            }
            if (bin_ind >= static_cast<int64_t>(n_bins)) {
                bin_ind = n_bins - 1;
            }
            ++ans[bin_ind];
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
0 -- гистограмма
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
    double eps;
    size_t k;
    if (argc > 4) {
        if (task_type == 1) {
            eps = atof(argv[4]);
        }
        if (task_type == 2) {
            k = atoi(argv[4]);
        }
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
    case 0:
        hist = dist_histogram(data);
        break;
    case 1:
        graph = eps_graph(data, eps);
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
