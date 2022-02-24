#include <bits/stdc++.h>

using namespace std;

struct Role {
    string skill;
    int level;
};

struct Project {
    string name;
    int D; //days
    int S; //score
    int B; //bestBefore
    vector <Role> roles;
};

struct Contributor {
    int id;
    string name;
    // redo to int?
    map<string, int> skill;
    int getLevel(string skillName) {
        auto it = skill.find(skillName);
        if (it == skill.end()) {
            return 0;
        }
        return it->second;
    }
};

struct Task {
    long long sum;

    int C, P;
    vector <Project> projects;
    vector <Contributor> contributors;
};

void execute(Task& task) {

}

Task readTask(string taskName) {
    freopen((taskName + ".in.txt").c_str(), "r", stdin);
    freopen((taskName + ".out.txt").c_str(), "w", stdout);
    Task t;
    cin >> t.C >> t.P;
    t.contributors.resize(t.C);
    int id = 0;
    for (auto& contributor : t.contributors) {
        cin >> contributor.name;
        int n;
        cin >> n;
        for (int i = 0; i < n; ++i) {
            string name;
            cin >> name;
            int level;
            cin >> level;
            contributor.skill[name] = level;
        }
        contributor.id = id++;
    }
    t.projects.resize(t.P);
    for (auto& project : t.projects) {
        cin >> project.name >> project.D >> project.S >> project.B;
        int R;
        cin >> R;
        project.roles.resize(R);
        for (auto& role : project.roles) {
            cin >> role.skill >> role.level;
        }
    }
    long long sum = 0;
    for (auto& prj : t.projects) {
        sum += prj.S;
    }
    t.sum = sum;
    return t;
}

int main() {
    //string taskName = "a_an_example";
    string taskName = "b_better_start_small";
    Task t = readTask(taskName);
    cerr << "Sum of projects: " << t.sum << endl;
    return 0;
}