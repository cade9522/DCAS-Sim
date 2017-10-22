//
// Author: Christian Evans
//

#include "../include/dcas.h"

std::vector<pthread_mutex_t> _mutex;               // create global mutex and board variables
Board* _board;

struct pthread_args{                               // pass in various pthread arguments
    Board* b;
    Drone* d;
    int mutex;
};


Drone::Drone(int delay) {
    _position = {0, 0};                            // set drone position to the airport
    _state = "rest";                               // set drone state to rest
    int x = rand() % 10;                           // create random drone destination between (0, 1) or (1, 0) and (9, 9)
    int y = rand() % 10;
    if ( x + y == 0 ){                             // if (0, 0) randomly change to (0, 1) or (1, 0)
        int guess = rand() % 2;
        if ( guess == 0 ){
            x = 1;
        }
        else {
            y = 1;
        }
    }
    _destination = {x, y};
    _delay = (delay * 4) + 1;                       // set drone release delay to 4 secs after last drone
    _flag = false;                                  // set destination flag to false
    _icon = (delay % 10) + 48;                      // set drone icon to a rotation of ints (0 - 9)
}


pair<int,int> Drone::getPosition() {
    return _position;                               // return current position
}


void Drone::setPosition(int x, int y) {
    _position = {x, y};                             // change current position
}


int Drone::getState() {
    if (_state == "rest")                           // return an integer representation of the
        return 0;                                   // current drone state
    else if (_state == "route to"){
        return 1;
    }
    else if (_state == "delivery"){
        return 2;
    }
    else if (_state == "route back"){
        return 3;
    }
    else {
        return 4;
    }
}


void Drone::setState(string s) {                    // change current drone state
    _state = s;
}


pair<int,int> Drone::getDestination() {
    return _destination;                            // return drone destination
}


char Drone::getChar(){
    return _icon;                                   // return drone character
}


void Drone::move(int mutex) {
    int state = getState();                         // create local copies of drone state,
    int x = getPosition().first;                    // position, and destination
    int y = getPosition().second;
    int x1 = getDestination().first;
    int y1 = getDestination().second;
    int run = abs( x - x1 );                        // calculate the distance to destination
    int rise = abs( y - y1 );

    switch(state) {

        case 0:                                     // for initial rest
            --_delay;
            if (_delay == 0){                       // switch to route phase
                setState("route to");
            }
            break;

        case 2:                                          // for route phase
            if (x == 0) {                                // check for destination in row 0
                pthread_mutex_lock(&_mutex.at(mutex));   // lock position set to avoid two writes
                if (checkAvailable(1, y)) {
                    setPosition(1, y);
                    setState("route back");              // route back to airport
                    _destination = {0, 0};
                }
                pthread_mutex_unlock(&_mutex.at(mutex));
            }
            else {
                setState("route back");                  // route back to airport
                _destination = {0, 0};
            }
            break;

        case 1:                                     // for routing phases
        case 3:
            if (rise == 0 && run == 0){             // check if destination reached
                if (getState() == 1){
                    if (_flag) {                    // check for destination flag
                        _destination = {x1, 0};     // route to real destination
                        _flag = false;
                    }
                    else{
                        setState("delivery");       // hold destination position for a second
                    }
                }
                else {
                    setState("done");               // complete drone cycle
                }
            }
            else if (x == 0 && y == 0 && y1 == 0){  // check for y = 0  initial destination
                y = 1;
                _destination = {x1, 1};             // change temp destination to y = 1
                _flag = true;                       // set flag
            }
            else if ( rise != 0 ){                  // check for  y destination reached
                if ( y > y1 ){                      // update +/- 1
                    --y;
                }
                if (y < y1){
                    ++y;
                }
            }
            else{                                   // update x position +/- 1
                if ( x > x1 ){
                    --x;
                }
                if ( x < x1 ){
                    ++x;
                }
            }
            pthread_mutex_lock(&_mutex.at(mutex));    // lock mutex down when checking for collision
            if (checkAvailable(x, y)){                // check for collision
                setPosition(x, y);                    // set new position
            }
            pthread_mutex_unlock(&_mutex.at(mutex));  // unlock mutex
            break;

        case 4:                                       // if "done", do nothing
            break;

    }

}


