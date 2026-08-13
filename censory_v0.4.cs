using System;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;

public class Combatant {
    public string name;
    public int VIT;
    public int STR;
    public int CHA;
    public int INT;
    public int VIT_mod = 0;
    public int STR_mod = 0;
    public int CHA_mod = 0;
    public int INT_mod = 0;
    public int HP;
    public int maxHP;
    public int HV;
    public int HA;
    public int actions = 1;
    public bool alive = true;
    public bool toggle = false;
    public List<string> skills = new List<string>();
};

public class Team {
    public List<Combatant> team = new List<Combatant>();
    public List<string> gradebook = new List<string>();
    public int semester = 1;
    public int battles_won = 0;
    public int TP;
};

public class Inventory {
    public List<string> skills = new List<string> {"Attack", "Stimulate", "Sedate"};
};

/*
struct Skill {
    std::string name;
    std::string effect;
    std::string stat_used;
    std::string stat_affected;
    int HP_change = 0;
    int HV_change = 0;
    int HA_change = 0;
    int stat_change = 0;
    double toggle_multiplier = 1;
    double Timing_multiplier = 1;
    bool Timing = false;
    bool aoe = false;
};
*/

public class Skill {
    public string name;
    public string effect;
    public string stat_used;
    public string stat_affected;
    public double multiplier = 1;
    public bool timing = false;
    public bool aoe = false;
};

public class Rules {
    public bool target_downed = false;
};

public static class BattleSystem {
    public static double Timing() {
        Console.WriteLine("Press ENTER!");
        Console.ReadLine();

        var input = Stopwatch.StartNew();
        Console.ReadLine();
        input.Stop();

        return input.Elapsed.TotalSeconds;
    }

    public static int SelectTarget(Team team, string who, Rules rules) {
        int target = 0;
        for (int i = 0; i < team.team.Count; i++) {
            Console.WriteLine(team.team[i].name + " HP: " + team.team[i].HP);
        }

        if (who == "enemy") {
            Console.WriteLine("Who will you target? Choose an enemy from 1 to " + team.team.Count + ": ");
        }
        else if (who == "teammate") {
            Console.WriteLine("Who will you target? Choose a teammate from 1 to " + team.team.Count + ": ");
        }
        else if (who == "tazewell_1") {
            Console.WriteLine("Whose stat points shall you reassign? Choose a teammate from 1 to " + team.team.Count + ": ");
        }
        else if (who == "tazewell_2") {
            Console.WriteLine("Whose skills will you change? Choose a teammate from 1 to " + team.team.Count + ": ");
        }

        target = int.Parse(Console.ReadLine());
        if (rules.target_downed == true) {
            while (target > team.team.Count || target < 1) {
                Console.WriteLine("Invalid! Choose from 1 to " + team.team.Count + ": ");
                target = int.Parse(Console.ReadLine());
            }
        }
        else {
            while (target > team.team.Count || target < 1 || team.team[target-1].alive == false) {
                Console.WriteLine("Invalid! Choose from 1 to " + team.team.Count + ": ");
                target = int.Parse(Console.ReadLine());
            }
        }
        return target - 1;
    }

    public static int SingleTarget(Team team) {
        int target = -1;
        for (int i = 0; i < team.team.Count; i++) {
            if (team.team[i].alive == true) {
                target = i;
            }
        }
        return target;
    }

    public static int PlayerTargeting(Team team, string who, ref int living_team, Rules rules) {
        int target = -1;
        if (living_team > 1) {
            target = SelectTarget(team, who, rules);
        }
        else {
            target = SingleTarget(team);
        }
        return target;
    }

    public static int EnemyTargetingLowestHP(Team player_team) {
        int optimal = -1;
        int min_hp = int.MaxValue;
        for (int i = 0; i < player_team.team.Count; i++) {
            if (player_team.team[i].alive == true) {
                if (player_team.team[i].HP < min_hp) {
                    optimal = i;
                    min_hp = player_team.team[i].HP;
                }
            }
        }
        return optimal;
    }

    public static int EnemyTargetingHighestSTR(Team player_team) {
        int optimal = -1;
        int max_str = int.MinValue;
        for (int i = 0; i < player_team.team.Count; i++) {
            if (player_team.team[i].alive == true) {
                if ((player_team.team[i].STR + player_team.team[i].STR_mod) > max_str) {
                    optimal = i;
                    max_str = player_team.team[i].STR + player_team.team[i].STR_mod;
                }
            }
        }
        return optimal;
    }

