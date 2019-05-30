#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>

#include <math.h>
#include <time.h>

using std::vector;

size_t LOG_PERIOD = 10'000;

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
        if (i == ind_1 || i == ind_2) {
            continue;
        }
        if (calc_dist(&mid[0], data[i], d) < rad) {
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

float max_dist(const FastMatrix &data, int64_t n_edges, int n_samples_below=250) {
    double n_nodes = data.n_rows_;
    double n_pairs = (n_nodes * (n_nodes - 1)) / 2;
    std::cerr << "n_edges = " << n_edges << std::endl;
    int n_samples = round(n_pairs * static_cast<double>(n_samples_below) / static_cast<double>(n_edges));
    std::cerr << "n_samples = " << n_samples << std::endl;

    vector<float> samples(n_samples);
    for (int i = 0; i < n_samples; ++i) {
        int v_1, v_2;
        do {
            v_1 = rand() % data.n_rows_;
            v_2 = rand() % data.n_rows_;
        } while (v_1 == v_2);
        samples[i] = calc_dist(data[v_1], data[v_2], data.n_cols_);
    }
    std::sort(samples.begin(), samples.end());
    return sqrt(samples[n_samples_below + 1]);
}

vector<vector<int>> eps_graph(const FastMatrix &data, float eps) {
    F_BEGIN;
    eps *= eps;
    for (size_t i = 0; i < n; ++i) {
        WRITE_LOG;
        for (size_t j = i + 1; j < n; ++j) {
            if (calc_dist(data[i], data[j], d) < eps) {
                ans[i].push_back(j);
            }
        }
    }
    return ans;
}

vector<vector<int>> eps_graph_n_edges(const FastMatrix &data, int n_edges) {
    return eps_graph(data, max_dist(data, n_edges));
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
