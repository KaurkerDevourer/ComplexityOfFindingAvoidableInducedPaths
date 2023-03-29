#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include <unordered_set>
#include <unistd.h>
#include <mutex>
#include <thread>

using namespace std;

#define ll long long

int cnt1 = 0;
int cnt2 = 0;

vector<vector<int> > contract(vector<vector<int>> g, int y, int last) {
    unordered_set<int> in_y;
    for (auto to : g[y]) {
        in_y.insert(to);
    }
    for (auto to : g[last]) {
        if (!in_y.contains(to)) {
            g[y].push_back(to);
        }
    }
    g[last].clear();
    for (int i = 0; i < g.size(); i++) {
        for (int j = 0; j < g[i].size(); j++) {
            if (g[i][j] == last) {
                g[i][j] = y;
            }
        }
    }

    return g;
}

bool check_if_edge(unordered_set<int> &path, int wer, int id, vector<vector<int> >& g, int one_more = -1) {
    for (int i = 0; i < g[id].size(); i++) {
        int to = g[id][i];
        if (to == wer)
            continue;
        if (path.contains(to) || to == one_more) {
            return true;
        }
    }
    return false;
}

vector<int> extension(vector<int>& path, vector<vector<int> >& g, int last = -1) {
    assert(path.size() >= 2);
    unordered_set<int> flex;
    flex.reserve(path.size());
    for (int x : path) {
        flex.insert(x);
    }
    int p_1 = path[0];
    int p_k = path.back();
    vector<int> ans;
    ans.reserve(path.size() + 2);
    for (int i = 0; i < g[p_1].size(); i++) {
        if (flex.contains(g[p_1][i]) || check_if_edge(flex, p_1, g[p_1][i], g, last)) {
            continue;
        }
        for (int j = 0; j < g[p_k].size(); j++) {
            if (flex.contains(g[p_k][j]) || g[p_k][j] == g[p_1][i]) {
                continue;
            }
            if (check_if_edge(flex, p_k, g[p_k][j], g, g[p_1][i])) {
                continue;
            }
            bool amogus = false;
            for (auto to : g[g[p_k][j]]) {
                if (to == last) {
                    amogus = true;
                }
            }
            if (last != -1 && !amogus) {
                continue;
            }
            ans.push_back(g[p_1][i]);
            for (auto x : path) {
                ans.push_back(x);
            }
            ans.push_back(g[p_k][j]);
            return ans;
        }
    }
    return ans;
}

int total_cnt = 0;
int recursion1 = 0;
int max_recursion1 = 0;
int recursion2 = 0;
int max_recursion2 = 0;
int counter_good1 = 0;
int counter_good2 = 0;
int counter_bad = 0;
int counter_best = 0;
vector<pair<int,int> > vec;

vector<vector<int> > procedure2(vector<int>& path, vector<vector<int> > &g) {
    int total_sum = 0;
    for (auto v : g) {
        total_sum += v.size();
    }
   // cout << "TOTAL SUM: " << total_sum << endl;
    total_cnt++;
    int p_last = path.back();
// P' = p1..pk
    vector<int> P(path.size() - 2);
    for (int i = 0; i < P.size(); i++) {
        P[i] = path[i];
    }
    vector<vector<int> > S;
// S - the one-element sequence containing path with P'
    S.push_back(P);

// G - N[P_{k + 2}]    
    vector<vector<int> > g_without_p_last = g;
    unordered_set<int> to_delete;
    for (auto to : g[p_last])
        to_delete.insert(to);
    to_delete.insert(p_last);
    for (int i = 0; i < g_without_p_last.size(); i++) {
        if (to_delete.contains(i)) {
            g_without_p_last[i].clear();
        }
        for (int j = 0; j < g_without_p_last[i].size(); j++) {
            if (to_delete.contains(g_without_p_last[i][j])) {
                swap(g_without_p_last[i][j], g_without_p_last[i][g_without_p_last[i].size() - 1]);
                g_without_p_last[i].pop_back();
                j--;
            }
        }
    }

// first recursion
    int xorik = 0;
    vector<int> ext = extension(P, g_without_p_last);
    if (ext.size() != 0) {
        cnt1++;
       // cout << "Recursion 1" << endl;
        recursion2 = 0;
        recursion1++;
        max_recursion1 = max(max_recursion1, recursion1);
        xorik |= 1;
        vector<vector<int> > ret = procedure2(ext, g_without_p_last);
        for (const auto &x : ret) {
            S.push_back(x);
        }
    }

    vector<int>& Q = S.back();
    ext = extension(Q, g, p_last);
    if (ext.size() != 0) {
        cnt2++;
        // second recursion
            // Contracting edge y <-> p_{k+2}

            vector<vector<int> > G_contr = contract(g, Q.back(), p_last);
        
        //cout << "Recursion 2" << endl;
        recursion2++;
        recursion1 = 0;
        max_recursion2 = max(recursion2, max_recursion2);
        xorik |= 2;
        vector<vector<int> > ret = procedure2(ext, G_contr);
        for (const auto &x : ret) {
            S.push_back(x);
        }
    }
    if (xorik == 0) {
        counter_best++;
    } else if (xorik == 1) {
        counter_good1++;
    } else if (xorik == 2) {
        counter_good2++;
    } else {
        int sum = 0;
        int tot = g.size();
        for (auto x : g) {
            if (x.size() == 0) {
                tot--;
            }
            sum += x.size();
        }
        vec.emplace_back(tot, sum / 2);
        counter_bad++;
    }
   // cout << "EXIT" << endl;
    recursion2 = 0;
    return S;
// 
}

