# Lab07 - Intuitionistic first-order logic in Agda

**LICENCE:** This tutorial contains adaptations of material from [Programming Language Foundations in Agda](https://plfa.github.io/) by Phil Wadler and Wen Kokke. It is licensed under Creative Commons Attribution 4.0 International.

**SYNTAX:** You can enter `→` by writing `\->` and pressing TAB. Similar useful combinations include `\lambda`, `\neg`, `\top`, `\bot`, `\and`, `\or`, `\forall`, `\exists`, `\Pi`, \\`Sigma`, ...

In this lab we will explore intuitionistic first-order logic via the Agda programming language. We reproduce without further comment the basic definitions for propositional logic.

```agda id=a6c6e43d-1d65-4fa7-b7bc-70585b95874b
module AgdaLab02 where

infixr 2 _∧_
data _∧_ (A : Set) (B : Set) : Set where
  _,_ : A → B → A ∧ B

fst : {A B : Set} → A ∧ B → A
fst (a , _) = a

snd : {A B : Set} → A ∧ B → B
snd (_ , b) = b

infixr 1 _∨_ 
data _∨_ (A : Set) (B : Set) : Set where
    left : A → A ∨ B
    right : B → A ∨ B

case : {A B C : Set} → (A → C) → (B → C) → A ∨ B → C
case f g (left x) = f x
case f g (right x) = g x
```

```agda id=3ba5800f-ece3-4013-8057-05b952fc64ad
data ⊤ : Set where
  tt : ⊤

A→⊤ : {A : Set} → A → ⊤
A→⊤ _ = tt

data ⊥ : Set where

⊥-elim : {A : Set} → ⊥ → A
⊥-elim ()

infix 3 ¬_ 
¬_ : Set → Set
¬ A = A → ⊥
```

# Dependent types in Agda

So far we have only seen *(polymorphic) simple types*, such as `A → B` for `(A B : Set)`. There are situations where one wants to impose some additional property on the output `B` which depends on the particular input `a : A`. In order to achieve this, the output `B` is generalised to have type `A → Set` (instead of just `Set`). One obtains *dependent* function space

```agda no-exec id=033b6d0f-adee-438c-b899-afe4ea96951b
(a : A) → B a
```

Notice how the type expression `B a` now contains the *term* `a`, i.e., the type `B a : Set` *depends* on `a`. The type of B is A -> Set

Our main application and source of examples of dependent types is to model intuitionistic first-order logic.

# Intuitionistic first-order logic

We show how dependent types can be used to implement universal quantification (dependent function space) and existential quantification (dependent product).

## The universal quantifier ∀

In intuitionistic logic a proof of $\forall (a : A) \cdot 
B$ is a function $f$ mapping a proof $a$ of $A$ into a proof $(f\; a)$ of $B\; a$, where we can see $B$ as a family of types indexed by proofs of $A$. The universal quantifier is implemented via the *dependent function space*

```agda id=80194f0d-0df7-45e3-b58e-5a4b62144faa
Π : (A : Set) → (B : A → Set) → Set
Π A B = (a : A) → B a
```

(Note how `Π` is a *type-level function*, i.e., it maps types to types.)

The type `((a : A) → B a)`  generalises implication `A → B`, which corresponds to non-dependent function space. In this sense, in intuitionistic logic implication is a special case of universal quantification.

(Universal quantification also generalises conjunction, since the type $B_1 \wedge B_2$ is isomorphic to $\Pi\; A\; B$ where $A = \{1, 2\}$ and $B = \{ 1 \mapsto B_1, 2 \mapsto B_2 \}$. For this reason, sometimes `Π` is called dependent product, hence the notation. However, for us dependent product is something else and it will be used below to model existential quantification.)

```agda id=17307f26-8c42-4967-8152-b3861783dd67
-- the type of the first argument of Π can be inferred from the second,
-- so we can make it implicit and save typing
Π' : {A : Set} → (B : A → Set) → Set
Π' {A} B = Π A B

-- we introduce a convenient syntax reminiscent of universal quantification in logic
infix 0 Π'
syntax Π' (λ x → B) = ∀[ x ] B

-- Π {A} (λ x → B)

-- thanks to the declaration above,
-- "∀[ x ] B" is the same as "Π' (λ x → B)"

-- dependent apply; corresponds to ∀-elimination
apply : {A : Set} → {B : A → Set} → Π A B → (a : A) → B a
apply f x = f x

-- apply is just the identity at type Π A B
```

### **Exercise** (`∀`)

1. Show that the universal quantifier commutes with itself: $(\forall a. \forall b. P\; a\; b) \to \forall b. \forall a. P\; a\; b$.
2. Show that one can diagonalise a universal relation: $(\forall a. \forall b. P\; a\; b) \to \forall a. P\; a\; a$.

```agda id=0d0a22e7-57f0-4fa2-bd3c-41e4462c0876
-- recall that "∀[ a ] ∀[ b ] P a b" is the same as
-- "Π A (λ a → Π B (λ b → P a b))", which in turn just means
-- "(a : A) → (b : A) → P a b"
∀-comm :
  {A B : Set} {P : A → B → Set} →
  ∀[ a ] ∀[ b ] P a b →
  -------------------------------
  ∀[ b ] ∀[ a ] P a b
  
∀-comm f b a = f a b

∀-diag :
  {A : Set} {P : A → A → Set} →
  ∀[ a ] ∀[ b ] P a b →
  ------------------------------
  ∀[ a ] P a a
  
∀-diag f a = f a a
```

### **Exercise** (`∀` and `→`)

Show that the universal quantifier distributes over implication: $(\forall a. P\; a\ \to Q\; a) \to (\forall a. P\; a) \to \forall a. Q\; a$.

```agda id=ce9d22ed-01bd-47d7-952c-6b36c92f5315
-- recall that "∀[ a ] P a" is the same as
-- "Π A (λ a → P a)", which in turn just means
-- "(a : A) → P a"
∀→-distr :
  {A : Set} {P Q : A → Set} →
  ∀[ a ] (P a → Q a) →
  ∀[ a ] P a →
  ---------------------------
  ∀[ a ] Q a
  
∀→-distr f g a = f a (g a)
```

### **Exercise** (`∀` and  `∧`)

Prove the following intuitionistic tautologies.

1. $\forall a. B \wedge C \to (\forall a. B) \wedge (\forall a. C)$.
2. $(\forall a. B) \wedge (\forall a. C) \to \forall a. (B \wedge C)$.

```agda id=b3b4fac2-38f6-4078-b4cb-a3b227e05eb0
-- 1.
∀∧-distr :
  {A : Set} {B C : A → Set} →
  ∀[ a ] B a ∧ C a →
  ----------------------------
  (∀[ a ] B a) ∧ (∀[ a ] C a)
  
∀∧-distr f = (λ a → fst (f a)) , (λ a → snd (f a))

{-
The notation "∀[ a ] B a" is just syntactic sugaring for "Π A (λ a → B a)"
(where A is inferred automatically from the type of B),
which in turn is defined to be "(a : A) → (λ a → B a) a",
i.e., "(a : A) → B a".

Using "∀[ a ] B a" instead of "(a : A) → B a" makes the syntax closer to logic.
-}

-- 2.
∧∀-distr :
  {A : Set} {B C : A → Set} →
  (∀[ a ] B a) ∧ (∀[ a ] C a) →
  -----------------------------
  ∀[ a ] B a ∧ C a
  
∧∀-distr (f , g) a = f a , g a
```

### **Exercise** (`∀` and `∨`)

Which of the following are intuitionistic tautologies? Which are classic tautologies? Which are neither? Prove the intuitionistic ones.

1. $\forall a . (B \vee C) \to (\forall a . B) \vee (\forall a C)$.
2. $\forall a. (B \vee C) \to (\forall a. B) \vee C$, where $a$ does not occur in $C$.
3. $(\forall a . B) \vee (\forall a . C) \to \forall a . (B \vee C)$.

*Hint:* If you cannot easily program a solution, then most likely there is no solution.

```agda id=90caf57d-7e4f-4899-949a-45be574097e0
-- 1.
∀∨-distr :
  {A : Set} {B C : A → Set} →
  ∀[ a ] B a ∨ C a →
  ----------------------------
  (∀[ a ] B a) ∨ (∀[ a ] C a)
  
--   Not an intuitionistic tautology, because we are trying to choose between (∀ a. B a) and (∀ a. C a), but the input only gives us ∀a, a case-by-case decision: either (B a) or (C a). There doesn't exist a way to unify these choices across every a in an intuitionistic logic unless we can check all of A (e.g. set A should be finite), therefore the solution doesn't exist.
∀∨-distr = ?

-- 2.
∀∨-distr' :
  {A C : Set} {B : A → Set} →
  ∀[ a ] (B a ∨ C) →
  ---------------------------
  (∀[ a ] B a) ∨ C
  
--   Not an intuitionistic tautology, because this fails intuitionistically in general unless you can decide whether all results are (B a) or one is C.  — only for finite types or decidable disjunctions. So in full generality, this is also not intuitionistically valid, therefore the solution doesn't exist.
∀∨-distr' f = ?

-- ∀∨-distr' would hold if A is a finite set A = {a1, ..., an}
-- f a1? : Ba1
-- f a2? : Ba2
-- ...
-- f an? : Ban

-- --> ∀[ a ] B a

-- 3.
∨∀-distr :
  {A : Set} {B C : A → Set} →
  (∀[ a ] B a) ∨ (∀[ a ] C a) →
  -----------------------------
  ∀[ a ] B a ∨ C a
  
∨∀-distr (left f) a = left (f a)
∨∀-distr (right g) a = right (g a)
```

### **Exercise** (`∀` and `¬`)

The *double negation shift* (DNS) is the following classical tautology: $(\forall a. \neg\neg P\; a) \to \neg\neg \forall a. P\; a$. While DNS does not hold intuitionistically, prove that its converse does.

```agda id=d93b42a9-25ca-418d-8c39-eff652d0d258
-- converse double negation shift
cdns :
  {A : Set} {P : A -> Set} →
  ¬ ¬ (∀[ a ] P a) →
  --------------------------
  ∀[ a ] ¬ ¬ P a
  
cdns nnp a np = nnp (λ p → np (p a))
```

## The existential quantifier ∃

In intuitionistic logic, a proof of $\exists (a : A) . B$ is a pair $(a, b)$, where $a$ is a proof of $A$ and $b$ is a proof of $B\; a$. Like in universal quantification, we can see $B$ as a family of types indexed by proofs of $A$. The existential quantifier is implemented with the *dependent product*:

```agda id=8f0cc24d-433b-4659-b1dd-ec4e79064fbc
data Σ (A : Set) (B : A → Set) : Set where
    _,_ : (a : A) → B a → Σ A B
```

Compare this with conjunction, which corresponds to non-dependent product $A \wedge B$:

```agda no-exec id=a64f2db0-9204-4535-b6a1-a4c79e6bf1c6
data _∧_ (A : Set) (B : Set) : Set where
		_,_ : A → B → A ∧ B
```

In this sense, in intuitionistic logic existential quantification `Σ` generalises conjunction, which justifies the name dependent product. (This can create confusion because `Π` is sometimes called dependent product too, since also `Π` generalises conjunction...)

(Existential quantification also generalises disjunction, since the type $B_1 \vee B_2$ is isomorphic to $\Sigma\; A\; B$ with $A = \{1, 2\}$ and $B = \{ 1 \mapsto B_1, 2 \mapsto B_2 \}$. For this reason, `Σ` is sometimes called dependent sum. However, we will not follow this terminology here.)

```agda id=a4ccc908-f18f-49db-a4b4-0e1d308fd48b
Σ' : ∀ {A : Set} (B : A → Set) → Set
Σ' {A} B = Σ A B

infix 0 Σ'
syntax Σ' (λ x → B) = ∃[ x ] B

-- ∃[ x ] B is a shortcut for Σ' (λ x → B) and Σ _ (λ x → B)

-- aka uncurry
∃-elim : {A : Set} {B : A → Set} {C : Set} → (∀ (a : A) → B a → C) → Σ A B → C
∃-elim a→b→c (a , b) = a→b→c a b
```

We can also define projection functions for dependent pairs.

```agda id=2c50e8f1-1e26-4291-b03b-c34794ea0942
dfst : {A : Set} {B : A → Set} → Σ A B → A
dfst (a , _) = a

-- notice how the *type* of the second projection dsnd needs to use the first projection dfst!
dsnd : {A : Set} {B : A → Set} → (p : Σ A B) → B (dfst p)
dsnd (_ , b) = b

-- dsnd : {A : Set} {B : A → Set} → ((a , _) : Σ A B) → B a ??
```

We note how the definition of the functions `dfst` and `dsnd` is exactly as before with non-dependent pairs, however the *type* of these functions is much more precise now.

### **Exercise** (`∃`)

1. Show that the existential quantifier commutes with itself: $(\exists a. \exists b. P\; a\; b) \to \exists b. \exists a. P\; a\; b$.

```agda id=09c93dad-6fe2-40a3-a1aa-d3a4d81f244e
∃-comm :
  {A B : Set} {P : A → B → Set} →
  ∃[ a ] ∃[ b ] P a b →
  -------------------------------
  ∃[ b ] ∃[ a ] P a b
  
∃-comm (a , (b , p)) = (b , (a , p))
```

### **Exercise** (`∃` and `→`)

Consider the following two implications (where $a$ is not free in $P$):

1. $(\exists a. P \to Q\; a) \to (P \to \exists a. Q\; a)$, and
2. $(P \to \exists a. Q\; a) \to (\exists a. P \to Q\; a)$.

They are both valid in classical logic. Find out which one is valid in intuitionistic logic, and prove it.

```agda id=8f4ebdc4-7529-4fcc-a268-1b3a9da0e869
∃→1 :
  {A : Set} {P : Set} {Q : A → Set} →
  ∃[ a ] (P → Q a) →
  P →
  -----------------------------------
  ∃[ a ] Q a
  
∃→1 (a , f) p = (a , f p)

∃→2 :
  {A : Set} {P : Set} {Q : A → Set} →
  (P → ∃[ a ] Q a) →
  ------------------------------------
  ∃[ a ] (P → Q a)

-- This is not intuitionistically valid, because:
-- From P → ∃ a. Q a, there may be no uniform choice of a for which you can construct P → Q a.
-- Even if P → ∃ a. Q a holds, it may choose different a depending on how P is used.
-- Therefore this is not valid in intuitionistic logic.
∃→2 = ?
```

### **Exercise** (`∃` and `∀`)

```agda id=2eff697c-1704-4ae9-b668-5fb511bfa7fb
-- 1
duncurry :
  {A : Set} {B : A → Set} {C : Set} →
  ∀[ a ] (B a → C) →
  ∃[ a ] B a →
  ------------------------------------
  C
  
duncurry f (a , b) = f a b

dcurry :
  {A : Set} {B : A → Set} {C : Set} →
  (∃[ a ] B a  → C) →
  -----------------------------------
  ∀[ a ] (B a → C)
  
dcurry f a b = f (a , b)
```

### **Exercise** (`∃`, `∧`, and `∨`)

Establish whether the following are intuitionistic tautologies and prove it for the positive cases:

1. $\exists a B \vee C \to (\exists a B) \vee (\exists a C)$.
2. $(\exists a B) \vee (\exists a C) \to \exists a B \vee C$.
3. $\exists a B \wedge C \to (\exists a B) \wedge (\exists a C)$.
4. $(\exists a B) \wedge (\exists a C) \to \exists a B \wedge C$.
5. $(\exists a B) \wedge C \to \exists a B \wedge C$, where $a$ does not occur in $C$.

```agda id=bb886d7b-09be-4a72-bf19-d08f34fcaf88
-- 1
∃∨-distr :
  {A : Set} {B C : A → Set} →
  ∃[ a ] B a ∨ C a →
  ---------------------------
  (∃[ a ] B a) ∨ (∃[ a ] C a)
  
∃∨-distr (a , boc) = case (λ b → left (a , b)) (λ c → right (a , c)) boc

-- 2
∨∃-distr :
  {A : Set} {B C : A → Set} →
  (∃[ a ] B a) ∨ (∃[ a ] C a) →
  -----------------------------
  ∃[ a ] B a ∨ C a
  
∨∃-distr (left (a , b)) = (a , left b)
∨∃-distr (right (a , c)) = (a , right c)

-- 3
∃∧-distr :
  {A : Set} {B C : A → Set} →
  ∃[ a ] B a ∧ C a →
  ---------------------------
  (∃[ a ] B a) ∧ (∃[ a ] C a)
  
∃∧-distr (a , (b , c)) = ((a , b) , (a , c))

-- 4
∧∃-distr :
  {A : Set} {B C : A → Set} →
  (∃[ a ] B a) ∧ (∃[ a ] C a) →
  -----------------------------
  ∃[ a ] B a ∧ C a

-- Not intuitionistically valid, because the witness a for B and aa for C might be different. So unless A has only one element or is decidable in some way, you can’t combine them.
∧∃-distr = ?

-- 5
∧∃-distr' :
  {A : Set} {B : A → Set} {C : Set} →
  (∃[ a ] B a) ∧ C →
  -----------------------------------
  ∃[ a ] B a ∧ C
∧∃-distr' ((a , b) , c) = (a , (b , c))
```

### **Exercise** (`∀`, `∃`, and `¬`)

Which of the following holds in intuitionistic logic? Prove those that hold.

1. $\exists a \forall b C \to \forall b \exists a C$, where $C$ depends on $a$ and $b$.
2. $\neg \exists a B \to \forall a \neg B$, where $B$ depends on $a$.
3. $\forall a \neg B \to \neg \exists a B$, where $B$ depends on $a$.
4. $\exists a \neg B \to \neg \forall a B$, where $B$ depends on $a$.
5. $\neg \forall a B \to \exists a \neg B$, where $B$ depends on $a$.

```agda id=07579287-79b7-478f-a9c3-14991a28553e
-- 1
-- (B cannot depend on (a : A) for the swap to be possible!)
∃∀-distr :
  {A : Set} {B : Set} {C : A → B → Set} →
  ∃[ a ] ∀[ b ] C a b →
  ---------------------------------------
  ∀[ b ] ∃[ a ] C a b
  
∃∀-distr (a , f) b = (a , f b)

-- 2
¬∃→∀¬ :
  {A : Set} {B : A → Set} →
  ¬ (∃[ a ] B a) →
  -------------------------
  ∀[ a ] ¬ B a
  
 -- dcurry
¬∃→∀¬  f a b = f (a , b)

-- 3
∀¬→¬∃ :
  {A : Set} {B : A → Set} →
  ∀[ a ] ¬ B a →
  -------------------------
  ¬ (∃[ a ] B a)
  
-- duncarry  
∀¬→¬∃ f (a , b) = f a b 

-- 4
∃¬→¬∀ :
  {A : Set} {B : A → Set} →
  ∃[ a ] ¬ B a →
  -------------------------
  ¬ (∀[ a ] B a)
  
∃¬→¬∀ (a , nb) f = nb (f a)

-- 5
¬∀→∃¬ :
  {A : Set} {B : A → Set} →
  ¬ (∀[ a ] B a) →
  -------------------------
  ∃[ a ] ¬ B a
  
-- Not intuitionistically valid (this is the contrapositive of the previous one, which is not intuitionistically valid).
¬∀→∃¬ = ?
```

### **Exercise** (`∀`, `∃`, and `→`)

Show that one can always push the existential quantifier inside an implication: $(\exists a. P\; a \to Q\; a) \to ((\forall a. P\; a) \to \exists a. Q\; a)$.

However, the converse implication does not hold intuitionistically, even when $Q$ does not contain $a$ free: $((\forall a. P\; a) \to Q) \to \exists a. P\; a \to Q$.

```agda id=ded22870-9f9b-4ed8-bf69-07b4a596f61b
∃∀→ :
  {A : Set} {P Q : A → Set} →
  (∃[ a ] (P a → Q a)) →
  (∀[ a ] P a) →
  ---------------------------
  (∃[ a ] (Q a))
  
∃∀→ (a , f) p = (a , f (p a))
```

# Relations - optional

The exercises in this section are optional.

We apply first-order logic to study some properties of binary relations. We first define what is a binary relation.

```agda id=6e780d79-ff7f-4282-8f22-2b4da7ac3fea
Rel : Set → Set → Set1
Rel A B = A → B → Set
```

(The type of `Rel A B` is `Set1`, which is the type of `Set : Set1`, which in turn is the same as `Set0`. We do not need to bother anymore with type levels.)

For example, we can express common properties of binary relations.

```agda id=ab487ea4-029f-4747-9863-ce9e20a12196
reflexive : ∀ {A : Set} → Rel A A → Set
reflexive R = ∀[ x ] R x x

symmetric : ∀ {A : Set} → Rel A A → Set
symmetric R = ∀[ x ] ∀[ y ] (R x y → R y x)

transitive : ∀ {A : Set} → Rel A A → Set
transitive R = ∀[ x ] ∀[ y ] ∀[ z ] (R x y → R y z → R x z)
```

### **Exercise** (Total relations)

1. Define what it means for a relation $R$ to be total: $\forall x. \exists y. R x y$.
2. Formalise and prove that every relation $R$ which is symmetric, transitive, and total, it is also necessarily reflexive.

```agda id=1f9ea2d0-718e-4106-ab6e-8968c9ec03eb
total : ∀ {A B : Set} → Rel A B → Set
total R = ∀[ x ] ∃[ y ] R x y

sym∧trans∧tot→refl : (A : Set) (R : Rel A A) → symmetric R → transitive R → total R → reflexive R
sym∧trans∧tot→refl a R symm trans tot x with tot x
... | (y , Rxy) = trans x y x Rxy (symm x y Rxy)
```

## Composition of relations

We can define the composition of binary relations.

```agda id=c55b1bb0-52ba-4f23-bcfc-9fb74d626a0a
_∘_ : ∀ {A B C : Set} → Rel A B → Rel B C → Rel A C
(R ∘ S) a c = ∃[ b ] (R a b ∧ S b c)
```

### **Exercise**

Show that the composition of reflexive relations is also reflexive.

```agda id=9d58e743-b2d5-40b9-b1bc-7e11e03ee059
∘-refl : ∀ {A : Set} {R S : Rel A A} → reflexive R → reflexive S → reflexive (R ∘ S)
∘-refl reflR reflS x = (x , (reflR x , reflS x))
```

## Inclusion

We can define that one relation is included into another.

```agda id=87e01d97-646b-49ea-b720-066944c3ae0b
_⊆_ : ∀ {A B : Set} → Rel A B → Rel A B → Set
R ⊆ S = ∀[ x ] ∀[ y ] (R x y → S x y)
```

With this in hand, we can define that one relation commutes with another.

```agda id=30aba41e-cb72-4185-8f6f-3bea4ee37b6d
commute : ∀ {A : Set} (R S : Rel A A) → Set
commute R S = (R ∘ S) ⊆ (S ∘ R)
```

### **Exercise**

Prove that if $R$ and $S$ are transitive binary relations and moreover $S$ commutes over $R$, then their composition $R \circ S$ is also transitive.

```agda id=84b9b175-be58-44e4-8002-cf4d12cbeb12
∘-trans : ∀ (A : Set) (R S : Rel A A) → transitive R → transitive S → commute S R → transitive (R ∘ S)
∘-trans = ?
```

<details id="com.nextjournal.article">
<summary>This notebook was exported from <a href="https://nextjournal.com/a/ULjyjYSKbbjm3PkqA5soR?change-id=Dsb3xdW6FbzW7Te1V8rkai">https://nextjournal.com/a/ULjyjYSKbbjm3PkqA5soR?change-id=Dsb3xdW6FbzW7Te1V8rkai</a></summary>

```edn nextjournal-metadata
{:article
 {:nodes
  {"033b6d0f-adee-438c-b899-afe4ea96951b"
   {:id "033b6d0f-adee-438c-b899-afe4ea96951b", :kind "code-listing"},
   "07579287-79b7-478f-a9c3-14991a28553e"
   {:compute-ref #uuid "20576845-d46a-46d9-a881-c59db29c99aa",
    :exec-duration 757,
    :id "07579287-79b7-478f-a9c3-14991a28553e",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "09c93dad-6fe2-40a3-a1aa-d3a4d81f244e"
   {:compute-ref #uuid "616da4e0-c30d-490b-86c0-5c9df95f6d6b",
    :exec-duration 554,
    :id "09c93dad-6fe2-40a3-a1aa-d3a4d81f244e",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "0d0a22e7-57f0-4fa2-bd3c-41e4462c0876"
   {:compute-ref #uuid "1b7d0175-a669-49f7-a627-da67ce3d33f1",
    :exec-duration 535,
    :id "0d0a22e7-57f0-4fa2-bd3c-41e4462c0876",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "17307f26-8c42-4967-8152-b3861783dd67"
   {:compute-ref #uuid "82386c08-c4ed-4fb4-8b92-c3db1b3c1845",
    :exec-duration 557,
    :id "17307f26-8c42-4967-8152-b3861783dd67",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "1f9ea2d0-718e-4106-ab6e-8968c9ec03eb"
   {:compute-ref #uuid "b27c05e5-83cb-4d97-9212-792d7e71b5d3",
    :exec-duration 818,
    :id "1f9ea2d0-718e-4106-ab6e-8968c9ec03eb",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "2c50e8f1-1e26-4291-b03b-c34794ea0942"
   {:compute-ref #uuid "3e5f7c46-2004-432a-a592-cd04f9b05d14",
    :exec-duration 554,
    :id "2c50e8f1-1e26-4291-b03b-c34794ea0942",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "2eff697c-1704-4ae9-b668-5fb511bfa7fb"
   {:compute-ref #uuid "f56d1d52-26e1-456f-965a-89052e03ee49",
    :exec-duration 663,
    :id "2eff697c-1704-4ae9-b668-5fb511bfa7fb",
    :kind "code",
    :output-log-lines {:stdout 4},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "30aba41e-cb72-4185-8f6f-3bea4ee37b6d"
   {:compute-ref #uuid "5d7b2e2d-4984-467e-81b7-4b361dceb5bd",
    :exec-duration 699,
    :id "30aba41e-cb72-4185-8f6f-3bea4ee37b6d",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "3ba5800f-ece3-4013-8057-05b952fc64ad"
   {:compute-ref #uuid "d05de36e-eabb-419c-834e-5b1e18e57b87",
    :exec-duration 431,
    :id "3ba5800f-ece3-4013-8057-05b952fc64ad",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "6e780d79-ff7f-4282-8f22-2b4da7ac3fea"
   {:compute-ref #uuid "b0a4b7dc-6b87-44f2-9fcd-b0d55abf8378",
    :exec-duration 650,
    :id "6e780d79-ff7f-4282-8f22-2b4da7ac3fea",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "80194f0d-0df7-45e3-b58e-5a4b62144faa"
   {:compute-ref #uuid "e13713e7-8a1d-460e-b8ea-0c7ee9fa5a58",
    :exec-duration 550,
    :id "80194f0d-0df7-45e3-b58e-5a4b62144faa",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "84b9b175-be58-44e4-8002-cf4d12cbeb12"
   {:compute-ref #uuid "1e5e7d90-33bd-455d-97f5-74b69335499e",
    :exec-duration 731,
    :id "84b9b175-be58-44e4-8002-cf4d12cbeb12",
    :kind "code",
    :output-log-lines {:stdout 9},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "87e01d97-646b-49ea-b720-066944c3ae0b"
   {:compute-ref #uuid "418f66dd-06d9-4f92-a08b-3c2972210cbc",
    :exec-duration 747,
    :id "87e01d97-646b-49ea-b720-066944c3ae0b",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "8f0cc24d-433b-4659-b1dd-ec4e79064fbc"
   {:compute-ref #uuid "7994390d-4a7f-4475-8c63-e388dcad3e9b",
    :exec-duration 612,
    :id "8f0cc24d-433b-4659-b1dd-ec4e79064fbc",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "8f4ebdc4-7529-4fcc-a268-1b3a9da0e869"
   {:compute-ref #uuid "9a24f394-0a14-4d02-b29d-a8c4eeb40ba2",
    :exec-duration 573,
    :id "8f4ebdc4-7529-4fcc-a268-1b3a9da0e869",
    :kind "code",
    :output-log-lines {:stdout 4},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "90caf57d-7e4f-4899-949a-45be574097e0"
   {:compute-ref #uuid "e4f8397b-3eec-45b8-8e49-b2839398b924",
    :exec-duration 552,
    :id "90caf57d-7e4f-4899-949a-45be574097e0",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "9d58e743-b2d5-40b9-b1bc-7e11e03ee059"
   {:compute-ref #uuid "0d282133-4279-43ae-b781-013e4404136e",
    :exec-duration 801,
    :id "9d58e743-b2d5-40b9-b1bc-7e11e03ee059",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "a4ccc908-f18f-49db-a4b4-0e1d308fd48b"
   {:compute-ref #uuid "0cb39eb0-e8d9-4c33-8536-5add751b842d",
    :exec-duration 541,
    :id "a4ccc908-f18f-49db-a4b4-0e1d308fd48b",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "a64f2db0-9204-4535-b6a1-a4c79e6bf1c6"
   {:id "a64f2db0-9204-4535-b6a1-a4c79e6bf1c6", :kind "code-listing"},
   "a6c6e43d-1d65-4fa7-b7bc-70585b95874b"
   {:compute-ref #uuid "53a4a292-27a0-498f-92bf-cbd8aad32969",
    :exec-duration 995,
    :id "a6c6e43d-1d65-4fa7-b7bc-70585b95874b",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "ab487ea4-029f-4747-9863-ce9e20a12196"
   {:compute-ref #uuid "e125522c-5250-442f-aa8b-7b7fc1558544",
    :exec-duration 775,
    :id "ab487ea4-029f-4747-9863-ce9e20a12196",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "b3b4fac2-38f6-4078-b4cb-a3b227e05eb0"
   {:compute-ref #uuid "28dcc09c-1b23-425f-82e9-115469766b88",
    :exec-duration 532,
    :id "b3b4fac2-38f6-4078-b4cb-a3b227e05eb0",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "b4cd1542-8803-4566-b5b1-7a9345e29075"
   {:environment
    [:environment
     {:article/nextjournal.id
      #uuid "02b5e9b4-9ab0-4adb-9164-9a36ba7b17a1",
      :change/nextjournal.id
      #uuid "5e861251-bb61-4668-bd60-70b6bc74dec6",
      :node/id "3159bc83-58eb-4c79-8f37-f0429767a98a",
      :jupyter/kernelspec
      {:argv
       ["/opt/conda/bin/python"
        "-m"
        "agda_kernel"
        "-f"
        "{connection_file}"],
       :display_name "agda",
       :language "agda",
       :name "agda"}}],
    :id "b4cd1542-8803-4566-b5b1-7a9345e29075",
    :kind "runtime",
    :language "agda",
    :type :jupyter,
    :jupyter/kernelspec
    {:argv
     ["/opt/conda/bin/python"
      "-m"
      "agda_kernel"
      "-f"
      "{connection_file}"],
     :display_name "agda",
     :language "agda",
     :name "agda"}},
   "bb886d7b-09be-4a72-bf19-d08f34fcaf88"
   {:compute-ref #uuid "7a548eb2-5a2d-4e9c-8c0b-ea650ee26c96",
    :exec-duration 677,
    :id "bb886d7b-09be-4a72-bf19-d08f34fcaf88",
    :kind "code",
    :output-log-lines {:stdout 5},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "c55b1bb0-52ba-4f23-bcfc-9fb74d626a0a"
   {:compute-ref #uuid "57f045cf-7fb7-42bf-92c3-bfc6a8492700",
    :exec-duration 710,
    :id "c55b1bb0-52ba-4f23-bcfc-9fb74d626a0a",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "ce9d22ed-01bd-47d7-952c-6b36c92f5315"
   {:compute-ref #uuid "3d73619e-30a6-43a6-9b0d-9528d1875743",
    :exec-duration 600,
    :id "ce9d22ed-01bd-47d7-952c-6b36c92f5315",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "d93b42a9-25ca-418d-8c39-eff652d0d258"
   {:compute-ref #uuid "d520b630-5185-413d-82c2-adf2605a0fa9",
    :exec-duration 509,
    :id "d93b42a9-25ca-418d-8c39-eff652d0d258",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]},
   "ded22870-9f9b-4ed8-bf69-07b4a596f61b"
   {:compute-ref #uuid "fc631d86-6a6a-42b9-85e2-3bcce6982db7",
    :exec-duration 779,
    :id "ded22870-9f9b-4ed8-bf69-07b4a596f61b",
    :kind "code",
    :output-log-lines {:stdout 6},
    :runtime [:runtime "b4cd1542-8803-4566-b5b1-7a9345e29075"]}},
  :nextjournal/id #uuid "03d13f63-b666-4bad-b150-ecd4bf454d7c",
  :article/change
  {:nextjournal/id #uuid "683dd64c-9733-4ace-b1d6-b8456fc0adc7"}}}

```
</details>
