#include <iostream>
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
    bool alive = true;
    bool toggle = false;
    std::vector<std::string> skills = {};
};

struct Team {
    std::vector<Combatant> team;
};

struct Inventory {
    std::vector<std::string> skills = {"Attack", "Stimulate", "Sedate"};
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
        if (team.team[i].alive == true) {
            cout << team.team[i].name << " HP: " << team.team[i].HP << "\n";
        }
    }

    if (who == "enemy") {
        cout << "Who will you target? Choose an enemy from 1 to " << living_team << ": ";
    }
    else if (who == "teammate") {
        cout << "Who will you target? Choose a teammate from 1 to " << living_team << ": ";
    }
    else if (who == "tazewell_1") {
        cout << "Whose stat points shall you reassign? Choose a teammate from 1 to " << team.team.size() << ": ";
    }
    else if (who == "tazewell_2") {
        cout << "Whose skills will you change? Choose a teammate from 1 to " << team.team.size() << ": ";
    }

    cin >> target;
    while (target > living_team || target < 1 || team.team[target-1].alive == false) {
        cout << "Invalid! Choose from 1 to " << living_team << ": ";
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
    }
}

int toggle_pro(Combatant& user) {
    int multiplier;
    if (user.name == "Bronson") {
        multiplier = 2;
    }
    else if (user.name == "Colt") {
        srand(time(NULL));
        multiplier = 2 * (rand() % 2);
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

void migraine(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.HV -= max(0, ((user.STR + user.STR_mod)/2) * toggle_multiplier);
    cout << user.name << " uses Migraine! " << target.name << " will take "<< int(max(0, ((user.STR + user.STR_mod)/2) * toggle_multiplier)) << " extra damage every turn!\n";
    user.STR_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "STR");
    }
}

void accelerant_migraine(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.HA -= max(0, ((user.STR + user.STR_mod)/4) * toggle_multiplier);
    cout << user.name << " uses Accelerant Migraine! " << target.name << " will take "<< int(max(0, ((user.STR + user.STR_mod)/4) * toggle_multiplier)) << " extra damage every turn, each turn increasing by that amount!\n";
    user.STR_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "STR");
    }
}

void stimulate(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.STR_mod += max(0, (user.CHA + user.CHA_mod) * toggle_multiplier);
    cout << user.name << " uses Stimulate! " << target.name << "'s STR was raised by " << int(max(0, (user.CHA + user.CHA_mod) * toggle_multiplier)) << "!\n";
    user.CHA_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "CHA");
    }
}

void rizz_up(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.CHA_mod += max(0, (user.CHA + user.CHA_mod) * toggle_multiplier);
    cout << user.name << " uses Rizz Up! " << target.name << "'s CHA was raised by " << int(max(0, (user.CHA + user.CHA_mod) * toggle_multiplier)) << "!\n";
    user.CHA_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "CHA");
    }
}

void brain_boost(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.INT_mod += max(0, (user.CHA + user.CHA_mod) * toggle_multiplier);
    cout << user.name << " uses Brain Boost! " << target.name << "'s INT was raised by " << int(max(0, (user.CHA + user.CHA_mod) * toggle_multiplier)) << "!\n";
    user.CHA_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "CHA");
    }
}

void sedate(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.STR_mod -= max(0, (user.INT + user.INT_mod) * toggle_multiplier);
    cout << user.name << " uses Sedate! " << target.name << "'s STR was lowered by " << int(max(0, (user.INT + user.INT_mod) * toggle_multiplier)) << "!\n";
    user.INT_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "INT");
    }
}

void rizz_down(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.CHA_mod -= max(0, (user.INT + user.INT_mod) * toggle_multiplier);
    cout << user.name << " uses Rizz Down! " << target.name << "'s CHA was lowered by " << int(max(0, (user.INT + user.INT_mod) * toggle_multiplier)) << "!\n";
    user.INT_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "INT");
    }
}