vector<vector<int> > procedure1(vector<int>& path, vector<vector<int>>& g) {
    vector<int> ext = extension(path, g);
    if (ext.size() == 0) {
        return {path};
    } else {
        return procedure2(ext, g);
    }
}

vector<pair<int,int> > get_all_pairs(vector<vector<int>>& g) {
    vector<pair<int,int> > pairs;
    for (int i = 0; i < g.size(); i++) {
        for (int j = 0; j < g[i].size(); j++) {
            int to = g[i][j];
            if (to <= i)
                continue;
            pairs.emplace_back(i, to);
        }
    }
    return pairs;
}


std::mutex print_mutex;

void compute(int left, int right) {
    for (int i = left; i < right; i++) {
        int k = 2;
        //cin >> k;
        p.clear();
        p.resize(k);
        total_cnt = 0;
        cnt1 = 0;
        cnt2 = 0;
        p[0] = pairs[i].first;
        p[1] = pairs[i].second;
        // for (int i = 0; i < k; i++) {
        //   cin >> p[i];
       // }
        vector<vector<int> > ans = procedure1(p, g);
        if (counter_bad > best_bad) {
            print_mutex.lock();
            std::cout << "BEST : " << counter_best << std::endl;
            std::cout << "GOOD1: " << counter_good1 << std::endl;
            std::cout << "GOOD2: " << counter_good2 << std::endl;
            std::cout << "BAD : " << counter_bad << std::endl;
            for (const auto& x : vec) {
                cout << x.first << ' ' << x.second << std::endl;
            }
            print_mutex.unlock();
            best_bad = counter_bad;
        }
        counter_best = 0;
        counter_good1 = 0;
        counter_good2 = 0;
        counter_bad = 0;
        vec.clear();

        if (total_cnt > maxn) {
            print_mutex.lock();
            cout << "START PATH: ";
            for (auto x : p) {
               cout << x << ' ';
            }
            cout << endl;
            cout << "SHIFT SIZE: ";
            cout << ans.size() << endl;
            cout << "SHIFTS:" << endl;
            /*for (auto& path : ans) {
                for (auto& x : path) {
                    cout << x << ' ';
                }
                cout << endl;
            }*/
            cout << "RECURSION_CNT: " << total_cnt << endl;
            cout << "CNT1: " << cnt1 << endl;
            cout << "CNT2: " << cnt2 << endl;
            print_mutex.unlock();
            maxn = max(maxn, total_cnt);
        }
    }
}

