//
//  main.cpp
//  FindPath
//
//  Created by Sara Lindström on 2017-02-08.
//  Copyright © 2017 Sara Lindström. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

using namespace std;

// ----REPRESENTING NODES ON MAP----
struct Node
{
    int x;
    int y;
    double costG;
    double costF;
    int parentX;
    int parentY;
    bool operator == (const Node node2)
    {
        return (x == node2.x && y == node2.y);
    }
};

// ----METHODS SPECIFIC FOR NODES----

// Check if a node is vertical to current node
bool isHorizontalOrVertical(const int x, const int y, const Node& current)
{
    if (x == current.x || y == current.y)
        return true;
    else
        return false;
}

// Returns cost to mode from current node to end node, using manhattan method
double calcCostH(Node endNode, int x, int y)
{
    return (std::abs(endNode.x - x) + std::abs(endNode.y - y)) * 10;
}

// ----REPRESENTING 2D MAP----
struct Map
{
    int _row;
    int _col;
    vector< vector<int> > _mapdata;
    
    // Transforming a 1D map into a 2D map
    Map(const int row, const int col, const unsigned char* pMap)
    {
        _row = row;
        _col = col;
        vector< vector<int> > pMap2D(_row, vector<int>(_col));
        
        //Looping through cols and rows to add values from the position on the 1D map to the 2D map
        int index = 0;
        for (int y = 0; y < _row; ++y)
        {
            for (int x = 0; x < _col; ++x)
            {
                pMap2D[y][x] = pMap[index];
                ++index;
            }
        }
        _mapdata = pMap2D;
    }
    
    const int getMap(const int x, const int y)
    {
        return _mapdata[y][x];
    }
    
    // Check if the node is on the map, that it's not a wall and not diagonal to current node
    bool isValid(const int x, const int y, const Node& current)
    {
        if ((x < 0 || x >= _col) || (y < 0 || y >= _row) || _mapdata[y][x] == 0 || !isHorizontalOrVertical(x, y, current))
            return false;
        else
            return true;
    }
    
    void setMap(const int x, const int y)
    {
        _mapdata[y][x] = 33;
    }
    
    void printMap()
    {
        for (int y = 0; y < _row; ++y)
        {
            for (int x = 0; x < _col; ++x)
                cout << setw(2) << _mapdata[y][x] << " ";
            cout << std::endl;
        }
    }
};

// ----METHODS FOR SEARCHING THROUGH LISTS----

// Check if node is in closed list
bool isInClosedList(const int x, const int y, const vector<Node>& closedList)
{
    bool isOnClosedList =  false;
    for (int i = 0; i < closedList.size(); ++i)
    {
        if (closedList[i].x == x && closedList[i].y == y)
        {
            isOnClosedList = true;
            break;
        }
    }
    return isOnClosedList;
}

// Get index for node in the closed list
Node findNodeInClosedList(const vector<Node>& closedList, const int x, const int y)
{
    Node invalidNode = {-1, -1, -1, -1, -1, -1};
    for (Node i : closedList)
    {
        if (i.x == x && i.y == y)
            return i;
    }
    return invalidNode;
}

// ----METHODS FINDING PATH AND RETURNING 1D MAP AND FILLING NODES IN BUFFER LIST----

// Finds path by going from end node to its parent, and from that to its parent and so on, then reverses the path
vector<Node> createPath(const vector<Node>& closedList, const int endX, const int endY)
{
    vector<Node> path;
    Node current = findNodeInClosedList(closedList, endX, endY);
    
    while(!(current.parentX == current.x && current.parentY == current.y))
    {
        path.push_back(current);
        current = findNodeInClosedList(closedList, current.parentX, current.parentY);
    }
    
    reverse(path.begin(), path.end());
    return path;
}

vector<int> convertTo1D(const vector<Node>& nodeList, const int nMapWidth)
{
    vector<int> nodeList1D;
    for(int i = 0; i < nodeList.size(); ++i)
        nodeList1D.push_back(nodeList[i].x + (nodeList[i].y * nMapWidth));
    return nodeList1D;
}

void printPathOnMap(const vector<Node>& path, Map& map)
{
    for (Node i : path)
    {
        map.setMap(i.x, i.y);
    }
    cout << "*----MAP-----*" << endl;
    map.printMap();
    cout << "*------------*" << endl;
    cout << endl;
}

void fillBuffer(const vector<Node>& path, int* pOutBuffer, const int nMapWidth)
{
    vector<int> path1D = convertTo1D(path, nMapWidth);
    cout << "*-BUFFERLIST-*" << endl;
    for (int i = 0; i < path1D.size(); ++i)
    {
        pOutBuffer[i] = path1D[i];
        cout << pOutBuffer[i] << endl;
    }
    cout << "*------------*" << endl;
    cout << endl;
}

