# Lab06 - Intuitionistic propositional logic in Agda

**LICENCE:** This tutorial contains adaptations of material from [Programming Language Foundations in Agda](https://plfa.github.io/) by Phil Wadler and Wen Kokke. It is licensed under Creative Commons Attribution 4.0 International.

**SYNTAX:** You can enter `→` by writing `\->` and pressing TAB. Similar useful combinations include `\lambda`, `\neg`, `\top`, `\bot`, `\and`, `\or`, `\forall`, `\exists`, `\Pi`, \\`Sigma`, ...

In this lab we will explore intuitionistic propositional logic via the Agda programming language. Agda can be seen as Haskell on steroids with a more powerful type system.

# Implication

Intuitionistic implication is implemented as *function space* $A \to B$. The idea is that a proof of $A \to B$ is a (terminating) *program* $t : A \to B$ that, given a proof $a : A$ of $A$, always terminates and produces a proof $(t\; a) : B$ of $B$.

For instance, the following Agda program `id` proves the intuitionistic tautology $A \to A$:

```agda id=db297514-9ebb-4649-8260-15545f7d2c0c
module AgdaLab01 where

id : {A : Set} -> A -> A
id a = a

-- the same with λ-abstraction:
id' id'' : {A : Set} -> A -> A
id' = λ a → a
id'' = \ a -> a -- alternative non-unicode syntax
```

Compare this with the equivalent Haskell definition:

```haskell no-exec id=8143ef57-8378-4252-8cf8-8a368c1de1f2
id :: A → A
id x = x
```

In Agda we need to explicitly say what `A` is with the *implicit argument* `{A : Set}`. (In Agda, `Set` means *type*.) Thus `id` is a higher order function that takes as (implicit) input a type `A`, a value of type `A`, and returns a value also of type `A`. This shows how already such a simple function uses a dependent type.

The $\to$-elimination rule from natural deduction is *function application* and the $\to$-introduction rule is $\lambda$-abstraction:

```agda id=8219c3bb-eea3-42bb-8558-c72f18c7bc08
→-elim : {A B : Set} → (A → B) → A → B
→-elim a→b a = a→b a
```

**SPACING:** Agda allows us to be very flexible in the variable names, which can be strings such as `x`, `idλ`, `a→b`, `&&true`, or even arbitrary unicode symbols `💔`, *provided there is no space in-between*. As a consequence, *spaces in Agda have a syntactic meaning as separators*, and we need to be very generous with them.

## **Exercise**

Prove in Agda following theorems of intuitionistic propositional logic:

$\begin{align}
	(1) &&& A \to B \to A, \\
	(2) &&& (A \to B \to C) \to (A \to B) \to A \to C, \\
  (3) &&& (A \to B) \to (A \to C) \to (B \to C \to D) \to A \to D, \\
  (4) &&& A \to A \to A. \end{align}$

 How many proofs does $(4)$ have?

```agda id=d7a3376e-e185-4c94-8b64-212d80cf4859
A1 : {A B : Set} → A → B → A
A1 a b = a

A2 : {A B C : Set} → (A → B → C) → (A → B) → A → C
A2 abc ab a = abc a (ab a)

diamond : {A B C D : Set} → (A → B) → (A → C) → (B → C → D) → A → D
diamond ab ac bcd a = bcd (ab a) (ac a)

proj : {A : Set} → A → A → A
proj a1 a2 = a1
-- (4) has at least two normal proofs, we can return either a1 or a2
```

# True and false

The two truth values $\top$ and $\bot$ are implemented via Agda's data type mechanism. The intuition is that the type $\top$ has precisely one inhabitant (called `tt` below) and that $\bot$ has no inhabitants at all.

```agda id=75296b06-3833-42cc-b999-c54bbf7b555b
data ⊤ : Set where
  tt : ⊤
  
data ⊥ : Set where
```

The ⊥-elimination rule is implemented via the absurd pattern "()":

```agda id=826064e4-ae36-4031-ac65-ab0ec3b2fa6c
⊥-elim : {A : Set} → ⊥ → A
⊥-elim ()

f : {A B : Set} → B → ⊥ → A
f _ absurd = ⊥-elim absurd
```

# Negation

Negation is not a primitive in intuitionistic logic. In intuitionistic logic $\neg A$ means that, if we had a proof of $A$, then we could derive a contradiction $\bot$:

$\neg A \;\equiv\; A \to \bot.$

```agda id=0c049470-0dcd-4f26-9695-c86b5e345638
infix 3 ¬_

¬_ : Set → Set
¬ A = A → ⊥
```

Note how `¬_` is a type-level function, i.e., a function mapping types to types!

## Exercise

Use `⊥-elim` to prove $\neg A \to A \to B$:

```agda id=d4bd65fa-ee87-483f-b27a-aeeb92de0d9a
exfalso : {A B : Set} → ¬ A → A → B
exfalso na a = ⊥-elim (na a)
```

## Exercise

The logic of Agda is intuitionistic. In particular, in Agda the following double negation law does *not* hold:

$A \leftrightarrow \neg \neg A.$

Which one of the two directions holds in intuitionistic logic?

* Formalise this and prove it in Agda. Recall that ¬ ¬ A = (A → ⊥) → ⊥.
* Does the proof (i.e., program) resemble something we have already seen?

```agda id=4397a89c-e6e6-4512-9bc2-a9ec3826c75e
-- double negation law does not hold (same as in lab)
¬¬-elim : {A : Set} → ¬ ¬ A → A
¬¬-elim g = ⊥-elim (g (\a -> ?))

¬¬-intro : {A : Set} → A → ¬ ¬ A
¬¬-intro a f = f a
```

## **Exercise**

Show that the following *triple negation* law holds intuitionistically. 

```agda id=3959a69a-98cf-427d-ad2a-898afc7da96d
triple-negation : {A : Set} → ¬ ¬ ¬ A → ¬ A
triple-negation nnna a = nnna (λ na → exfalso na a)
```

## **Exercise**

The *contrapositive* of an implication $A \to B$ is $\neg B \to \neg A$. In classical logic an implication and its contrapositive are logically equivalent, i.e., the following is a tautology:

$(A \to B) \leftrightarrow (\neg B \to \neg A).$

Prove which, if any, of the two directions above holds in intuitionistic logic.

```agda id=29b049fe-be5e-41c5-b950-9ea9a8f2bb27
contrapositive : {A B : Set} → (A → B) → (¬ B → ¬ A)
contrapositive ab nb a = nb (ab a)
```

# Conjunction

According to the BHK interpretation, a proof of $A \wedge B$ is a pair $\langle a, b \rangle$, where $a$ is a proof of $A$ and $b$ is a proof of $B$. This intuition translates immediately into the following *product* datatype:

```agda id=6a18dfc2-cec5-4ffe-9024-c5c292396f63
infix 2 _∧_ 

-- _∧_ is the cartesian product operator ×

data _∧_ (A : Set) (B : Set) : Set where
  _,_ : A → B → A ∧ B
```

The datatype `_∧_` is parametrised by two types, `A` and `B`, and it is written `A ∧ B` or, using prefix notation, `_∧_ A B`. It has only one constructor `_,_`, corresponding to the ∧-introduction rule: Given elements `a : A` and `b : B`,  `a , b` has type `A ∧ B` (notice the mandatory spaces!). The term above can also be written in prefix notation as `_,_ A B`.

We can then define the two projection functions, which correspond to the two ∧-elimination rules:

```agda id=a7d7bc3b-bd78-4efc-bd3f-90cddcdeda92
fst : {A B : Set} → A ∧ B → A
fst (a , _) = a

snd : {A B : Set} → A ∧ B → B
snd (_ , b) = b
```

## **Exercise**

Formalise and prove the following:

1. Conjunction is commutative.


1. Curry/uncurry: The formula/type $A → B → C$ is "the same" as $A \wedge B → C$.

```agda id=70920540-e80b-42f6-a818-36f9f6575b3f
∧-commutative : {A B C : Set} → A ∧ B → B ∧ A 
∧-commutative aib = (snd aib , fst aib)

uncurry : {A B C : Set} → (A → B → C) → A ∧ B → C
uncurry abc (a , b) = abc a b

curry : {A B C : Set} → (A ∧ B → C) → A → B → C
curry abc a b = abc (a , b)
```

# Disjunction

According to the BHK interpretation, a proof of $A_1 \vee A_2$ is a pair $(k, t_k)$, where $k \in \{1, 2\}$ specifies that we are proving $A_k$ and $t_k : A_k$ is a proof thereof. Since the first component is finite, disjunction can be implemented with two constructors. Each constructor corresponds to a ∨-introduction rule.

```agda id=621aa7f1-6ced-41bb-a763-1734a23c39eb
-- we give disjunction lower priority than conjunction
-- so we can omit parenthesis from (A ∧ B) ∨ C and write A ∧ B ∨ C instead.
infix 1 _∨_ 

data _∨_ (A : Set) (B : Set) : Set where
    left : A → A ∨ B
    right : B → A ∨ B
```

We can do case analysis by pattern matching on the constructor. This corresponds to the ∨-elimination rule:

```agda id=8753bd2c-21b0-4e38-92fb-8ed9d1d30a04
case : {A B C : Set} → (A → C) → (B → C) → A ∨ B → C
case f g (left a) = f a
case f g (right b) = g b
```

## **Exercise**

In classical logic we have the following *law of excluded middle*: 

$A \vee \neg A.$

1. Why there is no Agda program of the corresponding type `{A : Set} → A ∨ ¬ A`?
2. *Challenge*: Write an Agda program for the *irrefutability* of the law of excluded middle: $\neg \neg (A \vee \neg A).$

   Hint: Expand the definition of $\neg$. You will need: `left`, `right`, and $\lambda$-abstraction.

```agda id=7f8315bd-ac01-4f75-a1e8-e43cccf87c4b
-- -- ¬ ¬ (A ∨ ¬ A) = ((A ∨ (A → ⊥)) → ⊥) → ⊥ 

irrefutability : {A : Set} → ¬ ¬ (A ∨ ¬ A)
irrefutability naoa = naoa (right (λ a → naoa (left a)))
```

## **Exercise (De Morgan Laws)**

Are the following laws valid in intuitionistic logic? If so, write a proof in Agda.

$\begin{align}
(1) \qquad \neg (A \vee B) \leftrightarrow \neg A \wedge \neg B. \\
(2) \qquad \neg A \vee \neg B \to \neg (A \wedge B). \\
(3) \qquad \neg (A \wedge B) \to \neg A \vee \neg B. \end{align}$

```agda id=b39fec8d-b7da-4159-9bd9-4aa38f278ecb
de_morgan1-1 : {A B : Set} → ¬ (A ∨ B) → ¬ A ∧ ¬ B
de_morgan1-1 naob = (λ a → naob (left a)) , (λ b → naob (right b))

de_morgan1-2 : {A B : Set} → ¬ A ∧ ¬ B → ¬ (A ∨ B)
de_morgan1-2 naanb (left a) = fst naanb a
de_morgan1-2 naanb (right b) = snd naanb b

de_morgan2 : {A B : Set} → ¬ A ∨ ¬ B → ¬ (A ∧ B)
de_morgan2 (left na) naonb = na (fst naonb)
de_morgan2 (right nb) naonb = nb (snd naonb)

-- invalid - it requires a form of LEM, so there is no total constructive proof of in Agda.
de_morgan3 : {A B : Set} → ¬ (A ∧ B) → ¬ A ∨ ¬ B
de_morgan3 f = ?
```

## **Exercise**

In classical logic, $A \to B$ is logically equivalent to $\neg A \vee B$. Which of the following two directions hold in intuitionistic logic? Prove it in Agda.

$(A \to B) \leftrightarrow (\neg A \vee B).$

```agda id=8a7d270d-75e8-47be-a469-a60134de04d6
-- -- your solution here
impliesRightLeft : {A B : Set} → (¬ A ∨ B) → A → B
impliesRightLeft (left na) a = exfalso na a
impliesRightLeft (right b) a = b

-- implication from left to right does not hold
impliesLeftRight : {A B : Set} → (A → B) → ¬ A ∨ B
impliesLeftRight = ?
```

# Challenges

The following exercises are entirely optional and not mandatory to be completed. Nonetheless, they show that very interesting phenomena happen in intuitionistic logic already at the propositional level.

## Weak Peirce's law

We know that Peirce's law is not an intuitionistic tautology. Show that the following related formula is an intuitionistic tautology:

$((((𝐴→𝐵)→𝐴)→𝐴)→𝐵)→𝐵.$

```agda id=3835b1f5-08a0-4562-babb-8f206f309cef
wp : {A B : Set} → ((((A → B) → A) → A) → B) → B
wp f = f (λ g → g (λ a → f (λ _ → a)))
```

## Intuitionistically equivalent LEM formulations

In the previous exercises we have seen that the following principles are not intuitionistic tautologies:

1. Law of excluded middle: A∨¬A.
2. Elimination of double negation: ¬¬A→A.
3. Implication as disjunction: (A→B)→¬A∨B.
4. The Negated De Morgan's law: ¬(¬A∧¬B)→A∨B.
5. Peirce's Law: ((A→B)→A)→A.

Show that all principles above are logically equivalent in intuitionistic logic. Each propositional variable A,B is universally quantified in each principle.

*Hint:* Prove the following sequence of implications:

* 1→2.
* 2→3: Use irrefutability of (A→B)→¬A∨B, proved earlier: ¬¬((A→B)→¬A∨B).
* 3→1.
* 1→4: Use the excluded middle for 𝐴 and for 𝐵.
* 4→1: Use ¬(¬A∧¬B)→A∨B with B≡¬A.
* 1→5.
* 5→1: Use Peirce's law ((A′→B′)→A′)→A′ with A′≡A∨¬A and 𝐵′≡⊥.

```agda id=0ac22a26-77be-496a-8177-560402bfc0f4
1→2 : ?
1→2 = ?

2→3 : ?
2→3 = ?

3→1 : ?
3→1 = ?

1→4 : ?
1→4 = ?

4→1 : ?
4→1 = ?

1→5 : ?
1→5 = ?

5→1 : ?
5→1 = ?
```

<details id="com.nextjournal.article">
<summary>This notebook was exported from <a href="https://nextjournal.com/a/UKHAbaWpSsqNG4HHV9Ayc?change-id=DsLPqEysAiewZXJrmes4Uh">https://nextjournal.com/a/UKHAbaWpSsqNG4HHV9Ayc?change-id=DsLPqEysAiewZXJrmes4Uh</a></summary>

```edn nextjournal-metadata
{:article
 {:nodes
  {"0ac22a26-77be-496a-8177-560402bfc0f4"
   {:compute-ref #uuid "df8b4376-0869-4e51-90e5-17b55c12ee11",
    :exec-duration 507,
    :id "0ac22a26-77be-496a-8177-560402bfc0f4",
    :kind "code",
    :output-log-lines {:stdout 25},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "0c049470-0dcd-4f26-9695-c86b5e345638"
   {:compute-ref #uuid "d54195df-6bbe-4b2d-8048-740c4c2d1d72",
    :exec-duration 354,
    :id "0c049470-0dcd-4f26-9695-c86b5e345638",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "20507249-a9a4-4449-8b65-06650f56d21d"
   {:environment
    [:environment
     {:article/nextjournal.id
      #uuid "02b5e9b4-9ab0-4adb-9164-9a36ba7b17a1",
      :change/nextjournal.id
      #uuid "5dea6ba0-ab98-4a7d-9d55-1b781c48c8d0",
      :node/id "3159bc83-58eb-4c79-8f37-f0429767a98a"}],
    :id "20507249-a9a4-4449-8b65-06650f56d21d",
    :kind "runtime",
    :language "agda",
    :type :jupyter},
   "29b049fe-be5e-41c5-b950-9ea9a8f2bb27"
   {:compute-ref #uuid "c89b4e6a-d6c5-4779-b4ad-2c4fe27f54ef",
    :exec-duration 352,
    :id "29b049fe-be5e-41c5-b950-9ea9a8f2bb27",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "3835b1f5-08a0-4562-babb-8f206f309cef"
   {:compute-ref #uuid "5791d9c9-5741-495a-9df5-c04fb00ec91d",
    :exec-duration 444,
    :id "3835b1f5-08a0-4562-babb-8f206f309cef",
    :kind "code",
    :output-log-lines {:stdout 4},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "3959a69a-98cf-427d-ad2a-898afc7da96d"
   {:compute-ref #uuid "04749817-c90d-4156-9559-8702cfe4289f",
    :exec-duration 385,
    :id "3959a69a-98cf-427d-ad2a-898afc7da96d",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "4397a89c-e6e6-4512-9bc2-a9ec3826c75e"
   {:compute-ref #uuid "50e8dac2-28c3-4a8c-86bb-768b4f089db7",
    :exec-duration 334,
    :id "4397a89c-e6e6-4512-9bc2-a9ec3826c75e",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "621aa7f1-6ced-41bb-a763-1734a23c39eb"
   {:compute-ref #uuid "af678a6c-fa29-4286-86c0-99d6e919f222",
    :exec-duration 436,
    :id "621aa7f1-6ced-41bb-a763-1734a23c39eb",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "6a18dfc2-cec5-4ffe-9024-c5c292396f63"
   {:compute-ref #uuid "85982960-1a8e-4ec4-a7aa-d6fd85c5cc88",
    :exec-duration 371,
    :id "6a18dfc2-cec5-4ffe-9024-c5c292396f63",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "70920540-e80b-42f6-a818-36f9f6575b3f"
   {:compute-ref #uuid "e4d28dd4-8bea-4e59-9480-cc3129b729d8",
    :exec-duration 375,
    :id "70920540-e80b-42f6-a818-36f9f6575b3f",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "75296b06-3833-42cc-b999-c54bbf7b555b"
   {:compute-ref #uuid "f0759d86-4b4e-4292-9ae8-61d85171bff2",
    :exec-duration 336,
    :id "75296b06-3833-42cc-b999-c54bbf7b555b",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "7f8315bd-ac01-4f75-a1e8-e43cccf87c4b"
   {:compute-ref #uuid "5d1689ca-e414-4dce-b61a-062174f7647f",
    :exec-duration 445,
    :id "7f8315bd-ac01-4f75-a1e8-e43cccf87c4b",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "8143ef57-8378-4252-8cf8-8a368c1de1f2"
   {:id "8143ef57-8378-4252-8cf8-8a368c1de1f2", :kind "code-listing"},
   "8219c3bb-eea3-42bb-8558-c72f18c7bc08"
   {:compute-ref #uuid "a410a2fb-7e4d-45a5-9a8d-a76f020ee645",
    :exec-duration 317,
    :id "8219c3bb-eea3-42bb-8558-c72f18c7bc08",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "826064e4-ae36-4031-ac65-ab0ec3b2fa6c"
   {:compute-ref #uuid "16215312-1a97-4921-a64e-d3831f2f6c3d",
    :exec-duration 390,
    :id "826064e4-ae36-4031-ac65-ab0ec3b2fa6c",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "8753bd2c-21b0-4e38-92fb-8ed9d1d30a04"
   {:compute-ref #uuid "189e4394-5c38-4dd6-a328-608726da2040",
    :exec-duration 488,
    :id "8753bd2c-21b0-4e38-92fb-8ed9d1d30a04",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "8a7d270d-75e8-47be-a469-a60134de04d6"
   {:compute-ref #uuid "28775209-33be-4244-abde-aefa7d778cd9",
    :exec-duration 445,
    :id "8a7d270d-75e8-47be-a469-a60134de04d6",
    :kind "code",
    :output-log-lines {:stdout 4},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "a7d7bc3b-bd78-4efc-bd3f-90cddcdeda92"
   {:compute-ref #uuid "036bd2bd-6a25-4465-aec5-5d63292fb5a9",
    :exec-duration 369,
    :id "a7d7bc3b-bd78-4efc-bd3f-90cddcdeda92",
    :kind "code",
    :output-log-lines {:stdout 2},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "b39fec8d-b7da-4159-9bd9-4aa38f278ecb"
   {:compute-ref #uuid "9cbf1a59-c1e6-4d0b-bd68-6cf9384ed6c9",
    :exec-duration 438,
    :id "b39fec8d-b7da-4159-9bd9-4aa38f278ecb",
    :kind "code",
    :output-log-lines {:stdout 3},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "d4bd65fa-ee87-483f-b27a-aeeb92de0d9a"
   {:compute-ref #uuid "e46f352a-3fbb-4818-9bbb-7555dda74ff6",
    :exec-duration 419,
    :id "d4bd65fa-ee87-483f-b27a-aeeb92de0d9a",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "d7a3376e-e185-4c94-8b64-212d80cf4859"
   {:compute-ref #uuid "b87344b9-ab39-43d7-a64b-c31a6ac65acf",
    :exec-duration 332,
    :id "d7a3376e-e185-4c94-8b64-212d80cf4859",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"]},
   "db297514-9ebb-4649-8260-15545f7d2c0c"
   {:compute-ref #uuid "bac93347-e3fb-4ff4-ad55-98f3c8fc63b0",
    :exec-duration 321,
    :id "db297514-9ebb-4649-8260-15545f7d2c0c",
    :kind "code",
    :output-log-lines {:stdout 1},
    :runtime [:runtime "20507249-a9a4-4449-8b65-06650f56d21d"],
    :stdout-collapsed? false}},
  :nextjournal/id #uuid "03d058b0-d386-4797-9387-e33498e1b097",
  :article/change
  {:nextjournal/id #uuid "6834cdde-db58-4d3e-9cd2-3e5a6fc11712"}}}

```
</details>
