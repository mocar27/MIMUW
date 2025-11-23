# Lab04 - SAT solving in Python with Z3

# Example (long conjunctions and disjunctions)

```python id=ec237a8d-5061-4a90-a78a-e754f67c18b1
from z3 import *

n = 10
x = [Bool('x' + str(k)) for k in range(n)]
phi = And([x[k] for k in range(n)])
print(phi)

solve(phi) # satisfiable
```

# Exercise (The SCS problem)

Let $\Sigma = \{0, 1\}$ be a finite alphabet. The *shortest common superstring* problem (SCS) has as input a set of finite strings $S = \{w_1, \dots, w_m\}$ with $w_i \in \Sigma^*$ and asks for the smallest n such that there exists a string $w \in \Sigma^n$ of length $n$ s.t. $w$ is a (contiguous) superstring  of $w_1, \dots, w_m
$. The SCS problem is NP-complete. As there is a trivial upper bound - sum of lengths of strings in S - the most important question is the decision problem whether such a superstring $w \in \Sigma^n$ of length $n$ exists for a given $n$, further referred to as CS(n).

1. Encode the CS(n) problem as a SAT problem and use Z3 to solve it.
2. Find the smallest n using a binary search algorithm.

```python id=b7644971-9475-4027-b06a-3c03463ff9ad
from z3 import *

# input example
S = ["001010011101", "1001010101011", "101101010101010",
     "10111011001010101010", "10110101011010101010", "10110101101010101010"]
m = len(S)
maxn = sum(map(len, S)) 
minn = 0

# how to declare a list of n variables x0, ..., x(n-1)
x = [ Bool('x' + str(k)) for k in range(maxn) ]

# this procedure returns an assignment (as a dictionary) if any exists
def model(phi):
    # create a SAT instance
    s = Solver()
    s.add(phi)
    # return a satisfying assignment
    return s.model() if s.check() == sat else {}

def cs(n):
  if n == maxn: # it is so simple for n == maxn only :)
    s = "".join(S)
    phi = And([x[k] if s[k]=="1" else Not(x[k]) for k in range(len(s))])
    return model(phi)
  else:
    constraints = []
    for w_i in S:
        L = len(w_i)
        options = []
        for j in range(n - L + 1):
            match = [
                x[j + k] if w_i[k] == '1' else Not(x[j + k])
                for k in range(L)
            ]
            options.append(And(match))
        constraints.append(Or(options))
    
    phi = And(constraints)
    return model(phi)

def binary_search_scs():
    l, r = minn, maxn
    
    while l < r:
        mid = (l + r) // 2
        m = cs(mid)
        if m:
            r = mid
        else:
            l = mid + 1
            
    return l
  
solution = cs(maxn)
print([(var, solution[var]) for var in x[:maxn]]) # not very interesting to see :)
w = "".join(("1" if solution[var] else "0" for var in x[:maxn]))
print("Solution:", w)
print("Check: ", [(w_i in w) for w_i in S])
print("Smallest n:", binary_search_scs())
```

# Exercise (Phase transition)

In this exercise we explore the phenomenon of *phase transition* for the $k$-SAT problem, where $k$ is the size of each clause. Let $n$ be the number of variables $X = \{x_1, \dots, x_n\}$ and let $m$ be the number of clauses. A random $k$-SAT instance is obtained by producing each of the $m$ clauses according to the following process:

* Extract $k$ variables without replacement from $X$ and determine independently and uniformly whether each variable appears positively or negatively.

Fix $k = 3$, $n = 10$, and let $p_{m/n}$ be the probability that a SAT instance randomly generated as above is satisfiable.

1. For a fixed $m$, compute an estimate $\hat p_{m / n}$ by sampling $N = 100$ instances as above.
2. Plot the estimates $\hat p_{m / n}$ as a function of $m \in \{1, \dots, 10 \cdot n \}$.
3. What is an "interesting" interval for $m$? Refine the interval above if necessary.

