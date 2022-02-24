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
    };
    
    struct Contributor {
        int id;
        string name;
        // redo to int?
        map<string, int> skill;
        int getLevel(const string& skillName) const {
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
                return max(0, project.S - max(0, curTime - project.B));
            };
            sort(currentProjects.begin(), currentProjects.end(), [&](const Project& lhs, const Project& rhs) {
                int c1 = getCost(lhs);
                int c2 = getCost(rhs);
                return c1 < c2 || c1 == c2 && lhs.id < rhs.id;
            });
            for (auto& project : currentProjects) {
                if (getCost(project) == 0) {
                    continue;
                }
                vector <int> contributors;
                set<int> contributorsSet;
                bool projCanBeDone = true;
                for (auto& x : project.roles) {
                    bool assigned = false;
                    for (auto &contributor: contributorsAvailable) {
                        if (contributorsSet.count(contributor.id)) {
                            // skip assigned here
                            continue;
                        }
                        if (contributor.getLevel(x.skill) >= x.level) {
                            assigned = true;
                            contributors.push_back(contributor.id);
                            contributorsSet.insert(contributor.id);
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
                AssignedTask& assigned = result.assigned.emplace_back();
                assigned.projectName = project.name;
                for (int i = 0; i < contributors.size(); ++i) {
                    auto it = contributorsAvailable.find({contributors[i]});
                    Contributor c = *it;
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
            for (auto&& it2 : it->second) {
                contributorsAvailable.insert(std::move(it2));
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
        int prjId = 0;
        for (auto& project : t.projects) {
            project.id = prjId++;
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
        //string taskName = "b_better_start_small";
        //string taskName = "c_collaboration";
        //string taskName = "d_dense_schedule";
    //    string taskName = "e_exceptional_skills";
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