void brain_break(Combatant& user, Combatant& target) {
    int toggle_multiplier = 1;
    if (user.toggle == true) {
        toggle_multiplier = toggle_pro(user);
    }
    target.INT_mod -= max(0, (user.INT + user.INT_mod) * toggle_multiplier);
    cout << user.name << " uses Brain Break! " << target.name << "'s INT was lowered by " << int(max(0, (user.INT + user.INT_mod) * toggle_multiplier)) << "!\n";
    user.INT_mod /= 2;
    if (user.toggle == true) {
        toggle_con(user, "INT");
    }
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
            if (player_team.team[i].alive == true) {
                int command = 0;
                cout << "1. " << player_team.team[i].skills[0] << " \n";
                cout << "2. " << player_team.team[i].skills[1] << " \n";
                cout << "3. " << player_team.team[i].skills[2] << " \n";
                cout << "4. " << player_team.team[i].skills[3] << " \n";
                cout << "ABILITY: " << player_team.team[i].skills[4] << " \n";

                cout << "Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle ability: ";
                cin >> command;
                while (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                    cout << "Invalid! Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle Burnout: ";
                    cin >> command;
                }
                if (player_team.team[i].skills[command-1] == "Attack") {
                    int target = -1;
                    if (living_enemy_team > 1) {
                        target = select_target(enemy_team, living_enemy_team, "enemy");
                    }
                    else {
                        target = single_target(enemy_team);
                    }
                    attack(player_team.team[i], enemy_team.team[target]);
                    if (enemy_team.team[target].HP <= 0 && enemy_team.team[target].alive == true) {
                        enemy_team.team[target].alive = false;
                        living_enemy_team -= 1;
                    }
                }
                else if (player_team.team[i].skills[command-1] == "Migraine") {
                    int target = -1;
                    if (living_enemy_team > 1) {
                        target = select_target(enemy_team, living_enemy_team, "enemy");
                    }
                    else {
                        target = single_target(enemy_team);
                    }
                    migraine(player_team.team[i], enemy_team.team[target]);
                }
                else if (player_team.team[i].skills[command-1] == "Accelerant Migraine") {
                    int target = -1;
                    if (living_enemy_team > 1) {
                        target = select_target(enemy_team, living_enemy_team, "enemy");
                    }
                    else {
                        target = single_target(enemy_team);
                    }
                    accelerant_migraine(player_team.team[i], enemy_team.team[target]);
                }
                else if (player_team.team[i].skills[command-1] == "Stimulate") {
                    int target = -1;
                    if (living_player_team > 1) {
                        target = select_target(player_team, living_player_team, "teammate");
                    }
                    else {
                        target = single_target(player_team);
                    }
                    stimulate(player_team.team[i], player_team.team[target]);
                }
                else if (player_team.team[i].skills[command-1] == "Rizz Up") {
                    int target = -1;
                    if (living_player_team > 1) {
                        target = select_target(player_team, living_player_team, "teammate");
                    }
                    else {
                        target = single_target(player_team);
                    }
                    rizz_up(player_team.team[i], player_team.team[target]);
                }
                else if (player_team.team[i].skills[command-1] == "Brain Boost") {
                    int target = -1;
                    if (living_player_team > 1) {
                        target = select_target(player_team, living_player_team, "teammate");
                    }
                    else {
                        target = single_target(player_team);
                    }
                    brain_boost(player_team.team[i], player_team.team[target]);
                }
                else if (player_team.team[i].skills[command-1] == "Sedate") {
                    int target = -1;
                    if (living_enemy_team > 1) {
                        target = select_target(enemy_team, living_enemy_team, "enemy");
                    }
                    else {
                        target = single_target(enemy_team);
                    }
                    sedate(player_team.team[i], enemy_team.team[target]);
                }
                else if (player_team.team[i].skills[command-1] == "Rizz Down") {
                    int target = -1;
                    if (living_enemy_team > 1) {
                        target = select_target(enemy_team, living_enemy_team, "enemy");
                    }
                    else {
                        target = single_target(enemy_team);
                    }
                    rizz_down(player_team.team[i], enemy_team.team[target]);
                }
                else if (player_team.team[i].skills[command-1] == "Brain Break") {
                    int target = -1;
                    if (living_enemy_team > 1) {
                        target = select_target(enemy_team, living_enemy_team, "enemy");
                    }
                    else {
                        target = single_target(enemy_team);
                    }
                    brain_break(player_team.team[i], enemy_team.team[target]);
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
            }
        }

        for (int i = 0; i < enemy_team.team.size(); i++) {
            if (enemy_team.team[i].HP > 0) {
                if ((enemy_team.team[i].name == "Charisma Cannon" || enemy_team.team[i].name == "Stella") && enemy_team.team[i].STR_mod <= 0) {
                    enemy_team.team[i].STR_mod += max(0, int((enemy_team.team[i].CHA + enemy_team.team[i].CHA_mod)));
                    cout << enemy_team.team[i].name << " uses Stimulate! " << enemy_team.team[i].name << "'s STR was raised by " << int(max(0, int((enemy_team.team[i].CHA + enemy_team.team[i].CHA_mod)))) << "!\n";
                    enemy_team.team[i].CHA_mod /= 2;
                }
                else if (enemy_team.team[i].name == "Brainy Broom") {
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

        for (int i = 0; i < player_team.team.size(); i++) {
            if (player_team.team[i].alive == true) {
                player_team.team[i].HP += player_team.team[i].HV;
                if (player_team.team[i].HV != 0) {
                    cout << player_team.team[i].name << "'s Health Position (HP) changed by " << int(player_team.team[i].HV) << ".\n";
                    if (player_team.team[i].HP <= 0 && player_team.team[i].alive == true) {
                        player_team.team[i].alive = false;
                        living_player_team -= 1;
                    }
                }
            }
        }

        for (int i = 0; i < enemy_team.team.size(); i++) {
            if (enemy_team.team[i].alive == true) {
                enemy_team.team[i].HP += enemy_team.team[i].HV;
                if (enemy_team.team[i].HV != 0) {
                    cout << enemy_team.team[i].name << "'s Health Position (HP) changed by " << int(enemy_team.team[i].HV) << ".\n";
                    if (enemy_team.team[i].HP <= 0 && enemy_team.team[i].alive == true) {
                        enemy_team.team[i].alive = false;
                        living_enemy_team -= 1;
                    }
                }
            }
        }

        for (int i = 0; i < player_team.team.size(); i++) {
            if (player_team.team[i].alive == true) {
                player_team.team[i].HV += player_team.team[i].HA;
                if (player_team.team[i].HA != 0) {
                    cout << player_team.team[i].name << "'s Health Velocity (HV) changed by " << int(player_team.team[i].HA) << ".\n";
                }
            }
        }

        for (int i = 0; i < enemy_team.team.size(); i++) {
            if (enemy_team.team[i].alive == true) {
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

    reset_mods(player_team);

    return 0;
}

int tazewell(Team& player_team, Inventory& inventory) {
    int command = 0;
    cout << "What would you like to do? 1 to reassign stats, 2 to equip skills: ";
    cin >> command;
    while (command != 1 && command != 2 && command != 3 && command != 4) {
        cout << "Invalid! What would you like to do? 1 to reassign stats, 2 to equip skills: ";
        cin >> command;
    }
    if (command == 1) {
        int target = 0;
        if (player_team.team.size() > 1) {
            target = select_target(player_team, player_team.team.size(), "tazewell_1");
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

        cout << player_team.team[target].VIT;
        cout << player_team.team[target].STR;
        cout << player_team.team[target].CHA;
        cout << player_team.team[target].INT;
    }
    else if (command == 2) {
        int slot = 0;
        int target = 0;
        if (player_team.team.size() > 1) {
            target = select_target(player_team, player_team.team.size(), "tazewell_2");
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
    }
}

int main() {
    Combatant bronson = {"Bronson", 5, 5, 5, 5};
    bronson.skills = {"Attack", "Stimulate", "Sedate", "N/A", "Burnout"};
    Team player_team = { { bronson } };
    Inventory inventory;

    cout << "CENSORY - SEMESTER 1\n";
    cin.get();
    cout << "CARRIED-OVER TRAUMA\n";
    cin.get();
    cout << "BOSWELL HALL - 3:00 PM\n";
    cin.get();
    cout << "BRONSON: I gotta go to office hours... but shit, I don't have a pencil. Guess I gotta steal one.\n";
    cin.get();

    cout << "(Suddenly, the landscape around BRONSON changes completely.)\n";
    cin.get();
    cout << "BRONSON: Huh?! What is this place, and why does it look like a fucking math problem?\n";
    cin.get();
    cout << "BRONSON: Ah well, nothing to do but find a way out. Let's roll.\n";
    cin.get();

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

    cout << "BRONSON: This is it. The final room. There's gotta be...\n";
    cin.get();
    cout << "BRONSON: THERE IT IS!";
    cin.get();
    cout << "You notice a pencil underneath a desk. As you grab the pencil, the desk suddenly starts moving...\n";
    cin.get();
    cout << "BRONSON: Who do you think I am, Indiana Jones?\n";
    cin.get();
    cout << "The desk begins to shake the ground below you... you struggle to stand for a brief moment.\n";
    cin.get();
    cout << "BRONSON: Ah shit... do I have to fight... a literal fucking desk?!\n";
    cin.get();

    cout << "BATTLE 1-4: A LITERAL FUCKING DESK\n";
    Combatant a_literal_fucking_desk = {"A Literal Fucking Desk", 10, 10, 5, 5};
    enemy_team = { { a_literal_fucking_desk } };
    battle(player_team, enemy_team);
   
    cout << "BRONSON: Finally... at long last... I have a pencil for office hours!";
    cin.get();
    cout << "HEIST SUCCESSFUL!";
    cin.get();
    cout << "All of a sudden, the desk begins to sound. A blue laser is pointed towards BRONSON's head.";
    cin.get();
    cout << "A LITERAL FUCKING DESK: SCANNING BRAIN ACTIVITY...";
    cin.get();
    cout << "BRONSON: What is this, 1984?!";
    cin.get();
    cout << "The desk rapidly deconstructs and constructs itself into another form.";
    cin.get();
    cout << "???: Greetings, Bronson.";
    cin.get();
    cout << "BRONSON: No… this can’t be… Carrie? What are YOU doing here?! Why are you a desk?! Why-";
    cin.get();
    cout << "???: I am not the real person that you are referring to. As you just stated, I am the desk you just destroyed.";
    cin.get();
    cout << "BRONSON: Then why do you look like her? How do you know her?";
    cin.get();
    cout << "???: After my destruction, I had to quickly take a new form to prevent myself from dying. Thus, I scanned your brain activity for potential candidates. And what do you know, a certain someone just lives rent-free in your head.";
    cin.get();
    cout << "???: However, it was more than just that. I need your help with something, and I assumed that this form… would make you cooperate.";
    cin.get();
    cout << "BRONSON: What are you talking about?!";
    cin.get();
    cout << "???: You see, a military general by the name of Colonel Eigen has taken over this world we both currently reside in. He’s killed countless organisms, and he won’t stop until someone makes him.";
    cin.get();
    cout << "BRONSON: I wish I could help you out, but I have homework. Now if I can just go in peace...";
    cin.get();
    cout << "CARRIE: If you do that, this world, the Universal Set, shall overtake your own world, transforming your world into a place driven by logic alone. It will be perfect... however it will be hollow.";
    cin.get();
    cout << "BRONSON: Oh, so like my old high school then. Alright, fine. I’ll help you out... not Carrie.";
    cin.get();
    cout << "CARRIE: You can call me CARRIE. It’s short for Cognitive Artificial Reality Rendering Intelligence Expert.";
    cin.get();
    cout << "CARRIE: Welcome to the Universal Set, Bronson.";
    cin.get();
    cout << "(They shake hands. BRONSON blushes a bit.)";
    cin.get();
    cout << "NARRATOR: < END OF HEIST 1 >";
    cin.get();

    // insert goofy ahh side cutscene here
    tazewell(player_team, inventory);

    // insert pre-mission cutscene here
    Combatant colt = {"Colt", 6, 6, 6, 6};
    colt.skills = {"Attack", "Rizz Up", "Rizz Down", "N/A", "Coin Flip"};
    player_team.team.push_back(colt);
    inventory.skills.push_back("Rizz Up");
    inventory.skills.push_back("Rizz Down");

    cout << "BATTLE 2-1: AVERAGE JOE DUO \n";
    Combatant average_joe_a = {"Average Joe", 8, 8, 8, 8};
    Combatant average_joe_b = {"Average Joe", 8, 8, 8, 8};
    enemy_team = { { average_joe_a, average_joe_b }};
    battle(player_team, enemy_team);

    cout << "BATTLE 2-2: CHARISMA CANNON DUO \n";
    Combatant charisma_cannon_a = {"Charisma Cannon", 12, 4, 16, 4};
    Combatant charisma_cannon_b = {"Charisma Cannon", 12, 4, 16, 4};
    enemy_team = { { charisma_cannon_a, charisma_cannon_b }};
    battle(player_team, enemy_team);

    cout << "BATTLE 2-3: BRAINY BROOM DUO \n";
    Combatant brainy_broom_a = {"Brainy Broom", 12, 4, 4, 16};
    Combatant brainy_broom_b = {"Brainy Broom", 12, 4, 4, 16};
    enemy_team = { { brainy_broom_a, brainy_broom_b }};
    battle(player_team, enemy_team);

    // insert pre-boss cutscene here
    cout << "BATTLE 2-4: STELLA THE MOON MAIDEN \n";
    Combatant stella = {"Stella", 20, 10, 25, 25};
    enemy_team = { { stella }};
    battle(player_team, enemy_team);
    // insert post-boss cutscene here

    // insert goofy ahh side cutscene here
    tazewell(player_team, inventory);
}
