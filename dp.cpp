#include <iostream>
#include <vector>

using namespace std;


int main() {
    int n;
    cin >> n;
    vector<vector<long long> > dp(n + 1, vector<long long>(2));
    dp[0][0] = 1;
    dp[0][1] = 1;
    dp[1][0] = 1;
    dp[1][1] = 1;
    for (int i = 2; i < n + 1; i++) {
        dp[i][0] += dp[i - 2][0];
        dp[i][0] += dp[i - 2][1];
        dp[i][1] += dp[i - 1][0];
    }
    cout << dp[n][0] + dp[n][1] << endl;
}
