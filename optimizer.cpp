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
    // Iterate all parameters and check whether -h is given
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            cout << "Usage: " << argv[0] << " <ID_file> <layers_of_SORT>" << endl;
            cout << "Or input parameters manually: (1) number of distinct IDs; (2) bit length of IDs; (3) layers of SORT." << endl;
            return 0;
        }
    }
    int n, bit_length, l;
    if (argc < 3) {
        cout << "Input number of distinct IDs: ";
        cin >> n;
        cout << "Input bit length of IDs: ";
        cin >> bit_length;
        cout << "Input layers of SORT: ";
        cin >> l;
    }
    else {
        ifstream infile(argv[1]);
        l = atoi(argv[2]);
        if (!infile) {
            cerr << "Error: Cannot open file " << argv[1] << endl;
            return EXIT_FAILURE;
        }
        set<uint64_t> vertices;
        string s;
        uint64_t max_id = 0;
        std::cout << "Reading vertex IDs from " << argv[1] << " ..." << std::endl;
        while (getline(infile, s)) {
            if (s[0] == '#') continue;
            stringstream ss(s);
            uint64_t num;
            while (ss >> num) {
                max_id = max(max_id, num);
                vertices.insert(num);
            }
        }
        infile.close();
        n = vertices.size();
        bit_length = ceil(log2(max_id + 1));
        cout << "Number of distinct IDs: " << n << endl;
        cout << "Bit length of IDs: " << bit_length << endl;
    }
    vvld g(l);
    vvl pre(l);
    rep(i, 0, l) {
        g[i].assign(bit_length + 1, INF);
        pre[i].assign(bit_length + 1, -1);
    }
    rep(i, 0, bit_length + 1) {
        g[0][i] = (1ll << i);
    }
    rep(i, 1, l) {
        rep(j, 0, bit_length + 1) {
            rep(k, 0, j + 1) {
                ld cost = (1.0 - pow(ld((1ll << bit_length) - (1ll << bit_length - k)) / (1ll << bit_length), n)) * (1ll << j);
                if (g[i - 1][k] + cost >= g[i][j]) continue;
                cost = (1.0 - prob((1ll << bit_length), n, (1ll << bit_length - k))) * (1ll << j);
                if (g[i - 1][k] + cost < g[i][j]) {
                    g[i][j] = g[i - 1][k] + cost;
                    pre[i][j] = k;
                }
            }
        }
    }
    vl s(l);
    ll now = bit_length;
    rrep(i, l - 1, 0) {
        s[i] = now;
        now = pre[i][now];
    }
    rrep(i, l - 1, 1) {
        s[i] -= s[i - 1];
    }
    std::ofstream f("settings.txt");
    f << l << std::endl;
    rep(i, 0, s.size()) {
        f << s[i] << " ";
    }
    f << "\n";
    std::cout << "Optimal fanout settings saved to settings.txt" << std::endl;
    std::cout << "Format: <number_of_layers>\\n<fanout_array>" << std::endl;
    return 0;
}