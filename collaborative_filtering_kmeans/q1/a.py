import sys
import math

f1 = sys.argv[1]
f2 = sys.argv[2]

vec = []
users = {}
movies = {}

f = open(f1)
c1 = f.readlines()
f.close()

for c in c1:
    s = c.strip().split(",")
    if users.get(s[1]) is not None:
        i1 = users.get(s[1])
        l = vec[i1]
    else:
        i1 = len(users)
        users[s[1]] = i1
        vec.append({})
        l = vec[i1]
    if movies.get(s[0]) is not None:
        i0 = movies.get(s[0])
    else:
        i0 = len(movies)
        movies[s[0]] = i0
    l[s[0]] = float(s[2])

means = {}
for i in users.keys():
    sum = 0.0
    k = 0
    for j in vec[users[i]].values():
        sum+=j
        k+=1
    means[users[i]]= sum/k
mean = []
for i in range(len(users)):
    mean.append(means[i])

weights={}
e1 = 0.0
e2 = 0.0
nu = 0
with open(f2) as f:
    m = users.keys()
    for line in f:
        s = line.strip().split(",")
        i1 = users.get(s[1])
        i0 = movies.get(s[0])
        n = 0.0
        d = 0.0
        vec1 = vec[i1].copy()
        mean1 = mean[i1]
        set1 = set(vec1.keys())
        for x in m:
            i = users[x]
            if vec1.get(s[0]) != None:
                vec2 = vec[i].copy()
                mean2 = mean[i]
                i1i = "{},{}".format(i1, i)
                ii1 = "{},{}".format(i,i1)
                if weights.get(i1i) != None:
                    n += weights[i1i] * (vec2[s[0]] - mean2)
                    d += weights[i1i]
                else:
                    b = set(vec2.keys())
                    c = set1.intersection(b)
                    e = 0.0
                    f = 0.0
                    g = 0.0
                    for j in c:
                        e += (vec2[j]-mean2) * (vec1[j]-mean1)
                        f += (vec2[j]-mean2) * (vec2[j]-mean2)
                        g += (vec1[j]-mean1) * (vec1[j]-mean1)
                    if f*g != 0.0:
                        weights[i1i] = e/math.sqrt(f*g)
                        weights[ii1] = weights[i1i]
                    else:
                        weights[i1i] = 0.0
                        weights[ii1] = 0.0
                    n += weights[i1i] * (vec2[s[0]] - mean2)
                    d += weights[i1i].__abs__()
        p = mean[i1] + n/d if d!=0 else mean[i1]
        fs2 = float(s[2])
        e1 += (p - fs2).__abs__()
        e2 += (p - fs2) * (p - fs2)
        nu += 1
        # print("User {} on Movie {}: {}({})".format(s[1], s[0], p, s[2]))
print("Mean absolute error: {}".format(e1/nu))
print("Root mean squared error: {}".format(math.sqrt(e2/nu)))