// ----MAIN ALGORITHM----
int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY, const unsigned char* pMap,
             const int nMapWidth, const int nMapHeight, int* pOutBuffer, const int nOutBufferSize)
{
    // Set costG, create start and end node, create 2D map and open and closed list
    double childCostG = 10;
    Node endNode = {nTargetX, nTargetY, childCostG, 0};
    double costHStart = calcCostH(endNode, nStartX, nStartY);
    Node startNode = {nStartX, nStartY, 0, costHStart, 0, 0};
    Map map = Map(nMapHeight, nMapWidth, pMap);
    vector<Node> openList = {startNode};
    vector<Node> closedList;
    
    // Set start node to current
    Node current = openList[0];
    
    // Main loop, continue until end node is found or open list is empty
    while (openList.size() != 0)
    {
        // Create a vector with nodes horizontal and vercial to open list to later check if its empty then there is no valid step
        vector<Node> horizontalOrVertical;
        for (Node n : openList) {
            if(isHorizontalOrVertical(n.x, n.y, current)) {
                horizontalOrVertical.push_back(n);
            }
        }
        
        int index = 0;
        bool onlyDiagonal = false;
        
        if (openList.size() == 1)
        {
            // If only one node on open list and it's not the current one and it's diagonal, there is no path
            if(!(current == openList[0]) && !isHorizontalOrVertical(openList[0].x, openList[0].y, current))
                onlyDiagonal = true;
        }
        // If there is no horizontal or vertical node, there is no path
        else if (horizontalOrVertical.size() == 0)
        {
            onlyDiagonal = true;
        }
        else
        {
            // For horizontal or vertical node, get node with lowest F-cost
            for(int i = 0; i < openList.size(); ++i)
            {
                if (isHorizontalOrVertical(openList[i].x, openList[i].y, current))
                {
                    if (openList[i].costF <= openList[index].costF)
                        index = i;
                }
            }
        }
        
        // If current is the same as end, print path on map and break
        if (current == endNode)
        {
            vector<Node> path = createPath(closedList, endNode.x, endNode.y);
            
            // Checks if the nOutBufferSize is big enough and if not, increases it and calls the function again
            if (path.size() > nOutBufferSize)
            {
                cout << "nOutBufferSize too small, trying again with nOutBufferSize: " << nOutBufferSize*2 << endl;
                cout << endl;
                FindPath(nStartX, nStartY, nTargetX, nTargetY, pMap, nMapWidth, nMapHeight, pOutBuffer, nOutBufferSize*2);
            }
            else
            {
                printPathOnMap(path, map);
                fillBuffer(path, pOutBuffer, nMapWidth);
            }
            
            return static_cast<int>(path.size());
        }
        
        // If there is only diagonal nodes to walk on, end program with failure
        if (onlyDiagonal)
            return -1;
        
        // Update current node, openList and closedList
        current = openList[index];
        openList.erase(openList.begin() + index);
        closedList.push_back(current);
        
        // Loop through all adjacent nodes
        for(int x = (current.x - 1); (x <= current.x + 1); ++x)
        {
            for(int y = current.y - 1; y <= current.y + 1; ++y)
            {
                if (isInClosedList(x, y, closedList))
                    continue;
                // If it's a wall, outside map or diagonal
                if (!map.isValid(x, y, current))
                    continue;
                
                // Creates the child node
                double costH = calcCostH(endNode, x, y);
                double costG = childCostG + current.costG;
                double costF = costG + costH;
                Node child = {x, y, costG, costF, current.x, current.y};
                
                // Checks if its in openList
                bool isInOpenList = false;
                for (int i = 0; i < openList.size(); ++i)
                {
                    if ((openList[i] == child))
                    {
                        // If it's in openList, check if costG is smaller, if so update the node on the list
                        if (costG < openList[i].costG)
                        {
                            openList[i].costG = costG;
                            openList[i].costF = costF;
                            openList[i].parentX = current.x;
                            openList[i].parentY = current.y;
                        }
                        isInOpenList = true;
                        break;
                    }
                }
                
                // If it's not on the open list, add it
                if (!isInOpenList)
                    openList.push_back(child);
            }
        }
    }
    return -1;
}

int main(int argc, const char * argv[])
{
// TEST 1
//    unsigned char pMap[] = {1, 1, 1, 1,
//                            0, 1, 0, 1,
//                            0, 1, 1, 1};
//    int pOutBuffer[12];
//    cout << FindPath(0, 0, 1, 2, pMap, 4, 3, pOutBuffer, 2) << endl;

// TEST 2
//    unsigned char pMap[] = {0, 0, 1,
//                            0, 1, 1,
//                            1, 0, 1};
//    int pOutBuffer[7];
//    cout << FindPath(2, 0, 0, 2, pMap, 3, 3, pOutBuffer, 7) << endl;
    
// TEST 3
//    unsigned char pMap[] = {1, 1, 1, 1,
//                            0, 1, 1, 1,
//                            0, 1, 1, 1,
//                            0, 1, 1, 1,
//                            0, 0, 0, 1};
//    int pOutBuffer[30];
//    cout << FindPath(0, 0, 3, 4, pMap, 4, 5, pOutBuffer, 30) << endl;
    
// TEST 4
    unsigned char pMap[] = {1, 1, 1, 1, 0, 1, 1,
                            1, 0, 1, 1, 1, 1, 1,
                            1, 1, 0, 0, 0, 1, 1,
                            1, 1, 1, 0, 0, 1, 1,
                            1, 0, 0, 0, 0, 0, 1,
                            1, 1, 0, 0, 0, 1, 1};
    int pOutBuffer[30];
    cout << FindPath(0, 0, 6, 5, pMap, 7, 6, pOutBuffer, 4) << endl;
    
    cout << endl;
    
    return 0;
}