    public static void ResetMods(Team player_team) {
        for (int i = 0; i < player_team.team.Count; i++) {
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

    public static double TogglePro(Combatant user) {
        double multiplier = 1;
        if (user.toggle == true) {
            if (user.name == "Bronson") {
                multiplier = 2;
            }
            else if (user.name == "Colt") {
                multiplier = 2 * Random.Shared.Next(2);
            }
            else if (user.name == "Peter") {
                multiplier = 0.5;
            }
        }
        return multiplier;
    }

    public static void ToggleCon(Combatant user, string stat) {
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

    public static void CheckIfDowned(Combatant target, ref int living_team) {
        if (target.HP <= 0 && target.alive == true) {
            target.alive = false;
            living_team -= 1;
        }
    }

    public static void ApplyHV(Team team, ref int living_team) {
        for (int i = 0; i < team.team.Count; i++) {
            if (team.team[i].alive == true) {
                team.team[i].HP += team.team[i].HV;
                if (team.team[i].HV != 0) {
                    Console.WriteLine(team.team[i].name + "'s Health Position (HP) changed by " + (int)(team.team[i].HV) + ".");
                    CheckIfDowned(team.team[i], ref living_team);
                }
            }
        }
    }

    public static void ApplyHA(Team team) {
        for (int i = 0; i < team.team.Count; i++) {
            if (team.team[i].alive == true) {
                team.team[i].HV += team.team[i].HA;
                if (team.team[i].HA != 0) {
                    Console.WriteLine(team.team[i].name + "'s Health Velocity (HV) changed by " + (int)(team.team[i].HA) + ".");
                }
            }
        }
    }

    public static void Skill(Skill skill, Combatant user, Team player_team, Team enemy_team, int target, ref int living_team) {
        Console.WriteLine(user.name + " uses " + skill.name + "!");

        int effect_value = 0;
        double toggle_multiplier = TogglePro(user);
        double t = 1;

        if (skill.stat_used == "STR") {
            effect_value = user.STR + user.STR_mod;
        }
        else if (skill.stat_used == "CHA") {
            effect_value = user.CHA + user.CHA_mod;
        }
        else if (skill.stat_used == "INT") {
            effect_value = user.INT + user.INT_mod;
        }

        if (skill.timing == true) {
            t = Timing();
        }

        if (skill.effect == "HP damage") {
            if (user.toggle == true && user.name == "Peter") {
                for (int i = 0; i < enemy_team.team.Count; i++) {
                    enemy_team.team[i].HP -= (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                    CheckIfDowned(enemy_team.team[i], ref living_team);
                }
            }
            else {
                enemy_team.team[target].HP -= (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                CheckIfDowned(enemy_team.team[target], ref living_team);
            }
            Console.WriteLine(enemy_team.team[target].name + " took " + (int)(effect_value * toggle_multiplier * t * skill.multiplier) + " damage!");
        }
        if (skill.effect == "HV damage") {
            if (user.toggle == true && user.name == "Peter") {
                for (int i = 0; i < enemy_team.team.Count; i++) {
                    enemy_team.team[i].HV -= (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                }
            }
            else {
                enemy_team.team[target].HV -= (int)(effect_value * toggle_multiplier * t * skill.multiplier);
            }
            Console.WriteLine(enemy_team.team[target].name + " will take " + (int)(effect_value * toggle_multiplier * t) + " damage at the end of every turn!");
        }
        if (skill.effect == "HA damage") {
            if (user.toggle == true && user.name == "Peter") {
                for (int i = 0; i < enemy_team.team.Count; i++) {
                    enemy_team.team[i].HA -= (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                }
            }
            else {
                enemy_team.team[target].HA -= (int)(effect_value * toggle_multiplier * t * skill.multiplier);
            }
            Console.WriteLine(enemy_team.team[target].name + " will take an additional " + (int)(effect_value * toggle_multiplier * t * skill.multiplier) + " damage every turn, gradually accumulating by that amount!");
        }
        if (skill.effect == "buff") {
            if (skill.stat_affected == "STR") {
                if (user.toggle == true && user.name == "Peter") {
                    for (int i = 0; i < enemy_team.team.Count; i++) {
                        player_team.team[i].STR_mod += (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                    }
                }
                else {
                    player_team.team[target].STR_mod += (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                }
            }
            if (skill.stat_affected == "CHA") {
                if (user.toggle == true && user.name == "Peter") {
                    for (int i = 0; i < enemy_team.team.Count; i++) {
                        player_team.team[i].CHA_mod += (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                    }
                }
                else {
                    player_team.team[target].CHA_mod += (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                }
            }
            if (skill.stat_affected == "INT") {
                if (user.toggle == true && user.name == "Peter") {
                    for (int i = 0; i < enemy_team.team.Count; i++) {
                        player_team.team[i].INT_mod += (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                    }
                }
                else {
                    player_team.team[target].INT_mod += (int)(effect_value * toggle_multiplier * t * skill.multiplier);
                }
            }

            if (user.toggle == true && user.name == "Peter") {
                Console.WriteLine("The " + skill.stat_affected + " of all teammates was raised by " + (int)(effect_value * toggle_multiplier * t * skill.multiplier) + "!");
            }
            else {
                Console.WriteLine(player_team.team[target].name + "'s " + skill.stat_affected + " was raised by " + (int)(effect_value * toggle_multiplier * t * skill.multiplier) + "!");
            }
        }
        if (skill.effect == "debuff") {
            if (skill.stat_affected == "STR") {
                if (user.toggle == true && user.name == "Peter") {
                    for (int i = 0; i < enemy_team.team.Count; i++) {
                        enemy_team.team[i].STR_mod -= (int) (effect_value * toggle_multiplier * t * skill.multiplier);
                    }
                }
                else {
                    enemy_team.team[target].STR_mod -= (int) (effect_value * toggle_multiplier * t * skill.multiplier);
                }
            }
            if (skill.stat_affected == "CHA") {
                if (user.toggle == true && user.name == "Peter") {
                    for (int i = 0; i < enemy_team.team.Count; i++) {
                        enemy_team.team[i].CHA_mod -= (int) (effect_value * toggle_multiplier * t * skill.multiplier);
                    }
                }
                else {
                    enemy_team.team[target].CHA_mod -= (int) (effect_value * toggle_multiplier * t * skill.multiplier);
                }
            }
            if (skill.stat_affected == "INT") {
                if (user.toggle == true && user.name == "Peter") {
                    for (int i = 0; i < enemy_team.team.Count; i++) {
                        enemy_team.team[i].INT_mod -= (int) (effect_value * toggle_multiplier * t * skill.multiplier);
                    }
                }
                else {
                    enemy_team.team[target].INT_mod -= (int) (effect_value * toggle_multiplier * t * skill.multiplier);
                }
            }

            if (user.toggle == true && user.name == "Peter") {
                Console.WriteLine("The " + skill.stat_affected + " of all teammates was lowered by " + (int)(effect_value * toggle_multiplier * t * skill.multiplier) + "!");
            }
            else {
                Console.WriteLine(enemy_team.team[target].name + "'s " + skill.stat_affected + " was lowered by " + (int)(effect_value * toggle_multiplier * t * skill.multiplier) + "!");
            }
        }

        if (skill.stat_used == "STR") {
            user.STR_mod /= 2;
            ToggleCon(user, "STR");
        }
        if (skill.stat_used == "CHA") {
            user.CHA_mod /= 2;
            ToggleCon(user, "CHA");
        }
        if (skill.stat_used == "INT") {
            user.INT_mod /= 2;
            ToggleCon(user, "INT");
        }
    }

/*
void attack(Combatant& user, Team& enemy_team, int target) {
    double toggle_multiplier = TogglePro(user);
    if (user.toggle == true && user.name == "Peter") {
        for (int i = 0; i < enemy_team.team.Count; i++) {
            enemy_team.team[i].HP -= int(Math.Max(0.0, (user.STR + user.STR_mod) * toggle_multiplier));
        }
        cout << user.name << " attacks! " << int(Math.Max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage to all enemies!\n";
    }
    else {
        enemy_team.team[target].HP -= int(Math.Max(0.0, (user.STR + user.STR_mod) * toggle_multiplier));
        cout << user.name << " attacks! " << int(Math.Max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) << " damage!\n";
    }
    user.STR_mod /= 2;
    if (user.toggle == true) {
        ToggleCon(user, "STR");
    }
}
*/

    public static void Attack(Combatant user, Team enemy_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < enemy_team.team.Count; i++) {
                enemy_team.team[i].HP -= (int) (Math.Max(0.0, (user.STR + user.STR_mod) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " attacks! " + (int) (Math.Max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) + " damage to all enemies!");
        }
        else {
            enemy_team.team[target].HP -= (int) (Math.Max(0.0, (user.STR + user.STR_mod) * toggle_multiplier));
            Console.WriteLine(user.name + " attacks! " + (int) (Math.Max(0.0, (user.STR + user.STR_mod) * toggle_multiplier)) + " damage!");
        }
        user.STR_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "STR");
        }
    }

    public static void Migraine(Combatant user, Team enemy_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < enemy_team.team.Count; i++) {
                enemy_team.team[i].HV -= (int) (Math.Max(0.0, ((user.STR + user.STR_mod)/2) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " uses Migraine! All enemies will take " + (int) (Math.Max(0.0, ((user.STR + user.STR_mod)/2) * toggle_multiplier)) + " extra damage every turn!");
        }
        else {
            enemy_team.team[target].HV -= (int) (Math.Max(0.0, ((user.STR + user.STR_mod)/2) * toggle_multiplier));
            Console.WriteLine(user.name + " uses Migraine! " + enemy_team.team[target].name + " will take " + (int) (Math.Max(0.0, ((user.STR + user.STR_mod)/2) * toggle_multiplier)) + " extra damage every turn!");
        }
        user.STR_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "STR");
        }
    }

    public static void AccelerantMigraine(Combatant user, Team enemy_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < enemy_team.team.Count; i++) {
                enemy_team.team[i].HA -= (int) (Math.Max(0.0, ((user.STR + user.STR_mod)/4) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " uses Accelerant Migraine! All enemies will take " + (int) (Math.Max(0.0, ((user.STR + user.STR_mod)/4) * toggle_multiplier)) + " extra damage every turn, each turn increasing by that amount!");
        }
        else {
            enemy_team.team[target].HA -= (int) (Math.Max(0.0, ((user.STR + user.STR_mod)/4) * toggle_multiplier));
            Console.WriteLine(user.name + " uses Accelerant Migraine! " + enemy_team.team[target].name + " will take " + (int) (Math.Max(0.0, ((user.STR + user.STR_mod)/4) * toggle_multiplier)) + " extra damage every turn, each turn increasing by that amount!");
        }
        user.STR_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "STR");
        }
    }

    public static void Stimulate(Combatant user, Team player_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < player_team.team.Count; i++) {
                player_team.team[i].STR_mod += (int) (Math.Max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " uses Stimulate! The STR of all teammates was raised by " + (int) (Math.Max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier)) + "!");
        }
        else {
            player_team.team[target].STR_mod += (int) (Math.Max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier));
            Console.WriteLine(user.name + " uses Stimulate! " + player_team.team[target].name + "'s STR was raised by " + (int) (Math.Max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier)) + "!");
        }
        user.CHA_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "CHA");
        }
    }

