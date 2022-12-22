#include <iostream>
#include <queue>
#include <cstring>
#include<unistd.h>
#include <cmath>
#include <random>
#include<chrono> 

#define torpedo_yards500    650
#define torpedo_yards750	510
#define torpedo_yards1050	330	
#define torpedo_yards1350	330	
#define torpedo_yards1650	305	
#define torpedo_yards1950	273	
#define torpedo_yards2250	550	
#define torpedo_yards2550	500	
#define torpedo_yards2850	150	
#define torpedo_yards3130	250	

#define TORPEDO 0
#define GUN 1 

//map settings 
#define x_size 50
#define y_size 50
#define one_tile 3000
#define MOVE_SIZE 20


#define MOVEMENT 3333
#define INITIALSTATE 1400
#define CHASSESTATE 1401
#define SHOOT 1402
#define DESTROYED 20001
#define CHANCE_TO_NOT_STEER 90
#define AGILITY_STEERING_DEVIATION 5



//Ha-201-class submarine Nation Japan
#define Ha_201_hp 22
#define Ha_201_speed 6.08333
#define Ha_201_radar_range 17190


/*Boat section*/
//USS Claxton (DD-571)
#define USS_Claxton_hp 104	//2080 /20
#define USS_Claxton_speed 18.05556 //m/s
#define USS_Claxton_radar_range 37000 //m

//USS Amberjack (SS-219)
#define SS_219_hp 77	//1549 /20
#define SS_219_speed 10.8333333 //m/s
#define SS_219_radar_range 54864 //m

//German destroyer Z43
#define Z43_hp 128 // 2559/20
#define Z43_speed 18.61111 //m/s
#define Z43_radar_range 22000 //m

/*Weapon section*/
//5-inch/38-caliber gun
#define caliber_gun_weapon_dmg 5
#define caliber_gun_weapon_count 5
#define caliber_gun_reload_time 4 //s
#define caliber_gun_range 15903 //m
#define caliber_gun_magazine_size 1  
#define caliber_gun_shell_velocity 760 //m/s
#define caliber_gun_accuracy 100 // tbd
#define caliber_gun_count_magazines 200

//Mark 15 torpedo
#define mark_torpedo_weapon_dmg 65
#define mark_torpedo_weapon_count 4
#define mark_torpedo_reload_time 250 // mark_torpedo_range / mark_torpedo_shell_velocity
#define mark_torpedo_range 5500 //m
#define mark_torpedo_magazine_size 1  
#define mark_torpedo_shell_velocity 23.15 //m/s
#define mark_torpedo_accuracy 350 // tbd
#define mark_torpedo_count_magazines 2

//G7e torpedo
#define G7e_weapon_dmg 65
#define G7e_weapon_count 4
#define G7e_reload_time 350 // torpedo_range / shell_velocity
#define G7e_range 5000 //m
#define G7e_magazine_size 1  
#define G7e_shell_velocity 15.43333 //m/s
#define G7e_accuracy 350 // tbd
#define G7e_count_magazines 2

//12.7 cm SK C/34 naval gun
#define C_34_naval_gun_weapon_dmg 6
#define C_34_naval_gun_weapon_count 5
#define C_34_naval_gun_reload_time 4 //15–18 rpm
#define C_34_naval_gun_range 17400 //m
#define C_34_naval_gun_magazine_size 1  
#define C_34_naval_gun_shell_velocity 830 //m/s
#define C_34_naval_gun_count_magazines 100 //tbd
#define C_34_naval_gun_accuracy 80 // tbd


#define SS_219_torpedo_weapon_dmg 65
#define SS_219_torpedo_weapon_count 6
#define SS_219_torpedo_reload_time 150 // torpedo_range / shell_velocity
#define SS_219_torpedo_range 3130 //m
#define SS_219_torpedo_magazine_size 1  
#define SS_219_torpedo_shell_velocity 23.15 //m/s
#define SS_219_torpedo_count_magazines 3 //tbd
#define SS_219_torpedo_accuracy 350 // tbd


