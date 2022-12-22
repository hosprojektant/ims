#include "ims.h"


Boat boats[2000];

int stat_1 = 0;
int stat_2 = 0;

int random_range(int min,int max){

    int range = max - min + 1;  
    int num = rand() % range + min;
    return num;
}


void clean_que(int index){
    while (!boats[index].queue.empty())
    {
        
        boats[index].queue.pop();
    }
}

void clean_que_from_movent(int index){
    std::priority_queue<calendar> temp{};
    while (!boats[index].queue.empty())
    { 
        calendar head = boats[index].queue.top();
        if(head.id==SHOOT){
            temp.push(head);
        }   
        boats[index].queue.pop();
    }
    boats[index].queue = temp;
}

void clean_que_spam(int index){
    while (!boats[index].queue.empty())
    {
        
        boats[index].queue.pop();
    }
}



void move_to_boat(calendar new_move,int index,int time)
{
    if (time > boats[index].lock1.move){
        clean_que_from_movent(index);
        time ++;
        new_move.id = MOVEMENT;
        new_move.atime = -time;
        boats[index].lock1.move = time;
        new_move.priority = 1;
        boats[index].queue.push(new_move);
    }
}

distance_move get_index_closest(int index, int N)
{
    distance_move dis;
    distance_move dis_finall;
    dis_finall.x=0;
    double dist=0;
    double last_dist=9999;
    int index_to_save = 0;
    for (int i = 1; i < N; i++) {
        if(boats[i].id == DESTROYED) continue;
        if(boats[i].team == boats[index].team) continue;
        dis.x=boats[i].location.x - boats[index].location.x;
        dis.y=boats[i].location.y - boats[index].location.y;
        dist= sqrt((dis.x) * (dis.x)
                    + (dis.y) * (dis.y));

        if (dist <= boats[index].radar_range && dist < last_dist){
            last_dist = dist;
            dis_finall.id=i;
            dis_finall.x=dis.x;
            dis_finall.y=dis.y;
            dis_finall.distance=dist;
        }
        
    }
    return dis_finall; 
}

void checkPointRange( int index,int N,int time)
{
    calendar new_move{};
    
    distance_move dis = get_index_closest(index,N);
    if(dis.x){
        int abs_dis_x = abs(dis.x);
        int abs_dis_y = abs(dis.y);
        if(abs_dis_x < abs_dis_y){ 
          
                
                if(dis.y<0)  new_move.movement = &Boat::move_up;
                if(dis.y>0)  new_move.movement = &Boat::move_down;
                move_to_boat(new_move,index,time);
            
        }else{
        
                if(dis.x<0)  new_move.movement =    &Boat::move_left;
                if(dis.x>0)  new_move.movement =    &Boat::move_right;

                move_to_boat(new_move,index,time);
            
            
        }
        boats[index].state=2;
        
            
    }
        //printf("0");
    
}


int torpedo_chance(double distance){
    int pick_chance = random_range(0,1000);
    distance = distance *1.09*one_tile; //to yards
    if(distance < 500.0 ){
        if(pick_chance < torpedo_yards500){
            return 1;
        }
    }else if(distance < 750.0){
        if(pick_chance < torpedo_yards750){
            return 1;
        }
    }else if(distance < 1050.0){
        if(pick_chance < torpedo_yards1050){
            return 1;
        }
    }
    else if(distance < 1350.0){
        if(pick_chance < torpedo_yards1350){
            return 1;
        }
    }
    else if(distance < 1650.0){
        if(pick_chance < torpedo_yards1650){
            return 1;
        }
    }
    else if(distance < 1950.0){
        if(pick_chance < torpedo_yards1950){
            return 1;
        }
    }
    else if(distance < 2250.0){
        if(pick_chance < torpedo_yards2250){
            return 1;
        }
    }else if(distance < 2550.0){
        if(pick_chance < torpedo_yards2550){
            return 1;
        }
    }
    else if(distance < 2850.0){
        if(pick_chance < torpedo_yards2850){
            return 1;
        }
    }else if(distance < 3130.0){
        if(pick_chance < torpedo_yards3130){
            return 1;
        }
    }else{
        if(pick_chance < 100){
            return 1;
        }
    }
    return 9999;
}

