/*
Problem:
Implement a version of the game ¡°Hunt the Wumpus.¡± 
¡°Hunt the Wumpus¡± (or just ¡°Wump¡±) is a simple (non-graphical) computer game originally invented by Gregory Yob. 
The basic premise is that a rather smelly monster lives in a dark cave consisting of connected rooms. 
Your job is to slay the wumpus using bow and arrow. In addition to the wumpus, the cave has two hazards: bottomless pits and giant bats. 
If you enter a room with a bottomless pit, it¡¯s the end of the game for you. If you enter a room with a bat, the bat picks you up and drops you into another room. 
If you enter the room with the wumpus or he enters yours, he eats you. When you enter a room you will be told if a hazard is nearby:
¡°I smell the wumpus¡±: It¡¯s in an adjoining room.
¡°I feel a breeze¡±: One of the adjoining rooms is a bottomless pit.
¡°I hear a bat¡±: A giant bat is in an adjoining room.
For your convenience, rooms are numbered. Every room is connected by tunnels to three other rooms. 
When entering a room, you are told something like ¡°You are in room 12; there are tunnels to rooms 1, 13, and 4; move or shoot?¡± 
Possible answers are m13 (¡°Move to room 13¡±) and s13¨C4¨C3 (¡°Shoot an arrow through rooms 13, 4, and 3¡±). 
The range of an arrow is three rooms. At the start of the game, you have five arrows. 
The snag about shooting is that it wakes up the wumpus and he moves to a room adjoining the one he was in ¡ª that could be your room.
Probably the trickiest part of the exercise is to make the cave by selecting which rooms are connected with which other rooms. 
You¡¯ll probably want to use a random number generator (e.g., randint() from std_lib_facilities.h) to make different runs of the program use different caves and to move around the bats and the wumpus. 
Hint: Be sure to have a way to produce a debug output of the state of the cave.

Solution by Zhiyuan Xue, 02/06/2017
*/
#include<vector>
#include<string>
#include<iostream>
#include<sstream>
#include<random>
#include<ctime>
using namespace std;

struct Room
{
	int c1;		//adjoining room 1
	int c2;		//adjoining room 2
	int c3;		//adjoining room 3
	Room();
	bool is_connected(int x) { return (x == c1 || x == c2 || x == c3); };
};

Room::Room()
	:c1{ 0 }, c2{ 0 }, c3{ 0 }
{
}

void wumpus_warning()
{
	cout << "I smell the wumpus.\n";
}

void bat_warning()
{
	cout << "I hear a bat.\n";
}

void pit_warning()
{
	cout << "I feel a breeze.\n";
}

bool is_in(const vector<int>&v, int x)
//check if x is in v
{
	for (int i = 0; i < v.size(); ++i)
	{
		if (v[i] == x)return true;
	}
	return false;
}

void srand_time()
//set random seed with current system time
{
	srand(unsigned(time(NULL)));
}

vector<int> diff_rands(int n, int range)
//generate n different random integers in [0,range)
{
	if (n > range) {
		ostringstream errormsg;
		errormsg << "can't generate " << n << " different random integers in range [0," << range << ')';
		throw runtime_error(errormsg.str());
	}
	vector<int> v;
	int x = 0;
	for (int i = 0; i < n; ++i) {
		do{
			x = rand() % range;
			//cout << x << endl;	//test
		} while (is_in(v, x));
		v.push_back(x);
	}
	//for (int i = 0; i < v.size(); ++i)cout << v[i] << ' '; cout << endl;	//test
	return v;
}

class Cave
{public:
	Cave();
	void current_status();
	void warning();
	void moving(int);	//do changes after a moving
	void shooting(int, int, int);	//do changes after a shooting
	void command(istream&);		//taking a command
	void prompt();
	bool p_dead() const { return player_dead; }
	bool w_dead() const { return wumpus_dead; }
private:
	vector<Room>map;
	int player;
	int arrow;
	int wumpus;
	int bat1;
	int bat2;
	int pit1;
	int pit2;
	bool player_dead;
	bool wumpus_dead;
};

