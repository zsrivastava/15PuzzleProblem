
//  15_puzz.cpp
//  AI Project
//  zds238
//  Submitted by Zubin Srivastava on 4/9/20.
//  Copyright © 2020 Zubin Srivastava. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <string>
using namespace std;

class Board{ //Class created to begin program, creates the board for the initial state and calculates the manhattan distance
public:
    int current_display[4][4] = {};
    int m_sum = 0;
    map<int, vector<int>> goal_tiles;
    map<int, vector<int>> current_tiles;
    int create_manhattan(){
        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < 4; ++j){
                int value = current_display[i][j];
                m_sum += abs(current_tiles[value][0] - goal_tiles[value][0]);
                m_sum += abs(current_tiles[value][1] - goal_tiles[value][1]);
            }
        }
        return m_sum;
    }
    //move = move made, y_val = changes for u/d, x_val = changes for l/r, num = value that gets swapped with the empty space, 0.
};


class State{ //State includes the tiles both in array form and
public:
    int display[4][4]; //tiles display, used primarily to change the tiles when a move is made
    int level;  //g_n, number of moves made from start node
    int m_sum; //manhattan distance
    int space_x; //x coordinate of space
    int space_y; //y coordinate of space
    vector<char> moves; //used to keep track of the moves that are made
//    Board* spaces;
    //pointers connected to the node, creates relations which are helpful when determining if a future node is valid
    State* up = nullptr;
    State* down = nullptr;
    State* left = nullptr;
    State* right = nullptr;
    map<int, vector<int>> goal_tiles; //maps goal state tile to its x and y coordinates
    map<int, vector<int>> current_tiles; //maps current state tile to its x and y coordinates
    int a_star;
    bool leaf = true; //boolean to determine whether or not it is a leaf, and can be expanded further
    vector<State*> leaves; //vector of State* of the current leaves, is sorted to find smallest f(n) value
    vector<State*> searched_nodes; //vector of State* of the nodes that were already searched
    
        State(int board[4][4], int lev, int man, vector<char>mov, map<int, vector<int>>curr, const map<int, vector<int>>goal){
        level = lev;
        m_sum = man;
        moves = mov;
        current_tiles = curr;
        goal_tiles = goal;
        a_star = m_sum + level;
        space_y = current_tiles[0][0];
        space_x = current_tiles[0][1];
        
        for(int i = 0; i < 4; ++i){
           for(int j = 0; j < 4; ++j){
               int value = board[i][j];
               display[i][j] = value;
           }
        }
    }
    
