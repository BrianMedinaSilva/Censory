#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <list>
#include <climits>
#include <random>

using namespace std;

struct Combatant {
    std::string name;
    int VIT;
    int STR;
    int CHA;
    int INT;
    int VIT_mod = 0;
    int STR_mod = 0;
    int CHA_mod = 0;
    int INT_mod = 0;
    int HP;
    int maxHP;
    int HV;
    int HA;
    int actions = 1;
    bool alive = true;
    bool toggle = false;
    std::vector<std::string> skills = {};
};

struct Team {
    std::vector<Combatant> team;
    std::vector<std::string> gradebook = {};
};

struct Inventory {
    std::vector<std::string> skills = {"Attack", "Stimulate", "Sedate"};
};

struct Progress {
    int semester = 1;
    int battles_won = 0;
};

double timing() {
    std::string input = "";
    cout << "Press ENTER!";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    const auto start{std::chrono::steady_clock::now()};
    std::getline(cin, input);
    const auto finish{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{finish - start};
    return elapsed_seconds.count();
}

int select_target(Team& team, std::string who) {
    int target = 0;
    for (int i = 0; i < team.team.size(); i++) {
        cout << team.team[i].name << " HP: " << team.team[i].HP << "\n";
    }

    if (who == "enemy") {
        cout << "Who will you target? Choose an enemy from 1 to " << team.team.size() << ": ";
    }
    else if (who == "teammate") {
        cout << "Who will you target? Choose a teammate from 1 to " << team.team.size() << ": ";
    }
    else if (who == "tazewell_1") {
        cout << "Whose stat points shall you reassign? Choose a teammate from 1 to " << team.team.size() << ": ";
    }
    else if (who == "tazewell_2") {
        cout << "Whose skills will you change? Choose a teammate from 1 to " << team.team.size() << ": ";
    }

    cin >> target;
    while (target > team.team.size() || target < 1 || team.team[target-1].alive == false) {
        cout << "Invalid! Choose from 1 to " << team.team.size() << ": ";
        cin >> target;
    }
    return target-1;
}

int single_target(Team& team) {
    int target = -1;
    for (int i = 0; i < team.team.size(); i++) {
        if (team.team[i].alive == true) {
            target = i;
        }
    }
    return target;
}

int player_targeting(Team& team, std::string who, int living_team) {
    int target = -1;
    if (living_team > 1) {
        target = select_target(team, who);
    }
    else {
        target = single_target(team);
    }
    return target;
}

int enemy_targeting_lowest_hp(Team& player_team) {
    int optimal = -1;
    int min_hp = INT_MAX;
    for (int i = 0; i < player_team.team.size(); i++) {
        if (player_team.team[i].alive == true) {
            if (player_team.team[i].HP < min_hp) {
                optimal = i;
                min_hp = player_team.team[i].HP;
            }
        }
    }
    return optimal;
}

int enemy_targeting_highest_str(Team& player_team) {
    int optimal = -1;
    int max_str = INT_MIN;
    for (int i = 0; i < player_team.team.size(); i++) {
        if (player_team.team[i].alive == true) {
            if ((player_team.team[i].STR + player_team.team[i].STR_mod) > max_str) {
                optimal = i;
                max_str = player_team.team[i].STR + player_team.team[i].STR_mod;
            }
        }
    }
    return optimal;
}

void reset_mods(Team& player_team) {
    for (int i = 0; i < player_team.team.size(); i++) {
        player_team.team[i].VIT_mod = 0;
        player_team.team[i].STR_mod = 0;
        player_team.team[i].CHA_mod = 0;
        player_team.team[i].INT_mod = 0;
        player_team.team[i].HV = 0;
        player_team.team[i].HA = 0;
        player_team.team[i].alive = true;
        player_team.team[i].toggle = false;
    }
}

double toggle_pro(Combatant& user) {
    double multiplier = 1;
    if (user.toggle == true) {
        if (user.name == "Bronson") {
            multiplier = 2;
        }
        else if (user.name == "Colt") {
            srand(time(NULL));
            multiplier = 2 * (rand() % 2);
        }
        else if (user.name == "Peter") {
            multiplier = 0.5;
        }
    }
    return multiplier;
}

int toggle_con(Combatant& user, std::string stat) {
    if (user.name == "Bronson") {
        if (stat == "VIT") {
            user.VIT_mod -= user.VIT;
        }
        if (stat == "STR") {
            user.STR_mod -= user.STR;
        }
        if (stat == "CHA") {
            user.CHA_mod -= user.CHA;
        }
        if (stat == "INT") {
            user.INT_mod -= user.INT;
        }
    }
}

void check_if_downed(Combatant& target, int& living_team) {
    if (target.HP <= 0 && target.alive == true) {
        target.alive = false;
        living_team -= 1;
    }
}

void apply_HV(Team& team, int& living_team) {
    for (int i = 0; i < team.team.size(); i++) {
        if (team.team[i].alive == true) {
            team.team[i].HP += team.team[i].HV;
            if (team.team[i].HV != 0) {
                cout << team.team[i].name << "'s Health Position (HP) changed by " << int(team.team[i].HV) << ".\n";
                check_if_downed(team.team[i], living_team);
            }
        }
    }
}

void apply_HA(Team& team) {
    for (int i = 0; i < team.team.size(); i++) {
        if (team.team[i].alive == true) {
            team.team[i].HV += team.team[i].HA;
            if (team.team[i].HA != 0) {
                cout << team.team[i].name << "'s Health Velocity (HV) changed by " << int(team.team[i].HA) << ".\n";
            }
        }
    }
}

/*
void attack(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.HP -= max(0, (user.STR + user.STR_mod) * toggle_multiplier);
    cout << user.name << " attacks! " << int(max(0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage!\n";
    user.STR_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "STR");
    }
}
*/

/*
void attack(Combatant& user, Team& enemy_team, int target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
        if (user.name == "Peter") {
            toggle_multiplier /= enemy_team.team.size();
        }
    }
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[i].HP -= max(0, (user.STR + user.STR_mod) * toggle_multiplier);
        }
        cout << user.name << " attacks! " << int(max(0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage to all enemies!\n";
        user.STR_mod /= 2;
    }
    else {
        enemy_team.team[target].HP -= max(0, (user.STR + user.STR_mod) * toggle_multiplier);
        cout << user.name << " attacks! " << int(max(0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage!\n";
        user.STR_mod /= 2;
        if (user.toggle == true) {
            toggle_con(user, "STR");
        }
    }
}
*/

/*
void attack(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[i].HP -= max(0.0, (user.STR + user.STR_mod) * toggle_multiplier);
        }
        cout << user.name << " attacks! " << int(max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage to all enemies!\n";
        user.STR_mod /= 2;
    }
    else {
        enemy_team.team[target].HP -= max(0.0, (user.STR + user.STR_mod) * toggle_multiplier);
        cout << user.name << " attacks! " << int(max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage!\n";
        user.STR_mod /= 2;
        if (user.toggle == true) {
            toggle_con(user, "STR");
        }
    }
}
*/

/*
void attack(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[i].HP -= max(0.0, (user.STR + user.STR_mod) * toggle_multiplier);
        }
        cout << user.name << " attacks! " << int(max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage to all enemies!\n";
    }
    else {
        enemy_team.team[target].HP -= max(0.0, (user.STR + user.STR_mod) * toggle_multiplier);
        cout << user.name << " attacks! " << int(max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage!\n";
    }
    user.STR_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "STR");
    }
}
*/

void attack(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[i].HP -= int(max(0.0, (user.STR + user.STR_mod) * toggle_multiplier));
        }
        cout << user.name << " attacks! " << int(max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage to all enemies!\n";
    }
    else {
        enemy_team.team[target].HP -= int(max(0.0, (user.STR + user.STR_mod) * toggle_multiplier));
        cout << user.name << " attacks! " << int(max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage!\n";
    }
    user.STR_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "STR");
    }
}

