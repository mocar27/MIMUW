{-# LANGUAGE UnicodeSyntax, TypeSynonymInstances, FlexibleInstances #-}
{-# OPTIONS_GHC -Wno-unrecognised-pragmas #-}
{-# HLINT ignore "Unused LANGUAGE pragma" #-}
{-# HLINT ignore "Use camelCase" #-}

import Data.List
import Control.Monad
import Control.Monad.State
import Test.QuickCheck
import System.IO.Unsafe

import qualified Data.Map.Strict as Map
import Data.Ord ( comparing )

-- updating a function
update :: Eq a => (a -> b) -> a -> b -> a -> b
update ρ x v y = if x == y then v else ρ y

-- useful for debugging
debug :: Show a => String -> a -> a
debug str x = seq (unsafePerformIO $ do putStr "<"; putStr str; putStr ": "; print x; putStr ">") x

copy = undefined
todo = undefined

-- Variable names are just strings
type PropName = String

-- generation of fresh variable names
fresh :: [PropName] -> PropName
fresh vars = head $ filter (not . (`elem` vars)) $ map (("p"++) . show) [0..]

data Formula =
      T
    | F
    | Prop PropName -- atomic formulas
    | Not Formula
    | And Formula Formula
    | Or Formula Formula
    | Implies Formula Formula
    | Iff Formula Formula
    deriving (Eq, Show)

p, q, r, s, t :: Formula

p = Prop "p"
q = Prop "q"
r = Prop "r"
s = Prop "s"
t = Prop "t"

infixr 8 /\, ∧
(/\) = And
(∧) = And

infixr 5 \/, ∨, ==>
(\/) = Or
(∨) = Or -- input with "\or"
(==>) = Implies

infixr 4 <==>, ⇔
(<==>) = Iff
(⇔) = Iff -- input with "\lr"

instance Arbitrary Formula where
    arbitrary = sized f where

      f 0 = oneof $ map return $ [p, q, r, s, t] ++ [T, F]

      f size = frequency [
        (1, fmap Not (f (size - 1))),
        (4, do
              size' <- choose (0, size - 1)
              conn <- oneof $ map return [And, Or, Implies, Iff]
              left <- f size'
              right <- f $ size - size' - 1
              return $ conn left right)
        ]

    shrink (Not φ) = [φ]
    shrink (Or φ ψ) = [φ, ψ]
    shrink (And φ ψ) = [φ, ψ]
    shrink (Implies φ ψ) = [φ, ψ]
    shrink (Iff φ ψ) = [φ, ψ]
    shrink _ = []

type Valuation = PropName -> Bool

eval :: Formula -> Valuation -> Bool
eval T _ = True
eval F _ = False
eval (Prop p) ρ = ρ p
eval (Not φ) ρ = not (eval φ ρ)
eval (And φ ψ) ρ = eval φ ρ && eval ψ ρ
eval (Or φ ψ) ρ = eval φ ρ || eval ψ ρ
eval (Implies φ ψ) ρ = not (eval φ ρ) || eval ψ ρ
eval (Iff φ ψ) ρ = eval φ ρ == eval ψ ρ

variables :: Formula -> [PropName]
variables = nub . go where
  go T = []
  go F = []
  go (Prop p) = [p]
  go (Not φ) = go φ
  go (And φ ψ) = go φ ++ go ψ
  go (Or φ ψ) = go φ ++ go ψ
  go (Implies φ ψ) = go φ ++ go ψ
  go (Iff φ ψ) = go φ ++ go ψ

valuations :: [PropName] -> [Valuation]
valuations [] = [undefined]
valuations (x : xs) = concat [[update ϱ x True, update ϱ x False] | ϱ <- valuations xs]

type SATSolver = Formula -> Bool

satisfiable :: SATSolver
satisfiable φ = or [eval φ ϱ | ϱ <- valuations (variables φ)]

tautology :: Formula -> Bool
tautology φ = and [eval φ ρ | ρ <- valuations (variables φ)]

nnf :: Formula -> Formula
nnf T = T
nnf F = F
nnf r@(Prop p) = r
nnf (Not phi) = case nnf phi of
  T -> F
  F -> T
  Not phi' -> phi'
  Or phi' psi' -> And (nnf (Not phi')) (nnf (Not psi'))
  And phi' psi' -> Or (nnf (Not phi')) (nnf (Not psi'))
  phi' -> Not phi'
nnf (And phi psi) = And (nnf phi) (nnf psi)
nnf (Or phi psi) = Or (nnf phi) (nnf psi)
nnf (Implies phi psi) = Or (nnf (Not phi)) (nnf psi)
nnf (Iff phi psi) = Or (nnf (And phi psi)) (nnf (And (Not phi) (Not psi)))

data Literal = Pos PropName | Neg PropName deriving (Eq, Show, Ord)

literal2var :: Literal -> PropName
literal2var (Pos p) = p
literal2var (Neg p) = p

opposite :: Literal -> Literal
opposite (Pos p) = Neg p
opposite (Neg p) = Pos p

type CNFClause = [Literal]
type CNF = [CNFClause]

cnf2formula :: CNF -> Formula
cnf2formula [] = T
cnf2formula lss = foldr1 And (map go lss) where
  go [] = F
  go ls = foldr1 Or (map go2 ls)
  go2 (Pos p) = Prop p
  go2 (Neg p) = Not (Prop p)

positive_literals :: CNFClause -> [PropName]
positive_literals ls = nub [p | Pos p <- ls]

negative_literals :: CNFClause -> [PropName]
negative_literals ls = nub [p | Neg p <- ls]

literals :: [Literal] -> [PropName]
literals ls = nub $ positive_literals ls ++ negative_literals ls

cnf :: Formula -> CNF
cnf = go . nnf where
  go T = []
  go F = [[]]
  go (Prop p) = [[Pos p]]
  go (Not (Prop p)) = [[Neg p]]
  go (φ `And` ψ) = go φ ++ go ψ
  go (φ `Or` ψ) = [as ++ bs | as <- go φ, bs <- go ψ]

test_cnf :: Formula -> Bool
test_cnf φ = tautology $ φ ⇔ cnf2formula (cnf φ)

-- >>> quickCheckWith (stdArgs {maxSize = 18}) test_cnf

equi_satisfiable :: Formula -> Formula -> Bool
equi_satisfiable φ ψ = satisfiable φ == satisfiable ψ

ecnf3 :: (Formula -> Formula -> Formula) -> Formula -> Formula -> [PropName] -> (CNF, Formula, [PropName])
ecnf3 op f1 f2 vars =
  let nextVar = fresh vars
      vars' = vars ++ [nextVar]
      (cnf1, f1', vars'') = ecnf2 f1 vars'
      (cnf2, f2', vars''') = ecnf2 f2 vars''
  in (cnf1 ++ cnf2 ++ cnf (Iff (Prop nextVar) (f1 `op` f2)), Prop nextVar, vars''')

ecnf2 :: Formula -> [PropName] -> (CNF, Formula, [PropName])
ecnf2 T vars = ([], T, vars)
ecnf2 F vars = ([], F, vars)
ecnf2 (Prop x) vars = ([], Prop x, vars)
ecnf2 (Not f) vars =
  let (cnf', f', vars') = ecnf2 f vars
  in (cnf', Not f', vars')
ecnf2 (And f1 f2) vars = ecnf3 And f1 f2 vars
ecnf2 (Or f1 f2) vars = ecnf3 Or f1 f2 vars
ecnf2 (Implies f1 f2) vars = ecnf3 Implies f1 f2 vars
ecnf2 (Iff f1 f2) vars =  ecnf3 Iff f1 f2 vars

ecnf :: Formula -> CNF
ecnf f = let (cnf', f', _) = ecnf2 f [] in cnf f' ++ cnf'

-- >>> ecnf (T ∧ F ∨ T)

prop_ecnf :: Formula -> Bool
prop_ecnf phi = equi_satisfiable phi (cnf2formula $ ecnf phi)

-- >>> quickCheckWith (stdArgs {maxSize = 10}) prop_ecnf

-- data Literal = Pos PropName | Neg PropName deriving (Eq, Show, Ord)
-- type CNFClause = [Literal]
-- type CNF = [CNFClause]
-- null checks whether the structure (here: intersection of two lists) is empty
-- if it is not empty, then it returns False, then we negate it and in conclusion 
-- we're getting that the formula is a tautology
isTautology :: CNFClause -> Bool
isTautology clause = not . null $ positive_literals clause `intersect` negative_literals clause

remove_tautologies :: CNF -> CNF
remove_tautologies = filter (not . isTautology)

one_literal :: CNF -> CNF
one_literal cnf = case findSingleLiteral cnf of
    Nothing -> cnf  -- No single-literal clauses found, return CNF unchanged
    Just l  -> one_literal (simplifyCNF l cnf)  -- Recursively simplify
  where
    -- If a list has only one element -- it's single-literal clause, return it and simplify CNF
    findSingleLiteral :: CNF -> Maybe Literal
    findSingleLiteral [] = Nothing
    findSingleLiteral (c:cs) = case c of
      [l] -> Just l
      _ -> findSingleLiteral cs

    -- Filter whole CNF from clauses containing given literal, then remove all occurences of negated literal from clauses
    simplifyCNF :: Literal -> CNF -> CNF
    simplifyCNF lit = filter (notElem lit) . map (delete $ opposite lit) . filter (/= [lit])

-- >>> one_literal [[Pos "p"], [Pos "p", Pos "q", Pos "p", Pos "r"], [Neg "q", Pos "r", Neg "p", Neg "r", Neg "p"], [Neg "q", Neg "p"], [Pos "q", Pos "r", Pos "s"], [Neg "p", Pos "p"]]

prop_one_literal :: Bool
prop_one_literal =
  one_literal
    [[Pos "p"], [Pos "p", Pos "q", Pos "p", Pos "r"], [Neg "q", Pos "r", Neg "p", Neg "r", Neg "p"], [Neg "q", Neg "p"], [Pos "q", Pos "r", Pos "s"], [Neg "p", Pos "p"]] ==
    [[Pos "r",Pos "s"]] &&
  one_literal
    [[Pos "p"],[Pos "p1"],[Neg "p",Pos "q"],[Pos "p1",Pos "p0"],[Pos "q",Neg "p0",Pos "p1"],[Neg "p0",Pos "s"],[Neg "q0",Neg "p"],[Neg "s",Neg "p",Pos "p0"]] ==
    [[Neg "p0",Pos "s"],[Neg "s",Pos "p0"]] &&
  one_literal
    [[Pos "q"],[Pos "p0"],[Neg "p0",Pos "s"],[Neg "p0"]] ==
    [[]]

-- >>> quickCheck prop_one_literal

-- On ordered lists, nub is equivalent to nub, except that it runs in linear time instead of quadratic. 
-- As mentioned in the lab -- sort formulas and then delete duplicates
-- https://hackage.haskell.org/package/data-ordlist-0.4.7.0/docs/Data-List-Ordered.html
affirmative_negative :: CNF -> CNF
affirmative_negative cnf =
  let posLits = nub $ sort $ concatMap positive_literals cnf
      negLits = nub $ sort $ concatMap negative_literals cnf
      posForms = filter (`notElem` negLits) posLits
      negForms = filter (`notElem` posLits) negLits
      -- excLits - literals (formulas), that are ONLY JUST in positive or negative form
      excLits = map Pos posForms ++ map Neg negForms
  in filter (all (`notElem` excLits)) cnf -- filter all clauses that elements of clause are not in exclusive literals

prop_affirmative_negative :: Bool
prop_affirmative_negative =
  affirmative_negative
    [[Neg "p2",Pos "p"],[Neg "p2",Pos "p1"],[Neg "p",Neg "p1",Neg "p2"],[Neg "p1",Pos "q"],[Neg "p1",Pos "p0"],[Neg "q",Neg "p0",Pos "p1"],[Neg "p0",Pos "s"],[Neg "p0",Neg "p"],[Neg "s",Pos "p",Pos "p0"]] ==
    [[Neg "p1",Pos "q"],[Neg "p1",Pos "p0"],[Neg "q",Neg "p0",Pos "p1"],[Neg "p0",Pos "s"],[Neg "p0",Neg "p"],[Neg "s",Pos "p",Pos "p0"]] &&
  null (affirmative_negative
    [[Pos "p", Pos "q"], [Pos "p", Neg "q"]])

-- >>> quickCheck prop_affirmative_negative

-- Count occurrences of each variables - Map of type [literal_name, (positive_occurrences, negative_occurrences)]
-- We know, that at the point of counting resolution, there is no literal that occurs ONLY positively or ONLY negatively
-- so we will not take into consideration here the literals, that has on some position 0 occurrences (even tho we don't exclude them here)
countOccurrences :: CNF -> Map.Map PropName (Int, Int)
countOccurrences cnf = foldl updateCounts Map.empty (concat cnf)
  where
    updateCounts acc (Pos var) = Map.insertWith (\(p, n) (p', n') -> (p + p', n + n')) var (1, 0) acc
    updateCounts acc (Neg var) = Map.insertWith (\(p, n) (p', n') -> (p + p', n + n')) var (0, 1) acc

-- Function to find the variable with the least multiplication of Pos and Neg occurrences
leastVarByMul :: CNF -> PropName
leastVarByMul cnf = fst $ minimumByMultiplication counted
  where
    counted = Map.toList (countOccurrences cnf)
    multiplication (_, (p, n)) = p * n
    minimumByMultiplication = minimumBy (\a b -> compare (multiplication a) (multiplication b))

resolution :: CNF -> CNF
resolution cnf = resolve (leastVarByMul cnf) cnf
  where
    resolve v clauses =
        let posClauses = filter (elem (Pos v)) clauses
            negClauses = filter (elem (Neg v)) clauses
            rest = filter (notElem (Pos v)) $ filter (notElem (Neg v)) clauses
            newClauses = nub $ [sort (filter (/= Pos v) posC ++ filter (/= Neg v) negC) | posC <- posClauses, negC <- negClauses]
        in newClauses ++ rest

-- >>> resolution [[Pos "p", Pos "q"],[Neg "p", Neg "q"]]

prop_resolution :: Bool
prop_resolution = resolution [[Pos "p", Pos "q"],[Neg "p", Neg "q"]] == [[Pos "q", Neg "q"]]

-- >>> quickCheck prop_resolution

dp :: CNF -> Bool
dp cnf
  | null cnf = True  -- If CNF is empty, it's satisfiable
  | [] `elem` cnf = False  -- If CNF contains an empty clause, it's unsatisfiable
  | otherwise =
      let cnf1 = remove_tautologies cnf
          cnf2 = one_literal cnf1
          cnf3 = affirmative_negative cnf2
      in if cnf1 == cnf3 -- If no changes were made (no rule applies), then perform resolution
         then dp (resolution cnf3)
         else dp cnf3

sat_DP :: SATSolver
sat_DP form = dp cnf where
  cnf = ecnf form

prop_DP :: Formula -> Bool
prop_DP φ = sat_DP φ == satisfiable φ

-- >>> quickCheckWith (stdArgs {maxSize = 10}) prop_DP
