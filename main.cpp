#include <iostream>
#include <utility>
#include <bitset>
#include <set>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> feild_data;

const int maxn = 50;
int table[maxn][maxn];
//* domains for each variable using bitset. if the (i)th bit of a feild is 1 then we can put (i) in it.
bitset<maxn> domains[maxn][maxn];
//* <size of domain list, field position>
set<feild_data> var_set;
int n;

bool update_domains(set<feild_data> &cv_list, pii pos, int val)
{
    bool is_valid = true;
    bool is_deleted = var_set.erase({domains[pos.first][pos.second].count(), pos}) > 0;
    if (!is_deleted)
    {
        cout << "update: not deleted pos: " << pos.first << ' ' << pos.second << endl;
    }
    domains[pos.first][pos.second][0] = 1; //* is filled
    for (auto field : cv_list)
    {
        auto cv_pos = field.second;
        auto &cv_domain = domains[cv_pos.first][cv_pos.second];
        int domain_size = cv_domain.count(); //* or field.first
        if (cv_domain[0])
        {
            cout << "ERR Filled before!!!" << endl;
        }
        if (cv_domain[val])
        {
            cv_domain[val] = 0;
            bool is_deleted = var_set.erase({domain_size, cv_pos}) > 0;
            domain_size--;
            if (domain_size)
                var_set.insert({domain_size, cv_pos});
            if (!is_deleted)
            {
                cout << "update: not deleted: " << cv_pos.first << ' ' << cv_pos.second << endl;
            }
        }
        if (!domain_size)
            is_valid = false;
    }
    return is_valid;
}

void restore_domains(set<feild_data> &cv_list, feild_data tmp, int val)
{
    domains[tmp.second.first][tmp.second.second][0] = 0; //* not filled

    if (tmp.first != int(domains[tmp.second.first][tmp.second.second].count()))
    {
        cout << "domain size mismatch!!!" << endl;
    }
    var_set.insert(tmp);
    for (auto old_field : cv_list)
    {
        auto cv_pos = old_field.second;
        auto &cv_domain = domains[cv_pos.first][cv_pos.second];
        int domain_size = cv_domain.count();
        if (cv_domain[0])
        {
            cout << "ERR Filled before!!!" << endl;
        }
        if (domain_size != old_field.first)
        {
            cv_domain[val] = 1;
            bool is_deleted = var_set.erase({domain_size, cv_pos}) > 0;
            domain_size++;
            var_set.insert({domain_size, cv_pos});

            if (!is_deleted && domain_size != 1)
            {
                cout << "restore: not deleted: " << tmp.second.first << ' ' << tmp.second.second << endl;
            }
        }
    }
}

pii get_MRV()
{
    return var_set.begin()->second;
}

vector<pii> get_LCV(pii &pos, set<feild_data> &cv_list)
{
    vector<pii> vals;
    auto &domain = domains[pos.first][pos.second];                               //*
    for (int val = domain._Find_first(); val <= n; val = domain._Find_next(val)) //* 1_based
    {
        int cnt = 0;
        for (auto neighbor : cv_list)
        {
            auto cv_pos = neighbor.second;
            auto &cv_domain = domains[cv_pos.first][cv_pos.second];
            if (cv_domain[val])
                cnt++;
        }
        vals.push_back({cnt, val});
    }
    sort(vals.begin(), vals.end());
    return vals;
}

set<feild_data> get_cv_list_backup(pii pos)
{
    set<feild_data> cv_list;
    for (int i = 0; i < n; i++)
    {
        if (i != pos.first && !domains[i][pos.second][0])
            cv_list.insert({domains[i][pos.second].count(), {i, pos.second}});
        if (i != pos.second && !domains[pos.first][i][0])
            cv_list.insert({domains[pos.first][i].count(), {pos.first, i}});
    }
    int sqrt_n = sqrt(n);
    int sub_grid_fit_bound_x = int(pos.first / sqrt_n) * sqrt_n;
    int sub_grid_fit_bound_y = int(pos.second / sqrt_n) * sqrt_n;
    for (int i = 0; i < sqrt_n; i++)
    {
        for (int j = 0; j < sqrt_n; j++)
        {
            int x = i + sub_grid_fit_bound_x, y = j + sub_grid_fit_bound_y;
            if (x != pos.first && y != pos.second && !domains[x][y][0])
            {
                auto cv_pos = pair<int, int>(x, y);
                cv_list.insert({domains[cv_pos.first][cv_pos.second].count(), cv_pos});
            }
        }
    }
    return cv_list;
}