void gun_chance(double distance, int accuracy ){
    


}


void shoot(int yours_index,int enemy_index, int  weapon_index, double distance, int time){
    int pick_chance = 9999;
    if(boats[yours_index].weapon[weapon_index].type == TORPEDO)
    {
        pick_chance = torpedo_chance(distance); 
    }else{
        pick_chance = random_range(0,1000);
    }
    if(pick_chance <  boats[yours_index].weapon[weapon_index].accuracy){ 
              
        calendar c1;
        int time_shoot = time + round(distance/(boats[yours_index].weapon[weapon_index].speed));
        c1.atime = -time_shoot;
        c1.id = SHOOT;
        c1.priority = 2;
        c1.creator = yours_index;
        
        unsigned seed = chrono::steady_clock::now().time_since_epoch().count(); 
        default_random_engine e (seed);
        normal_distribution<double> distribution(boats[yours_index].weapon[weapon_index].dmg,boats[yours_index].weapon[weapon_index].dmg/4);

        c1.dmg = distribution(e);
        if(c1.dmg < 0 ){
            c1.dmg = 0;
        }
        boats[enemy_index].queue.push(c1);
    }
}

void start_shoting(int index,int N, int time){
    distance_move dis = get_index_closest(index, N);
        for (int y = 0; y < boats[index].weapon_size-1; y++)
        {
            // if is in range 
            if(dis.x){
                if (dis.distance <= boats[index].weapon[y].range){     
                    if(boats[index].weapon[y].start_shooting > time){ continue;} //it means we are realoading 
                    else {
                        if (boats[index].weapon[y].magazine_size == 0){
                            boats[index].weapon[y].magazine_size = boats[index].weapon[y].magazine_size_default;
                        }
                    }
                   
                    shoot(index,dis.id,y,dis.distance,time);
                    boats[index].weapon[y].magazine_size--;
                    if(boats[index].weapon[y].magazine_size == 0 ){ //if magazine have no ammo we are realoading 
                        boats[index].weapon[y].start_shooting = time+boats[index].weapon[y].reaload_time;
                    }
                    
                
                }
                else{
                
                }
            }
        }
    
   
}