```python id=06d0dd56-541a-4aad-9853-e0683dcb27e9
%matplotlib inline
import matplotlib.pyplot as plt
import numpy as np
import random

# number of variables
n = 10

# size of a clause
k = 3

# number of samples per point
N = 100

variables = [Bool('x' + str(i)) for i in range(n)]

# Hints:
# np.random.choice(list) returns a random element from list
# np.random.choice(list, k, replace=False) returns k random elements from list *without replacement*
# np.mean(list) computes the average of the numbers in the list
# plt.plot(list) generates a plot from a list of values
# plt.show() displays the plot

probabilities = []

def is_satisfiable(phi):
    s = Solver()
    s.add(phi)
    return s.check() == sat
  
for m in range(1, 10 * n + 1):
    results = []
    # 1. Sampling N instances
    for _ in range(N):
        clauses = []
        for _ in range(m):
            vars_sample = np.random.choice(variables, size=k, replace=False)
            clause = []
            for var in vars_sample:
                if random.choice([True, False]):
                    clause.append(var)
                else:
                    clause.append(Not(var))
            clauses.append(Or(clause))
        formula = And(clauses)
        results.append(is_satisfiable(formula))
    
    # Compute the estimate of satisfiability probability
    hat_p = np.mean(results)
    probabilities.append(hat_p)

# 2. Plot the estimates as a function of m
plt.figure(figsize=(10, 6))
plt.plot(range(1, 10 * n + 1), probabilities, marker='o')
plt.xlabel('Number of clauses (m)')
plt.ylabel('Estimated satisfiability probability')
plt.title('Phase transition in random 3-SAT (n=10)')
plt.grid(True)
plt.show()
```

<details id="com.nextjournal.article">
<summary>This notebook was exported from <a href="https://nextjournal.com/a/U8opzoWvZDU4QcAxyweA2?change-id=DqXiMXnsZ4PXTooECuFCgY">https://nextjournal.com/a/U8opzoWvZDU4QcAxyweA2?change-id=DqXiMXnsZ4PXTooECuFCgY</a></summary>

```edn nextjournal-metadata
{:article
 {:settings {:numbered? true},
  :nodes
  {"06d0dd56-541a-4aad-9853-e0683dcb27e9"
   {:compute-ref #uuid "c631077c-59fd-47d3-ae69-9f5004a517b5",
    :exec-duration 262829,
    :id "06d0dd56-541a-4aad-9853-e0683dcb27e9",
    :kind "code",
    :output-log-lines {},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"]},
   "b7644971-9475-4027-b06a-3c03463ff9ad"
   {:compute-ref #uuid "0c034461-853c-439b-95bd-9ea84805a814",
    :exec-duration 4539,
    :id "b7644971-9475-4027-b06a-3c03463ff9ad",
    :kind "code",
    :output-log-lines {:stdout 5},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"]},
   "cfb143c9-8c0c-4b55-bccb-3ac934f59152"
   {:environment
    [:environment
     {:article/nextjournal.id
      #uuid "02da8ea2-3a1d-46a1-81de-711fc447bbcb",
      :change/nextjournal.id
      #uuid "5e9adb34-34b4-417c-8030-fafcb7dd0ec3",
      :node/id "bab9f562-df84-4d17-9df0-d2b0a87cb385"}],
    :id "cfb143c9-8c0c-4b55-bccb-3ac934f59152",
    :kind "runtime",
    :language "python",
    :type :jupyter},
   "ec237a8d-5061-4a90-a78a-e754f67c18b1"
   {:compute-ref #uuid "4b534661-5741-4edf-a7f6-f17252a13bda",
    :exec-duration 590,
    :id "ec237a8d-5061-4a90-a78a-e754f67c18b1",
    :kind "code",
    :output-log-lines {:stdout 12},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"],
    :stdout-collapsed? false}},
  :nextjournal/id #uuid "03c9e4ac-41b2-4c50-957b-fd7fcf5d264f",
  :article/change
  {:nextjournal/id #uuid "67f44add-3e73-4b94-84ae-f601c79b7b11"}}}

```
</details>
