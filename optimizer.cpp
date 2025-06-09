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
    ll n = 0, l = 0, u = 0;
    int logu = 0;
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <file> <layers>" << endl;
        cout << "Or input parameters manually:" << endl;
        cout << "n = ";
        cin >> n;
        cout << "log(u) = ";
        cin >> logu;
        cout << "l = ";
        cin >> l;
    }
    else {
        ifstream infile(argv[1]);
        l = atoi(argv[2]);
        if (!infile) {
            cerr << "Error: Cannot open file " << argv[1] << endl;
            return EXIT_FAILURE;
        }
        set<ll> vertices;
        string s;
        while (getline(infile, s)) {
            if (s[0] == '#') continue;
            stringstream ss(s);
            ll num;
            while (ss >> num) {
                u = max(u, num);
                vertices.insert(num);
            }
        }
        u++;
        n = vertices.size();
        logu = ceil(log2(u));
    }
    u = (1ll << logu);
    vvld g(l);
    vvl pre(l);
    rep(i, 0, l) {
        g[i].assign(logu + 1, INF);
        pre[i].assign(logu + 1, -1);
    }
    rep(i, 0, logu + 1) {
        g[0][i] = (1ll << i);
    }
    rep(i, 1, l) {
        rep(j, 0, logu + 1) {
            rep(k, 0, j + 1) {
                ld cost = (1.0 - pow(ld(u - (1ll << logu - k)) / u, n)) * (1ll << j);
                if (g[i - 1][k] + cost >= g[i][j]) continue;
                cost = (1.0 - prob(u, n, (1ll << logu - k))) * (1ll << j);
                if (g[i - 1][k] + cost < g[i][j]) {
                    g[i][j] = g[i - 1][k] + cost;
                    pre[i][j] = k;
                }
            }
        }
    }
    vl s(l);
    ll now = logu;
    rrep(i, l - 1, 0) {
        s[i] = now;
        now = pre[i][now];
    }
    rrep(i, l - 1, 1) {
        s[i] -= s[i - 1];
        if (s[i] == 0) l--;
    }
    if (s[0] == 0) l--;
    rep(i, 0, s.size()) {
        if (s[i]) cout << s[i] << " ";
    }
    cout << "\n";
    cout << g[l - 1][logu] << "\n";
    return 0;
}