int args(int argc, char** argv){
    int arg_list_counter=-1;
    int params_left=0;
    int flag_start_cords=0;
    for (size_t i = 1; i < argc; i++)
    {   

        char* item = argv[i];
            if(params_left < 0){
                cerr<<"error1"<<endl;
                exit(-2);
            }
            if(!strcmp(item,"-b")){
                if(params_left > 0){
                    cerr<<"error2"<<endl;
                    exit(-2);
                }
                arg_list_counter++;
                params_left = 4; 
            }
            switch (params_left)
            {
            case 3:
                arg_list[arg_list_counter].name=item;
                break;
            case 2:
                arg_list[arg_list_counter].team=atoi(item);
                break;
            case 1:
                arg_list[arg_list_counter].count=atoi(item);
                break;
            default:
                break;
            }
            params_left--;
     
    }

    return arg_list_counter;
}
void create_boat(int id,int team,string name,int x, int y){
    //USA
    if(name == "DD571"){
        Weapon weapons[100];
        int weapon_counter = 0;
        for ( weapon_counter=weapon_counter ; weapon_counter < mark_torpedo_weapon_count; weapon_counter++)
        {
            weapons[weapon_counter] = Weapon(TORPEDO,mark_torpedo_weapon_dmg, mark_torpedo_range, mark_torpedo_accuracy, mark_torpedo_magazine_size,
                                                mark_torpedo_count_magazines, mark_torpedo_reload_time, mark_torpedo_shell_velocity);
            
        }
        for ( weapon_counter=weapon_counter ; weapon_counter < caliber_gun_weapon_count; weapon_counter++)
        {
            weapons[weapon_counter] = Weapon(GUN,caliber_gun_weapon_dmg, caliber_gun_range,caliber_gun_accuracy, caliber_gun_magazine_size,
                                                caliber_gun_count_magazines, caliber_gun_reload_time, caliber_gun_shell_velocity);
            
        }
        
        coords start_cord;
        start_cord.x = x;
        start_cord.y = y;
        Boat USS_DD_571(id,name,USS_Claxton_hp,USS_Claxton_speed,start_cord,USS_Claxton_radar_range,weapons,team,INITIALSTATE);  
        boats[id] = USS_DD_571;
        boats[id].weapon_size = weapon_counter;
        map[int(round(start_cord.y))][int(round(start_cord.x))]=team;
    }

    //Germany
    if(name == "Z43"){
        Weapon weapons[100];
        int weapon_counter = 0;
        for ( weapon_counter=weapon_counter ; weapon_counter < G7e_weapon_count; weapon_counter++)
        {
            weapons[weapon_counter] = Weapon(TORPEDO,G7e_weapon_dmg, G7e_range, G7e_accuracy, G7e_magazine_size,
                                                G7e_count_magazines, G7e_reload_time, G7e_shell_velocity);
            
        }
        for ( weapon_counter=weapon_counter ; weapon_counter < C_34_naval_gun_weapon_count; weapon_counter++)
        {
            weapons[weapon_counter] = Weapon(GUN,C_34_naval_gun_weapon_dmg, C_34_naval_gun_range, C_34_naval_gun_accuracy, C_34_naval_gun_magazine_size,
                                                C_34_naval_gun_count_magazines, C_34_naval_gun_reload_time, C_34_naval_gun_shell_velocity);
        }
        coords start_cord;
        start_cord.x = x;
        start_cord.y = y;
        Boat USS_DD_571(id,name,Z43_hp,Z43_speed,start_cord,Z43_radar_range,weapons,team,INITIALSTATE);  
        boats[id] = USS_DD_571;
        boats[id].weapon_size = weapon_counter;
        map[int(round(start_cord.y))][int(round(start_cord.x))]=team;
    }
    if(name == "SS219"){
        Weapon weapons[100];
        int weapon_counter = 0;
        for ( weapon_counter=weapon_counter ; weapon_counter < SS_219_torpedo_weapon_count; weapon_counter++)
        {
            weapons[weapon_counter] = Weapon(TORPEDO,SS_219_torpedo_weapon_dmg, SS_219_torpedo_range, SS_219_torpedo_accuracy, SS_219_torpedo_magazine_size,
                                                SS_219_torpedo_count_magazines, SS_219_torpedo_reload_time, SS_219_torpedo_shell_velocity);
            
        }

        coords start_cord;
        start_cord.x = x;
        start_cord.y = y;
        Boat SS_219(id,name,SS_219_hp,SS_219_speed,start_cord,SS_219_radar_range,weapons,team,INITIALSTATE);  
        boats[id] = SS_219;
        boats[id].weapon_size = weapon_counter+1;
        map[int(round(start_cord.y))][int(round(start_cord.x))]=team;
    }
    if(name == "HA201"){
        Weapon weapons[100];
        int weapon_counter = 0;
        for ( weapon_counter=weapon_counter ; weapon_counter < Type_95_torpedo_weapon_count; weapon_counter++)
        {
            weapons[weapon_counter] = Weapon(TORPEDO,Type_95_torpedo_weapon_dmg, Type_95_torpedo_range, Type_95_torpedo_accuracy, Type_95_torpedo_magazine_size,
                                                Type_95_torpedo_count_magazines, Type_95_torpedo_reload_time, Type_95_torpedo_shell_velocity);
            
        }
        coords start_cord;
        start_cord.x = x;
        start_cord.y = y;
        Boat Ha_201(id,name,Ha_201_hp,Ha_201_speed,start_cord,Ha_201_radar_range,weapons,team,INITIALSTATE);  
        boats[id] = Ha_201;
        boats[id].weapon_size = weapon_counter+1;
        map[int(round(start_cord.y))][int(round(start_cord.x))]=team;
    }

}

void team_counter(int team_counter){
    if(team_counter > x_size/4){
        cerr<<"Error max number of all boats for one team its  "<< x_size/4 <<endl;
        exit(-2);
    }
}