Cave::Cave()
//generate the starting game map
	:map{ vector<Room>(20) }, player{ 0 }, arrow{ 5 }, wumpus{ 0 }, bat1{ 0 }, bat2{ 0 }, pit1{ 0 }, pit2{ 0 }, player_dead{ false }, wumpus_dead{ false }
{
	//setting all adjoining rooms
	map[0].c1 = 1;		map[0].c2 = 2;		map[0].c3 = 3;
	map[1].c1 = 0;		map[1].c2 = 4;		map[1].c3 = 5;
	map[2].c1 = 0;		map[2].c2 = 6;		map[2].c3 = 7;
	map[3].c1 = 0;		map[3].c2 = 8;		map[3].c3 = 9;
	map[4].c1 = 1;		map[4].c2 = 9;		map[4].c3 = 10;
	map[5].c1 = 1;		map[5].c2 = 6;		map[5].c3 = 11;
	map[6].c1 = 2;		map[6].c2 = 5;		map[6].c3 = 12;
	map[7].c1 = 2;		map[7].c2 = 8;		map[7].c3 = 13;
	map[8].c1 = 3;		map[8].c2 = 7;		map[8].c3 = 14;
	map[9].c1 = 3;		map[9].c2 = 4;		map[9].c3 = 15;
	map[10].c1 = 4;		map[10].c2 = 11;	map[10].c3 = 16;
	map[11].c1 = 5;		map[11].c2 = 10;	map[11].c3 = 17;
	map[12].c1 = 6;		map[12].c2 = 13;	map[12].c3 = 17;
	map[13].c1 = 7;		map[13].c2 = 12;	map[13].c3 = 18;
	map[14].c1 = 8;		map[14].c2 = 15;	map[14].c3 = 18;
	map[15].c1 = 9;		map[15].c2 = 14;	map[15].c3 = 16;
	map[16].c1 = 10;	map[16].c2 = 15;	map[16].c3 = 19;
	map[17].c1 = 11;	map[17].c2 = 12;	map[17].c3 = 19;
	map[18].c1 = 13;	map[18].c2 = 14;	map[18].c3 = 19;
	map[19].c1 = 16;	map[19].c2 = 17;	map[19].c3 = 18;
	//room 0 will be player's starting room, so I keep it empty
	//setting bats and pits
	vector<int>rds = diff_rands(4, 19);
	bat1 = rds[0] + 1;
	bat2 = rds[1] + 1;
	pit1 = rds[2] + 1;
	pit2 = rds[3] + 1;
	//setting wumpus starting room
	wumpus = rand() % 19 + 1;	
}

void Cave::current_status()
//for testing
{
	cout << "player: " << player << ' ' << player_dead << endl
		<< "arrow: " << arrow << endl
		<< "wumpus: " << wumpus << ' ' << wumpus_dead << endl
		<< "bats: " << bat1 << ' ' << bat2 << endl
		<< "pits: " << pit1 << ' ' << pit2 << endl;
}

void Cave::warning()
{
	cout << "Warning:\n";
	const Room& rm = map[player];
	if (wumpus == rm.c1 || wumpus == rm.c2 || wumpus == rm.c3) wumpus_warning();
	if ((bat1 == rm.c1 || bat1 == rm.c2 || bat1 == rm.c3) || (bat2 == rm.c1 || bat2 == rm.c2 || bat2 == rm.c3))bat_warning();
	if ((pit1 == rm.c1 || pit1 == rm.c2 || pit1 == rm.c3) || (pit2 == rm.c1 || pit2 == rm.c2 || pit2 == rm.c3))pit_warning();
}