void migraine(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[i].HV -= int(max(0.0, ((user.STR + user.STR_mod)/2) * toggle_multiplier));
        }
        cout << user.name << " uses Migraine! All enemies will take " << int(max(0.0, ((user.STR + user.STR_mod)/2) * toggle_multiplier)) << " extra damage every turn!\n";
    }
    else {
        enemy_team.team[target].HV -= int(max(0.0, ((user.STR + user.STR_mod)/2) * toggle_multiplier));
        cout << user.name << " uses Migraine! " << enemy_team.team[target].name << " will take "<< int(max(0.0, ((user.STR + user.STR_mod)/2) * toggle_multiplier)) << " extra damage every turn!\n";
    }
    user.STR_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "STR");
    }
}

void accelerant_migraine(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[target].HA -= int(max(0.0, ((user.STR + user.STR_mod)/4) * toggle_multiplier));
        }
        cout << user.name << " uses Accelerant Migraine! All enemies will take " << int(max(0.0, ((user.STR + user.STR_mod)/4) * toggle_multiplier)) << " extra damage every turn, each turn increasing by that amount!\n";
    }
    else {
        enemy_team.team[target].HA -= int(max(0.0, ((user.STR + user.STR_mod)/4) * toggle_multiplier));
        cout << user.name << " uses Accelerant Migraine! " << enemy_team.team[target].name << " will take "<< int(max(0.0, ((user.STR + user.STR_mod)/4) * toggle_multiplier)) << " extra damage every turn, each turn increasing by that amount!\n";
    }
    user.STR_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "STR");
    }
}

void stimulate(Combatant& user, Team& player_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < player_team.team.size(); i++) {
            player_team.team[i].STR_mod += int(max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier));
        }
        cout << user.name << " uses Stimulate! The STR of all teammates was raised by " << int(max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier)) << "!\n";
    }
    else {
        player_team.team[target].STR_mod += int(max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier));
        cout << user.name << " uses Stimulate! " << player_team.team[target].name << "'s STR was raised by " << int(max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier)) << "!\n";
    }
    user.CHA_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "CHA");
    }
}