int setup_boats(int size_arg_list){
    int id = 1; 
    int team1_x_counter=0;
    int team2_x_counter=0;
    for (size_t i = 0; i < size_arg_list; i++)
    {
        if(arg_list[i].team==1){
            for (size_t q = 0; q < arg_list[i].count; q++)
            {
                int x = random_range(0,3)+team1_x_counter*4;
                int y = random_range(0,6);
                create_boat(id,arg_list[i].team,arg_list[i].name,x,y);
                team1_x_counter++;
                id++;
                team_counter(team1_x_counter);
            }
        }else if(arg_list[i].team==2){
            for (size_t q = 0; q < arg_list[i].count; q++)
            {
                int x = random_range(0,3)+team2_x_counter*4;
                int y = y_size-random_range(0,6)-1;
                create_boat(id,arg_list[i].team,arg_list[i].name,x,y);
                team2_x_counter++;
                id++;
                team_counter(team2_x_counter);
            }
        }

        
    }
    return id;
}



void setup_map(){
    
    for (size_t y = 0; y < y_size; y++)
    {
        for (size_t x = 0; x < x_size; x++)
        {
            map[y][x]=0;
        }
    }


}


int get_initial_move(int index,  int accumulated_action_time) {
    calendar first_move{};
    int last_movement = NONE;
    int chance = random_range(0, 100);
    if (chance < 80) {
        if (boats[index].team == 1) {
            first_move.movement = &Boat::move_down;
            last_movement = DOWN;
        } else {
            first_move.movement = &Boat::move_up;
            last_movement = UP;
        }
    }

    if (last_movement != UP && last_movement != DOWN) {
        if (chance < 90) {
            first_move.movement = &Boat::move_left;
            last_movement = LEFT;
        } else {
            first_move.movement = &Boat::move_right;
            last_movement = RIGHT;
        }
    }

    if (boats[index].speed == 0.0) {
        cerr << "The boat has 0 speed (immovable object detected)" << endl;
        exit(-2);
    }
    accumulated_action_time += 1;
    first_move.id = MOVEMENT;
    first_move.atime = -(accumulated_action_time);
    first_move.priority = 1;
    boats[index].queue.push(first_move);
    return last_movement;
}

void get_default_movement(int  index, int accumulated_action_time, int last_movement) {
    int chance_to_not_steer = 80;
    int chance_to_steer = 100 - chance_to_not_steer;
    int chance_tns_side = chance_to_not_steer - (chance_to_not_steer / 5);
    int chance_ts_side = 100 - chance_tns_side;
    int chance;
    for (int i = 0; i < MOVE_SIZE - 1; i++) { // TODO exponencialne rozdelenie so simlibom
        calendar new_move{};
        chance = random_range(0, 100);
        if (last_movement == NONE) {
            new_move.movement = &Boat::move_up;
            last_movement = UP;
        } else if (last_movement == UP) {
            if (chance < chance_to_not_steer) {
                new_move.movement = &Boat::move_up;
            } else if (chance < (chance_to_not_steer + chance_to_steer / 2)) {
                new_move.movement = &Boat::move_left;
                last_movement = LEFT;
            } else {
                new_move.movement = &Boat::move_right;
                last_movement = RIGHT;
            }
        } else if (last_movement == LEFT) {
            if (chance < chance_tns_side) {
                new_move.movement = &Boat::move_left;
            } else if (chance < (chance_tns_side + chance_ts_side / 2)) {
                if (boats[index].team == 1) {
                    new_move.movement = &Boat::move_down;
                    last_movement = DOWN;
                } else {
                    new_move.movement = &Boat::move_up;
                    last_movement = UP;
                }
            } else {
                if (boats[index].team == 1) {
                    new_move.movement = &Boat::move_up;
                    last_movement = UP;
                } else {
                    new_move.movement = &Boat::move_down;
                    last_movement = DOWN;
                }
            }
        } else if (last_movement == RIGHT) {
            if (chance < chance_tns_side) {
                new_move.movement = &Boat::move_right;
            } else if (chance < (chance_tns_side + chance_ts_side / 2)) {
                if (boats[index].team == 1) {
                    new_move.movement = &Boat::move_down;
                    last_movement = DOWN;
                } else {
                    new_move.movement = &Boat::move_up;
                    last_movement = UP;
                }
            } else {
                if (boats[index].team == 1) {
                    new_move.movement = &Boat::move_up;
                    last_movement = UP;
                } else {
                    new_move.movement = &Boat::move_down;
                    last_movement = DOWN;
                }
            }
        } else if (last_movement == DOWN) {
            if (chance < chance_to_not_steer) {
                new_move.movement = &Boat::move_down;
            } else if (chance < (chance_to_not_steer + chance_to_steer / 2)) {
                new_move.movement = &Boat::move_left;
                last_movement = LEFT;
            } else {
                new_move.movement = &Boat::move_right;
                last_movement = RIGHT;
            }
        }

        if (boats[index].speed == 0) {
            cerr << "The boat has 0 speed (immovable object detected)" << endl;
            exit(-2);
        }
        accumulated_action_time += 1;
        new_move.id = MOVEMENT;
        new_move.atime = -(accumulated_action_time);
        new_move.priority = 1;
        boats[index].queue.push(new_move);
       
    }
}

