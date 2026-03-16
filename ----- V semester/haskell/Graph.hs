-- Graph.hs

module Graph where
import Set(Set)
import qualified Set as Set
import Data.List(nub)
class Graph g where
  empty   :: g a
  vertex  :: a -> g a
  union   :: g a -> g a -> g a
  connect :: g a -> g a -> g a

data Relation a = Relation { domain :: Set a, relation :: Set (a, a) }
    deriving (Eq, Show)

data Basic a = Empty
             | Vertex a
             | Union (Basic a) (Basic a)
             | Connect (Basic a) (Basic a)

instance Graph Relation where
  empty                                     = Relation Set.empty Set.empty

  vertex v                                  = Relation (Set.singleton v) Set.empty

  union (Relation v1 e1) (Relation v2 e2)   = Relation verticesUnion edgesUnion
    where
      verticesUnion = Set.union v1 v2
      edgesUnion    = Set.union e1 e2

  connect (Relation v1 e1) (Relation v2 e2) = Relation verticesUnion edgesUnion
    where
      verticesUnion = Set.union v1 v2
      edgesUnion    = Set.union (Set.union e1 e2) (Set.fromList [(x, y) | x <- Set.elems v1, y <- Set.elems v2])

instance (Ord a, Num a) => Num (Relation a) where
  fromInteger = vertex . fromInteger
  (+)         = union
  (*)         = connect
  signum      = const empty
  abs         = id
  negate      = id

instance Graph Basic where
  empty               = Empty

  vertex              = Vertex

  union Empty Empty   = Empty
  union Empty g       = g
  union g Empty       = g
  union g1 g2         = Union g1 g2

  connect Empty Empty = Empty
  connect Empty g     = g
  connect g Empty     = g
  connect g1 g2       = Connect g1 g2

getVertices :: Ord a => Basic a -> Set a
getVertices Empty           = Set.Empty
getVertices (Vertex v)      = Set.Singleton v
getVertices (Union g1 g2)   = getVertices g1 `Set.union` getVertices g2
getVertices (Connect g1 g2) = getVertices g1 `Set.union` getVertices g2

getEdges :: Ord a => Basic a -> Set (a, a)
getEdges Empty           = Set.Empty
getEdges (Vertex v)      = Set.Empty
getEdges (Union g1 g2)   = getEdges g1 `Set.union` getEdges g2
getEdges (Connect g1 g2) = getEdges g1 `Set.union` getEdges g2 `Set.union` edgesList
  where
    firstGraphVertices  = Set.toList $ getVertices g1
    secondGraphVertices = Set.toList $ getVertices g2
    edgesList           = Set.fromList [(x, y) | x <- firstGraphVertices, y <- secondGraphVertices]

instance Ord a => Eq (Basic a) where
  g1 == g2 = edgesGraph1 == edgesGraph2 && verticesGraph1 == verticesGraph2
    where
      edgesGraph1    = Set.toAscList $ getEdges g1
      edgesGraph2    = Set.toAscList $ getEdges g2
      verticesGraph1 = Set.toAscList $ getVertices g1
      verticesGraph2 = Set.toAscList $ getVertices g2

instance (Ord a, Num a) => Num (Basic a) where
  fromInteger = vertex . fromInteger
  (+)         = union
  (*)         = connect
  signum      = const empty
  abs         = id
  negate      = id

instance Semigroup (Basic a) where
  (<>) = union

instance Monoid (Basic a) where
  mempty = Empty

fromBasic :: Graph g => Basic a -> g a
fromBasic Empty           = empty
fromBasic (Vertex v)      = vertex v
fromBasic (Union g1 g2)   = fromBasic g1 `union` fromBasic g2
fromBasic (Connect g1 g2) = fromBasic g1 `connect` fromBasic g2

concatEdges :: Eq a => [(a, a)] -> [a]
concatEdges []    = []
concatEdges edges = nub $ concatMap (\(x, y) -> [x, y]) edges

filterVerticesWithoutEdges :: Eq a => [a] -> [a] -> [a]
filterVerticesWithoutEdges vertices edges = filter (`notElem` edges) vertices

instance (Ord a, Show a) => Show (Basic a) where
  show g = "edges " ++ show edgesList ++ " + vertices " ++ show verticesList
    where
      vs           = Set.toAscList $ getVertices g
      ces          = concatEdges edgesList
      edgesList    = Set.toAscList $ getEdges g
      verticesList = filterVerticesWithoutEdges vs ces

-- | Example graph
-- >>> example34
-- edges [(1,2),(2,3),(2,4),(3,5),(4,5)] + vertices [17]

example34 :: Basic Int
example34 = 1*2 + 2*(3+4) + (3+4)*5 + 17

todot :: (Ord a, Show a) => Basic a -> String
todot g = "digraph {\n" ++ edges ++ vertices ++ "}"
  where
    vs       = Set.toAscList $ getVertices g
    es       = Set.toAscList $ getEdges g
    ces      = concatEdges es
    edges    = concatMap (\(x, y) -> show x ++ " -> " ++ show y ++ ";\n") es
    vertices = concatMap (\v -> show v ++ ";\n") $ filterVerticesWithoutEdges vs ces

instance Functor Basic where
  fmap f Empty           = Empty
  fmap f (Vertex v)      = Vertex (f v)
  fmap f (Union g1 g2)   = Union (fmap f g1) (fmap f g2)
  fmap f (Connect g1 g2) = Connect (fmap f g1) (fmap f g2)

-- | Merge vertices
-- >>> mergeV 3 4 34 example34
-- edges [(1,2),(2,34),(34,5)] + vertices [17]

mergeV :: Eq a => a -> a -> a -> Basic a -> Basic a
mergeV a b c = fmap (\x -> if x == a || x == b then c else x)

instance Applicative Basic where
    pure              = Vertex

    Empty       <*> _ = Empty
    Vertex f    <*> v = fmap f v
    Union f g   <*> v = Union (f <*> v) (g <*> v)
    Connect f g <*> v = Connect (f <*> v) (g <*> v)

-- https://learnyouahaskell.com/a-fistful-of-monads

instance Monad Basic where
    return            = pure

    Empty       >>= _ = Empty
    Vertex v    >>= f = f v
    Union g h   >>= f = Union (g >>= f) (h >>= f)
    Connect g h >>= f = Connect (g >>= f) (h >>= f)

-- | Split Vertex
-- >>> splitV 34 3 4 (mergeV 3 4 34 example34)
-- edges [(1,2),(2,3),(2,4),(3,5),(4,5)] + vertices [17]

splitV :: Eq a => a -> a -> a -> Basic a -> Basic a
splitV a b c g = g >>= \x -> if x == a then Union (Vertex b) (Vertex c) else Vertex x
