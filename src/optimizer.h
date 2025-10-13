#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "utils.h"

double prob(long long u, int n, long long desc) {
    if (u - desc < n) return 0;
    long double res = 1;
    for (int i = 1; i <= n; i++) {
        res *= double(u - n - desc + i) / (u - n + i);
    }
    return res;
}

std::vector<int> OptimizeParameters(int n, int logu, int l) {
    std::vector<std::vector<double>> g(l);
    std::vector<std::vector<int>> pre(l);
    for (int i = 0; i < l; i++) {
        g[i].assign(logu + 1, (1ll << 62));
        pre[i].assign(logu + 1, -1);
    }
    for (int i = 0; i <= logu; i++) {
        g[0][i] = (1ll << i);
    }
    for (int i = 1; i < l; i++) {
        for (int j = 0; j <= logu; j++) {
            for (int k = 0; k <= j; k++) {
                double cost = (1.0 - pow(double((1ll << logu) - (1ll << logu - k)) / (1ll << logu), n)) * (1ll << j);
                if (g[i - 1][k] + cost >= g[i][j]) continue;
                cost = (1.0 - prob(1ll << logu, n, (1ll << logu - k))) * (1ll << j);
                if (g[i - 1][k] + cost < g[i][j]) {
                    g[i][j] = g[i - 1][k] + cost;
                    pre[i][j] = k;
                }
            }
        }
    }
    std::vector<int> s(l);
    int now = logu;
    for (int i = l - 1; i >= 0; i--) {
        s[i] = now;
        now = pre[i][now];
    }
    for (int i = l - 1; i >= 1; i--) {
        s[i] -= s[i - 1];
    }
    return s;
}

#endif