void move_boat(int index, unsigned int act_time) {

    int accumulated_action_time = act_time;
    int last_movement = get_initial_move(index, accumulated_action_time);
    get_default_movement(index, accumulated_action_time, last_movement);
}




int main(int argc, char** argv) {
    srand(time(NULL));
    int test= 0;
    int size_arg_list = args(argc,argv)+1;
    setup_map();
    int id = setup_boats(size_arg_list);
    
    
    int t = 0;
    while (1)
    {   
        for (int i = 1; i < id; i++)
        {

            if(boats[i].queue.empty() && boats[i].state != DESTROYED){
                boats[i].state = INITIALSTATE; 
            }
            switch (boats[i].state)
            {
            case INITIALSTATE:
                if(boats[i].queue.empty()){
                    move_boat(i,t);
                    boats[i].state = 1; 
                }
                break;
            case 1:
                checkPointRange(i,id,t);
                break;
            case 2:
                start_shoting(i,id,t);
                break;
            default:
                break;
            }
        }
            //*
        for (int i = 1; i < id; i++)
        {
  
             
                calendar head = boats[i].queue.top();
                int flag = 0; 
                while (-head.atime == t){
                     if(head.id == SHOOT ){
                        boats[i].hp = boats[i].hp-head.dmg;
                        if(boats[i].hp < 0 ){
                            boats[i].state = DESTROYED;
                            boats[i].id = DESTROYED;
                            map[int(round(boats[i].location.y))][int(round(boats[i].location.x))] = DESTROYED;

                            test = 2;
                        }
                    }
                    else if(head.id == MOVEMENT){
                        (boats[i].*head.movement)();
                    }
                        if(boats[i].queue.empty()){ break;}
                        
                        boats[i].queue.pop();
                        head = boats[i].queue.top();
                } 
                
               
            
        }
        
        if((t+1)%50000==0)
        {  
            int count_not_destroyed_t1 = 0; 
            int count_not_destroyed_t2 = 0; 
            for (int i = 1; i < id; i++){
                if(boats[i].team==1 && boats[i].state!=DESTROYED){
                    count_not_destroyed_t1++;
                }
                if(boats[i].team==2 && boats[i].state!=DESTROYED){
                    count_not_destroyed_t2++;
                }
            }
            exit(0);
        }

        t++;
        if(t%500 == 0 ){

        for (size_t y = 0; y < y_size; y++)
        {
            for (size_t x = 0; x < x_size; x++)
            {   
                if(map[y][x] == DESTROYED){
                    printf("@");
                }
                else if(map[y][x] == 0){
                    printf("-");
                }else{

                    printf("%d",map[y][x]);
                }
            }
            printf("\n");
        }
        printf("\n");
   
        usleep(200000); //sleep 0.2s
        }
    }
    
    return 0;
}
