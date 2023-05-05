#include <iostream>
#include <utility>
#include <bitset>
#include <set>
#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;
typedef pair<int, int> pii;
struct Position
{
    int x, y;
};

bool operator<(const Position &a, const Position &b)
{
    if (a.x == b.x)
        return a.y < b.y;
    return a.x < b.x;
}

//* <size of domain list, variable position>
typedef pair<int, Position> variable_data;

const int maxn = 50;
int table[maxn][maxn];
/**
 ** domains for each variable using bitset. if the i_th bit of a variable bitset is 1 then we can put the value (i) in it.
 ** Initially, all bits from one to n are equal to 1.
 ** The 0th bit is used to show whether the variable is filled or not.
 */
bitset<maxn> domains[maxn][maxn];
set<variable_data> var_set;
int n;
/**
 * @brief Update neighbors variable domain list (Forward Checking)
 * @return Constraint Propagation
 */
bool update_domains(set<variable_data> &neighbors_list, Position pos, int val)
{
    bool is_valid = true; //* Constraint Propagation
    bool is_deleted = var_set.erase({domains[pos.x][pos.y].count(), pos}) > 0;
    if (!is_deleted)
    {
        //! SHOULD NOT HAPPEN
        cout << "update: not deleted pos: " << pos.x << ' ' << pos.y << endl;
    }
    domains[pos.x][pos.y][0] = 1; //* is filled
    for (auto neighbor : neighbors_list)
    {
        auto cv_pos = neighbor.second;
        auto &cv_domain = domains[cv_pos.x][cv_pos.y];
        int domain_size = cv_domain.count(); //* or neighbor.first
        if (cv_domain[0])
        {
            //! SHOULD NOT HAPPEN
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
                //! SHOULD NOT HAPPEN
                cout << "update: not deleted: " << cv_pos.x << ' ' << cv_pos.y << endl;
            }
        }
        if (!domain_size)
            is_valid = false;
    }
    return is_valid; //* Constraint Propagation
}

/**
 * @brief Restore Forward Checking
 */
void restore_domains(set<variable_data> &neighbors_list, variable_data tmp, int val)
{
    domains[tmp.second.x][tmp.second.y][0] = 0; //* not filled
    if (tmp.first != int(domains[tmp.second.x][tmp.second.y].count()))
    {
        //! SHOULD NOT HAPPEN
        cout << "domain size mismatch!!!" << endl;
    }
    var_set.insert(tmp);
    for (auto old_neighbor : neighbors_list)
    {
        auto cv_pos = old_neighbor.second;
        auto &cv_domain = domains[cv_pos.x][cv_pos.y];
        int domain_size = cv_domain.count();
        if (cv_domain[0])
        {
            cout << "ERR Filled before!!!" << endl;
        }
        if (domain_size != old_neighbor.first)
        {
            cv_domain[val] = 1;
            bool is_deleted = var_set.erase({domain_size, cv_pos}) > 0;
            domain_size++;
            var_set.insert({domain_size, cv_pos});

            if (!is_deleted && domain_size != 1)
            {
                cout << "restore: not deleted: " << tmp.second.x << ' ' << tmp.second.y << endl;
            }
        }
    }
}

/**
 * @brief MRV Algorithm
 *
 * @return Position of minimum remaining values
 */
Position get_MRV()
{
    return var_set.begin()->second;
}

/**
 * @brief LCV Algorithm
 *
 * @param pos position
 * @param neighbors_list list of (pos) neighbors
 * @return list<pair<count, value>>
 */
vector<pii> get_LCV(Position &pos, set<variable_data> &neighbors_list)
{
    vector<pii> vals;
    auto &domain = domains[pos.x][pos.y];
    for (int val = domain._Find_first(); val <= n; val = domain._Find_next(val)) //* 1_based
    {
        int cnt = 0;
        for (auto neighbor : neighbors_list)
        {
            auto cv_pos = neighbor.second;
            auto &cv_domain = domains[cv_pos.x][cv_pos.y];
            if (cv_domain[val])
                cnt++;
        }
        vals.push_back({cnt, val});
    }
    sort(vals.begin(), vals.end());
    return vals;
}

/**
 * @brief Get the neighbors list object
 *
 * @param pos postion
 * @return set<variable_data>
 */
set<variable_data> get_neighbors_list(Position pos)
{
    set<variable_data> neighbors_list;
    for (int i = 0; i < n; i++)
    {
        if (i != pos.x && !domains[i][pos.y][0])
            neighbors_list.insert({domains[i][pos.y].count(), {i, pos.y}});
        if (i != pos.y && !domains[pos.x][i][0])
            neighbors_list.insert({domains[pos.x][i].count(), {pos.x, i}});
    }
    int sqrt_n = sqrt(n);
    int sub_grid_fit_bound_x = int(pos.x / sqrt_n) * sqrt_n;
    int sub_grid_fit_bound_y = int(pos.y / sqrt_n) * sqrt_n;
    for (int i = 0; i < sqrt_n; i++)
    {
        for (int j = 0; j < sqrt_n; j++)
        {
            int x = i + sub_grid_fit_bound_x, y = j + sub_grid_fit_bound_y;
            if (x != pos.x && y != pos.y && !domains[x][y][0])
            {
                auto cv_pos = Position({x, y});
                neighbors_list.insert({domains[cv_pos.x][cv_pos.y].count(), cv_pos});
            }
        }
    }
    return neighbors_list;
}

/**
 * @brief check the table is a valid sudoku
 *
 * @param is_pre pre check(check before solving)
 * @return true if table is valid
 * @return false
 */
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
    if (table_filled_counter == n * n)
    {
        cout << "Answer Found: " << endl;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                if (n > 9)
                    cout << (table[i][j] > 9 ? "" : "0") << table[i][j] << (j < n - 1 ? ", " : "");
                else
                    cout << table[i][j];
            }
            cout << endl;
        }
        cout << "is sudoku valid: " << (check_valid_sudoku(false) ? "True" : "False") << endl;
        exit(0);
        return;
    }

    if (!var_set.size()) //! SHOULD NOT HAPPEN
    {
        cout << "domains set is empty" << endl;
        return;
    }
    Position pos = get_MRV(); //* MRV Algorithm

    set<variable_data> neighbors_list = get_neighbors_list(pos);
    variable_data tmp = {domains[pos.x][pos.y].count(), pos};
    vector<pii> vals = get_LCV(pos, neighbors_list); //* LCV Algorithm
    for (auto val : vals)
    {
        int value = val.second;
        table[pos.x][pos.y] = value;
        if (update_domains(neighbors_list, pos, value)) //* Forward Checking & Constraint Propagation
        {
            solveCSP(table_filled_counter + 1);
        }
        restore_domains(neighbors_list, tmp, value); //* Restore Forward Checking
        table[pos.x][pos.y] = 0;
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
            var_set.insert({n, {i, j}});
        }

    for (int i = 0; i < c; i++)
    {
        int x, y, val;
        cin >> x >> y >> val;
        table[x][y] = val;
        set<variable_data> neighbors_list = get_neighbors_list({x, y});
        update_domains(neighbors_list, {x, y}, val);
    }
    if (check_valid_sudoku(true))
        solveCSP(c);
    cout << "Unsolvable CSP!";
    return 0;
}