void rizz_up(Combatant& user, Team& player_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < player_team.team.size(); i++) {
            player_team.team[i].CHA_mod += int(max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier));
        }
        cout << user.name << " uses Rizz Up! The CHA of all teammates was raised by " << int(max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier)) << "!\n";
    }
    else {
        player_team.team[target].CHA_mod += int(max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier));
        cout << user.name << " uses Rizz Up! " << player_team.team[target].name << "'s CHA was raised by " << int(max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier)) << "!\n";
    }
    user.CHA_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "CHA");
    }
}

void brain_boost(Combatant& user, Team& player_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < player_team.team.size(); i++) {
            player_team.team[i].INT_mod += int(max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier));
        }
        cout << user.name << " uses Brain Boost! The INT of all teammates was raised by " << int(max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier)) << "!\n";
    }
    else {
        player_team.team[target].INT_mod += int(max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier));
        cout << user.name << " uses Brain Boost! " << player_team.team[target].name << "'s INT was raised by " << int(max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier)) << "!\n";
    }
    user.CHA_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "CHA");
    }
}

void sedate(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[i].STR_mod -= int(max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier));
        }
        cout << user.name << " uses Sedate! The STR of all enemies was lowered by " << int(max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier)) << "!\n";
    }
    else {
        enemy_team.team[target].STR_mod -= int(max(0.0, (user.INT + user.INT_mod) * toggle_multiplier));
        cout << user.name << " uses Sedate! " << enemy_team.team[target].name << "'s STR was lowered by " << int(max(0.0, (user.INT + user.INT_mod) * toggle_multiplier)) << "!\n";
    }
    user.INT_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "INT");
    }
}

void rizz_down(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[i].CHA_mod -= int(max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier));
        }
        cout << user.name << " uses Rizz Down! The CHA of all enemies was lowered by " << int(max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier)) << "!\n";
    }
    else {
        enemy_team.team[target].CHA_mod -= int(max(0.0, (user.INT + user.INT_mod) * toggle_multiplier));
        cout << user.name << " uses Rizz Down! " << enemy_team.team[target].name << "'s CHA was lowered by " << int(max(0.0, (user.INT + user.INT_mod) * toggle_multiplier)) << "!\n";
    }
    user.INT_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "INT");
    }
}

void brain_break(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = toggle_pro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.size(); i++) {
            enemy_team.team[i].INT_mod -= int(max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier));
        }
        cout << user.name << " uses Brain Break! The INT of all enemies was lowered by " << int(max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier)) << "!\n";
    }
    else {
        enemy_team.team[target].INT_mod -= int(max(0.0, (user.INT + user.INT_mod) * toggle_multiplier));
        cout << user.name << " uses Brain Break! " << enemy_team.team[target].name << "'s INT was lowered by " << int(max(0.0, (user.INT + user.INT_mod) * toggle_multiplier)) << "!\n";
    }
    user.INT_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "INT");
    }
}

void analyze(Combatant& target) {
    cout << target.name << "\n";
    cout << "HP: " << target.HP << "/" << target.maxHP << "\n";
    cout << "HV: " << target.HV << "\n";
    cout << "HA: " << target.HA << "\n";
    cout << "VIT: " << target.VIT << " + " << target.VIT_mod << " = " << target.VIT + target.VIT_mod << "\n";
    cout << "STR: " << target.STR << " + " << target.STR_mod << " = " << target.STR + target.STR_mod << "\n";
    cout << "CHA: " <<  target.CHA << " + " << target.CHA_mod << " = " << target.CHA + target.CHA_mod << "\n";
    cout << "INT: " << target.INT << " + " << target.INT_mod << " = " << target.INT + target.INT_mod << "\n";
}

void dialogue(std::string line) {
    cout << line << "\n";
    cin.get();
}