    public static void RizzUp(Combatant user, Team player_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < player_team.team.Count; i++) {
                player_team.team[i].CHA_mod += (int) (Math.Max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " uses Rizz Up! The CHA of all teammates was raised by " + (int) (Math.Max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier)) + "!");
        }
        else {
            player_team.team[target].CHA_mod += (int) (Math.Max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier));
            Console.WriteLine(user.name + " uses Rizz Up! " + player_team.team[target].name + "'s CHA was raised by " + (int) (Math.Max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier)) + "!");
        }
        user.CHA_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "CHA");
        }
    }

    public static void BrainBoost(Combatant user, Team player_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < player_team.team.Count; i++) {
                player_team.team[i].INT_mod += (int) (Math.Max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " uses Brain Boost! The INT of all teammates was raised by " + (int) (Math.Max(0.0, ((user.CHA + user.CHA_mod)/2) * toggle_multiplier)) + "!");
        }
        else {
            player_team.team[target].INT_mod += (int) (Math.Max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier));
            Console.WriteLine(user.name + " uses Brain Boost! " + player_team.team[target].name + "'s INT was raised by " + (int) (Math.Max(0.0, (user.CHA + user.CHA_mod) * toggle_multiplier)) + "!");
        }
        user.CHA_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "CHA");
        }
    }

    public static void Sedate(Combatant user, Team enemy_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < enemy_team.team.Count; i++) {
                enemy_team.team[i].STR_mod -= (int) (Math.Max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " uses Sedate! The STR of all enemies was lowered by " + (int) (Math.Max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier)) + "!");
        }
        else {
            enemy_team.team[target].STR_mod -= (int) (Math.Max(0.0, (user.INT + user.INT_mod) * toggle_multiplier));
            Console.WriteLine(user.name + " uses Sedate! " + enemy_team.team[target].name + "'s STR was lowered by " + (int) (Math.Max(0.0, (user.INT + user.INT_mod) * toggle_multiplier)) + "!");
        }
        user.INT_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "INT");
        }
    }

    public static void RizzDown(Combatant user, Team enemy_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < enemy_team.team.Count; i++) {
                enemy_team.team[i].CHA_mod -= (int) (Math.Max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " uses Rizz Down! The CHA of all enemies was lowered by " + (int) (Math.Max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier)) + "!");
        }
        else {
            enemy_team.team[target].CHA_mod -= (int) (Math.Max(0.0, (user.INT + user.INT_mod) * toggle_multiplier));
            Console.WriteLine(user.name + " uses Rizz Down! " + enemy_team.team[target].name + "'s CHA was lowered by " + (int) (Math.Max(0.0, (user.INT + user.INT_mod) * toggle_multiplier)) + "!");
        }
        user.INT_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "INT");
        }
    }

    public static void BrainBreak(Combatant user, Team enemy_team, int target) {
        double toggle_multiplier = TogglePro(user);
        if (user.toggle == true && user.name == "Peter") {
            for (int i = 0; i < enemy_team.team.Count; i++) {
                enemy_team.team[i].INT_mod -= (int) (Math.Max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier));
            }
            Console.WriteLine(user.name + " uses Brain Break! The INT of all enemies was lowered by " + (int) (Math.Max(0.0, ((user.INT + user.INT_mod)/2) * toggle_multiplier)) + "!");
        }
        else {
            enemy_team.team[target].INT_mod -= (int) (Math.Max(0.0, (user.INT + user.INT_mod) * toggle_multiplier));
            Console.WriteLine(user.name + " uses Brain Break! " + enemy_team.team[target].name + "'s INT was lowered by " + (int) (Math.Max(0.0, (user.INT + user.INT_mod) * toggle_multiplier)) + "!");
        }
        user.INT_mod /= 2;
        if (user.toggle == true) {
            ToggleCon(user, "INT");
        }
    }

    public static void Analyze(Combatant target) {
        Console.WriteLine(target.name);
        Console.WriteLine("HP: " + target.HP + "/" + target.maxHP);
        Console.WriteLine("HV: " + target.HV);
        Console.WriteLine("HA: " + target.HA);
        Console.WriteLine("VIT: " + target.VIT + " + " + target.VIT_mod + " = " + (target.VIT + target.VIT_mod));
        Console.WriteLine("STR: " + target.STR + " + " + target.STR_mod + " = " + (target.STR + target.STR_mod));
        Console.WriteLine("CHA: " + target.CHA + " + " + target.CHA_mod + " = " + (target.CHA + target.CHA_mod));
        Console.WriteLine("INT: " + target.INT + " + " + target.INT_mod + " = " + (target.INT + target.INT_mod));
    }

    public static void Dialogue(string line) {
        Console.WriteLine(line);
        Console.ReadLine();
    }

    public static int Battle(Team player_team, Team enemy_team, Rules rules) {
        for (int i = 0; i < player_team.team.Count; i++) {
            player_team.team[i].HP = player_team.team[i].VIT * 5;
            player_team.team[i].maxHP = player_team.team[i].VIT * 5;
            player_team.team[i].HV = 0;
            player_team.team[i].HA = 0;
        }
        for (int i = 0; i < enemy_team.team.Count; i++) {
            enemy_team.team[i].HP = enemy_team.team[i].VIT * 5;
            enemy_team.team[i].maxHP = enemy_team.team[i].VIT * 5;
            enemy_team.team[i].HV = 0;
            enemy_team.team[i].HA = 0;
        }

        int living_player_team = player_team.team.Count;
        int living_enemy_team = enemy_team.team.Count;

        while (living_player_team > 0 && living_enemy_team > 0) {
            for (int i = 0; i < player_team.team.Count; i++) {
                Console.WriteLine(player_team.team[i].name + " HP: " + player_team.team[i].HP);
                player_team.team[i].actions = 1;
            }
            if (player_team.semester >= 2) {
                Console.WriteLine("Party TP: " + player_team.TP);
            }
            for (int i = 0; i < enemy_team.team.Count; i++) {
                Console.WriteLine(enemy_team.team[i].name + " HP: " + enemy_team.team[i].HP);
                enemy_team.team[i].actions = 1;
            }

            for (int i = 0; i < player_team.team.Count; i++) {
                if (player_team.team[i].alive == true) {
                    while (player_team.team[i].actions > 0) {
                        int command = 0;
                        Console.WriteLine("1. " + player_team.team[i].skills[0] + " \n");
                        Console.WriteLine("2. " + player_team.team[i].skills[1] + " \n");
                        Console.WriteLine("3. " + player_team.team[i].skills[2] + " \n");
                        Console.WriteLine("4. " + player_team.team[i].skills[3] + " \n");
                        Console.WriteLine("ABILITY: " + player_team.team[i].skills[4] + " \n");

                        /*
                        Console.WriteLine("Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle ability, 6 to Analyze: ");
                        command = int.Parse(Console.ReadLine());
                        while (command != 1 && command != 2 && command != 3 && command != 4 && command != 5 && command != 6) {
                            Console.WriteLine("Invalid! Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle Burnout, 6 to Analyze: ");
                            command = int.Parse(Console.ReadLine());
                        }
                        */

                        Console.WriteLine("Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle ability, 6 to Analyze: ");
                        while (command != 1 && command != 2 && command != 3 && command != 4 && command != 5 && command != 6) {
                            if (!int.TryParse(Console.ReadLine(), out command)) {
                                Console.WriteLine("Invalid! Hey you, whatcha gonna do? 1-4 for skills, 5 to toggle Burnout, 6 to Analyze: ");
                                command = 0;
                            }
                        }

                        if (command == 6) {
                            int side;
                            Console.WriteLine("Who will you Analyze? 1 for teammates, 2 for enemies: ");
                            side = int.Parse(Console.ReadLine());
                            while (side != 1 && side != 2) {
                                Console.WriteLine("Who will you Analyze? 1 for teammates, 2 for enemies: ");
                                side = int.Parse(Console.ReadLine());
                            }
                            if (side == 1) {
                                int target = PlayerTargeting(player_team, "teammate", ref living_player_team, rules);
                                Analyze(player_team.team[target]);
                            }
                            else if (side == 2) {
                                int target = PlayerTargeting(enemy_team, "enemy", ref living_enemy_team, rules);
                                Analyze(enemy_team.team[target]);
                            }
                            continue;
                        }
                        else if (player_team.team[i].skills[command-1] == "Attack") {
                            int target = PlayerTargeting(enemy_team, "enemy", ref living_enemy_team, rules);
                            Attack(player_team.team[i], enemy_team, target);
                            if (player_team.team[i].toggle == true && player_team.team[i].name == "Peter") {
                                for (int j = 0; j < enemy_team.team.Count; j++) {
                                    CheckIfDowned(enemy_team.team[j], ref living_enemy_team);
                                }
                            }
                            else {
                                CheckIfDowned(enemy_team.team[target], ref living_enemy_team);
                            }
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Migraine") {
                            int target = PlayerTargeting(enemy_team, "enemy", ref living_enemy_team, rules);
                            Migraine(player_team.team[i], enemy_team, target);
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Accelerant Migraine") {
                            int target = PlayerTargeting(enemy_team, "enemy", ref living_enemy_team, rules);
                            AccelerantMigraine(player_team.team[i], enemy_team, target);
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Stimulate") {
                            int target = PlayerTargeting(player_team, "teammate", ref living_player_team, rules);
                            Stimulate(player_team.team[i], player_team, target);
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Rizz Up") {
                            int target = PlayerTargeting(player_team, "teammate", ref living_player_team, rules);
                            RizzUp(player_team.team[i], player_team, target);
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Brain Boost") {
                            int target = PlayerTargeting(player_team, "teammate", ref living_player_team, rules);
                            BrainBoost(player_team.team[i], player_team, target);
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Sedate") {
                            int target = PlayerTargeting(enemy_team, "enemy", ref living_enemy_team, rules);
                            Sedate(player_team.team[i], enemy_team, target);
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Rizz Down") {
                            int target = PlayerTargeting(enemy_team, "enemy", ref living_enemy_team, rules);
                            RizzDown(player_team.team[i], enemy_team, target);
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Brain Break") {
                            int target = PlayerTargeting(enemy_team, "enemy", ref living_enemy_team, rules);
                            BrainBreak(player_team.team[i], enemy_team, target);
                            player_team.team[i].actions -= 1;
                        }
                        else if (player_team.team[i].skills[command-1] == "Burnout") {
                            if (player_team.team[i].toggle == false) {
                                player_team.team[i].toggle = true;
                                Console.WriteLine(player_team.team[i].name + " activates Burnout.");
                            }
                            else {
                                player_team.team[i].toggle = false;
                                Console.WriteLine(player_team.team[i].name + " deactivates Burnout.");
                            }
                            continue;
                        }
                        else if (player_team.team[i].skills[command-1] == "Coin Flip") {
                            if (player_team.team[i].toggle == false) {
                                player_team.team[i].toggle = true;
                                Console.WriteLine(player_team.team[i].name + " activates Coin Flip.");
                            }
                            else {
                                player_team.team[i].toggle = false;
                                Console.WriteLine(player_team.team[i].name + " deactivates Coin Flip.");
                            }
                            continue;
                        }
                        else if (player_team.team[i].skills[command-1] == "Distribute") {
                            if (player_team.team[i].toggle == false) {
                                player_team.team[i].toggle = true;
                                Console.WriteLine(player_team.team[i].name + " activates Distribute.");
                            }
                            else {
                                player_team.team[i].toggle = false;
                                Console.WriteLine(player_team.team[i].name + " deactivates Distribute.");
                            }
                            continue;
                        }
                    }
                }
            }

            for (int i = 0; i < enemy_team.team.Count; i++) {
                if (enemy_team.team[i].HP > 0) {
                    if ((enemy_team.team[i].name == "Charisma Cannon" || enemy_team.team[i].name == "Stella" || enemy_team.team[i].name == "Lovebird" || enemy_team.team[i].name == "Glad Doctor" || enemy_team.team[i].name == "CARRIE" || enemy_team.team[i].name == "Louie") && enemy_team.team[i].STR_mod <= 0) {
                        enemy_team.team[i].STR_mod += Math.Max(0, (int)((enemy_team.team[i].CHA + enemy_team.team[i].CHA_mod)));
                        Console.WriteLine(enemy_team.team[i].name + " uses Stimulate! " + enemy_team.team[i].name + "'s STR was raised by " + (int)Math.Max(0, (int)((enemy_team.team[i].CHA + enemy_team.team[i].CHA_mod))) + "!");
                        enemy_team.team[i].CHA_mod /= 2;
                    }
                    else if (enemy_team.team[i].name == "Brainy Broom" || enemy_team.team[i].name == "Ambivalant" || enemy_team.team[i].name == "Sad Doctor" || enemy_team.team[i].name == "Nellie") {
                        int optimal = EnemyTargetingHighestSTR(player_team);
                        double t = Timing();
                        if (player_team.team[optimal].STR_mod >= 0) {
                            player_team.team[optimal].STR_mod -= Math.Max(0, (int) ((enemy_team.team[i].INT + enemy_team.team[i].INT_mod) * t));
                            Console.WriteLine(enemy_team.team[i].name + " uses Sedate! " + player_team.team[optimal].name + "'s STR was lowered by " + (int) (Math.Max(0, (int)((enemy_team.team[i].INT + enemy_team.team[i].INT_mod) * t))) + "!");
                            enemy_team.team[i].INT_mod /= 2;
                        }
                        else {
                            optimal = EnemyTargetingLowestHP(player_team);
                            t = Timing();
                            player_team.team[optimal].HP -= Math.Max(0, (int) ((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t));
                            Console.WriteLine(enemy_team.team[i].name + " attacks! " + (int) (Math.Max(0, (int)((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t))) + " damage!");
                            if (player_team.team[optimal].HP <= 0 && player_team.team[optimal].alive == true) {
                                player_team.team[optimal].alive = false;
                                living_player_team -= 1;
                            }
                            enemy_team.team[i].STR_mod /= 2;
                        }
                    }
                    else {
                        int optimal = EnemyTargetingLowestHP(player_team);
                        double t = Timing();
                        player_team.team[optimal].HP -= Math.Max(0, (int) ((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t));
                        Console.WriteLine(enemy_team.team[i].name + " attacks! " + (int) (Math.Max(0, (int)((enemy_team.team[i].STR + enemy_team.team[i].STR_mod) * t))) + " damage!");
                        if (player_team.team[optimal].HP <= 0 && player_team.team[optimal].alive == true) {
                            player_team.team[optimal].alive = false;
                            living_player_team -= 1;
                        }
                        enemy_team.team[i].STR_mod /= 2;
                    }
                }
            }

            ApplyHV(player_team, ref living_player_team);
            ApplyHV(enemy_team, ref living_enemy_team);
            ApplyHA(player_team);
            ApplyHA(enemy_team);

            if (player_team.semester >= 2) {
                player_team.TP += 1;
            }

            if (living_enemy_team == 0) {
                break;
            }

            if (living_player_team == 0) {
                Console.WriteLine("GAME OVER");
                Environment.Exit(0);
            }
        }

        Console.WriteLine("VICTORY!\n");

        double grade_numerator = 0;
        double grade_denominator = 0;
        for (int i = 0; i < player_team.team.Count; i++) {
            grade_numerator += player_team.team[i].HP;
            grade_denominator += player_team.team[i].maxHP;
        }
        double battle_grade = grade_numerator / grade_denominator;
        if (battle_grade >= 0.9) {
            player_team.gradebook.Add("A");
            Console.WriteLine("YOU GOT AN A!");
        }
        else if (battle_grade >= 0.8) {
            player_team.gradebook.Add("B");
            Console.WriteLine("You got a B!");
        }
        else if (battle_grade >= 0.7) {
            player_team.gradebook.Add("C");
            Console.WriteLine("You got a C.");
        }
        else if (battle_grade >= 0.6) {
            player_team.gradebook.Add("D");
            Console.WriteLine("You got a D.");
        }
        else {
            player_team.gradebook.Add("F");
            Console.WriteLine("You got an F...");
        }

        int stat_boost = 0;
        Console.WriteLine("Which stat will you increase? 1 for VIT, 2 for STR, 3 for CHA, 4 for INT: ");
        while (stat_boost != 1 && stat_boost != 2 && stat_boost != 3 && stat_boost != 4) {
            if (!int.TryParse(Console.ReadLine(), out stat_boost)) {
                Console.WriteLine("Invalid! Which stat will you increase? 1 for VIT, 2 for STR, 3 for CHA, 4 for INT: ");
                stat_boost = 0;
            }
        }
        for (int i = 0; i < player_team.team.Count; i++) {
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

        ResetMods(player_team);

        player_team.battles_won += 1;

        return 0;
    }

    public static void Tazewell(Team player_team, Inventory inventory, Rules rules) {
        bool in_tazewell = true;
        while (in_tazewell == true) {
            int command = 0;
            Console.WriteLine("What would you like to do? 1 to reassign stats, 2 to equip skills, 3 to check gradebook, 4 to save your game, 5 to leave Tazewell: ");
            command = int.Parse(Console.ReadLine());
            while (command != 1 && command != 2 && command != 3 && command != 4 && command != 5) {
                Console.WriteLine("Invalid! What would you like to do? 1 to reassign stats, 2 to equip skills, 3 to check gradebook, 4 to save your game, 5 to leave Tazewell: ");
                command = int.Parse(Console.ReadLine());
            }
            if (command == 1) {
                int target = 0;
                if (player_team.team.Count > 1) {
                    target = SelectTarget(player_team, "tazewell_1", rules);
                }
                int total = player_team.team[target].VIT + player_team.team[target].STR + player_team.team[target].CHA + player_team.team[target].INT;

                int invest = 0;
                Console.WriteLine("How many points would you like to invest into Vitality? You have " + total + " stat point(s) left.");
                invest = int.Parse(Console.ReadLine());
                while (1 > invest || invest > (total - 3)) {
                    Console.WriteLine("Invalid! How many points would you like to invest into Vitality?");
                    invest = int.Parse(Console.ReadLine());
                }
                player_team.team[target].VIT = invest;
                total -= invest;

                Console.WriteLine("How many points would you like to invest into Strength? You have " + total + " stat point(s) left.");
                invest = int.Parse(Console.ReadLine());
                while (1 > invest || invest > (total - 2)) {
                    Console.WriteLine("Invalid! How many points would you like to invest into Strength?");
                    invest = int.Parse(Console.ReadLine());
                }
                player_team.team[target].STR = invest;
                total -= invest;

                Console.WriteLine("How many points would you like to invest into Charisma? You have " + total + " stat point(s) left.");
                invest = int.Parse(Console.ReadLine());
                while (1 > invest || invest > (total - 1)) {
                    Console.WriteLine("Invalid! How many points would you like to invest into Charisma?");
                    invest = int.Parse(Console.ReadLine());
                }
                player_team.team[target].CHA = invest;
                total -= invest;

                Console.WriteLine("How many points would you like to invest into Intellect? You have " + total + " stat point(s) left.");
                invest = int.Parse(Console.ReadLine());
                while (1 > invest || invest > total) {
                    Console.WriteLine("Invalid! How many points would you like to invest into Intellect?");
                    invest = int.Parse(Console.ReadLine());
                }
                player_team.team[target].INT = invest;
                total -= invest;

                Console.WriteLine("VIT: " + player_team.team[target].VIT);
                Console.WriteLine("STR: " + player_team.team[target].STR);
                Console.WriteLine("CHA: " + player_team.team[target].CHA);
                Console.WriteLine("INT: " + player_team.team[target].INT);
                continue;
            }
            else if (command == 2) {
                int slot = 0;
                int target = 0;
                if (player_team.team.Count > 1) {
                    target = SelectTarget(player_team, "tazewell_2", rules);
                }

                Console.WriteLine("1. " + player_team.team[target].skills[0]);
                Console.WriteLine("2. " + player_team.team[target].skills[1]);
                Console.WriteLine("3. " + player_team.team[target].skills[2]);
                Console.WriteLine("4. " + player_team.team[target].skills[3]);
                Console.WriteLine("Which slot would you like to change? Choose a slot from 1 to 4: ");
                slot = int.Parse(Console.ReadLine());
                while (slot > 4 || slot < 1) {
                    Console.WriteLine("Invalid! Which slot would you like to change?");
                    slot = int.Parse(Console.ReadLine());
                }

                int replacement = 0;
                for (int i = 0; i < inventory.skills.Count; i++) {
                    Console.WriteLine(inventory.skills[i]);
                }
                Console.WriteLine("Which skill will you put in its place? 1 for the first slot, 2 for the second slot, etc: ");
                replacement = int.Parse(Console.ReadLine());
                while (replacement > inventory.skills.Count || replacement < 1) {
                    Console.WriteLine("Invalid! Which skill will you put in its place? 1 for the first slot, 2 for the second slot, etc: ");
                    replacement = int.Parse(Console.ReadLine());
                }

                player_team.team[target].skills[slot-1] = inventory.skills[replacement-1];

                Console.WriteLine("1. " + player_team.team[target].skills[0]);
                Console.WriteLine("2. " + player_team.team[target].skills[1]);
                Console.WriteLine("3. " + player_team.team[target].skills[2]);
                Console.WriteLine("4. " + player_team.team[target].skills[3]);
                continue;
            }
            else if (command == 3) {
                int grade_a = 0;
                bool has_reward = false;

                for (int i = 0; i < player_team.gradebook.Count; i++) {
                    Console.WriteLine(player_team.gradebook[i]);
                    if (player_team.gradebook[i] == "A") {
                        grade_a += 1;
                    }
                }
                if (grade_a >= 4) {
                    has_reward = false;
                    for (int j = 0; j < inventory.skills.Count; j++) {
                        if (inventory.skills[j] == "Migraine") {
                            has_reward = true;
                        }
                    }
                    if (has_reward == false) {
                        inventory.skills.Add("Migraine");
                        Console.WriteLine("For getting 4 As, you can now equip the skill Migraine.\n");
                    }
                }
                if (grade_a >= 8) {
                    has_reward = false;
                    for (int j = 0; j < inventory.skills.Count; j++) {
                        if (inventory.skills[j] == "Accelerant Migraine") {
                            has_reward = true;
                        }
                    }
                    if (has_reward == false) {
                        inventory.skills.Add("Accelerant Migraine");
                        Console.WriteLine("For getting 8 As, you can now equip the skill Accelerant Migraine.\n");
                    }
                }
                continue;
            }
            else if (command == 4) {
                StreamWriter file;
                file = new StreamWriter("save.txt");
                file.WriteLine(player_team.semester);
                file.WriteLine(player_team.battles_won);
                for (int i = 0; i < player_team.gradebook.Count; i++) {
                    file.WriteLine(player_team.gradebook[i]);
                }
                for (int i = 0; i < player_team.team.Count; i++) {
                    file.WriteLine(player_team.team[i].name);
                    file.WriteLine(player_team.team[i].VIT);
                    file.WriteLine(player_team.team[i].STR);
                    file.WriteLine(player_team.team[i].CHA);
                    file.WriteLine(player_team.team[i].INT);
                    file.WriteLine(player_team.team[i].skills[0]);
                    file.WriteLine(player_team.team[i].skills[1]);
                    file.WriteLine(player_team.team[i].skills[2]);
                    file.WriteLine(player_team.team[i].skills[3]);
                }
                file.Close();
                Console.WriteLine("The save system is currently a work-in-progress. ");
                continue;
            }
            else if (command == 5) {
                Console.WriteLine("You left Tazewell.\n");
                in_tazewell = false;
            }
        }
    }


    public static void RunGame() {
        Combatant bronson = new Combatant {name = "Bronson", VIT = 5, STR = 5, CHA = 5, INT = 5};
        bronson.skills = new List<string> { "Attack", "Stimulate", "Sedate", "N/A", "Burnout" };
        Team player_team = new Team { team = new List<Combatant> { bronson } };
        Inventory inventory = new Inventory();
        Rules rules = new Rules();

        Dialogue("CENSORY - SEMESTER 1");
        Dialogue("CARRIED-OVER TRAUMA");
        Dialogue("BOSWELL HALL - 3:00 PM");
        Dialogue("BRONSON: I gotta go to office hours... but shit, I don't have a pencil. Guess I gotta steal one.");
        Dialogue("(Suddenly, the landscape around BRONSON changes completely.)");
        Dialogue("BRONSON: Huh?! What is this place, and why does it look like a fucking math problem?");
        Dialogue("BRONSON: Ah well, nothing to do but find a way out. Let's roll.");

        Console.WriteLine("BATTLE 1-1-1: AVERAGE JOE \n");
        Combatant average_joe_1 = new Combatant {name = "Average Joe", VIT = 6, STR = 6, CHA = 6, INT = 6};
        Team enemy_team = new Team { team = new List<Combatant> { average_joe_1 } };
        Battle(player_team, enemy_team, rules);

        Console.WriteLine("BATTLE 1-1-2: POSTSYNAPTIC POLICE \n");
        Combatant postsynaptic_police_1 = new Combatant {name = "Postsynaptic Police", VIT = 8, STR = 8, CHA = 4, INT = 4};
        enemy_team = new Team { team = new List<Combatant> { postsynaptic_police_1 } };
        Battle(player_team, enemy_team, rules);

        Console.WriteLine("BATTLE 1-1-3: BUMBLING BURGLAR \n");
        Combatant bumbling_burglar_1 = new Combatant {name = "Bumbling Burglar", VIT = 4, STR = 12, CHA = 4, INT = 4};
        enemy_team = new Team { team = new List<Combatant> { bumbling_burglar_1 } };
        Battle(player_team, enemy_team, rules);
        player_team.battles_won += 1;

        Dialogue("BRONSON: This is it. The final room. There's gotta be...");
        Dialogue("BRONSON: THERE IT IS!");
        Dialogue("You notice a pencil underneath a desk. As you grab the pencil, the desk suddenly starts moving...");
        Dialogue("BRONSON: Who do you think I am, Indiana Jones?");
        Dialogue("The desk begins to shake the ground below you... you struggle to stand for a brief moment.");
        Dialogue("BRONSON: Ah shit... do I have to fight... a literal fucking desk?!");

        Console.WriteLine("BATTLE 1-1-4: A LITERAL FUCKING DESK\n");
        Combatant a_literal_fucking_desk = new Combatant {name = "A Literal Fucking Desk", VIT = 10, STR = 10, CHA = 5, INT = 5};
        enemy_team = new Team { team = new List<Combatant> { a_literal_fucking_desk } };
        Battle(player_team, enemy_team, rules);
    
        Dialogue("BRONSON: Finally... at long last... I have a pencil for office hours!");
        Dialogue("HEIST SUCCESSFUL!");
        Dialogue("All of a sudden, the desk begins to sound. A blue laser is pointed towards BRONSON's head.");
        Dialogue("A LITERAL FUCKING DESK: SCANNING BRAIN ACTIVITY...");
        Dialogue("BRONSON: What is this, 1984?!");
        Dialogue("The desk rapidly deconstructs and constructs itself into another form.");
        Dialogue("???: Greetings, Bronson.");
        Dialogue("BRONSON: No... this can't be... Carrie? What are YOU doing here?! Why are you a desk?! Why-");
        Dialogue("???: I am not the real person that you are referring to. As you just stated, I am the desk you just destroyed.");
        Dialogue("BRONSON: Then why do you look like her? How do you know her?");
        Dialogue("???: After my destruction, I had to quickly take a new form to prevent myself from dying. Thus, I scanned your brain activity for potential candidates. And what do you know, a certain someone just lives rent-free in your head.");
        Dialogue("???: However, it was more than just that. I need your help with something, and I assumed that this form... would make you cooperate.");
        Dialogue("BRONSON: What are you talking about?!");
        Dialogue("???: You see, a military general by the name of Colonel Eigen has taken over this world we both currently reside in. He's killed countless organisms, and he won't stop until someone makes him.");
        Dialogue("BRONSON: I wish I could help you out, but I have homework. Now if I can just go in peace...");
        Dialogue("CARRIE: If you do that, this world, the Universal Set, shall overtake your own world, transforming your world into a place driven by logic alone. It will be perfect... however it will be hollow.");
        Dialogue("BRONSON: Oh, so like my old high school then. Alright, fine. I'll help you out... not Carrie.");
        Dialogue("CARRIE: You can call me CARRIE. It's short for Cognitive Artificial Reality Rendering Intelligence Expert.");
        Dialogue("CARRIE: Welcome to the Universal Set, Bronson.");
        Dialogue("(They shake hands. BRONSON blushes a bit.)");
        Dialogue("< END OF HEIST 1 >");

        // insert goofy ahh side cutscene here
        Tazewell(player_team, inventory, rules);

        // insert pre-mission cutscene here
        Combatant colt = new Combatant {name = "Colt", VIT = 6, STR = 6, CHA = 6, INT = 6};
        colt.skills = new List<string> { "Attack", "Rizz Up", "Rizz Down", "N/A", "Coin Flip" };
        player_team.team.Add(colt);
        inventory.skills.Add("Rizz Up");
        inventory.skills.Add("Rizz Down");

        Console.WriteLine("BATTLE 1-2-1: AVERAGE JOE DUO \n");
        Combatant average_joe_2 = new Combatant {name = "Average Joe", VIT = 8, STR = 8, CHA = 8, INT = 8};
        Combatant average_joe_3 = new Combatant {name = "Average Joe", VIT = 8, STR = 8, CHA = 8, INT = 8};
        enemy_team = new Team { team = new List<Combatant> { average_joe_2, average_joe_3 } };
        Battle(player_team, enemy_team, rules);

        Console.WriteLine("BATTLE 1-2-2: CHARISMA CANNON DUO \n");
        Combatant charisma_cannon_1 = new Combatant {name = "Charisma Cannon", VIT = 12, STR = 4, CHA = 16, INT = 4};
        Combatant charisma_cannon_2 = new Combatant {name = "Charisma Cannon", VIT = 12, STR = 4, CHA = 16, INT = 4};
        enemy_team = new Team { team = new List<Combatant> { charisma_cannon_1, charisma_cannon_2 } };
        Battle(player_team, enemy_team, rules);

        Console.WriteLine("BATTLE 1-2-3: BRAINY BROOM DUO \n");
        Combatant brainy_broom_1 = new Combatant {name = "Brainy Broom", VIT = 12, STR = 4, CHA = 4, INT = 16};
        Combatant brainy_broom_2 = new Combatant {name = "Brainy Broom", VIT = 12, STR = 4, CHA = 4, INT = 16};
        enemy_team = new Team { team = new List<Combatant> { brainy_broom_1, brainy_broom_2 } };
        Battle(player_team, enemy_team, rules);

        // insert pre-boss cutscene here
        Console.WriteLine("BATTLE 1-2-4: STELLA THE MOON MAIDEN \n");
        Combatant stella = new Combatant {name = "Stella", VIT = 20, STR = 10, CHA = 25, INT = 25};
        enemy_team = new Team { team = new List<Combatant> { stella } };
        Battle(player_team, enemy_team, rules);
        // insert post-boss cutscene here

        // insert goofy ahh side cutscene here
        Tazewell(player_team, inventory, rules);

        // insert pre-mission cutscene here
        Combatant peter = new Combatant {name = "Peter", VIT = 7, STR = 7, CHA = 7, INT = 7};
        peter.skills = new List<string> { "Attack", "Brain Boost", "Brain Break", "N/A", "Distribute" };
        player_team.team.Add(peter);
        inventory.skills.Add("Brain Boost");
        inventory.skills.Add("Brain Break");

        Console.WriteLine("BATTLE 1-3-1: AVERAGE JOE THREESOME \n");
        Combatant average_joe_4 = new Combatant {name = "Average Joe", VIT = 10, STR = 10, CHA = 10, INT = 10};
        Combatant average_joe_5 = new Combatant {name = "Average Joe", VIT = 10, STR = 10, CHA = 10, INT = 10};
        Combatant average_joe_6 = new Combatant {name = "Average Joe", VIT = 10, STR = 10, CHA = 10, INT = 10};
        enemy_team = new Team { team = new List<Combatant> { average_joe_4, average_joe_5, average_joe_6 } };
        Battle(player_team, enemy_team, rules);

        Console.WriteLine("BATTLE 1-3-2: HEARTSTEALERS \n");
        Combatant lovebird_1 = new Combatant {name = "Lovebird", VIT = 12, STR = 4, CHA = 20, INT = 4};
        Combatant bumbling_burglar_2 = new Combatant {name = "Bumbling Burglar", VIT = 8, STR = 24, CHA = 4, INT = 4};
        Combatant charisma_cannon_3 = new Combatant {name = "Charisma Cannon", VIT = 16, STR = 4, CHA = 16, INT = 4};
        enemy_team = new Team { team = new List<Combatant> { lovebird_1, bumbling_burglar_2, charisma_cannon_3 } };
        Battle(player_team, enemy_team, rules);

        Console.WriteLine("BATTLE 1-3-3: LOVE TRIANGLE \n");
        Combatant lovebird_2 = new Combatant {name = "Lovebird", VIT = 8, STR = 4, CHA = 24, INT = 4};
        Combatant hatebee_1 = new Combatant {name = "Hatebee", VIT = 8, STR = 24, CHA = 4, INT = 4};
        Combatant ambivalant_1 = new Combatant {name = "Ambivalant", VIT = 8, STR = 4, CHA = 4, INT = 24};
        enemy_team = new Team { team = new List<Combatant> { lovebird_2, hatebee_1, ambivalant_1 } };
        Battle(player_team, enemy_team, rules);

        // insert pre-boss cutscene here
        Console.WriteLine("BATTLE 1-3-4: COLONEL EIGEN \n");
        Combatant eigen = new Combatant {name = "Colonel Eigen", VIT = 50, STR = 40, CHA = 30, INT = 30};
        enemy_team = new Team { team = new List<Combatant> { eigen } };
        Battle(player_team, enemy_team, rules);
        // insert post-boss cutscene here

        // insert goofy ahh side cutscene here
        Tazewell(player_team, inventory, rules);

        int choice = 0;
        Console.WriteLine("You hear howling near Swem Library... will you investigate? Type 1 if yes, 0 if no.\n");
        Console.WriteLine("NOTE: This is an optional challenge, and it will be your hardest one yet. Please think twice before proceeding.");
        choice = int.Parse(Console.ReadLine());
        // insert pre-boss cutscene here
        if (choice == 1) {
            // insert pre-boss cutscene here
            Console.WriteLine("OPTIONAL SUPERBOSS 1-1: LIBRARY CORGIS \n");
            Combatant nellie = new Combatant {name = "Nellie", VIT = 20, STR = 5, CHA = 5, INT = 20};
            Combatant tucker = new Combatant {name = "Tucker", VIT = 20, STR = 20, CHA = 5, INT = 5};
            Combatant louie = new Combatant {name = "Louie", VIT = 20, STR = 5, CHA = 20, INT = 5};
            enemy_team = new Team { team = new List<Combatant> { nellie, tucker, louie } };
            Battle(player_team, enemy_team, rules);
            // insert post-boss cutscene here
        }

        Console.WriteLine("BATTLE 1-4-1: GROUP THERAPY \n");
        Combatant average_joe_7 = new Combatant {name = "Average Joe", VIT = 12, STR = 12, CHA = 12, INT = 12};
        Combatant average_joe_8 = new Combatant {name = "Average Joe", VIT = 12, STR = 12, CHA = 12, INT = 12};
        Combatant average_joe_9 = new Combatant {name = "Average Joe", VIT = 12, STR = 12, CHA = 12, INT = 12};
        enemy_team = new Team { team = new List<Combatant> { average_joe_7, average_joe_8, average_joe_9 } };
        Battle(player_team, enemy_team, rules);

        Console.WriteLine("BATTLE 1-4-2: KLEPTOMANIACS \n");
        Combatant bumbling_burglar_3 = new Combatant {name = "Bumbling Burglar", VIT = 8, STR = 32, CHA = 4, INT = 4};
        Combatant bumbling_burglar_4 = new Combatant {name = "Bumbling Burglar", VIT = 8, STR = 32, CHA = 4, INT = 4};
        Combatant bumbling_burglar_5 = new Combatant {name = "Bumbling Burglar", VIT = 8, STR = 32, CHA = 4, INT = 4};
        enemy_team = new Team { team = new List<Combatant> { bumbling_burglar_3, bumbling_burglar_4, bumbling_burglar_5 } };
        Battle(player_team, enemy_team, rules);

        Console.WriteLine("BATTLE 1-4-3: PSYCHOTIC DOCTORS \n");
        Combatant mad_doctor_1 = new Combatant {name = "Mad Doctor", VIT = 10, STR = 30, CHA = 4, INT = 4};
        Combatant glad_doctor_1 = new Combatant {name = "Glad Doctor", VIT = 10, STR = 4, CHA = 30, INT = 4};
        Combatant sad_doctor_1 = new Combatant {name = "Sad Doctor", VIT = 10, STR = 4, CHA = 4, INT = 30};
        enemy_team = new Team { team = new List<Combatant> { mad_doctor_1, glad_doctor_1, sad_doctor_1 } };
        Battle(player_team, enemy_team, rules);

        // insert pre-boss cutscene here
        Console.WriteLine("BATTLE 1-4-4: CARRIE \n");
        Combatant carrie = new Combatant {name = "CARRIE", VIT = 60, STR = 20, CHA = 50, INT = 50};
        enemy_team = new Team { team = new List<Combatant> { carrie } };
        Battle(player_team, enemy_team, rules);
        // insert post-boss cutscene here

        player_team.semester += 1;
        rules.target_downed = true;
    }
}