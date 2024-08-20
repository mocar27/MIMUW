{-# LANGUAGE OverloadedStrings #-}
import CodeWorld
import qualified Data.Text as T

-- ============================ SOKOBAN GAME ============================ -- 

type Program = IO ()

main :: Program
main = etap5

game :: Activity State
game = Activity initialWorldState handleEvent draw

sokobanGame :: IO ()
sokobanGame = runActivity (withStartScreen (withUndo (resettable game)))

-- ============================ POLYMORPHIC LIST FUNCTIONS ============================ -- 

elemList :: Eq a => a -> [a] -> Bool
elemList x xs = foldList (\y acc -> acc || y == x) False xs

appendList :: [a] -> [a] -> [a]
appendList xs ys = foldList (:) ys xs

listLength :: [a] -> Integer
listLength xs = foldList (\_ acc -> acc + 1) 0 xs

filterList :: (a -> Bool) -> [a] -> [a]
filterList f xs = foldList (\x acc -> if f x then x : acc else acc) [] xs

nth :: [a] -> Integer -> a
nth (x:xs) n
  | n == 0    = x
  | otherwise = nth xs (n - 1)

mapList :: (a -> b) -> [a] -> [b]
mapList f xs = foldList (\x acc -> f x : acc) [] xs

andList :: [Bool] -> Bool
andList xs = foldList (&&) True xs

allList :: (a -> Bool) -> [a] -> Bool
allList f xs = foldList (\x acc -> f x && acc) True xs

foldList :: (a -> b -> b) -> b -> [a] -> b
foldList _ acc [] = acc
foldList f acc (x:xs) = foldList f (f x acc) xs

-- ============================ GRAPH & MAZES CHECKS ============================ -- 

isGraphClosed :: Eq a => a -> (a -> [a]) -> (a -> Bool) -> Bool
isGraphClosed initial neighbours isOk = checkClosure [initial] []
  where
    checkClosure [] _ = True
    
    checkClosure (v:vs) visited
      | elemList v visited = checkClosure vs visited
      | not (isOk v) = False
      | otherwise =
          let newVertices = filterList (`notElem` visited) (neighbours v)
          in checkClosure (appendList vs newVertices) (v : visited)
          
reachable :: Eq a => a -> a -> (a -> [a]) -> Bool
reachable v initial neighbours = isGraphClosed initial neighbours (\x -> x == v)
  
allReachable :: Eq a => [a] -> a -> (a -> [a]) -> Bool
allReachable vs initial neighbours = allList (\v -> reachable v initial neighbours) vs

isClosed :: Mazes -> Bool
isClosed (Mazes start maze) = isGraphClosed start (adjacentCoords maze) (\x -> maze x /= Blank)

isSane :: Mazes -> Bool
isSane (Mazes startCoord maze) = listLength storages >= listLength boxes
  where
    storages = filterList (\x -> maze x == Storage) tiles
    boxes = filterList (\x -> maze x == Box) tiles
    tiles = filterList (\x -> reachable x startCoord (adjacentCoords maze)) (nonBlankTilesCoords maze)
    
nonBlankTilesCoords :: Maze -> [Coord]
nonBlankTilesCoords maze =
  [coord | (coord, tile) <- allTilesCoords maze, tile /= Blank]

allCoords :: [Coord]
allCoords = [C x y | x <- [-10..10], y <- [-10..10]]

allTilesCoords :: Maze -> [(Coord, Tile)]
allTilesCoords maze = [(coord, maze coord) | coord <- allCoords]

pictureOfBools :: [Bool] -> Picture
pictureOfBools xs = translated (-fromIntegral k /2) (fromIntegral k) (go 0 xs)
  where n = length xs
        k = findK 0 -- k is the integer square of n
        findK i | i * i >= n = i
                | otherwise  = findK (i+1)
        go _ [] = blank
        go i (b:bs) =
          translated (fromIntegral (i `mod` k))
                     (-fromIntegral (i `div` k))
                     (pictureOfBool b)
          & go (i+1) bs

        pictureOfBool True =  colored green (solidCircle 0.4)
        pictureOfBool False = colored red   (solidCircle 0.4)

-- ============================ MAZES DRAWINGS DESING ============================ -- 

square :: Double -> Picture
square side = rectangle side side

solidSquare :: Double -> Picture
solidSquare side = solidRectangle side side

thickSquare :: Double -> Double -> Picture
thickSquare side thickness = thickRectangle thickness side side

wall, ground, storage, box :: Picture
wall = translated (-0.15) 0.3 (colored (lighter 0.1 grey) (solidRectangle 0.4 0.1))
       & translated 0.2 (-0.05) (colored (lighter 0.1 grey) (solidRectangle 0.35 0.2))
       & translated (-0.25) (-0.25) (colored (lighter 0.1 grey) (solidSquare 0.2))
       & square 1
       & colored (darker 0.1 grey) (solidSquare 1)
ground = colored green (solidSquare 0.97) 
         & colored (darker 0.05 green) (thickSquare 0.97 0.03)
storage = circle 0.2 
          & colored grey (solidCircle 0.2) 
          & ground
box = thickSquare 0.7 0.03
      & thickPolyline 0.03 [(-0.2, -0.2), (0.2, 0.2)]
      & colored yellow (solidSquare 0.95)
      & thickSquare 0.95 0.05
    
data Tile = Wall | Ground | Storage | Box | Blank deriving (Eq, Show)
    
drawTile :: Tile -> Picture
drawTile Wall    = wall
drawTile Ground  = ground
drawTile Storage = storage
drawTile Box     = box
drawTile Blank   = blank

createMazePictures :: Maze -> [Picture]
createMazePictures maze =
  [ translated x y (drawTile ((removeBoxes maze) (C (round x) (round y))))
    | x <- [-10 .. 10],
      y <- [-10 .. 10]
  ]

createBoxesPictures :: [Coord] -> [Picture]
createBoxesPictures [] = []
createBoxesPictures boxesPos = [ translated (fromIntegral x) (fromIntegral y) box | (C x y) <- boxesPos ]
  
getBoxesPositions ::  Mazes -> [(Coord)]
getBoxesPositions (Mazes _ maze) =
  [(C x y) | x <- [-10 .. 10], y <- [-10 .. 10], maze (C x y) == Box]

pictureOfMaze :: State -> Picture
pictureOfMaze (S _ _ boxes (Mazes _ maze) level _) = foldr (&) blank (createBoxesPictures boxes ++ createMazePictures maze)

-- ============================ PLAYER DESIGN ============================ -- 

player1 :: Picture
player1 = playerUp

player2 :: Direction -> Picture
player2 d
  | d == R = playerRight
  | d == U = playerUp
  | d == L = playerLeft
  | d == D = playerDown

playerHead :: Picture
playerHead = translated 0 0.2 (colored white (thickCurve 0.02 [(-0.12, 0.15), (0, 0.19), (0.12, 0.15)]))
             & translated (-0.05) 0.25 (colored white (solidCircle 0.02))
             & translated 0.05 0.25 (colored white (solidCircle 0.02))
             & translated 0 0.25 (solidPolygon [(-0.23, 0.2), (0, -0.2), (0.23, 0.2)])
             
playerBodyTemplate :: Picture
playerBodyTemplate = thickPolyline 0.04 [(0, 0), (0.2, -0.15)]
                     & thickPolyline 0.04 [(0, -0.2), (0, 0.1)]
                     & thickPolyline 0.04 [(0, -0.2), (0.1, -0.5)]
          
playerRight :: Picture
playerRight = translated 0.3 0.2 (rotated (pi/2) playerHead) 
              & playerBodyTemplate
              & thickPolyline 0.04 [(0, -0.2), (0, -0.5)]
              & thickPolyline 0.04 [(0, 0), (0.22, 0)]
              
playerLeft = scaled (-1) 1 playerRight

playerDown :: Picture
playerDown = playerHead
             & playerBodyTemplate
             & scaled (-1) 1 playerBodyTemplate
             
playerUp :: Picture
playerUp = translated 0 0.5 (rotated pi playerHead) 
           & playerBodyTemplate
           & scaled (-1) 1 playerBodyTemplate

-- ============================ STARTING & ENDING SCREENS ============================ -- 

data Activity world = Activity {
    actState  :: world,
    actHandle :: (Event -> world -> world),
    actDraw   ::(world -> Picture)
}

data SSState world = StartScreen | Running world

data WithUndo a = WithUndo a [a]

runActivity :: Activity s -> IO ()
runActivity (Activity initialWorld handle draw) = activityOf initialWorld handle draw

resettable :: Activity s -> Activity s
resettable (Activity initialState handle draw)
  = Activity initialState handle' draw
  where handle' (KeyPress key) _ | key == "Esc" = initialState
        handle' e s = handle e s
        
withUndo :: Eq a => Activity a -> Activity (WithUndo a)
withUndo (Activity state0 handle draw) = Activity state0' handle' draw' where
    state0' = WithUndo state0 []
    handle' (KeyPress key) (WithUndo s stack) | key == "U"
      = case stack of s':stack' -> WithUndo s' stack'
                      []        -> WithUndo s []
    handle' e              (WithUndo s stack)
       | s' == s = WithUndo s stack
       | otherwise = WithUndo (handle e s) (s:stack)
      where s' = handle e s
    draw' (WithUndo s _) = draw s

withStartScreen :: Activity s -> Activity (SSState s)
withStartScreen (Activity state0 handle draw)
  = Activity state0' handle' draw'
  where
    state0' = StartScreen

    handle' (KeyPress key) StartScreen
         | key == " "                  = Running state0
    handle' _              StartScreen = StartScreen
    handle' e              (Running s) = Running (handle e s)

    draw' StartScreen = startScreen
    draw' (Running s) = draw s


startScreen :: Picture
startScreen = translated 0 7.5 (scaled 2.5 2.5 (lettering "Sokoban!"))
              -- & etap4 -- My computer can't handle running this, even on the website
              & translated 0 (-7) (lettering "Press Space button to start")
              & draw initialWorldState
              
isWinning :: State -> Bool
isWinning (S _ _ boxes (Mazes _ maze) level _) = all isStorage boxes
  where
    isStorage coord = maze coord == Storage

-- ============================ MOVEMENT LOGIC ============================ -- 

data Direction = R | U | L | D deriving (Eq, Show)

data Coord = C Int Int deriving (Eq, Show)

data State = S {
  stPlayer :: Coord,
  stDir    :: Direction,
  stBoxes  :: [Coord],
  stMaze   :: Mazes,
  stLevel  :: Integer,
  stMove   :: Integer
}
instance Eq State where
  S p d b _ l m == S p' d' b' _ l' m' = p == p' && d == d' && b == b' && l == l' && m == m'

initialPlayerCoord :: Coord
initialPlayerCoord = C 0 1

initialWorldState :: State
initialWorldState = loadMaze 0

loadMaze :: Integer -> State
loadMaze n = S initialPlayerCoord D (getBoxesPositions m) m n 0
  where m = nth mazes n

initializeLevel :: Integer -> State
initializeLevel lvl = S start D (getBoxesPositions m) m lvl 0
  where
    m@(Mazes start maze) = nth mazes lvl

atCoord :: Coord -> Picture -> Picture
atCoord (C x y) picture = translated (fromIntegral x) (fromIntegral y) picture

getCoord :: Direction -> Coord -> Coord
getCoord d (C x y)
      | d == R = (C (x + 1) y)
      | d == U = (C x (y + 1))
      | d == L = (C (x - 1) y)
      | d == D = (C x (y - 1))

checkCoord :: State -> Direction -> Coord -> Bool
checkCoord (S _ _ boxes (Mazes _ m) _ _) d (C x y) 
  | ((any (== (C x y)) boxes) && ((any (== anotherCoord) boxes) || (m anotherCoord == Wall))) = False
  | m (C x y) == Wall = False
  | otherwise = True
  where 
    anotherCoord = getCoord d (C x y)

adjacentCoord :: Direction -> Coord -> State -> State
adjacentCoord dir (C x y) s
  | dir == R && checkCoord s dir moveCoord = newState dir moveCoord
  | dir == U && checkCoord s dir moveCoord = newState dir moveCoord
  | dir == L && checkCoord s dir moveCoord = newState dir moveCoord
  | dir == D && checkCoord s dir moveCoord = newState dir moveCoord
  | otherwise = newState dir (C x y)
  where
    moveCoord = getCoord dir (C x y)
    newState newDir newCoord =
      let stDir' = newDir
          stMove' = stMove s + 1
          stPlayer' = newCoord
          stBoxes' = if newCoord /= (C x y)
                     then updateBoxes newDir newCoord (stBoxes s)
                     else stBoxes s
      in s { stPlayer = stPlayer', stDir = stDir', stMove = stMove', stBoxes = stBoxes' }

adjacentCoords :: Maze -> Coord -> [Coord]
adjacentCoords maze c = filterList (\x -> maze x /= Wall) [getCoord dir c | dir <- [R, U, L, D]]

handleEvent :: Event -> State -> State
handleEvent (KeyPress key) s
  | key == "N" = initializeLevel (if stLevel s == (listLength mazes) - 1 then 0 else stLevel s + 1)
  | isWinning s = s
  | key == "Right" || key == "D" = go R
  | key == "Up" || key == "W" = go U
  | key == "Left" || key == "A" = go L
  | key == "Down" || key == "S" = go D 
  where go d = adjacentCoord d (stPlayer s) s
handleEvent _ s = s
        
drawState1 :: State -> Picture
drawState1 s = (atCoord (stPlayer s) playerUp) & (pictureOfMaze s)

drawState2 :: State -> Picture
drawState2 s = (atCoord (stPlayer s) (player2 (stDir s))) & (pictureOfMaze s)

draw :: State -> Picture
draw s = if isWinning s 
          then 
            translated 0 5.5 (lettering (T.pack ("Level " ++ show (stLevel s) ++ " completed")))
            & translated 0 3.5 (lettering (T.pack ("in " ++ show (stMove s) ++ " moves.")))
            & (lettering (T.pack ("Press Esc or N button to continue.")))
          else 
            (atCoord (stPlayer s) (player2 (stDir s))) 
            & pictureOfMaze s
            & translated (-3) 5.5 (lettering (T.pack ("Level: " ++ show (stLevel s))))
            & translated 3 5.5 (lettering (T.pack ("Moves: " ++ show (stMove s))))
        
walk1 :: IO ()
walk1 = activityOf initialWorldState handleEvent drawState1

walk2 :: IO ()
walk2 = runActivity myActivity
  where
    myActivity = Activity initialWorld respond drawState
    initialWorld = initialWorldState
    respond event world = handleEvent event world
    drawState world = drawState2 world

walk3 :: IO ()
walk3 = runActivity (resettable myActivity)
  where
    myActivity = Activity initialWorld respond drawState
    initialWorld = initialWorldState
    respond event world = handleEvent event world
    drawState world = drawState2 world

walk4 :: IO ()
walk4 = runActivity (withStartScreen (resettable myActivity))
  where
    myActivity = Activity initialWorld respond drawState
    initialWorld = initialWorldState
    respond event world = handleEvent event world
    drawState world = draw world
    startScreen = startScreen
    
etap4 :: Picture
etap4 = pictureOfBools (appendList (mapList isClosed (appendList mazes badMazes))
                                   (mapList isSane   (appendList mazes badMazes)))

etap5 :: IO ()
etap5 = runActivity (withStartScreen (withUndo (resettable myActivity)))
  where
    myActivity = Activity initialWorld respond drawState
    initialWorld = initialWorldState
    respond event world = handleEvent event world
    drawState world = draw world
    startScreen = startScreen

-- ============================ OBJECTS INTERACTIONS ============================ -- 

type Maze = Coord -> Tile
  
addBoxes :: [Coord] -> Maze -> Maze
addBoxes [] maze = maze 
addBoxes (coord:boxesPos) maze = addBoxes boxesPos (\c -> if c == coord then Box else maze c) 

removeBoxes :: Maze -> Maze
removeBoxes maze = f . maze 
  where 
    f tile = if tile == Box then Ground else tile
    
updateBoxes :: Direction -> Coord -> [Coord] -> [Coord]
updateBoxes d oldBoxCoord boxesPos = map (\c -> if c == oldBoxCoord then newBoxCoord else c) boxesPos 
  where
    newBoxCoord = getCoord d oldBoxCoord
    
-- ============================ MAZES DESIGNS ============================ -- 

data Mazes = Mazes Coord Maze 
mazes :: [Mazes]
mazes = [Mazes (C 0 1) maze0, 
         Mazes (C 2 0) maze1,
         Mazes (C 0 1) maze2, 
         Mazes (C 0 0) maze3,
         Mazes (C 1 0) maze4,
         Mazes (C 0 1) maze5,
         Mazes (C 1 0) maze6,
         Mazes (C 0 0) maze7,
         Mazes (C 1 1) maze8,
         Mazes (C 0 0) maze9]
badMazes :: [Mazes]
badMazes = [Mazes (C 0 1) badMaze1, 
            Mazes (C 0 1) badMaze2, 
            Mazes (C 0 1) badMaze3]

maze0 :: Maze
maze0 (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x ==  2 && y <= 0        = Wall
  | x ==  3 && y <= 0        = Storage
  | x >= -2 && y == 0        = Box
  | otherwise                = Ground
  
maze1 :: Maze
maze1 (C x y)
  | abs x > 5  || abs y > 4  = Blank
  | abs x == 5 || abs y == 4 = Wall
  | x == 3 && y == 0         = Storage
  | x == 1 && y == 0         = Box
  | x == 0 && y == 0         = Box
  | x == (-1) && y == 0      = Wall
  | x == (-2) && y == 0      = Box
  | x == (-3) && y == 0      = Storage
  | x == 4 && y == 0         = Wall
  | x == (-4) && y == 0      = Storage
  | x == 1 && y == 1         = Wall
  | x == (-1) && y == 1      = Box
  | x == 2 && y == 2         = Wall
  | x == (-2) && y == 2      = Storage
  | otherwise                = Ground

    
maze2 :: Maze
maze2 (C x y)
  | y > 3 || y < -3 = Blank
  | x > 3 || x < -3 = Blank
  | x == -1 && y == -1 = Storage
  | x == 1 && y == -1 = Storage
  | x == -1 && y == 0 = Box
  | x == 0 && y == 0 = Box
  | x >= -3 && x <= -3 && y >= -3 && y <= 3 = Wall
  | x >= -2 && x <= 3 && y >= -3 && y <= -3 = Wall
  | x >= -2 && x <= -2 && y >= -2 && y <= -1 = Wall
  | x >= -2 && x <= 3 && y >= 3 && y <= 3 = Wall
  | x >= 2 && x <= 3 && y >= 0 && y <= 2 = Wall
  | x >= 3 && x <= 3 && y >= -2 && y <= -1 = Wall
  | otherwise = Ground
    
maze3 :: Maze
maze3 (C x y)
  | y > 3 || y < -2 = Blank
  | x > 3 || x < -3 = Blank
  | x == -1 && y == 1 = Storage
  | x == 1 && y == 1 = Storage
  | x == -1 && y == 0 = Box
  | x == 0 && y == 1 = Box
  | x >= -3 && x <= 3 && y >= -2 && y <= -2 = Wall
  | x >= -3 && x <= -3 && y >= -1 && y <= 3 = Wall
  | x >= -2 && x <= -2 && y >= -1 && y <= -1 = Wall
  | x >= -2 && x <= 3 && y >= 3 && y <= 3 = Wall
  | x >= 0 && x <= 3 && y > 2 && y < 2 = Wall
  | x >= 1 && x <= 1 && y >= 0 && y <= 0 = Wall
  | x >= 3 && x <= 3 && y >= -1 && y <= 1 = Wall
  | x == 3 && y == 2 = Wall
  | otherwise = Ground
   
maze4 :: Maze
maze4 (C x y)
  | y > 3 || y < -3 = Blank
  | x > 4 || x < -3 = Blank
  | x == 0 && y == -1 = Storage
  | x == 2 && y == -1 = Storage
  | x == 0 && y == 1 = Storage
  | x == 2 && y == 1 = Storage
  | x == 1 && y == -1 = Box
  | x == 0 && y == 0 = Box
  | x == 2 && y == 0 = Box
  | x == 1 && y == 1 = Box
  | x >= -3 && x <= 4 && y >= -3 && y <= -3 = Wall
  | x >= -3 && x <= -3 && y >= -2 && y <= 3 = Wall
  | x >= -2 && x <= -2 && y >= 0 && y <= 3 = Wall
  | x >= -1 && x <= 4 && y >= 3 && y <= 3 = Wall
  | x >= 4 && x <= 4 && y >= -2 && y <= 2 = Wall
  | otherwise = Ground
    
maze5 :: Maze
maze5 (C x y)
  | abs x > 4  || abs y > 4   = Blank
  | x == 0 && y == 2          = Box
  | x == 1 && y == 0         = Box
  | abs x == 4 || abs y == 4  = Wall
  | not (x == 1) && y == 2    = Wall
  | not (x == -2) && y == -1  = Wall
  | ((x ==  3) || (x == 2)) && y == -3 = Storage
  | otherwise                 = Ground
  
maze6 :: Maze
maze6 (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x == 2 && y == 0         = Storage
  | x == 0 && y == 0         = Box
  | x == (-1) && y == 0      = Box
  | x == (-2) && y == 0      = Storage
  | x == 1 && y == 1         = Box
  | x == 1 && y == (-1)      = Box
  | x == 0 && y == 1         = Storage
  | x == 0 && y == (-1)      = Storage
  | otherwise                = Ground

maze7 :: Maze
maze7 (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x == 2 && y == 0         = Box
  | x == 1 && y == 0         = Storage
  | x == (-1) && y == 0      = Box
  | x == (-2) && y == 0      = Box
  | x == 3 && y == 0         = Storage
  | x == 4 && y == 0         = Box
  | x == 2 && y == 1         = Storage
  | x == 2 && y == (-1)      = Storage
  | otherwise                = Ground
  
maze8 :: Maze
maze8 (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x == 3 && y == 0         = Storage
  | x == 1 && y == 0         = Box
  | x == 0 && y == 0         = Box
  | x == (-1) && y == 0      = Wall
  | x == (-2) && y == 0      = Box
  | x == (-3) && y == 0      = Storage
  | x == 4 && y == 0         = Wall
  | x == 3 && y == -3        = Storage
  | otherwise                = Ground
  
maze9 :: Maze
maze9 (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x == (-1) && y == 2      = Box
  | x == 2 && y == (-2)      = Box
  | x == 2 && y == 0         = Box
  | x == 1 && y == 0         = Storage
  | x == (-1) && y == 0      = Box
  | x == (-2) && y == 0      = Wall
  | x == 3 && y == 0         = Storage
  | x == 4 && y == 0         = Wall
  | x == 2 && y == 1         = Storage
  | x == 2 && y == (-1)      = Wall
  | x == 0 && y == 1         = Wall
  | x == 0 && y == (-1)      = Storage
  | otherwise                = Ground

badMaze1 :: Maze
badMaze1 (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x ==  2 && y <= 0        = Wall
  | x ==  3 && y <= 0        = Box
  | x >= -2 && y == 0        = Storage
  | otherwise                = Ground
  
badMaze2 :: Maze
badMaze2 (C x y)
  | abs x > 4  || abs y > 4  = Blank
  | abs x == 4 || abs y == 4 = Wall
  | x == 1 || y == -1        = Wall
  | x == 2 && y == 0         = Storage
  | x == 0 && y == 1         = Box
  | otherwise                = Ground
  
badMaze3 :: Maze
badMaze3 (C x y)
  | abs x > 4  || abs y > 4            = Blank
  | x == 1 || abs x == 4 || abs y == 4 = Wall
  | x == -2 && y == 3                  = Storage
  | x == 2 && y == 0                   = Storage
  | x == -3 && y == 3                  = Storage
  | x == -3 && y == 2                  = Storage
  | x == -1 &&  y == 2                 = Box 
  | x == -1 && abs y < 2               = Box 
  | otherwise                          = Ground