bool Drone::checkAvailable(int x, int y){             // check for drone space availability
    if (x == 0 && y == 0){                            // return if at airport
        return true;
    }
    std::vector<Drone*> drones = _board->getDrones(); // compare new position with other drone positions
    for (int i = 0; i < drones.size(); i++) {
        Drone* next = drones.at(i);
        if (this != next && next->getPosition().first == x && next->getPosition().second == y){
            return false;
        }
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////////


Board::Board() {       // declare empty board with airport at (0, 0)

    _board = {{'A', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'},
              {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '\n'}};
}


void Board::setBoard(char c, int x, int y) {         // set new character at position (x, y)
    _board.at(y).at(x) = c;
}


void Board::add(Drone* drone){                  // add new drone to the board
    _drones.push_back(drone);
}


std::vector<Drone*> Board::getDrones(){         // return vector of drones
    return _drones;
}


void Board::printLayout() {
    std::cout << "\033[2J\033[1;1H";            // clear linux terminal
    for (int i = 0; i < 10; i++){               // print each character of the board
        for (int j = 0; j < 11; j++){
            std::cout << _board.at(i).at(j);
        }
    }
}


void Board::updateBoard(Drone* drone, int mutex){
    int x = drone->getPosition().first;             // store current drone position
    int y = drone->getPosition().second;
    drone->move(mutex);                             // move drone
    if( x + y != 0 ) {                              // replace old position with empty space if not at the airport
        setBoard('-', x, y);
    }
    x = drone->getPosition().first;                 // store new drone position
    y = drone->getPosition().second;
    if ( x + y != 0 ){
        setBoard(drone->getChar(), x, y);           // replace board position with drone character
    }
}


void* move(void* arg){                              // pthread move function
    pthread_args* p = (pthread_args*) arg;          // convert to struct to attain multiple pthread arguments
    p->b->updateBoard(p->d, p->mutex);
    pthread_exit(NULL);                             // exit pthread
}


///////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) {
    while (true) {
        int num;                            // set the number of drones to either the first argument
        if (argv[1] == nullptr) {           // or 10 as a default setting
            num = 10;
        }
        else {
            num = atoi(argv[1]);
        }
        std::cout << "Welcome to the Drone Collision Avoidance System (DCAS) simulation!\nWould you like to continue? (Y/N)";
        bool input = true;
        while (input) {                               // wait for yes or no input to continue to give srand() variance
            string s;
            std::cin >> s;
            if (s == "y" || s == "Y" || s == "yes" || s == "YES") {
                input = false;
            } else if (s == "n" || s == "N" || s == "no" || s == "NO") {
                std::cout << "Have a nice day." << std::endl;
                return 0;
            } else {
                std::cout << "Invalid input, type 'Y' or 'N' only." << std::endl;
            }
        }

        srand(time(NULL));                                       // randomize number set
        _board = new Board();
        pthread_t threads[num];                                  // create a thread for each drone
        int num_threads[num];
        for (int i = 0; i < num; ++i) {                          // initialize a mutex for each drone
            Drone *drone = new Drone(i);
            _board->add(drone);
            _mutex.push_back(PTHREAD_MUTEX_INITIALIZER);
        }
        std::vector<Drone *> drones = _board->getDrones();
        _board->printLayout();                                   // print initial board layout
        int check = drones.size();
        while (check > 0) {                                      // loop drone movement until all drones at "done" state
            check = drones.size();
            pthread_args p[num];
            for (int i = 0; i < drones.size(); ++i) {            // initialize each active drone's pthread
                p[i] = {_board, drones.at(i)};
                num_threads[i] = pthread_create(&threads[i], NULL, move, &p[i] );
                if (drones.at(i)->getState() == 4) {
                    --check;                                     // check for drone state
                }
            }
            for (int i = 0; i < num; i++){                       // join pthreads together
                pthread_join(threads[i], NULL);
            }
            usleep(1000000);                                     // wait one second
            _board->printLayout();                               // print new board layout
        }

        std::cout << "\n\nNo collisions reported, all drones have landed safely." << std::endl;
        std::cout << "\nWould you like to run the program again? (Y/N)" << std::endl;
        input = true;
        while (input) {                                                   // repeat program if wanted
            string s;
            std::cin >> s;
            if (s == "y" || s == "Y" || s == "yes" || s == "YES")
                input = false;
            else if (s == "n" || s == "N" || s == "no" || s == "NO") {
                pthread_exit(NULL);                                       // exit the program
            }
            else
                std::cout << "Invalid input, type 'Y' or 'N' only." << std::endl;
        }
    }
}
