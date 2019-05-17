#include <iostream>

#include "community_detection.cpp"

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

void output_lp(const LabelPropRes &res) {
    for (int i : res.first) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    for (int i : res.second) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}

void output_cnm(const vector<int> &labels) {
    // TODO
}

int main(int argc, char **argv) {
    int alg = atoi(argv[1]);
    if (alg == 1) {
        std::cerr << "running label propagation" << std::endl;
        int max_iter = atoi(argv[2]);
        int min_delta = atoi(argv[3]);
        int async;
        if (argc >= 5) {
            async = atoi(argv[4]);
        } else {
            async = false;
        }
        output_lp(label_prop(input(), max_iter, min_delta, async));
    } else {
        std::cerr << "running CNM" << std::endl;
        output_cnm(CNM(input()));
    }
    return 0;
}
