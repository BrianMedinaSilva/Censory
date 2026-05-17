#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <list>
#include <climits>

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
    bool toggle = false;
    std::vector<std::string> skills = {"Attack", "Migraine", "Stimulate", "Sedate", "Burnout"};
};

struct Team {
    std::vector<Combatant> team;
};

struct Inventory {
    std::vector<std::string> skills = {"Attack", "Migraine", "Stimulate", "Sedate"};
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

int select_target(Team& team, int living_team, std::string who) {
    int target = 0;
    for (int i = 0; i < team.team.size(); i++) {
        if (team.team[i].HP > 0) {
            cout << team.team[i].name << " HP: " << team.team[i].HP << "\n";
        }
    }

    if (who == "enemy") {
        cout << "Who will you target? Choose an enemy from 1 to " << living_team << ": ";
    }
    else if (who == "teammate") {
        cout << "Who will you target? Choose a teammate from 1 to " << living_team << ": ";
    }

    cin >> target;
    while (target > living_team || target < 1 || team.team[target-1].HP <= 0) {
        cout << "Invalid! Who will you target? Choose from 1 to " << living_team << ": ";
        cin >> target;
    }
    return target-1;
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
        }
        for (int i = 0; i < enemy_team.team.size(); i++) {
            cout << enemy_team.team[i].name << " HP: " << enemy_team.team[i].HP << "\n";
        }

