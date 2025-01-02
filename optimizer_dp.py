import math, sys, os

# Input
n = u = 0
if len(sys.argv) > 1:
    ffname = sys.argv[len(sys.argv)-1]
    with open(ffname, 'r') as f:
        root_dir = ffname.split("/")
        root_dir = root_dir[:len(root_dir)-1]
        root_dir = '/'.join(root_dir)
        lines = f.readlines()
        fname = ''
        for s in lines:
            if 'vertex-file' in s:
                fname = s.split(' = ')[1].strip('\n')
        with open(os.path.join(root_dir, fname), 'r') as g:
            lines = g.readlines()
            max_vid = 0
            n = 0
            for s in lines:
                n += 1
                max_vid = max(max_vid, int(s))
            u = max_vid
            g.close()
        f.close()
    print("n =", n)
    print("u =", u)
else:
    n = int(input("n = "))
    u = int(input("u = "))
d = int(input("d = "))

# Dynamic programming
def compute(u, n, desc):
    if u - desc < n:
        return 0
    res = 1
    for i in range(1, n + 1):
        res *= (u - n - desc + i) / (u - n + i)
    return res

INF = 1e20
logu = math.ceil(math.log2(u))
g = [[INF for _ in range(logu + 1)] for _ in range(d)]
decision = [[None for _ in range(logu + 1)] for _ in range(d)]
for j in range(logu + 1):
    g[0][j] = 2 ** j
for i in range(1, d):
    for j in range(i, logu + 1):
        for k in range(i, j):
            cost = 2 ** j * (1 - compute(u, n, 2 ** (logu - k)))
            if g[i - 1][k] + cost < g[i][j]:
                g[i][j] = g[i - 1][k] + cost
                decision[i][j] = k
print("Minimum expected space:", g[d - 1][logu])
A = []
now = logu
for i in range(d - 1, -1, -1):
    A.append(now)
    now = decision[i][now]
A.reverse()
for i in range(len(A) - 1, 0, -1):
    A[i] -= A[i - 1]
print("Optimal setting:", A)

# Output
if len(sys.argv) > 1:
    with open("settings", "w") as f:
        s = str(d) + "\n"
        for i in A:
            s += str(i) + " "
        f.writelines(s)