void Cave::moving(int x)
//check validity in command()
{
	player = x;
	if (player == wumpus) {
		player_dead = true;
		cout << "You are eaten by the wumpus.\n";
		return;
	}
	if (player == bat1 || player == bat2)
	{
		cout << "There is a giant bat in the room.\n"
			<< "You are droped to Room ";
		int i = rand() % 3;
		switch (i)
		{
		case 0:
			cout << map[player].c1 << endl;
			moving(map[player].c1);
			break;
		case 1:
			cout << map[player].c2 << endl;
			moving(map[player].c2);
			break;
		case 2:
			cout << map[player].c3 << endl;
			moving(map[player].c3);
			break;
		}
	}
	if (player == pit1 || player == pit2) { 
		player_dead = true;
		cout << "You fall into a bottomless pit.\n";
	}
}

void Cave::shooting(int a, int b, int c)
//check validity in command()
{
	if (wumpus == a || wumpus == b || wumpus == c) {
		cout << "You shoot the wumpus.\n";
		wumpus_dead = true;
		--arrow;
	}
	else {
		cout << "You wake up the wumpus.\n";
		int i = rand() % 3;
		switch (i)
		{
		case 0:
			wumpus = map[wumpus].c1;
			break;
		case 1:
			wumpus = map[wumpus].c2;
			break;
		case 2:
			wumpus = map[wumpus].c3;
			break;
		}
		if (wumpus == player) {
			player_dead = true;
			cout << "You are eaten by the wumpus.\n";
		}
		--arrow;
		if (arrow == 0)player_dead = true;
	}
	cout << "You have " << arrow << " arrow(s) left.\n";
}

void ignore_line(istream&is)
{
	string s;
	getline(is, s);
}

void Cave::command(istream&is) {
	while (true) {
		char ch;
		is >> ch;
		switch (ch)
		{
		case 'm':
		{
			is >> ch;
			if (!isdigit(ch)) {
				cout << "Invalid command m: unrecognized format\n";
				ignore_line(is);
				break;
			}
			is.unget();
			int next;
			is >> next;
			if (!map[player].is_connected(next)) {
				cout << "Invalid command m: not an adjoining room\n";
				ignore_line(is);
				break;
			}
			moving(next);
			ignore_line(is);
			return;
		}
		case 's':
		{
			int s1, s2, s3 = 0;
			char ch1, ch2 = 0;
			is >> s1 >> ch1 >> s2 >> ch2 >> s3;
			if (!is) {
				is.clear();
				ignore_line(is);
				cout << "Invalid command s: unrecognized format\n";
				break;
			}
			if (ch1 != '-' || ch2 != '-') {
				ignore_line(is);
				cout << "Invalid command s: unrecognized format\n";
				break;
			}
			if (!(map[player].is_connected(s1) && map[s1].is_connected(s2) && map[s2].is_connected(s3))) {
				ignore_line(is);
				cout << "Invalid command s: not a series of connected rooms\n";
				break;
			}
			shooting(s1, s2, s3);
			ignore_line(is);
			return;
		}
		default:
			cout << "Invalid command\n";
			ignore_line(is);
		}
	}
}

void Cave::prompt()
//to prompt for a command
{
	cout << "You are in room " << player
		<< "; there are tunnels to rooms "
		<< map[player].c1 << ", " << map[player].c2 << ", and " << map[player].c3
		<< "; move or shoot ?\n";
}

//-------------------------------------------------------------

int main()
try{
	srand_time();	//initialize random seed
	Cave c;
	while (true)
	{
		c.prompt();
		c.warning();
		c.command(cin);
		//c.current_status();	//test
		if (c.w_dead()) {
			cout << "The wumpus is hunted down, victory!\n";
			return 0;
		}
		if (c.p_dead()) {
			cout << "Game over!\n";
			return 0;
		}
	}
}
catch (exception&e)
{
	cerr << e.what() << endl;
	return 1;
}
catch (...)
{
	cerr << "Unknown error!\n";
	return 2;
}