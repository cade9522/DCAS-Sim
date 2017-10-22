//
// Author: Christian Evans
//

#ifndef DCAS_DCAS_H
#define DCAS_DCAS_H

#include <iostream>
#include <vector>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using std::pair;
using std::string;

//////////////////////////////////////////////////////////////////////////

class Drone{                        // class to define each Drone object

private:
    pair<int,int> _position;        // x and y position coordinates
    string _state;                  // the current state of the drone
    int _delay;                     // time delay for station release
    pair<int,int> _destination;     // drone destination coordinates
    bool _flag;                     // destination flag
    char _icon;                     // character representation of drone

public:
    Drone(int time);                // create new drone

    pair<int,int> getPosition();    // return the coordinate pair

    void setPosition(int x, int y); // set the drone coordinates

    int getState();                 // return the drone state

    void setState(string s);        // change the state of the drone

    pair<int,int> getDestination(); // return the destination of the drone

    char getChar();                 // return drone icon

    void move(int mutex);                 // update drone coordinates

    bool checkAvailable(int x, int y);    // check for collision

};

//////////////////////////////////////////////////////////////////////////

class Board {                                    // define board for (x,y) grid

private:
    std::vector<std::vector<char>> _board;       // create 2D vector of chars for coordinate storage
    std::vector<Drone*> _drones;                 // vector of drones

public:
    Board();                 // create new board, arg1 = number of drones, arg2 = time delay between takoff

    std::vector<std::vector<char>> getBoard();   // return the board

    void setBoard(char c, int x, int y);         // set a board position

    std::vector<Drone*> getDrones();             // return the vector of drones

    void add(Drone* drone);                      // add drone to the board

    void updateBoard(Drone* drone, int mutex);   // update all drone positions

    void printLayout();                          // print the board to the console
};

//////////////////////////////////////////////////////////////////////////

#endif //DCAS_DCAS_H