    void display_board(){
        //Displays the Board
        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < 4; ++j){
                cout << display[i][j] << " ";
            }
            cout << endl;
        }
    }
    
    State* make_move(){
        //Leaf at back of leaves vector will be popped because it is the state that has the smallest f(n) value and will now be expanded
        leaves.pop_back();
        //checks to see if move is made in all directions, if it is then it is made
        if(left == nullptr && space_x != 0){
            move_left(display, space_x);
        }
        if(right == nullptr && space_x != 3){
            move_right(display, space_x);
        }
        if(up == nullptr && space_y != 0){
            move_up(display, space_y);
        }
        if(down == nullptr && space_y != 3){
            move_down(display, space_y);
        }
        //leaves are sorted
        sort_leaves();
        
        //now new States are given vectors that are the same as the State that was just expanded
        if(left != nullptr && left->leaf == true){
            left->leaves = leaves;
            left->searched_nodes = this->searched_nodes;
        }
        if(right != nullptr && right->leaf == true){
            right->leaves = leaves;
            right->searched_nodes = this->searched_nodes;
        }
        if(up != nullptr && up->leaf == true){
            up->leaves = leaves;
            up->searched_nodes = this->searched_nodes;
            //            cout << "# Leaves: " << up->leaves.size() << endl;
        }
        if(down != nullptr && down->leaf == true){
            down->leaves = leaves;
            down->searched_nodes = this->searched_nodes;
        }
        
        //now new State in leaves vector that has the smallest f(n) value, aka will be expanded next, is assigned the leaves and searched_nodes vectors
        leaves.back()->leaves = leaves;
        leaves.back()->searched_nodes = this->searched_nodes;
        return leaves.back();
    }
    
    void move_left(int display[4][4], int space){
        //makes moves the zero tile one space left
            int new_display[4][4];
            for(int i = 0; i < 4; ++i){
                for(int j = 0; j < 4; ++j){
                    new_display[i][j] = display[i][j];
                }
            }
            //swaps values in the display
            swap(new_display[space_y][space], new_display[space_y][space-1]);
            //value of tile being swapped with zero
            int value = new_display[space_y][space];
            map<int, vector<int>> copy = current_tiles;
            //updates the tiles, as well as the manhattan values
            int dist = update_manhattan(m_sum, 'L', space_y, space, value, copy);
        //checks to see if the new tiles match with any of the states in the searched nodes vector
        if(compare_states(copy)==1){
            //if they don't match they a new state is created
            create_new('L', new_display, copy, dist, space_y, space);
        }
    }
    
    void move_right(int display[4][4], int space){
        //works same as move_left, just different values/tiles are being swapped
        int new_display[4][4];
        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < 4; ++j){
                new_display[i][j] = display[i][j];
            }
        }
        swap(new_display[space_y][space], new_display[space_y][space+1]);
        int value = new_display[space_y][space];
        map<int, vector<int>> copy = current_tiles;
        int dist = update_manhattan(m_sum, 'R', space_y, space, value, copy);
        if(compare_states(copy)==1){
            create_new('R', new_display, copy, dist, space_y, space);
        }
    }
    
    void move_up(int display[4][4], int space){
        //works same as move_left, just different values/tiles are being swapped
            int new_display[4][4];
            for(int i = 0; i < 4; ++i){
                for(int j = 0; j < 4; ++j){
                    new_display[i][j] = display[i][j];
                }
            }
            swap(new_display[space][space_x], new_display[space-1][space_x]);
            int value = new_display[space][space_x];
            map<int, vector<int>> copy = current_tiles;
            int dist = update_manhattan(m_sum, 'U', space, space_x, value, copy);
            if(compare_states(copy)==1){
                create_new('U', new_display, copy, dist, space, space_x);
            }
        }
    
    void move_down(int display[4][4], int space){
        //works same as move_left, just different values/tiles are being swapped
            int new_display[4][4];
            for(int i = 0; i < 4; ++i){
                for(int j = 0; j < 4; ++j){
                    new_display[i][j] = display[i][j];
                }
            }
            swap(new_display[space][space_x], new_display[space+1][space_x]);
            int value = new_display[space][space_x];
            map<int, vector<int>> copy = current_tiles;
            int dist = update_manhattan(m_sum, 'D', space, space_x, value, copy);
            if(compare_states(copy)==1){
                create_new('D', new_display, copy, dist, space, space_x);
            }
        }
    
    int update_manhattan(int m_dist, char move, int y_val, int x_val, int num, map<int, vector<int>>& tiles){
        int dist1 = 0;
        int dist2 = 0;
        if(move == 'L'){ //Move Left
            //First the manhattan distance is calculated between the zero tile and the tile to the left of it
            dist1 += abs(tiles[0][1] - goal_tiles[0][1]);
            dist1 += abs(tiles[num][1] - goal_tiles[num][1]);
            //then the tiles are swapped
            tiles[0][1] = x_val - 1;
            tiles[num][1] = x_val;
            //Then the manhattan distance is calculated between the two tiles after they are swapped
            dist2 += abs(goal_tiles[0][1] - tiles[0][1]);
            dist2 += abs(goal_tiles[num][1] - tiles[num][1]);
            //dist2-dist1 is the net change in manhattan distance, which can be positive or negative, and is added to m_dist, which is returned which will be the new manhattan distance
            m_dist += (dist2 - dist1);
        }
        else if(move == 'R'){ //Move Right
            //First the manhattan distance is calculated between the zero tile and the tile to the right of it
            dist1 += abs(tiles[0][1] - goal_tiles[0][1]);
            dist1 += abs(tiles[num][1] - goal_tiles[num][1]);
            //then the tiles are swapped
            tiles[0][1] = x_val + 1;
            tiles[num][1] = x_val;
            //Then the manhattan distance is calculated between the two tiles after they are swapped
            dist2 += abs(goal_tiles[0][1] - tiles[0][1]);
            dist2 += abs(goal_tiles[num][1] - tiles[num][1]);
            //dist2-dist1 is the net change in manhattan distance, which can be positive or negative, and is added to m_dist, which is returned which will be the new manhattan distance
            m_dist += (dist2 - dist1);
        }

        else if(move == 'U'){ //Move Up
            //First the manhattan distance is calculated between the zero tile and the tile above it
            dist1 += abs(tiles[0][0] - goal_tiles[0][0]);
            dist1 += abs(tiles[num][0] - goal_tiles[num][0]);
            //then the tiles are swapped
            tiles[0][0] = y_val - 1;
            tiles[num][0] = y_val;
            //Then the manhattan distance is calculated between the two tiles after they are swapped
            dist2 += abs(tiles[0][0] - goal_tiles[0][0]);
            dist2 += abs(tiles[num][0] - goal_tiles[num][0]);
            //dist2-dist1 is the net change in manhattan distance, which can be positive or negative, and is added to m_dist, which is returned which will be the new manhattan distance
            m_dist += (dist2 - dist1);
        }
        
        else if(move == 'D'){ //Move Down
            //First the manhattan distance is calculated between the zero tile and the tile below it
            dist1 += abs(tiles[0][0] - goal_tiles[0][0]);
            dist1 += abs(tiles[num][0] - goal_tiles[num][0]);
            //then the tiles are swapped
            tiles[0][0] = y_val + 1;
            tiles[num][0] = y_val;
            //Then the manhattan distance is calculated between the two tiles after they are swapped
            dist2 += abs(goal_tiles[0][0] - tiles[0][0]);
            dist2 += abs(goal_tiles[num][0] - tiles[num][0]);
            //dist2-dist1 is the net change in manhattan distance, which can be positive or negative, and is added to m_dist, which is returned which will be the new manhattan distance
            m_dist += (dist2 - dist1);
        }
        return m_dist;
    }

    
    int is_goal_node(){
        //checks to see if the node (State) is the goal node
        if(m_sum == 0){
            return 1;
        }
        return 0;
    }
    
