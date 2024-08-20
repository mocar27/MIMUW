-- Set.hs

module Set(Set(..), empty, null, singleton, union, fromList
              , member, toList, toAscList, elems, insert
              ) where
import Prelude hiding(null)
import GHC.Hs (HsPatSynDir(Unidirectional))
import Data.List(sort)

data Set a = Empty
           | Singleton a
           | Union (Set a) (Set a)

empty :: Set a
empty = Empty

null :: Set a -> Bool
null Empty = True
null _     = False

member :: Eq a => a -> Set a -> Bool
member _ Empty         = False
member x (Singleton s) = x == s
member x (Union s1 s2) = member x s1 || member x s2

singleton :: a -> Set a
singleton = Singleton

fromList :: [a] -> Set a
fromList = foldr insert empty

toList :: Set a -> [a]
toList Empty         = []
toList (Singleton s) = [s]
toList (Union s1 s2) = toList s1 ++ toList s2

toAscList :: Ord a => Set a -> [a]
toAscList = nub' . sort . toList
    where
        nub' [] = []
        nub' [x] = [x]
        nub' (x:y:xs) = if x == y then nub' (y:xs) else x : nub' (y:xs)

elems :: Set a -> [a]
elems = toList

union :: Set a -> Set a -> Set a
union Empty Empty = Empty
union Empty s     = s
union s Empty     = s
union s1 s2       = Union s1 s2

insert :: a -> Set a -> Set a
insert x Empty         = Singleton x
insert x (Singleton s) = Union (Singleton x) (Singleton s)
insert x s             = singleton x `union` s

instance Ord a => Eq (Set a) where
    s1 == s2 = toAscList s1 == toAscList s2

instance Semigroup (Set a) where
    (<>) = union

instance Monoid (Set a) where
    mempty  = Empty
    mappend = (<>)
    mconcat = foldr union empty

instance Show a => Show (Set a) where
    show s = show (toList s)

instance Functor Set where
    fmap f Empty         = Empty
    fmap f (Singleton s) = Singleton (f s)
    fmap f (Union s1 s2) = Union (fmap f s1) (fmap f s2)