int battle(Team& player_team, Team enemy_team) {
    for (int i = 0; i < player_team.team.size(); i++) {
        player_team.team[i].HP = player_team.team[i].VIT * 5;
        player_team.team[i].maxHP = player_team.team[i].VIT * 5;
        player_team.team[i].HV = 0;
        player_team.team[i].HA = 0;
    }
    for (int i = 0; i < enemy_team.team.size(); i++) {
        enemy_team.team[i].HP = enemy_team.team[i].VIT * 5;
        enemy_team.team[i].maxHP = enemy_team.team[i].VIT * 5;
        enemy_team.team[i].HV = 0;
        enemy_team.team[i].HA = 0;
    }

    int living_player_team = player_team.team.size();
    int living_enemy_team = enemy_team.team.size();

    while (living_player_team > 0 && living_enemy_team > 0) {
        for (int i = 0; i < player_team.team.size(); i++) {
            cout << player_team.team[i].name << " HP: " << player_team.team[i].HP << "\n";
            player_team.team[i].actions = 1;
        }
        for (int i = 0; i < enemy_team.team.size(); i++) {
            cout << enemy_team.team[i].name << " HP: " << enemy_team.team[i].HP << "\n";
            enemy_team.team[i].actions = 1;
        }

        for (int i = 0; i < player_team.team.size(); i++) {
            if (player_team.team[i].alive == true) {
                while (player_team.team[i].actions > 0) {
                    int command = 0;
                    cout << "1. " << player_team.team[i].skills[0] << " \n";
                    cout << "2. " << player_team.team[i].skills[1] << " \n";
                    cout << "3. " << player_team.team[i].skills[2] << " \n";
                    cout << "4. " << player_team.team[i].skills[3] << " \n";
                    cout << "ABILITY: " << player_team.team[i].skills[4] << " \n";

                    cout << "Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle ability, 6 to Analyze: ";
                    cin >> command;
                    while (command != 1 && command != 2 && command != 3 && command != 4 && command != 5 && command != 6) {
                        cout << "Invalid! Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle Burnout, 6 to Analyze: ";
                        cin >> command;
                    }
                    if (command == 6) {
                        int side;
                        cout << "Who will you Analyze? 1 for teammates, 2 for enemies: ";
                        cin >> side;
                        while (side != 1 && side != 2) {
                            cout << "Who will you Analyze? 1 for teammates, 2 for enemies: ";
                            cin >> side;
                        }
                        if (side == 1) {
                            int target = player_targeting(player_team, "teammate", living_player_team);
                            analyze(player_team.team[target]);
                        }
                        else if (side == 2) {
                            int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                            analyze(enemy_team.team[target]);
                        }
                        continue;
                    }
                    else if (player_team.team[i].skills[command-1] == "Attack") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        attack(player_team.team[i], enemy_team, target);
                        if (player_team.team[i].toggle == true && player_team.team[i].name == "Peter") {
                            for (int j = 0; j < enemy_team.team.size(); j++) {
                                check_if_downed(enemy_team.team[j], living_enemy_team);
                            }
                        }
                        else {
                            check_if_downed(enemy_team.team[target], living_enemy_team);
                        }
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Migraine") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        migraine(player_team.team[i], enemy_team, target);
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Accelerant Migraine") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        accelerant_migraine(player_team.team[i], enemy_team, target);
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Stimulate") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        stimulate(player_team.team[i], player_team, target);
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Rizz Up") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        rizz_up(player_team.team[i], player_team, target);
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Brain Boost") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        brain_boost(player_team.team[i], player_team, target);
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Sedate") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        sedate(player_team.team[i], enemy_team, target);
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Rizz Down") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        rizz_down(player_team.team[i], enemy_team, target);
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Brain Break") {
                        int target = player_targeting(enemy_team, "enemy", living_enemy_team);
                        brain_break(player_team.team[i], enemy_team, target);
                        player_team.team[i].actions -= 1;
                    }
                    else if (player_team.team[i].skills[command-1] == "Burnout") {
                        if (player_team.team[i].toggle == false) {
                            player_team.team[i].toggle = true;
                            cout << player_team.team[i].name << " activates Burnout.\n";
                        }
                        else {
                            player_team.team[i].toggle = false;
                            cout << player_team.team[i].name << " deactivates Burnout.\n";
                        }
                        continue;
                    }
                    else if (player_team.team[i].skills[command-1] == "Coin Flip") {
                        if (player_team.team[i].toggle == false) {
                            player_team.team[i].toggle = true;
                            cout << player_team.team[i].name << " activates Coin Flip.\n";
                        }
                        else {
                            player_team.team[i].toggle = false;
                            cout << player_team.team[i].name << " deactivates Coin Flip.\n";
                        }
                        continue;
                    }
                    else if (player_team.team[i].skills[command-1] == "Distribute") {
                        if (player_team.team[i].toggle == false) {
                            player_team.team[i].toggle = true;
                            cout << player_team.team[i].name << " activates Distribute.\n";
                        }
                        else {
                            player_team.team[i].toggle = false;
                            cout << player_team.team[i].name << " deactivates Distribute.\n";
                        }
                        continue;
                    }
                }
            }
        }

        for (int i = 0; i < enemy_team.team.size(); i++) {
            if (enemy_team.team[i].HP > 0) {
                if ((enemy_team.team[i].name == "Charisma Cannon" || enemy_team.team[i].name == "Stella" || enemy_team.team[i].name == "Lovebird" || enemy_team.team[i].name == "Glad Doctor" || enemy_team.team[i].name == "CARRIE" || enemy_team.team[i].name == "Louie") && enemy_team.team[i].STR_mod <= 0) {
                    enemy_team.team[i].STR_mod += max(0, int((enemy_team.team[i].CHA + enemy_team.team[i].CHA_mod)));
                    cout << enemy_team.team[i].name << " uses Stimulate! " << enemy_team.team[i].name << "'s STR was raised by " << int(max(0, int((enemy_team.team[i].CHA + enemy_team.team[i].CHA_mod)))) << "!\n";
                    enemy_team.team[i].CHA_mod /= 2;
                }
                else if (enemy_team.team[i].name == "Brainy Broom" || enemy_team.team[i].name == "Ambivalant" || enemy_team.team[i].name == "Sad Doctor" || enemy_team.team[i].name == "Nellie") {
                    int optimal = enemy_targeting_highest_str(player_team);
                    double t = timing();
                    if (player_team.team[optimal].STR_mod >= 0) {
                        player_team.team[optimal].STR_mod -= max(0, int((enemy_team.team[i].INT + enemy_team.team[i].INT_mod) * t));
                        cout << enemy_team.team[i].name << " uses Sedate! " << player_team.team[optimal].name << "'s STR was lowered by " << int(max(0, int((enemy_team.team[i].INT + enemy_team.team[i].INT_mod) * t))) << "!\n";
                        enemy_team.team[i].INT_mod /= 2;
                    }
                    else {
                        optimal = enemy_targeting_lowest_hp(player_team);
                        double t = timing();
                        player_team.team[optimal].HP -= max(0, int((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t));
                        cout << enemy_team.team[i].name << " attacks! " << int(max(0, int((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t))) << " damage!\n";
                        if (player_team.team[optimal].HP <= 0 && player_team.team[optimal].alive == true) {
                            player_team.team[optimal].alive = false;
                            living_player_team -= 1;
                        }
                        enemy_team.team[i].STR_mod /= 2;
                    }
                }
                else {
                    int optimal = enemy_targeting_lowest_hp(player_team);
                    double t = timing();
                    player_team.team[optimal].HP -= max(0, int((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t));
                    cout << enemy_team.team[i].name << " attacks! " << int(max(0, int((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t))) << " damage!\n";
                    if (player_team.team[optimal].HP <= 0 && player_team.team[optimal].alive == true) {
                        player_team.team[optimal].alive = false;
                        living_player_team -= 1;
                    }
                    enemy_team.team[i].STR_mod /= 2;
                }
            }
        }

        apply_HV(player_team, living_player_team);
        apply_HV(enemy_team, living_enemy_team);
        apply_HA(player_team);
        apply_HA(enemy_team);

        if (living_enemy_team == 0) {
            break;
        }

        if (living_player_team == 0) {
            cout << "GAME OVER";
            exit(0);
        }
    }

    cout << "VICTORY!\n";

    double grade_numerator = 0;
    double grade_denominator = 0;
    for (int i = 0; i < player_team.team.size(); i++) {
        grade_numerator += player_team.team[i].HP;
        grade_denominator += player_team.team[i].maxHP;
    }
    double battle_grade = grade_numerator / grade_denominator;
    if (battle_grade >= 0.9) {
        player_team.gradebook.push_back("A");
        cout << "YOU GOT AN A!";
    }
    else if (battle_grade >= 0.8) {
        player_team.gradebook.push_back("B");
        cout << "You got a B!";
    }
    else if (battle_grade >= 0.7) {
        player_team.gradebook.push_back("C");
        cout << "You got a C.";
    }
    else if (battle_grade >= 0.6) {
        player_team.gradebook.push_back("D");
        cout << "You got a D.";
    }
    else {
        player_team.gradebook.push_back("F");
        cout << "You got an F...";
    }

    int stat_boost = 0;
    cout << "Which stat will you increase? 1 for VIT, 2 for STR, 3 for CHA, 4 for INT: ";
    cin >> stat_boost;
    while (stat_boost != 1 && stat_boost != 2 && stat_boost != 3 && stat_boost != 4) {
        cout << "Invalid! Which stat will you increase? 1 for VIT, 2 for STR, 3 for CHA, 4 for INT: ";
        cin >> stat_boost;
    }
    for (int i = 0; i < player_team.team.size(); i++) {
        if (stat_boost == 1) {
            player_team.team[i].VIT += 1;
        }
        else if (stat_boost == 2) {
            player_team.team[i].STR += 1;
        }
        else if (stat_boost == 3) {
            player_team.team[i].CHA += 1;
        }
        else if (stat_boost == 4) {
            player_team.team[i].INT += 1;
        }
    }

    reset_mods(player_team);

    return 0;
}

int tazewell(Team& player_team, Inventory& inventory, Progress& progress) {
    bool in_tazewell = true;
    while (in_tazewell == true) {
        int command = 0;
        cout << "What would you like to do? 1 to reassign stats, 2 to equip skills, 3 to check gradebook, 4 to save your game, 5 to leave Tazewell: ";
        cin >> command;
        while (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
            cout << "Invalid! What would you like to do? 1 to reassign stats, 2 to equip skills, 3 to check gradebook, 4 to save your game, 5 to leave Tazewell: ";
            cin >> command;
        }
        if (command == 1) {
            int target = 0;
            if (player_team.team.size() > 1) {
                target = select_target(player_team, "tazewell_1");
            }
            int total = player_team.team[target].VIT + player_team.team[target].STR + player_team.team[target].CHA + player_team.team[target].INT;

            int invest = 0;
            cout << "How many points would you like to invest into Vitality? You have " << total << " stat point(s) left.";
            cin >> invest;
            while (1 > invest || invest > (total - 3)) {
                cout << "Invalid! How many points would you like to invest into Vitality?";
                cin >> invest;
            }
            player_team.team[target].VIT = invest;
            total -= invest;

            cout << "How many points would you like to invest into Strength? You have " << total << " stat point(s) left.";
            cin >> invest;
            while (1 > invest || invest > (total - 2)) {
                cout << "Invalid! How many points would you like to invest into Strength?";
                cin >> invest;
            }
            player_team.team[target].STR = invest;
            total -= invest;

            cout << "How many points would you like to invest into Charisma? You have " << total << " stat point(s) left.";
            cin >> invest;
            while (1 > invest || invest > (total - 1)) {
                cout << "Invalid! How many points would you like to invest into Charisma?";
                cin >> invest;
            }
            player_team.team[target].CHA = invest;
            total -= invest;

            cout << "How many points would you like to invest into Intellect? You have " << total << " stat point(s) left.";
            cin >> invest;
            while (1 > invest || invest > total) {
                cout << "Invalid! How many points would you like to invest into Intellect?";
                cin >> invest;
            }
            player_team.team[target].INT = invest;
            total -= invest;

            cout << "VIT: " << player_team.team[target].VIT << "\n";
            cout << "STR: " << player_team.team[target].STR << "\n";
            cout << "CHA: " << player_team.team[target].CHA << "\n";
            cout << "INT: " << player_team.team[target].INT << "\n";
            continue;
        }
        else if (command == 2) {
            int slot = 0;
            int target = 0;
            if (player_team.team.size() > 1) {
                target = select_target(player_team, "tazewell_2");
            }

            cout << "1. " << player_team.team[target].skills[0] << " \n";
            cout << "2. " << player_team.team[target].skills[1] << " \n";
            cout << "3. " << player_team.team[target].skills[2] << " \n";
            cout << "4. " << player_team.team[target].skills[3] << " \n";
            cout << "Which slot would you like to change? Choose a slot from 1 to 4: ";
            cin >> slot;
            while (slot > 4 || slot < 1) {
                cout << "Invalid! Which slot would you like to change?";
                cin >> slot;
            }

            int replacement = 0;
            for (int i = 0; i < inventory.skills.size(); i++) {
                cout << inventory.skills[i] << " \n";
            }
            cout << "Which skill will you put in its place? 1 for the first slot, 2 for the second slot, etc: ";
            cin >> replacement;
            while (replacement > inventory.skills.size() || replacement < 1) {
                cout << "Invalid! Which skill will you put in its place? 1 for the first slot, 2 for the second slot, etc: ";
                cin >> replacement;
            }

            player_team.team[target].skills[slot-1] = inventory.skills[replacement-1];

            cout << "1. " << player_team.team[target].skills[0] << " \n";
            cout << "2. " << player_team.team[target].skills[1] << " \n";
            cout << "3. " << player_team.team[target].skills[2] << " \n";
            cout << "4. " << player_team.team[target].skills[3] << " \n";
            continue;
        }
        else if (command == 3) {
            int grade_a = 0;
            bool has_reward = false;

            for (int i = 0; i < player_team.gradebook.size(); i++) {
                cout << player_team.gradebook[i] << " \n";
                if (player_team.gradebook[i] == "A") {
                    grade_a += 1;
                }
            }
            if (grade_a >= 4) {
                has_reward = false;
                for (int j = 0; j < inventory.skills.size(); j++) {
                    if (inventory.skills[j] == "Migraine") {
                        has_reward = true;
                    }
                }
                if (has_reward == false) {
                    inventory.skills.push_back("Migraine");
                    cout << "For getting 4 As, you can now equip the skill Migraine.\n";
                }
            }
            if (grade_a >= 8) {
                has_reward = false;
                for (int j; j < inventory.skills.size(); j++) {
                    if (inventory.skills[j] == "Accelerant Migraine") {
                        has_reward = true;
                    }
                }
                if (has_reward == false) {
                    inventory.skills.push_back("Accelerant Migraine");
                    cout << "For getting 8 As, you can now equip the skill Accelerant Migraine.\n";
                }
            }
            continue;
        }
        else if (command == 4) {
            ofstream file;
            file.open("save.txt");
            file << progress.semester << "\n";
            file << progress.battles_won << "\n";
            for (int i = 0; i < player_team.gradebook.size(); i++) {
                file << player_team.gradebook[i] << "\n";
            }
            for (int i = 0; i < player_team.team.size(); i++) {
                file << player_team.team[i].name << "\n";
                file << player_team.team[i].VIT << "\n";
                file << player_team.team[i].STR << "\n";
                file << player_team.team[i].CHA << "\n";
                file << player_team.team[i].INT << "\n";
                file << player_team.team[i].skills[0] << "\n";
                file << player_team.team[i].skills[1] << "\n";
                file << player_team.team[i].skills[2] << "\n";
                file << player_team.team[i].skills[3] << "\n";
            }
            file.close();
            cout << "The save system is currently a work-in-progress. ";
            continue;
        }
        else if (command == 5) {
            cout << "You left Tazewell.\n";
            in_tazewell = false;
        }
    }
    }

int main() {
    Combatant bronson = {"Bronson", 5, 5, 5, 5};
    bronson.skills = {"Attack", "Stimulate", "Sedate", "N/A", "Burnout"};
    Team player_team = { { bronson } };
    Inventory inventory;
    Progress progress;

    dialogue("CENSORY - SEMESTER 1");
    dialogue("CARRIED-OVER TRAUMA");
    dialogue("BOSWELL HALL - 3:00 PM");
    dialogue("BRONSON: I gotta go to office hours... but shit, I don't have a pencil. Guess I gotta steal one.");
    dialogue("(Suddenly, the landscape around BRONSON changes completely.)");
    dialogue("BRONSON: Huh?! What is this place, and why does it look like a fucking math problem?");
    dialogue("BRONSON: Ah well, nothing to do but find a way out. Let's roll.");

    cout << "BATTLE 1-1-1: AVERAGE JOE \n";
    Combatant average_joe_1 = {"Average Joe", 6, 6, 6, 6};
    Team enemy_team = { { average_joe_1 } };
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    cout << "BATTLE 1-1-2: POSTSYNAPTIC POLICE \n";
    Combatant postsynaptic_police_1 = {"Postsynaptic Police", 8, 8, 4, 4};
    enemy_team = { { postsynaptic_police_1 } };
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    cout << "BATTLE 1-1-3: BUMBLING BURGLAR \n";
    Combatant bumbling_burglar_1 = {"Bumbling Burglar", 4, 12, 4, 4};
    enemy_team = { { bumbling_burglar_1 } };
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    dialogue("BRONSON: This is it. The final room. There's gotta be...");
    dialogue("BRONSON: THERE IT IS!");
    dialogue("You notice a pencil underneath a desk. As you grab the pencil, the desk suddenly starts moving...");
    dialogue("BRONSON: Who do you think I am, Indiana Jones?");
    dialogue("The desk begins to shake the ground below you... you struggle to stand for a brief moment.");
    dialogue("BRONSON: Ah shit... do I have to fight... a literal fucking desk?!");

    cout << "BATTLE 1-1-4: A LITERAL FUCKING DESK\n";
    Combatant a_literal_fucking_desk = {"A Literal Fucking Desk", 10, 10, 5, 5};
    enemy_team = { { a_literal_fucking_desk } };
    battle(player_team, enemy_team);
    progress.battles_won += 1;
   
    dialogue("BRONSON: Finally... at long last... I have a pencil for office hours!");
    dialogue("HEIST SUCCESSFUL!");
    dialogue("All of a sudden, the desk begins to sound. A blue laser is pointed towards BRONSON's head.");
    dialogue("A LITERAL FUCKING DESK: SCANNING BRAIN ACTIVITY...");
    dialogue("BRONSON: What is this, 1984?!");
    dialogue("The desk rapidly deconstructs and constructs itself into another form.");
    dialogue("???: Greetings, Bronson.");
    dialogue("BRONSON: No... this can't be... Carrie? What are YOU doing here?! Why are you a desk?! Why-");
    dialogue("???: I am not the real person that you are referring to. As you just stated, I am the desk you just destroyed.");
    dialogue("BRONSON: Then why do you look like her? How do you know her?");
    dialogue("???: After my destruction, I had to quickly take a new form to prevent myself from dying. Thus, I scanned your brain activity for potential candidates. And what do you know, a certain someone just lives rent-free in your head.");
    dialogue("???: However, it was more than just that. I need your help with something, and I assumed that this form... would make you cooperate.");
    dialogue("BRONSON: What are you talking about?!");
    dialogue("???: You see, a military general by the name of Colonel Eigen has taken over this world we both currently reside in. He's killed countless organisms, and he won't stop until someone makes him.");
    dialogue("BRONSON: I wish I could help you out, but I have homework. Now if I can just go in peace...");
    dialogue("CARRIE: If you do that, this world, the Universal Set, shall overtake your own world, transforming your world into a place driven by logic alone. It will be perfect... however it will be hollow.");
    dialogue("BRONSON: Oh, so like my old high school then. Alright, fine. I'll help you out... not Carrie.");
    dialogue("CARRIE: You can call me CARRIE. It's short for Cognitive Artificial Reality Rendering Intelligence Expert.");
    dialogue("CARRIE: Welcome to the Universal Set, Bronson.");
    dialogue("(They shake hands. BRONSON blushes a bit.)");
    dialogue("< END OF HEIST 1 >");

    // insert goofy ahh side cutscene here
    tazewell(player_team, inventory, progress);

    // insert pre-mission cutscene here
    Combatant colt = {"Colt", 6, 6, 6, 6};
    colt.skills = {"Attack", "Rizz Up", "Rizz Down", "N/A", "Coin Flip"};
    player_team.team.push_back(colt);
    inventory.skills.push_back("Rizz Up");
    inventory.skills.push_back("Rizz Down");

    cout << "BATTLE 1-2-1: AVERAGE JOE DUO \n";
    Combatant average_joe_2 = {"Average Joe", 8, 8, 8, 8};
    Combatant average_joe_3 = {"Average Joe", 8, 8, 8, 8};
    enemy_team = { { average_joe_2, average_joe_3 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    cout << "BATTLE 1-2-2: CHARISMA CANNON DUO \n";
    Combatant charisma_cannon_1 = {"Charisma Cannon", 12, 4, 16, 4};
    Combatant charisma_cannon_2 = {"Charisma Cannon", 12, 4, 16, 4};
    enemy_team = { { charisma_cannon_1, charisma_cannon_2 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    cout << "BATTLE 1-2-3: BRAINY BROOM DUO \n";
    Combatant brainy_broom_1 = {"Brainy Broom", 12, 4, 4, 16};
    Combatant brainy_broom_2 = {"Brainy Broom", 12, 4, 4, 16};
    enemy_team = { { brainy_broom_1, brainy_broom_2 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    // insert pre-boss cutscene here
    cout << "BATTLE 1-2-4: STELLA THE MOON MAIDEN \n";
    Combatant stella = {"Stella", 20, 10, 25, 25};
    enemy_team = { { stella }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;
    // insert post-boss cutscene here

    // insert goofy ahh side cutscene here
    tazewell(player_team, inventory, progress);

    // insert pre-mission cutscene here
    Combatant peter = {"Peter", 7, 7, 7, 7};
    peter.skills = {"Attack", "Brain Boost", "Brain Break", "N/A", "Distribute"};
    player_team.team.push_back(peter);
    inventory.skills.push_back("Brain Boost");
    inventory.skills.push_back("Brain Break");

    cout << "BATTLE 1-3-1: AVERAGE JOE THREESOME \n";
    Combatant average_joe_4 = {"Average Joe", 10, 10, 10, 10};
    Combatant average_joe_5 = {"Average Joe", 10, 10, 10, 10};
    Combatant average_joe_6 = {"Average Joe", 10, 10, 10, 10};
    enemy_team = { { average_joe_4, average_joe_5, average_joe_6 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    cout << "BATTLE 1-3-2: HEARTSTEALERS \n";
    Combatant lovebird_1 = {"Lovebird", 12, 4, 20, 4};
    Combatant bumbling_burglar_2 = {"Bumbling Burglar", 8, 24, 4, 4};
    Combatant charisma_cannon_3 = {"Charisma Cannon", 16, 4, 16, 4};
    enemy_team = { { lovebird_1, bumbling_burglar_2, charisma_cannon_3 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    cout << "BATTLE 1-3-3: LOVE TRIANGLE \n";
    Combatant lovebird_2 = {"Lovebird", 8, 4, 24, 4};
    Combatant hatebee_1 = {"Hatebee", 8, 24, 4, 4};
    Combatant ambivalant_1 = {"Ambivalant", 8, 4, 4, 24};
    enemy_team = { { lovebird_2, hatebee_1, ambivalant_1 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    // insert pre-boss cutscene here
    cout << "BATTLE 1-3-4: COLONEL EIGEN \n";
    Combatant eigen = {"Colonel Eigen", 50, 40, 30, 30};
    enemy_team = { { eigen }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;
    // insert post-boss cutscene here

    // insert goofy ahh side cutscene here
    tazewell(player_team, inventory, progress);

    int choice = 0;
    cout << "You hear howling near Swem Library... will you investigate? Type 1 if yes, 0 if no.\n";
    cout << "NOTE: This is an optional challenge, and it will be your hardest one yet. Please think twice before proceeding.";
    cin >> choice;
    // insert pre-boss cutscene here
    if (choice == 1) {
           // insert pre-boss cutscene here
        cout << "OPTIONAL SUPERBOSS 1-1: LIBRARY CORGIS \n";
        Combatant nellie = {"Nellie", 20, 5, 5, 20};
        Combatant tucker = {"Tucker", 20, 20, 5, 5};
        Combatant louie = {"Louie", 20, 5, 20, 5};
        enemy_team = { { nellie, tucker, louie }};
        battle(player_team, enemy_team);
        // insert post-boss cutscene here
    }

    cout << "BATTLE 1-4-1: GROUP THERAPY \n";
    Combatant average_joe_7 = {"Average Joe", 12, 12, 12, 12};
    Combatant average_joe_8 = {"Average Joe", 12, 12, 12, 12};
    Combatant average_joe_9 = {"Average Joe", 12, 12, 12, 12};
    enemy_team = { { average_joe_7, average_joe_8, average_joe_9 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    cout << "BATTLE 1-4-2: KLEPTOMANIACS \n";
    Combatant bumbling_burglar_3 = {"Bumbling Burglar", 8, 32, 4, 4};
    Combatant bumbling_burglar_4 = {"Bumbling Burglar", 8, 32, 4, 4};
    Combatant bumbling_burglar_5 = {"Bumbling Burglar", 8, 32, 4, 4};
    enemy_team = { { bumbling_burglar_3, bumbling_burglar_4, bumbling_burglar_5 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    cout << "BATTLE 1-4-3: PSYCHOTIC DOCTORS \n";
    Combatant mad_doctor_1 = {"Mad Doctor", 10, 30, 4, 4};
    Combatant glad_doctor_1 = {"Glad Doctor", 10, 4, 30, 4};
    Combatant sad_doctor_1 = {"Sad Doctor", 10, 4, 4, 30};
    enemy_team = { { mad_doctor_1, glad_doctor_1, sad_doctor_1 }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;

    // insert pre-boss cutscene here
    cout << "BATTLE 1-4-4: CARRIE \n";
    Combatant carrie = {"CARRIE", 60, 20, 50, 50};
    enemy_team = { { carrie }};
    battle(player_team, enemy_team);
    progress.battles_won += 1;
    // insert post-boss cutscene here

    progress.semester += 1;
}