int compare_states(map<int, vector<int>> curr){
    //checks to see if the map of tiles being passed match any already in the searched_nodes vector
     for(size_t i = 0; i < searched_nodes.size(); ++i){
         if(searched_nodes[i]->current_tiles == curr){
             //if the tiles are the same, then a new State will not be created
             return 0;
         }
     }
    return 1;
 }
    
    void create_new(char cmd, int display[4][4], map<int, vector<int>> tiles, int m_dist, int y_value, int x_value){
        //Creates new node, will be passed when confirmed that State has not already been searched
        leaf = false; //no longer a leaf because it is being expanded
//        int dup = 0;
        //new State created on the heap
        State* newbie = new State(display, level+1, m_dist, moves, tiles, goal_tiles);
//        searched_nodes.
        if(cmd == 'L'){
            //Make Left Pointer
            this->left = newbie;
            //pointers on left and right are created to make it easier not to make the State being expanded repeat
            newbie->right = this;
            //move added to move vector
            newbie->moves.push_back('L');
        }
        if(cmd == 'R'){
            //Make Right Pointer
            this->right = newbie;
            //pointers on left and right are created to make it easier not to make the State being expanded repeat
            newbie->left = this;
            //move added to move vector
            newbie->moves.push_back('R');
        }
        if(cmd == 'U'){
            //Make Up Pointer
            this->up = newbie;
            //pointers up and down are created to make it easier not to make the State being expanded repeat
            newbie->down = this;
            //move added to move vector
            newbie->moves.push_back('U');
        }
        if(cmd == 'D'){
            //Make Down Pointer
            this->down = newbie;
            //pointers up and down are created to make it easier not to make the State being expanded repeat
            newbie->up = this;
            //move added to move vector
            newbie->moves.push_back('D');
        }
        //new State pushed on leaves vector because it will not be searched
            leaves.push_back(newbie);
        
    }
    
    void sort_leaves(){
        //sorts leaves in descending a_star (f(n)) value
        sort(leaves.begin(), leaves.end(), [](State*& lhs, State*& rhs){
            return lhs->a_star > rhs->a_star;
        });
    }
};

State* play(State* curr){
    //game is played until it reaches a State that is not the goal State with the smallest a* value
    while(!(curr->is_goal_node())){
        //searched_nodes vector added on to
        curr->searched_nodes.push_back(curr);
        State* back = curr->make_move();
        curr = back;
    }
    return curr;
}

void f_values(State* top){
    //recursively writes f(n) values along path from initial to goal states
    int level = top->level;
    if(level == 0){
        cout << top->a_star << " ";
        return;
    }
    if(top->left != nullptr && top->left->level == level-1){
        f_values(top->left);
    }
    else if(top->right != nullptr && top->right->level == level-1){
        f_values(top->right);
    }
    else if(top->up != nullptr && top->up->level == level-1){
        f_values(top->up);
    }
    else if(top->down != nullptr && top->down->level == level-1){
        f_values(top->down);
    }
    cout << top->a_star << " ";
}




int main(int argc, const char * argv[]) {
    Board game;
    int goal_display[4][4];
    ifstream infile;
    //Change file name below
    infile.open("Input4.txt");
    if(!infile){
        cout << "File could not be uploaded\n";
        exit(1);
    }
    //File Read
    while(!infile.eof()){
        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < 4; ++j){
                infile >> game.current_display[i][j];
            }
        }
    
        for(int i = 0; i < 4; ++i){
            for(int j = 0; j < 4; ++j){
                infile >> goal_display[i][j];
            }
        }
    }
    //Map of tiles created
    for(int i = 0; i < 4; ++i){
        for(int j = 0; j < 4; ++j){
            int c_value = game.current_display[i][j];
            int g_value = goal_display[i][j];
            game.goal_tiles[g_value].push_back(i);
            game.goal_tiles[g_value].push_back(j);
            game.current_tiles[c_value].push_back(i);
            game.current_tiles[c_value].push_back(j);

        }
    }

    //REMINDER: When testing move left/right, space_x must be second parameter. When testing up/down, space_y must be second parameter
    //State created
    State* x = new State(game.current_display, 0, game.create_manhattan(), {}, game.current_tiles, game.goal_tiles);
    x->leaves.push_back(x);
    State* y = play(x);
    x->display_board();
    cout << endl;
    y->display_board();
    cout << endl;
    cout << y->level << endl;
    cout << (y->searched_nodes.size() + y->leaves.size()) << endl;
    for(size_t i = 0; i < y->moves.size(); ++i){
        cout << y->moves[i] << " ";
    }
    cout << endl;
    f_values(y);
    cout << endl;
    return 0;
    
}