int main() {
    vector<vector<int> > g;
    vector<int> p;
    int n = 10;
    //cin >> n;
    if (n == 6) {
        g.resize(6);
        g[0].push_back(1);
        g[1].push_back(0);
        g[0].push_back(4);
        g[4].push_back(0);
        g[0].push_back(3);
        g[3].push_back(0);
        g[1].push_back(2);
        g[2].push_back(1);
        g[1].push_back(5);
        g[5].push_back(1);
        g[4].push_back(5);
        g[5].push_back(4);
        g[2].push_back(3);
        g[3].push_back(2);
    }
    /*
    g.resize(n); // Цикл с маленькими циклами через t
    for (int i = 0; i < n; i++) {
        g[i].push_back((i+1)%n);
        g[(i+1)%n].push_back(i);
    }
    int t;
    cin >> t;
    for (int i = 0; i < n; i++) {
        g[i].push_back((i+t)%n);
        g[(i+t)%n].push_back(i);
    }*/
    /*g.resize(n); simple cicle
    p.push_back(1);
    p.push_back(0);
    for (int i = 0; i < n; i++) {
        g[i].push_back((i+1)%n);
        g[(i+1)%n].push_back(i);
    }*/
    ll amogus = (1 << n);  // cube
    g.resize(amogus);
    for (int i = 0; i < amogus; i++) {
        for (int j = 0; j < n; j++) {
            g[i].push_back(i^(1 << j));
        }
    };
    //cout << "GRAPH SIZE: " << n * n << endl;
    /*g.resize(n * n);
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n - 1; i++) {
            g[j * n + i].push_back(j * n + i + 1); // сетка P_k * P_k
            g[j * n + i + 1].push_back(j * n + i); // по горизонтали заполняем
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1; j++) {
            g[j * n + i].push_back((j + 1) * n + i); // сетка P_k * P_k
            g[(j + 1) * n + i].push_back(j * n + i); // по вертикали заполняем
        }
    } */

    vector<pair<int,int> > pairs = get_all_pairs(g);
    // for https://csacademy.com/app/graph_editor/
    /* for (int i = 0; i < g.size(); i++) {
        cout << i << endl; 
    }

    for (int i = 0; i < pairs.size() i++) {
        cout << pairs[i].first << ' ' << pairs[i].second << endl;
    } */

    int maxn = 0;
    int now = 0;
    int ten_prc = pairs.size() / 10;
    int best_bad = 0;
    // i * 100 * now > pairs.size()
    std::vector<std::thread> workers;
    for (int i = 0; i < 10; i++) {
        workers.emplace_back(compute((pair.size() / 10) * i, (pair.size()/10) * (i + 1)));
    }
    for (int i = 0; i < 10; i++) {
        workers[i].join();
    }
    for (int i = 0; i < pairs.size(); i++) {
        if (i >= ten_prc * now) {
            cout << "prc: " << now * 10 << ' ' << i << ' ' << pairs.size() << endl;
            now++;
        }
        int k = 2;
        //cin >> k;
        p.clear();
        p.resize(k);
        total_cnt = 0;
        cnt1 = 0;
        cnt2 = 0;
        p[0] = pairs[i].first;
        p[1] = pairs[i].second;
        // for (int i = 0; i < k; i++) {
        //   cin >> p[i];
       // }
        vector<vector<int> > ans = procedure1(p, g);
        if (counter_bad > best_bad) {
            std::cout << "BEST : " << counter_best << std::endl;
            std::cout << "GOOD1: " << counter_good1 << std::endl;
            std::cout << "GOOD2: " << counter_good2 << std::endl;
            std::cout << "BAD : " << counter_bad << std::endl;
            for (const auto& x : vec) {
                cout << x.first << ' ' << x.second << std::endl;
            }
            best_bad = counter_bad;
        }
        counter_best = 0;
        counter_good1 = 0;
        counter_good2 = 0;
        counter_bad = 0;
        vec.clear();

        if (total_cnt > maxn) {
            cout << "START PATH: ";
            for (auto x : p) {
               cout << x << ' ';
            }
            cout << endl;
            cout << "SHIFT SIZE: ";
            cout << ans.size() << endl;
            cout << "SHIFTS:" << endl;
            /*for (auto& path : ans) {
                for (auto& x : path) {
                    cout << x << ' ';
                }
                cout << endl;
            }*/
            cout << "RECURSION_CNT: " << total_cnt << endl;
            cout << "CNT1: " << cnt1 << endl;
            cout << "CNT2: " << cnt2 << endl;
            maxn = max(maxn, total_cnt);
        }
    }
    cout << "MAX: " << maxn << endl;
    cout << max_recursion1 << endl;
    cout << max_recursion2 << endl;
}

