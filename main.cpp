#include <iostream>
#include <utility>
#include <bitset>
#include <set>
#include <math.h>
#include <vector>

using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> feild_data;

const int maxn = 50;
int table[maxn][maxn];
//* domains for each variable using bitset. if the (i)th bit of a feild is 1 then we can put (i) in it.
bitset<maxn> domains[maxn][maxn];
//* <size of domain list, field position>
set<feild_data> dom_set;
int n;

bool update_domains(set<feild_data> &cv_list, pii pos, int val)
{
    bool is_valid = true;
    dom_set.erase({domains[pos.first][pos.second].count(), pos});
    for (auto field : cv_list)
    {
        auto cv_pos = field.second;
        auto &cv_domain = domains[cv_pos.first][cv_pos.second];
        int domain_size = cv_domain.count(); //* or field.first
        if (cv_domain[val])
        {
            cv_domain[val] = 0;
            dom_set.erase({domain_size, cv_pos});
            domain_size--;
            if (domain_size)
                dom_set.insert({domain_size, cv_pos});
        }
        if (!domain_size)
            is_valid = false;
    }
    return is_valid;
}

void restore_domains(set<feild_data> &cv_list, feild_data tmp, int val)
{
    dom_set.insert(tmp);
    for (auto old_field : cv_list)
    {
        auto cv_pos = old_field.second;
        auto &cv_domain = domains[cv_pos.first][cv_pos.second];
        int domain_size = cv_domain.count();
        if (domain_size != old_field.first)
        {
            cv_domain[val] = 1;
            dom_set.erase({domain_size, cv_pos});
            domain_size++;
            dom_set.insert({domain_size, cv_pos});
        }
    }
}

pii get_MRV()
{
    if (!dom_set.size())
        throw "domains set is empty";
    return dom_set.begin()->second;
}

set<pair<int, pii>> get_cv_list_backup(pii pos)
{
    set<pair<int, pii>> cv_list;
    for (int i = 0; i < n; i++)
    {
        if (i != pos.first)
            cv_list.insert({domains[i][pos.second].count(), {i, pos.second}});
        if (i != pos.second)
            cv_list.insert({domains[i][pos.second].count(), {pos.first, i}});
    }
    int sqrt_n = sqrt(n);
    int sub_grid_fit_bound_x = (pos.first / sqrt_n) * sqrt_n;
    int sub_grid_fit_bound_y = (pos.second / sqrt_n) * sqrt_n;
    for (int i = 0; i < sqrt_n; i++)
    {
        for (int j = 0; j < sqrt_n; j++)
        {
            if (i + sub_grid_fit_bound_x != pos.first && j + sub_grid_fit_bound_y != pos.second)
            {
                auto cv_pos = pair<int, int>(i + sub_grid_fit_bound_x, j + sub_grid_fit_bound_y);
                cv_list.insert({domains[cv_pos.first][cv_pos.second].count(), cv_pos});
            }
        }
    }
    return cv_list;
}

void solveCSP(int table_filled_counter = 0)
{
    if (table_filled_counter == n * n)
    {
        cout << "Answer Found: " << endl;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
                cout << table[i][j];
            cout << endl;
        }
        exit(0);
        return;
    }
    pii mrv = get_MRV();
    auto domain = domains[mrv.first][mrv.second];
    for (int val = domain._Find_first(); val <= n; val = domain._Find_next(val)) //* 1_based
    {
        table[mrv.first][mrv.second] = val;
        feild_data tmp = {domains[mrv.first][mrv.second].count(), mrv};
        set<feild_data> cv_list_backup = get_cv_list_backup(mrv);
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
            //todo
            dom_set.insert({n, {i, j}});
        }
    for (int i = 0; i < n; i++)
        for (int i = 0; i < c; i++)
        {
            int x, y, val;
            cin >> x >> y >> val;
            table[x][y] = val;
            // todo: set domains
        }
    solveCSP(c);
    cout << "Unsolvable CSP!";
    return 0;
}