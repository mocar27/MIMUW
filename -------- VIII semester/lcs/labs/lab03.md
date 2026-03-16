# Lab03 - SAT solving with Z3 in Python

We will be using Z3 programmatically in Python. Consider the following classical logic tautologies:

$$

\varphi_1 \equiv\ p \vee \neg p \qquad \textrm{(excluded middle)} \\
\varphi_2 \equiv\ ((p \to q) \to p) \to p \qquad \textrm{(Pierce's law)}
$$
The Z3 code below (via its convenient Python interface) checks that the formulas above are tautologies by trying to find a satisfying assignment to their negation.

```python id=bb63df73-ebd8-4e16-9d2e-72210c14ccd1
from z3 import *

# create a fresh propositional variable uniquely identified by its name 'p'
p = Bool('p')

# excluded middle
solve(Not(Or(p, Not(p))))

# Pierce's law
q = Bool('q')
solve(Not(Implies(Implies(Implies(p, q), p), p)))

#Other useful connectives:
#Implies, And
```

# Exercise (tautologies)

Prove that the following propositional formula is a classical tautology by checking that its negation is unsatisfiable: $((p \land q) \to r) \leftrightarrow (p \to (q \to r))$.

What about the following Łukasiewicz's formula? $((p \to q) \to r) \to ((r \to p) \to (s \to p))$.

```python id=a3221512-aa18-4e18-ae43-6e7eb3bc94dd
r = Bool('r')
s = Bool('s')

left = Implies((And(p, q)), r)
right = Implies(p, Implies(q, r))

# Łukasiewicz's formula
left1 = Implies(Implies(p, q), r)
right1 = Implies(Implies(r, p), Implies(s, p))

solve(Not(And(Implies(left, right), Implies(right, left))))
solve(Not(Implies(left1, right1)))
```

# Example (long conjunctions and disjunctions)

We can conveniently use lists to represent long conjunctions `And` and disjunctions `Or`. For instance the next cell implements the following formula: $x_1 \wedge x_2 \wedge \cdots \wedge x_n$.

```python id=ec237a8d-5061-4a90-a78a-e754f67c18b1
n = 10
x = [Bool('x' + str(k)) for k in range(n)]
phi = And([x[k] for k in range(n)])
print(phi)

solve(phi) # satisfiable
```

# Exercise (All satisfying assignments)

If Z3 solves a formula, it returns some satisfying assignment. How can we use Z3 in order to construct all satisfying assignments? Tip: if the number of satisfying assignments is $n$, then we want to call Z3 $n+1$ times. Write a Python program that returns all satisfying assignments of a given SAT instance.

```python id=1e403801-e3bd-4ee7-8a98-ca85a29b0fff
from z3 import *

# this procedure returns an assignment (as a dictionary) if any exists
def model(phi):
    # create a SAT instance
    s = Solver()
    s.add(phi)
    # return a satisfying assignment
    return s.model() if s.check() == sat else {}

def all_models(phi):
    models = []
    m = model(phi)
    
    while m:
        models.append(m)
        
        block_clause = []
        for var in m:
          t = var()
          block_clause.append(t != m[var])

        phi = And(phi, Or(block_clause))
        m = model(phi)
    
    return models
  
p = Bool("p")
q = Bool("q")
r = Bool("r")
phi = Or(And(p, Not(q)), r)
m = model(phi)
print(p, '=', m[p])
print(q, '=', m[q])
print(r, '=', m[r])

# Pro tip 1: one can enumerate all variables by looking at the keys of m
# Pro tip 2: if x is a key of m,
# the corresponding Z3 variable can be reconstructed with "Bool(str(x))"
# (it would be cool if it was just "x", but it is not :)
vars = [Bool(str(x)) for x in m]

models = all_models(phi)
for i, model in enumerate(models):
    print(f"Model {i+1}:")
    print(p, '=', model[p])
    print(q, '=', model[q])
    print(r, '=', model[r])
```

# Dimacs format

The DIMACS format is a concrete syntax for propositional formulas in CNF. The following example of the DIMACS syntax corresponds to the formula:

$(𝑥_1∨¬𝑥_3) ∧ (𝑥_2∨𝑥_3∨¬𝑥_1)$

```no-exec id=81816403-88ad-442a-b1dc-8d571a5465d6
c this is a comment and the next line says that this SAT instance has 3 variables and 2 clauses
p cnf 3 2
1 -3 0
2 3 -1 0
```

The comment line starts with c. The next line states that the formula is in the cnf format and there are 3 variables and 2 clauses.

The first clause contains "x₁" and "not x₃", "0" is a terminator that allows clauses to span multiple lines. The second clause contains "x₂", "x₃", and "not x₁"

Such a file can be directly given to the Z3 standalone prover (using the "z3 file" command).

```python id=9aa83bfa-340d-4e1c-96e2-447fbf876b12
%%script z3 -dimacs -in

p cnf 3 2
1 -3 0
2 3 -1 0
```

The output is a satisfying assignment, in this case all three variables are set to false.

# Exercise (**Pebbling formulas**)

This exercise shows that solvers can be fast on certain (unsatisfiable) fragments of SAT. For an $𝑛\times 𝑛$ grid, the $p_n$ *pebbling formula* says that: 

1. There is a pebble on the top left vertex.
2. A vertex is pebbled if both the neighbour to the left and above are pebbled.
3. There is no pebble on the bottom right vertex.

The formula $p_n$ is clearly unsatisfiable. We are interested to determine whether this is a hard or an easy SAT instance. 

* Write down the CNF formula *$p_𝑛$.* Does this formula belong to some efficient class of SAT instances? 
* Write a Python program using Z3 to test satisfiability for $p_𝑛$*.*  For how large *$n$ *is the running time feasible (e.g., <1 sec.)?
* Write the formula $p_n$ in DIMACS format and check how fast is Z3 without Python interface (run Z3 from the command line).  For how large *$n$ *is the running time feasible (e.g., <1 sec.)?

```python id=8e927b9b-909a-42c1-9283-919ca0b9b506
from z3 import *
from datetime import datetime

# should a dimacs output be produced (on stdout) or rather
# a python z3 formula (and the solver called)?
dimacs = False

# grid size: n×n
n = 250

#(0,0)
#  1 → ? → …
#  ↓   ↓
#  ? → ? → …
#  ↓   ↓
#  …   …
#            0 
#        (n-1,n-1)

if not dimacs:
    # Create a n×n "matrix" (list of lists) of boolean variables
    X = [[Bool(f"x_{i}_{j}") for j in range(n)] for i in range(n)]
    s = Solver()

    # 2. Pebbling rule: A vertex is pebbled only if both its left and top neighbors are pebbled
    for i in range(n):
        for j in range(n):
            if i == 0 or j == 0:
              # Most-Top-row and Most-Left-Column is pebbled ([0][0] implies it)
              # This condition additionally cover the "1. The top-left corner must have a pebble"
              s.add(X[i][j])
            else:
              # (pebbled <=> left AND above neighbour pebbled)
              pebbled = And(X[i][j], X[i-1][j], X[i][j-1])
              # (not pebbled <=> left OR above neighbour NOT pebbled)
              not_pebbled = And(X[i][j], Or(Not(X[i-1][j]), Not(X[i][j-1])))
              # X[i][j] either pebbled or not_pebbled
              s.add(Or(pebbled, not_pebbled))

    # 3. The bottom-right corner must NOT have a pebble
    s.add(Not(X[n - 1][n - 1]))

    # Measure execution time
    start_time = datetime.now()
    result = s.check()
    end_time = datetime.now()
    elapsed_time = (end_time - start_time).total_seconds()

    print("SAT result:", result == sat)
    print("Execution time with Z3:", elapsed_time, "seconds for n =", n)
else:
    X = [[Bool(f"x_{i}_{j}") for j in range(n)] for i in range(n)]
    clauses = []

    # 2. Pebbling rule: A vertex is pebbled if both its left and top neighbors are pebbled
    for i in range(n):
        for j in range(n):
            if i == 0 or j == 0:
                # 1. The top-left corner must have a pebble
                # First row and column must be pebbled
                clauses.append(f"{i*n + j + 1} 0") 
            else:
                v = i*n + j + 1
                left = (i-1)*n + j + 1
                above = i*n + j
                # If left not pebbled, v not pebbled
                clauses.append(f"-{v} -{left} 0")
                # If above not pebbled, v not pebbled
                clauses.append(f"-{v} -{above} 0") 
                # If both left and above are pebbled, v must be pebbled
                clauses.append(f"{v} {left} {above} 0")

    # 3. The bottom-right corner must NOT have a pebble
    clauses.append(f"-{n*n} 0")

    vars = n*n
    clen = len(clauses)
    # Write DIMACS format to file
    # File creations takes a lot, but using locally `z3 -dimacs peb.dimacs` runs in fact pretty fast
    with open("peb.dimacs", "w") as f:
        f.write(f"p cnf {vars} {clen}\n")
        for clause in clauses:
            f.write(clause + "\n")
```

# Exercise (Queens)

The eight queens puzzle is the problem of placing eight chess queens on an 8×8 chessboard so that no two queens threaten each other; thus, a solution requires that no two queens share the same row, column, or diagonal. The eight queens puzzle is a special case of the more general *n* queens problem of placing *n* non-attacking queens on an *n*×*n* chessboard. Solutions exist for all natural numbers *n* with the exception of *n* = 2 and *n* = 3. 

Write a program that computes a solution for given *n*.

```python id=26aff919-bba6-428f-a88f-e51184ee826c
from z3 import *

# chessboard size
n = 20

# Create a rxr "matrix" (list of lists) of boolean variables
X = [ [ Bool("x_%s_%s" % (i, j)) for j in range(n) ]
      for i in range(n) ]
print(X)

# this procedure returns an assignment (as a dictionary) if any exists
def model(phi):
    # create a SAT instance
    s = Solver()
    s.add(phi)

    # return a satisfying assignment
    return s.model() if s.check() == sat else {}


# this procedure draws the board corresponding to the model, e.g:
# - - H -
# H - - -
# - - - H
# - H - -
def drawBoard(model):
    print("Verdict for n =", n)
    if not model:
        print("No solution")
    else:
        print("Solution found:")
        for i in range(n):
            for j in range(n):
                print("H" if model[X[i][j]] else "-", end=" ")
            print()

# pro tips:
# 1) there must be a queen in every row
# 2) no two queens on the same column
#    (i.e. for any column k and any two rows i, j, the queen is not in (i,k) or not in (j,k))
# (the two above imply that there must be exactly one queen in each row and each column)
# 3) no two queens on the same "positive" diagonal
# 4) no two queens on the same "negative" diagonal

constraints = []

# 1) there must be a queen in every row
for i in range(n):
    constraints.append(Or([X[i][j] for j in range(n)]))

# 2) no two queens on the same column
for k in range(n):
    for i in range(n):
        for j in range(i + 1, n):
            # for any column k and any two rows i, j, the queen is not in (i,k) or not in (j,k)
            constraints.append(Or(Not(X[i][k]), Not(X[j][k])))

# 3) no two queens on the same "positive" diagonal
for i in range(n):
    for j in range(n):
        for k in range(n):
            for l in range(n):
                if i < k and i + j == k + l:
                    constraints.append(Or(Not(X[i][j]), Not(X[k][l])))

# 4) no two queens on the same "negative" diagonal
for i in range(n):
    for j in range(n):
        for k in range(n):
            for l in range(n):
                if i < k and i - j == k - l:
                    constraints.append(Or(Not(X[i][j]), Not(X[k][l])))

solution = model(constraints)
drawBoard(solution)
```

<details id="com.nextjournal.article">
<summary>This notebook was exported from <a href="https://nextjournal.com/a/U7eoYywP888hRP7eijyG8?change-id=DqPBANwZH5j31w5aEQC86U">https://nextjournal.com/a/U7eoYywP888hRP7eijyG8?change-id=DqPBANwZH5j31w5aEQC86U</a></summary>

```edn nextjournal-metadata
{:article
 {:settings {:numbered? true},
  :nodes
  {"0ff80045-076b-4643-a458-a49a80be4e65"
   {:compute-ref #uuid "35784398-242b-49de-afa8-9f6257159c03",
    :exec-duration 1924,
    :id "0ff80045-076b-4643-a458-a49a80be4e65",
    :kind "code",
    :output-log-lines {:stdout 24},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"]},
   "1e403801-e3bd-4ee7-8a98-ca85a29b0fff"
   {:compute-ref #uuid "3122720c-c0b9-4a94-8480-8fdab0a2f47e",
    :exec-duration 512,
    :id "1e403801-e3bd-4ee7-8a98-ca85a29b0fff",
    :kind "code",
    :output-log-lines {:stdout 24},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"],
    :stdout-collapsed? false},
   "26aff919-bba6-428f-a88f-e51184ee826c"
   {:compute-ref #uuid "eaa45180-54d2-431e-9653-0033366bf208",
    :exec-duration 1941,
    :id "26aff919-bba6-428f-a88f-e51184ee826c",
    :kind "code",
    :output-log-lines {:stdout 24},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"]},
   "81816403-88ad-442a-b1dc-8d571a5465d6"
   {:id "81816403-88ad-442a-b1dc-8d571a5465d6", :kind "code-listing"},
   "8e927b9b-909a-42c1-9283-919ca0b9b506"
   {:compute-ref #uuid "a59b4874-2a25-4f57-b60e-8380c15e621a",
    :exec-duration 30034,
    :id "8e927b9b-909a-42c1-9283-919ca0b9b506",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"]},
   "9aa83bfa-340d-4e1c-96e2-447fbf876b12"
   {:compute-ref #uuid "a1a94230-d40c-42fc-9dad-d667e362b1fa",
    :exec-duration 308,
    :id "9aa83bfa-340d-4e1c-96e2-447fbf876b12",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"]},
   "a3221512-aa18-4e18-ae43-6e7eb3bc94dd"
   {:compute-ref #uuid "fbca77da-7eb0-4bc4-970b-70dd9b0300e5",
    :exec-duration 332,
    :id "a3221512-aa18-4e18-ae43-6e7eb3bc94dd",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"]},
   "bb63df73-ebd8-4e16-9d2e-72210c14ccd1"
   {:compute-ref #uuid "76cb1b02-59d9-4118-a0d4-1799224587d8",
    :exec-duration 807,
    :id "bb63df73-ebd8-4e16-9d2e-72210c14ccd1",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"],
    :stdout-collapsed? false},
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
   {:compute-ref #uuid "b5ec5b09-0e73-4604-825e-37e35c654998",
    :exec-duration 302,
    :id "ec237a8d-5061-4a90-a78a-e754f67c18b1",
    :kind "code",
    :output-log-lines {:stdout 12},
    :runtime [:runtime "cfb143c9-8c0c-4b55-bccb-3ac934f59152"],
    :stdout-collapsed? false}},
  :nextjournal/id #uuid "03c92e5b-cee7-41b1-9b9a-7103b6595e0d",
  :article/change
  {:nextjournal/id #uuid "67ef07e4-b311-4981-972f-50b21e0208c1"}}}

```
</details>
