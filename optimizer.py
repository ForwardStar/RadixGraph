import math, sys, os

A = None

def p(n, i, a):
    tmp = 0
    for j in range(i):
        tmp += a[j]
    return 1 - (1 - 1 / 2 ** tmp) ** n

def E(n, i, a):
    tmp = 0
    for j in range(i):
        tmp += a[j]
    return 2 ** tmp * p(n, i, a)

def objective(n, d, a):
    res = 0
    for i in range(d):
        res += 2 ** a[i] * E(n, i, a)
    return res

def nextc(n, x, y):
    a = (1 - y) ** (n - 1)
    if a == 1:
        a = 1 - y * (n - 1)
    b = (1 - x) ** n
    if b == 1:
        b = 1 - x * n
    if a == 0 or b == 1:
        return -1
    return y ** 2 * n * a / (1 - b)

def solve_C(n, u, d):
    c1 = None
    cd = None
    for i in range(1, u + 1):
        x = 1.0
        y = 1 / 2 ** i
        flag = False
        for _ in range(d - 1):
            tmp = y
            y = nextc(n, x, y)
            x = tmp
            if y < 1 / 2 ** u * 1e-5:
                flag = True
                break
        if flag or y > 1 / 2 ** u:
            continue
        if cd is None:
            cd = 1 / 2 ** u - y
            c1 = 1 / 2 ** i
        elif 1 / 2 ** u - y < cd:
            cd = 1 / 2 ** u - y 
            c1 = 1 / 2 ** i
    return c1

def find_optimal(n, u, now, d, a):
    if now == d:
        if u == 0:
            global A
            if A is None or objective(n, d, a) < objective(n, d, A):
                A = a[:]
        return
    
    if u == 0:
        return
    
    if now == d - 1:
        a.append(u)
        find_optimal(n, 0, now + 1, d, a)
        a.pop()
        return
    
    for i in range(1, u + 1):
        a.append(i)
        find_optimal(n, u - i, now + 1, d, a)
        a.pop()

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
            u = math.ceil(math.log2(max_vid))
            g.close()
        f.close()
    print("n =", n)
    print("log(u) =", u)
else:
    n = int(input("n = "))
    u = int(input("log(u) = "))
d = int(input("d = "))
c0 = 1
c1 = solve_C(n, u, d)
a = []
for i in range(d):
    a.append(math.ceil(math.log2(c0 / c1)))
    tmp = c1
    c1 = nextc(n, c0, c1)
    c0 = tmp
s = sum(a)
for i in range(u, s):
    a_opt = None
    for j in range(len(a)):
        if a[j] == 1:
            continue
        a_new = a[:]
        a_new[j] -= 1
        if a_opt is None or objective(n, d, a_new) < objective(n, d, a_opt):
            a_opt = a_new[:]
    a = a_opt[:]

with open("settings", "w") as f:
    s = str(d) + "\n"
    for i in a:
        s += str(i) + ' '
    f.writelines(s)
# print("Approximate solution by KKT conditions:", a)
# print("Objective =", objective(n, d, a))
# find_optimal(n, u, 0, d, [])
# print("Exact solution:", A)
# print("Objective = ", objective(n, d, A))
# a_base = [math.ceil(u / d) for _ in range(d)]
# print("Baseline:", a_base)
# print("Objective =", objective(n, d, a_base))