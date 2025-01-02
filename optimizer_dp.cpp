#include <bits/stdc++.h>
using namespace std;
 
#define rep(i,a,b) for(int i = a; i < (b); ++i)
#define rrep(i,a,b) for(int i = a; i >= (b); i--)
#define ALL(x) begin(x), end(x)
#define MAX(a,b) a = max(a,b)
#define MIN(a,b) a = min(a,b)
#define sz(a) (int)a.size()

#define int long long
using ll = long long;
using ld = double;
using ii = pair<ll,ll>;
using vl = vector<ll>;
using vvl = vector<vector<ll>>;
using vvld = vector<vector<ld>>;
 
const ll INF = (1ll << 62);
const ld EPS = 1e-9;
const ll MOD = 1e9 + 7;

ld prob(ll u, ll n, ll desc) {
    if (u - desc < n) return 0;
    ld res = 1;
    rep(i, 1, n + 1) {
        res *= (ld)(u - n - desc + i) / (u - n + i);
    }
    return res;
}

signed main(signed argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <file> <layers>" << endl;
        return EXIT_FAILURE;
    }
    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error: Cannot open file " << argv[1] << endl;
        return EXIT_FAILURE;
    }
    ll l = atoi(argv[2]), u = 0, n = 0, num;
    while (infile >> num) {
        n++;
        u = max(u, num);
    }
    u++;
    int logu = ceil(log2(u));
    vvld g(l);
    vvl pre(l);
    rep(i, 0, l) {
        g[i].assign(logu + 1, INF);
        pre[i].assign(logu + 1, -1);
    }
    rep(i, 1, logu + 1) {
        g[0][i] = (1ll << i);
    }
    rep(i, 1, l) {
        rep(j, i, logu + 1) {
            rep(k, i, j) {
                ld cost = (1ll << j) * (1.0 - prob(u, n, (1ll << logu - k)));
                if (g[i - 1][k] + cost < g[i][j]) {
                    g[i][j] = g[i - 1][k] + cost;
                    pre[i][j] = k;
                }
            }
        }
    }
    cout << l << "\n";
    vl s(l);
    ll now = logu;
    rrep(i, l - 1, 0) {
        s[i] = now;
        now = pre[i][now];
    }
    rrep(i, l - 1, 1) {
        s[i] -= s[i - 1];
    }
    rep(i, 0, l) {
        cout << s[i] << " ";
    }
    cout << "\n";
    cout << g[l - 1][logu] << "\n";
    return 0;
}