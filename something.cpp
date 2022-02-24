#include <bits/stdc++.h>

using namespace std;

struct Role {
    string skill;
    int level;
};

struct Project {
    int id;
    string name;
    int D; //days
    int S; //score
    int B; //bestBefore
    vector <Role> roles;
    // helper
    set<string> skills;
};

struct Contributor {
    int id;
    string name;
    // redo to int?
    map<string, int> skill;
    [[nodiscard]] int getLevel(const string& skillName) const {
        auto it = skill.find(skillName);
        if (it == skill.end()) {
            return 0;
        }
        return it->second;
    }
    bool operator <(const Contributor& rhs) const {
        return this->id < rhs.id;
    }
};

struct Task {
    long long sum;

    int C, P;
    vector <Project> projects;
    vector <Contributor> contributors;
};

struct AssignedTask {
    string projectName;
    vector<string> contributorNames;
};

struct Assignment {
    vector <AssignedTask> assigned;
};

Assignment execute(Task& task) {
    set <int> projects;
    for (int i = 0; i < task.P; ++i) {
        projects.insert(i);
    }
    map<int, vector<Contributor>> guysFree;
    set<Contributor> contributorsAvailable;
    for (auto&& contr : task.contributors) {
        contributorsAvailable.insert(contr);
    }
    Assignment result;
    for (int curTime = 0; ;) {
        vector <Project> currentProjects;
        currentProjects.reserve(projects.size());
        for (int id : projects) {
            currentProjects.push_back(task.projects[id]);
        }
        auto getCost = [&](const Project& project) {
            return max(0, project.S - max(0, curTime + project.D - project.B));
        };
        sort(currentProjects.begin(), currentProjects.end(), [&](const Project& lhs, const Project& rhs) {
//            int c1 = getCost(lhs);
//            int c2 = getCost(rhs);
//            return c1 < c2 || c1 == c2 && lhs.id < rhs.id;
            return lhs.B < rhs.B;
        });
        map<string, map<int, set<int>>> skillLevelContributorId;
        auto&& pop_guy = [&](const Contributor& contributor) {
            for (auto& x : contributor.skill) {
                skillLevelContributorId[x.first][x.second].erase(contributor.id);
            }
        };

        auto&& add_guy = [&](const Contributor& contributor) {
            for (auto& x : contributor.skill) {
                skillLevelContributorId[x.first][x.second].insert(contributor.id);
            }
        };
        for (auto&& x : contributorsAvailable) {
            add_guy(x);
        }


        for (auto& project : currentProjects) {
            if (getCost(project) == 0) {
                continue;
            }
            vector <int> contributors;
            set<int> contributorsSet;
            bool projCanBeDone = true;
            map<string, int> mentoredRoles;

            for (const auto& x : project.roles) {
                bool assigned = false;
                int start = x.level;
                auto mentor = mentoredRoles.find(x.skill);
                if (mentor != mentoredRoles.end()) {
                    if (start <= mentor->second) {
                        --start;
                    }
                }
                auto begin = skillLevelContributorId[x.skill].lower_bound(start);
                auto end = skillLevelContributorId[x.skill].end();
                for (auto it = begin; it != end; ++it) {
                    for (auto contributorId : it->second) {
                        if (contributorsSet.count(contributorId)) {
                            // skip assigned here
                            continue;
                        }
                        contributors.push_back(contributorId);
                        contributorsSet.insert(contributorId);
                        auto contrIt = contributorsAvailable.find({contributorId});
                        for (auto& lr : contrIt->skill) {
                            auto it2 = project.skills.find(lr.first);
                            if (it2 != project.skills.end()) {
                                auto iter = mentoredRoles.find(lr.first);
                                int val = lr.second;
                                if (iter != mentoredRoles.end()) {
                                    val = max(val, iter->second);
                                }
                                mentoredRoles[lr.first] = val;
                            }
                        }
                        assigned = true;
                        break;
                    }
                    if (assigned) {
                        break;
                    }
                }
                if (!assigned) {
                    projCanBeDone = false;
                    break;
                }
            }
            if (!projCanBeDone) {
                continue;
            }

            // assign guys to task
            result.assigned.push_back({});
            AssignedTask& assigned = result.assigned.back();
            assigned.projectName = project.name;
            for (int i = 0; i < contributors.size(); ++i) {
                auto it = contributorsAvailable.find({contributors[i]});
                Contributor c = *it;
                pop_guy(c);
                contributorsAvailable.erase(it);
                if (c.getLevel(project.roles[i].skill) <= project.roles[i].level) {
                    c.skill[project.roles[i].skill]++;
                }
                guysFree[curTime + project.D].push_back(c);
                assigned.contributorNames.push_back(c.name);
            }
            projects.erase(project.id);
        }
        auto it = guysFree.begin();
        if (it == guysFree.end()) {
            break;
        }
        for (auto&& contributor : it->second) {
            add_guy(contributor);
            contributorsAvailable.insert(contributor);
        }
        curTime = it->first;
        guysFree.erase(it);
    }
    return result;
}

Task readTask(string taskName) {
    freopen((taskName + ".in.txt").c_str(), "r", stdin);
    freopen((taskName + ".out.txt").c_str(), "w", stdout);
    Task t;
    cin >> t.C >> t.P;
    t.contributors.resize(t.C);
    int id = 0;
    set<string> allRoles;
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
            allRoles.insert(name);
        }
        contributor.id = id++;
    }
    for (auto& contributor : t.contributors) {
        for (auto& x : allRoles) {
            contributor.skill[x];
        }
    }
    t.projects.resize(t.P);
    int prjId = 0;
    for (auto& project : t.projects) {
        project.id = prjId++;
        cin >> project.name >> project.D >> project.S >> project.B;
        int R;
        cin >> R;
        project.roles.resize(R);
        for (auto& role : project.roles) {
            cin >> role.skill >> role.level;
            project.skills.insert(role.skill);
        }
        sort(project.roles.begin(), project.roles.end(), [](const Role& lhs, const Role& rhs) {
           return lhs.skill < rhs.skill || (lhs.skill == rhs.skill && lhs.level > rhs.level);
        });
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
    //string taskName = "b_better_start_small";
    //string taskName = "c_collaboration";
    //string taskName = "d_dense_schedule";
    //string taskName = "e_exceptional_skills";
    string taskName = "f_find_great_mentors";
    Task t = readTask(taskName);
    cerr << "Sum of projects: " << t.sum << endl;
    auto res = execute(t);
    cout << res.assigned.size() << endl;
    for (auto& assignedTask : res.assigned) {
        cout << assignedTask.projectName << endl;
        for (auto& name : assignedTask.contributorNames) {
            cout << name << " ";
        }
        cout << endl;
    }
    return 0;
}