        for (int i = 0; i < player_team.team.size(); i++) {
            int command = 0;
            cout << "1. " << player_team.team[i].skills[0] << " \n";
            cout << "2. " << player_team.team[i].skills[1] << " \n";
            cout << "3. " << player_team.team[i].skills[2] << " \n";
            cout << "4. " << player_team.team[i].skills[3] << " \n";

            cout << "Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle Burnout: ";
            cin >> command;
            while (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                cout << "Invalid! Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle Burnout: ";
                cin >> command;
            }
            if (player_team.team[i].skills[command-1] == "Attack") {
                if (player_team.team[i].toggle == true) {
                    if (living_enemy_team > 1) {
                        int target = select_target(enemy_team, living_enemy_team, "enemy");
                        enemy_team.team[target].HP -= max(0, (player_team.team[i].STR + player_team.team[i].STR_mod) * 2);
                        cout << player_team.team[i].name << " attacks! " << int(max(0, (player_team.team[i].STR + player_team.team[i].STR_mod) * 2)) << " damage!\n";
                        if (enemy_team.team[target].HP <= 0) {
                            living_enemy_team -= 1;
                        }
                        player_team.team[i].STR_mod /= 2;
                        player_team.team[i].STR_mod -= player_team.team[i].STR;
                    }
                    else {
                        enemy_team.team[0].HP -= max(0, (player_team.team[i].STR + player_team.team[i].STR_mod) * 2);
                        cout << player_team.team[i].name << " attacks! " << int(max(0, (player_team.team[i].STR + player_team.team[i].STR_mod) * 2)) << " damage!\n";
                        if (enemy_team.team[0].HP <= 0) {
                            living_enemy_team -= 1;
                        }
                        player_team.team[i].STR_mod /= 2;
                        player_team.team[i].STR_mod -= player_team.team[i].STR;
                    }
                }
                else {
                    if (living_enemy_team > 1) {
                        int target = select_target(enemy_team, living_enemy_team, "enemy");
                        enemy_team.team[target].HP -= max(0, (player_team.team[i].STR + player_team.team[i].STR_mod));
                        cout << player_team.team[i].name << " attacks! " << int(max(0, player_team.team[i].STR + player_team.team[i].STR_mod)) << " damage!\n";
                        if (enemy_team.team[target].HP <= 0) {
                            living_enemy_team -= 1;
                        }
                        player_team.team[i].STR_mod /= 2;
                    }
                    else {
                        enemy_team.team[0].HP -= max(0, player_team.team[i].STR + player_team.team[i].STR_mod);
                        cout << player_team.team[i].name << " attacks! " << int(max(0, player_team.team[i].STR + player_team.team[i].STR_mod)) << " damage!\n";
                        if (enemy_team.team[0].HP <= 0) {
                            living_enemy_team -= 1;
                        }
                        player_team.team[i].STR_mod /= 2;
                    }
                }
            }
            else if (player_team.team[i].skills[command-1] == "Stimulate") {
                if (player_team.team[i].toggle == true) {
                    if (living_player_team > 1) {
                        int target = select_target(player_team, living_player_team, "teammate");
                        player_team.team[target].STR_mod += max(0, (player_team.team[i].CHA + player_team.team[i].CHA_mod) * 2);
                        cout << player_team.team[i].name << " uses Stimulate! " << player_team.team[target].name << "'s STR was raised by " << int(max(0, (player_team.team[i].CHA + player_team.team[i].CHA_mod) * 2)) << "!\n";
                        player_team.team[i].CHA_mod /= 2;
                        player_team.team[i].CHA_mod -= player_team.team[i].CHA;
                    }
                    else {
                        player_team.team[i].STR_mod += max(0, (player_team.team[i].CHA + player_team.team[i].CHA_mod) * 2);
                        cout << player_team.team[i].name << " uses Stimulate! " << player_team.team[i].name << "'s STR was raised by " << int(max(0, (player_team.team[i].CHA + player_team.team[i].CHA_mod) * 2)) << "!\n";
                        player_team.team[i].CHA_mod /= 2;
                        player_team.team[i].CHA_mod -= player_team.team[i].CHA;
                    }
                }
                else {
                    if (living_player_team > 1) {
                        int target = select_target(player_team, living_player_team, "teammate");
                        player_team.team[target].STR_mod += max(0, (player_team.team[i].CHA + player_team.team[i].CHA_mod));
                        cout << player_team.team[i].name << " uses Stimulate! " << player_team.team[target].name << "'s STR was raised by " << int(max(0, (player_team.team[i].CHA + player_team.team[i].CHA_mod))) << "!\n";
                        player_team.team[i].CHA_mod /= 2;
                    }
                    else {
                        player_team.team[i].STR_mod += max(0, (player_team.team[i].CHA + player_team.team[i].CHA_mod));
                        cout << player_team.team[i].name << " uses Stimulate! " << player_team.team[i].name << "'s STR was raised by " << int(max(0, (player_team.team[i].CHA + player_team.team[i].CHA_mod))) << "!\n";
                        player_team.team[i].CHA_mod /= 2;
                    }
                }
            }
            else if (player_team.team[i].skills[command-1] == "Sedate") {
                if (player_team.team[i].toggle == true) {
                    if (living_enemy_team > 1) {
                        int target = select_target(enemy_team, living_enemy_team, "enemy");
                        enemy_team.team[target].STR_mod -= max(0, (player_team.team[i].INT + player_team.team[i].INT_mod) * 2);
                        cout << player_team.team[i].name << " uses Sedate! " << enemy_team.team[target].name << "'s STR was lowered by " << int(max(0, (player_team.team[i].INT + player_team.team[i].INT_mod) * 2)) << "!\n";
                        player_team.team[i].INT_mod /= 2;
                        player_team.team[i].INT_mod -= player_team.team[i].INT;
                    }
                    else {
                        enemy_team.team[0].STR_mod -= max(0, (player_team.team[i].INT + player_team.team[i].INT_mod) * 2);
                        cout << player_team.team[i].name << " uses Sedate! " << enemy_team.team[0].name << "'s STR was lowered by " << int(max(0, (player_team.team[i].INT + player_team.team[i].INT_mod) * 2)) << "!\n";
                        player_team.team[i].INT_mod /= 2;
                        player_team.team[i].INT_mod -= player_team.team[i].INT;
                    }
                }
                else {
                    if (living_enemy_team > 1) {
                        int target = select_target(enemy_team, living_enemy_team, "enemy");
                        enemy_team.team[target].STR_mod -= max(0, (player_team.team[i].INT + player_team.team[i].INT_mod));
                        cout << player_team.team[i].name << " uses Sedate! " << enemy_team.team[target].name << "'s STR was lowered by " << int(max(0, player_team.team[i].INT + player_team.team[i].INT_mod)) << "!\n";
                        player_team.team[i].INT_mod /= 2;
                    }
                    else {
                        enemy_team.team[0].STR_mod -= max(0, (player_team.team[i].INT + player_team.team[i].INT_mod));
                        cout << player_team.team[i].name << " uses Sedate! " << enemy_team.team[0].name << "'s STR was lowered by " << int(max(0, player_team.team[i].INT + player_team.team[i].INT_mod)) << "!\n";
                        player_team.team[i].INT_mod /= 2;
                    }
                }
            }
            else if (player_team.team[i].skills[command-1] == "Migraine") {
                if (player_team.team[i].toggle == true) {
                    if (living_enemy_team > 1) {
                        int target = select_target(enemy_team, living_enemy_team, "enemy");
                        enemy_team.team[target].HV -= int(max(0, player_team.team[i].STR + player_team.team[i].STR_mod));
                        cout << player_team.team[i].name << " uses Migraine! " << enemy_team.team[target].name << " will take " << int(max(0, player_team.team[i].STR + player_team.team[i].STR_mod)) << " extra damage every turn!\n";
                        player_team.team[i].STR_mod /= 2;
                        player_team.team[i].STR_mod -= player_team.team[i].STR;
                    }
                    else {
                        enemy_team.team[0].HV -= int(max(0, player_team.team[i].STR + player_team.team[i].STR_mod));
                        cout << player_team.team[i].name << " uses Migraine! " << enemy_team.team[0].name << " will take " << int(max(0, player_team.team[i].STR + player_team.team[i].STR_mod)) << " extra damage every turn!\n";
                        player_team.team[i].STR_mod /= 2;
                        player_team.team[i].STR_mod -= player_team.team[i].STR;
                    }
                }
                else {
                    if (living_enemy_team > 1) {
                        int target = select_target(enemy_team, living_enemy_team, "enemy");
                        enemy_team.team[target].HV -= int(max(0, (player_team.team[i].STR + player_team.team[i].STR_mod)/2));
                        cout << player_team.team[i].name << " uses Migraine! " << enemy_team.team[target].name << " will take " << int(max(0, (player_team.team[i].STR + player_team.team[i].STR_mod)) / 2) << " extra damage every turn!\n";
                        player_team.team[i].STR_mod /= 2;
                    }
                    else {
                        enemy_team.team[0].HV -= int(max(0, (player_team.team[i].STR + player_team.team[i].STR_mod)/2));
                        cout << player_team.team[i].name << " uses Migraine! " << enemy_team.team[0].name << " will take " << int(max(0, (player_team.team[i].STR + player_team.team[i].STR_mod)) / 2) << " extra damage every turn!\n";
                        player_team.team[i].STR_mod /= 2;
                    }
                }
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
        }

        for (int i = 0; i < enemy_team.team.size(); i++) {
            if (enemy_team.team[i].HP > 0) {
                int optimal = -1;
                int min_hp = INT_MAX;
                for (int j = 0; j < player_team.team.size(); j++) {
                    if (player_team.team[j].HP > 0) {
                        if (player_team.team[j].HP < min_hp) {
                            optimal = j;
                            min_hp = player_team.team[j].HP;
                        }
                    }
                }
                double t = timing();
                player_team.team[optimal].HP -= max(0, int((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t));
                cout << enemy_team.team[i].name << " attacks! " << int(max(0, int((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t))) << " damage!\n";
                if (player_team.team[optimal].HP <= 0) {
                    living_player_team -= 1;
                }
                enemy_team.team[i].STR_mod /= 2;
            }
        }

        for (int i = 0; i < player_team.team.size(); i++) {
            if (player_team.team[i].HP > 0) {
                player_team.team[i].HP += player_team.team[i].HV;
                if (player_team.team[i].HV != 0) {
                    cout << player_team.team[i].name << "'s Health Position (HP) changed by " << int(player_team.team[i].HV) << ".\n";
                    if (player_team.team[i].HP <= 0) {
                        living_player_team -= 1;
                    }
                }
            }
        }

        for (int i = 0; i < enemy_team.team.size(); i++) {
            if (enemy_team.team[i].HP > 0) {
                enemy_team.team[i].HP += enemy_team.team[i].HV;
                if (enemy_team.team[i].HV != 0) {
                    cout << enemy_team.team[i].name << "'s Health Position (HP) changed by " << int(enemy_team.team[i].HV) << ".\n";
                    if (enemy_team.team[i].HP <= 0) {
                        living_enemy_team -= 1;
                    }
                }
            }
        }

        for (int i = 0; i < player_team.team.size(); i++) {
            if (player_team.team[i].HP > 0) {
                player_team.team[i].HV += player_team.team[i].HA;
                if (player_team.team[i].HA != 0) {
                    cout << player_team.team[i].name << "'s Health Velocity (HV) changed by " << int(player_team.team[i].HA) << ".\n";
                }
            }
        }

        for (int i = 0; i < enemy_team.team.size(); i++) {
            if (enemy_team.team[i].HP > 0) {
                enemy_team.team[i].HV += enemy_team.team[i].HA;
                if (enemy_team.team[i].HA != 0) {
                    cout << enemy_team.team[i].name << "'s Health Velocity (HV) changed by " << int(enemy_team.team[i].HA) << ".\n";
                }
            }
        }

        if (living_enemy_team == 0) {
            break;
        }

        if (living_player_team == 0) {
            cout << "GAME OVER";
            exit(0);
        }
    }

    cout << "VICTORY!\n";
    int stat_boost = 0;
    cout << "Which stat will you increase? 1 for VIT, 2 for STR, 3 for CHA, 4 for INT: ";
    cin >> stat_boost;
    while (stat_boost != 1 && stat_boost != 2 && stat_boost != 3 && stat_boost != 4) {
        cout << "Invalid! Which stat will you increase? 1 for VIT, 2 for STR, 3 for CHA, 4 for INT: ";
        cin >> stat_boost;
    }
    if (stat_boost == 1) {
        player_team.team[0].VIT += 1;
    }
    else if (stat_boost == 2) {
        player_team.team[0].STR += 1;
    }
    else if (stat_boost == 3) {
        player_team.team[0].CHA += 1;
    }
    else if (stat_boost == 4) {
        player_team.team[0].INT += 1;
    }

    return 0;
}

int tazewell(Combatant& bronson, Inventory& inventory) {
    int command = 0;
    cout << "What would you like to do? 1 to reassign stats, 2 to equip skills: ";
    cin >> command;
    while (command != 1 && command != 2 && command != 3 && command != 4) {
        cout << "Invalid! What would you like to do? 1 to reassign stats, 2 to equip skills: ";
        cin >> command;
    }
    if (command == 1) {
        int total = bronson.VIT + bronson.STR + bronson.CHA + bronson.INT;

        int invest = 0;
        cout << "How many points would you like to invest into Vitality? You have " << total << " stat points left.";
        cin >> invest;
        while (1 > invest || invest > (total - 3)) {
            cout << "Invalid! How many points would you like to invest into Vitality?";
            cin >> invest;
        }
        bronson.VIT = invest;
        total -= invest;

        cout << "How many points would you like to invest into Strength? You have " << total << " stat points left.";
        cin >> invest;
        while (1 > invest || invest > (total - 2)) {
            cout << "Invalid! How many points would you like to invest into Strength?";
            cin >> invest;
        }
        bronson.STR = invest;
        total -= invest;

        cout << "How many points would you like to invest into Charisma? You have " << total << " stat points left.";
        cin >> invest;
        while (1 > invest || invest > (total - 1)) {
            cout << "Invalid! How many points would you like to invest into Charisma?";
            cin >> invest;
        }
        bronson.CHA = invest;
        total -= invest;

        cout << "How many points would you like to invest into Intellect? You have " << total << " stat points left.";
        cin >> invest;
        while (1 > invest || invest > total) {
            cout << "Invalid! How many points would you like to invest into Intellect?";
            cin >> invest;
        }
        bronson.INT = invest;
        total -= invest;

        cout << bronson.VIT;
        cout << bronson.STR;
        cout << bronson.CHA;
        cout << bronson.INT;
    }
    else if (command == 2) {
        int slot = 0;

        cout << "1. " << bronson.skills[0] << " \n";
        cout << "2. " << bronson.skills[1] << " \n";
        cout << "3. " << bronson.skills[2] << " \n";
        cout << "4. " << bronson.skills[3] << " \n";
        cout << "Which slot would you like to change? Choose a slot from 1 to 4: ";
        cin >> slot;
        while (slot > 4 || slot < 1) {
            cout << "Invalid! Which slot would you like to change?";
            cin >> slot;
        }

        int replacement = 0;
        cout << "Which skill will you put in its place? 1 for the first slot, 2 for the second slot, etc: ";
        cin >> replacement;
        while (replacement > inventory.skills.size() || replacement < 1) {
            cout << "Invalid! Which skill will you put in its place? 1 for the first slot, 2 for the second slot, etc: ";
            cin >> replacement;
        }
        std::string temp = bronson.skills[slot-1];
        bronson.skills[slot-1] = inventory.skills[replacement-1];
        inventory.skills[replacement-1] = temp;

        cout << "1. " << bronson.skills[0] << " \n";
        cout << "2. " << bronson.skills[1] << " \n";
        cout << "3. " << bronson.skills[2] << " \n";
        cout << "4. " << bronson.skills[3] << " \n";
    }
}

int main() {
    Combatant bronson = {"Bronson", 5, 5, 5, 5};
    Team player_team = { { bronson } };
    Inventory inventory;

    cout << "BATTLE 1-1: AVERAGE JOE \n";
    Combatant average_joe = {"Average Joe", 6, 6, 6, 6};
    Team enemy_team = { { average_joe } };
    battle(player_team, enemy_team);

    cout << "BATTLE 1-2: POSTSYNAPTIC POLICE \n";
    Combatant postsynaptic_police = {"Postsynaptic Police", 8, 8, 4, 4};
    enemy_team = { { postsynaptic_police } };
    battle(player_team, enemy_team);

    cout << "BATTLE 1-3: BUMBLING BURGLAR \n";
    Combatant bumbling_burglar = {"Bumbling Burglar", 4, 12, 4, 4};
    enemy_team = { { bumbling_burglar } };
    battle(player_team, enemy_team);

    // insert pre-boss cutscene here
    cout << "BATTLE 1-4: A LITERAL FUCKING DESK \n";
    Combatant a_literal_fucking_desk = {"A Literal Fucking Desk", 10, 10, 5, 5};
    enemy_team = { { a_literal_fucking_desk } };
    battle(player_team, enemy_team);
    // insert post-boss cutscene here

    // insert goofy ahh side cutscene here
    tazewell(bronson, inventory);

    // insert pre-mission cutscene here
    Combatant colt = {"Colt", 6, 6, 6, 6};
    player_team.team.push_back(colt);
    cout << "BATTLE 2-1: AVERAGE JOE DUO \n";
    Combatant average_joe_a = {"Average Joe A", 8, 8, 8, 8};
    Combatant average_joe_b = {"Average Joe B", 8, 8, 8, 8};
    enemy_team = { { average_joe_a, average_joe_b }};
    battle(player_team, enemy_team);
}