# "Labyrinth" task

The program looks for the shortest path in the ***k-dimensional*** maze that is described as:

* The first line of data contains ***k*** numbers, where ***k*** is number of dimensions in maze.
* The second line of data contains coordinates of the starting position.
* The third line of data contains coordinates of the end position.
* The fourth line of data contains coordinates of points which it is impossible to pass through.

If the input data is incorrect at any moment program ends with: ***ERROR X***, where ***X*** is a number of the line, that the first problem appeared.

If there are no problems with the data, the program ends either: 

* with a number, that is the shortest path from the start to the end point.
* with a caption: ***"NO WAY"***, when the path from the start to the end point doesn't exist.

Project operates on self written ***Bitset*** and ***Queue*** structures.
