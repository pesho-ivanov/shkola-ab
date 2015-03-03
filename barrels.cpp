#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

const double eps = 1e-9;  // pesho: експоненциалният запис е за предпочитане

// pesho: големи статични неща по-добре определяй глобално (отиват в heap, където default-ното ограничение е това, което пише в условието на задачата), а не в stack, за който обикновено ограничението е няколко MB.
int n, a[1024];
double delta[1024];

int LineCheck(int shift){
    int br = 0, sum2 = 0;
    for (int i=0; i<n; i++) {  // извърта всички бъчви
        sum2 += delta[(i + shift) % n];  // натрупва разликите
        if (abs(sum2) > eps)
           br++;  // ако не е нула сумата, то трябва да добавим още една бъчва, която да се опита да уравновеси съдържанието
    }

    return br;
}

int main() {
    // pesho: long <=> int при повечето текущи архитектури, 32bit
    int sum = 0;

    cin >> n;
    for (int i = 0; i < n; i++) {
        cin >> a[i];
        sum += a[i];
    }
    a[n] = a[0];

   // pesho: определяй променливите колкото може по-късно, а не в началото на програмата. така кашата е по-малка
    double TotalAvg = (double)sum / n;  // средно аритметично

    for (int i = 0; i < n; i++) {
        delta[i] = (double)a[i] - TotalAvg;  // разлики със средната стойност
    }

    int totalBr = 0, br;
    for (int j = 0; j < n; j++) {  // отмества началото и разглеждаме линейния случай
    	br = LineCheck(j);
        if (br != 0 && (totalBr == 0 || totalBr > br))
           totalBr = br; // намиране на минималния ненулев брой на отворени кранчета
    }

    cout << totalBr << endl;

    return 0;
}