bool check_valid_sudoku(bool is_pre = false)
{
    for (int i = 0; i < n; i++)
    {
        set<int> s1, s2;
        int c1 = 0, c2 = 0;
        for (int j = 0; j < n; j++)
        {
            if (!table[i][j] && !is_pre)
            {
                //* sudoku should not have 0 when it is completed
                return false;
            }
            if (table[i][j])
                s1.insert(table[i][j]), c1++;

            if (!table[j][i] && !is_pre)
            {
                //* sudoku should not have 0 when it is completed
                return false;
            }
            if (table[j][i])
                s2.insert(table[j][i]), c2++;
        }
        if (!is_pre && (int(s1.size()) != n || int(s2.size()) != n))
            return false;
        if (is_pre && (int(s1.size()) != c1 || int(s2.size()) != c2))
            return false;
    }
    int sqrt_n = sqrt(n);
    for (int sub_grid_counter = 0; sub_grid_counter < n; sub_grid_counter++)
    {
        int sub_grid_fit_bound_x = (sub_grid_counter % sqrt_n) * sqrt_n;
        int sub_grid_fit_bound_y = (sub_grid_counter / sqrt_n) * sqrt_n;
        int cnt = 0;
        set<int> s;
        for (int i = 0; i < sqrt_n; i++)
        {
            for (int j = 0; j < sqrt_n; j++)
            {
                int x = i + sub_grid_fit_bound_x, y = j + sub_grid_fit_bound_y;
                if (!table[x][y] && !is_pre)
                {
                    //* sudoku should not have 0 when it is completed
                    return false;
                }
                if (table[x][y])
                    s.insert(table[x][y]), cnt++;
            }
        }
        if ((!is_pre && int(s.size()) != n) || (is_pre && int(s.size()) != cnt))
            return false;
    }
    return true;
}

void solveCSP(int table_filled_counter = 0)
{
    // if (table_filled_counter > 480)
    // {
    //     cout << table_filled_counter << "   " << var_set.size() << endl;
    // }
    if (table_filled_counter == n * n)
    {
        cout << "Answer Found: " << endl;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (n > 9)
                {
                    cout << (table[i][j] > 9 ? "" : "0") << table[i][j] << (j < n - 1 ? ", " : "");
                }
                else
                    cout << table[i][j];
            }
            cout << endl;
        }
        cout << "is sudoku valid: " << (check_valid_sudoku(false) ? "true" : "false") << endl;
        ;
        exit(0);
        return;
    }
    if (!var_set.size())
    {
        cout << "domains set is empty" << endl;
        return;
    }
    pii mrv = get_MRV();
    // cout << table_filled_counter << endl;
    // for (auto i : var_set)
    //     cout << i.first << "  " << i.second.first << ' ' << i.second.second << endl;
    // cout << "---" << endl;
    //auto domain = domains[mrv.first][mrv.second];
    set<feild_data> cv_list_backup = get_cv_list_backup(mrv);
    feild_data tmp = {domains[mrv.first][mrv.second].count(), mrv};

    vector<pii> vals = get_LCV(mrv, cv_list_backup);
    //*for (int val = domain._Find_first(); val <= n; val = domain._Find_next(val)) //* 1_based
    for (auto value : vals)
    {
        int val = value.second;
        // cout << mrv.first << ' ' << mrv.second << " - " << val  << endl;
        table[mrv.first][mrv.second] = val;
        // cout << "-> " << table_filled_counter << "   " << mrv.first << ' ' << mrv.second << endl;
        // for (auto i : cv_list_backup)
        //     cout << i.first << "  " << i.second.first << ' ' << i.second.second << endl;
        // cout << "---" << endl;
        if (update_domains(cv_list_backup, mrv, val))
        {
            solveCSP(table_filled_counter + 1);
        }
        restore_domains(cv_list_backup, tmp, val);
        table[mrv.first][mrv.second] = 0;
    }
}

int main()
{
    int c;
    cin >> n >> c;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            for (int k = 1; k <= n; k++)
                domains[i][j][k] = 1;
            // todo
            var_set.insert({n, {i, j}});
        }

    for (int i = 0; i < c; i++)
    {
        int x, y, val;
        cin >> x >> y >> val;
        table[x][y] = val;
        set<feild_data> cv_list = get_cv_list_backup({x, y});
        update_domains(cv_list, {x, y}, val);
    }
    if (check_valid_sudoku(true))
        solveCSP(c);
    cout << "Unsolvable CSP!";
    return 0;
}