//Type 95 torpedo
#define Type_95_torpedo_weapon_dmg 65
#define Type_95_torpedo_weapon_count 3
#define Type_95_torpedo_reload_time 350 // torpedo_range / shell_velocity
#define Type_95_torpedo_range 9000 //m
#define Type_95_torpedo_magazine_size 1  
#define Type_95_torpedo_shell_velocity 26.66 //m/s
#define Type_95_torpedo_count_magazines 2 //tbd
#define Type_95_torpedo_accuracy 350 // tbd




using namespace std;



void hello(){
    //printf("%s","hello");
}

enum lastMovement {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};


struct coords
{
    double x;
    double y;
};

struct distance_move
{
    double distance;
    int id;
    int x;
    int y;
};

struct lock
{
    int move;
};


struct arguments
{
    string name;
    int team; 
    int count;
    coords start_cords[100];
};
struct boats;
arguments arg_list[400];
int map[y_size][x_size];

class Weapon{
    public:
        double range{}; 
        int reaload_time;
        int start_shooting; 
        int dmg;
        int accuracy; //0-1000 1000 is 100% hit 
        int magazine_size;
        int magazine_size_default;
        int count_magazines; 
        int type;
        double speed;
        Weapon() {}
        Weapon(int type_ ,int dmg_,double weapon_range,int accuracy_,int magazine_size_, int count_magazines_, int reaload_time_, double speed_)
        {
            range = weapon_range/one_tile;
            reaload_time = reaload_time_;
            dmg = dmg_;
            type= type_;
            accuracy = accuracy_;
            magazine_size = magazine_size_;
            magazine_size_default = magazine_size_;
            count_magazines = count_magazines_; 
            speed = speed_/one_tile;
            start_shooting = 0 ;
        }

};



struct calendar;
struct CompareTime;
class Boat {
    public:
        coords location;

        int id{}; 
        std::priority_queue<calendar> queue{};
        double speed{}; 
        int team{};
        int hp{};
        lock lock1;
        double radar_range{};
        Weapon weapon[100]{};
        int weapon_size{};
        int state{}; 
        Boat() = default;
        Boat(   int id_,
                string type,
                int hp_,
                double speed_,
                coords start_location,
                double radar_range_,
                Weapon weapon_[100],
                int team_,
                int state_)
         {
            location=start_location;
            id=id_;
            hp=hp_;
            team=team_;
            speed=speed_/one_tile;
            radar_range=radar_range_/one_tile;
            state = state_;
            lock1.move = 0;
            weapon_size = 0;
            *weapon = *weapon_;
            queue;
        }
        int move_up(){
            if(int(round(location.y))==0) return 0;
            if(map[int(round(location.y))-1][int(round(location.x))]!=0) return -1;
            map[int(round(location.y))][int(round(location.x))] = 0;
            location.y = location.y - this->speed;
            map[int(round(location.y))][int(round(location.x))] = team;
            return 1; 
        }
        int move_down(){
            if(int(round(location.y))==y_size-1) return 0;
            if(map[int(round(location.y))+1][int(round(location.x))]!=0) return -1;
            map[int(round(location.y))][int(round(location.x))] = 0;
            location.y = location.y + this->speed;
            map[int(round(location.y))][int(round(location.x))] = team;
            return 1; 
        }
        int move_left(){
            if(int(round(location.x))==0) return 0;
            if(map[int(round(location.y))][int(round(location.x))-1]!=0) return -1;
            map[int(round(location.y))][int(round(location.x))] = 0;
            location.x = location.x-this->speed;
            map[int(round(location.y))][int(round(location.x))] = team;
            return 1;
        }
        int move_right(){
            if(int(round(location.x))==x_size-1) return 0; //detekce konce mapy  
            if(map[int(round(location.y))][int(round(location.x))+1]!=0) return -1; //detekce střetu 
            map[int(round(location.y))][int(round(location.x))] = 0;
            location.x = location.x+this->speed;
            map[int(round(location.y))][int(round(location.x))] = team;
            return 1;
        }
      
};

struct calendar
{
    int id;
    unsigned int atime;
    unsigned int priority;
    int dmg;
    int creator;
    int (Boat::*movement)();
     bool operator<(const calendar& rhs) const
    {
        return atime < rhs.atime;
    }
};

struct CompareTime {
    bool operator()(calendar const& p1, calendar const& p2)
    {
        // return "true" if "p1" is ordered
        // before "p2", for example:
        return p1.atime < p2.atime;
    }
};







