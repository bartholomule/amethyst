#include "graphics/rgbcolor.hpp"
#include "test_framework/testinclude.hpp"

#include <iostream>
using namespace std;

#define ERROR_TEXT(text) ((++error_count), text)

template <class T>
ostream& operator<<(ostream& o, const amethyst::rgbcolor<T>& c)
{
    o << "<" << c.r() << ", " << c.g() << ", " << c.b() << ">";
    return o;
}

int main(int, char**)
{
    int error_count = 0;
    const char* passed = "Passed";
    const char* failed = "FAILED!";

    typedef amethyst::rgbcolor<int> color;

    color blah01;

    cout << "-------------------------------------" << endl;
    cout << "Testing rgbcolor member functions/ops" << endl;
    cout << "-------------------------------------" << endl;

    TEST_RESULT_NAMED("to see if get/set functions work",
                      ((blah01.set_r(1), (blah01.r() == 1)) && // single r
                       (blah01.set_g(2), (blah01.g() == 2)) && // single g
                       (blah01.set_b(3), (blah01.b() == 3)) && // single b
                       (blah01.set(3, 2, 1), ((blah01.r() == 3) && // multiple
                                              (blah01.g() == 2) &&
                                              (blah01.b() == 1)))
                      ),
                      passed,
                      ERROR_TEXT(failed));

    // Check both const/non-const [] operators
    TEST_RESULT_NAMED("to see if operator[] works",
                      (((blah01[0] = 1), (((const color)blah01)[0] == 1)) &&
                       ((blah01[1] = 2), (((const color)blah01)[1] == 2)) &&
                       ((blah01[2] = 3), (((const color)blah01)[2] == 3))
                      ),
                      passed,
                      ERROR_TEXT(failed));

    color blah02(blah01);
    TEST_RESULT_NAMED("to see if the copy constructor works",
                      ((blah02[0] == 1) && (blah02[1] == 2) && (blah02[2] == 3)),
                      passed,
                      ERROR_TEXT(failed));

    color blah03; blah03 = blah01;
    TEST_RESULT_NAMED("to see if the assignment operator works",
                      ((blah03[0] == 1) && (blah03[1] == 2) && (blah03[2] == 3)),
                      passed,
                      ERROR_TEXT(failed));

    //
    // NOTE: These tests rely on integer truncation (always rounding down).
    //
    color blah04 = blah01; blah04 *= 2;
    TEST_RESULT_NAMED("to see if operator *= (specific) works",
                      ((blah04[0] == 2) && (blah04[1] == 4) && (blah04[2] == 6)),
                      passed,
                      ERROR_TEXT(failed));

    color blah05 = blah01; blah05 *= 2.5;
    TEST_RESULT_NAMED("to see if operator *= (general) works",
                      ((blah05[0] == 2) && (blah05[1] == 5) && (blah05[2] == 7)),
                      passed,
                      ERROR_TEXT(failed));

    color blah06 = blah04; blah06 /= 2;
    TEST_RESULT_NAMED("to see if operator /= (specific) works",
                      ((blah06[0] == 1) && (blah06[1] == 2) && (blah06[2] == 3)),
                      passed,
                      ERROR_TEXT(failed));

    color blah07 = blah05; blah07 /= 2.5;
    TEST_RESULT_NAMED("to see if operator /= (general) works",
                      ((blah07[0] == 0) && (blah07[1] == 2) && (blah07[2] == 2)),
                      passed,
                      ERROR_TEXT(failed));

    color blah08 = blah05; blah08 += blah04;
    TEST_RESULT_NAMED("to see if operator += works",
                      ((blah08[0] == 4) && (blah08[1] == 9) && (blah08[2] == 13)),
                      passed,
                      ERROR_TEXT(failed));

    color blah09 = blah05; blah09 -= blah04;
    TEST_RESULT_NAMED("to see if operator -= works",
                      ((blah09[0] == 0) && (blah09[1] == 1) && (blah09[2] == 1)),
                      passed,
                      ERROR_TEXT(failed));

    cout << "-------------------------------------" << endl;
    cout << "Testing rgbcolor global functions/ops" << endl;
    cout << "-------------------------------------" << endl;

    color blah10 = 2 * blah01;
    TEST_RESULT_NAMED("to see if global binary operator * (specific) works",
                      ((blah10[0] == 2) && (blah10[1] == 4) && (blah10[2] == 6)),
                      passed,
                      ERROR_TEXT(failed));

    color blah11 = 2.5 * blah01;
    TEST_RESULT_NAMED("to see if global binary operator * (general) works",
                      ((blah11[0] == 2) && (blah11[1] == 5) && (blah11[2] == 7)),
                      passed,
                      ERROR_TEXT(failed));

    color blah12 = blah10 + blah11;
    TEST_RESULT_NAMED("to see if global binary operator + works",
                      ((blah12[0] == 4) && (blah12[1] == 9) && (blah12[2] == 13)),
                      passed,
                      ERROR_TEXT(failed));

    color blah13 = blah10 - blah11;
    TEST_RESULT_NAMED("to see if global binary operator - works",
                      ((blah13[0] == 0) && (blah13[1] == -1) && (blah13[2] == -1)),
                      passed,
                      ERROR_TEXT(failed));

    color blah14 = -blah12;
    TEST_RESULT_NAMED("to see if global unary operator - works",
                      ((blah14[0] == -4) && (blah14[1] == -9) && (blah14[2] == -13)),
                      passed,
                      ERROR_TEXT(failed));

    int barfola[] = { 0, 1, 2,
                      3, 4, 5,
                      6, 7, 8,
                      9, 10, 11 };

    color* foo = reinterpret_cast<color*>(&barfola);
    cout << "------------------------" << endl;
    cout << "Testing reinterpretation" << endl;
    cout << "------------------------" << endl;

    TEST_RESULT_NAMED("to see if reverse reinterpretation works as required",
                      (((foo[0][0] == 0) && (foo[0][1] == 1) && (foo[0][2] == 2)) &&
                       ((foo[1][0] == 3) && (foo[1][1] == 4) && (foo[1][2] == 5)) &&
                       ((foo[2][0] == 6) && (foo[2][1] == 7) && (foo[2][2] == 8)) &&
                       ((foo[3][0] == 9) && (foo[3][1] == 10) && (foo[3][2] == 11))),
                      passed,
                      ERROR_TEXT(failed));


    color bar[4];

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            bar[i][j] = i * 3 + j + 10;
        }
    }

    int* bar_rp = reinterpret_cast<int*>(bar);

    TEST_RESULT_NAMED("to see if reinterpretation works as required",
                      ((bar_rp[0] = 10) && (bar_rp[1] = 11) &&
                       (bar_rp[2] = 12) && (bar_rp[3] = 13) &&
                       (bar_rp[4] = 14) && (bar_rp[5] = 15) &&
                       (bar_rp[6] = 16) && (bar_rp[7] = 17) &&
                       (bar_rp[8] = 18) && (bar_rp[9] = 19) &&
                       (bar_rp[10] = 20) && (bar_rp[11] = 21)),
                      passed,
                      ERROR_TEXT(failed));

    if (!error_count)
    {

        cout << "----------------------------------" << endl;
        cout << "*** All rgbcolor tests passed. ***" << endl;
        cout << "----------------------------------" << endl;
        return 0;
    }
    else
    {
        cout << "---------------------------------" << endl;
        cout << "ERROR: Failed " << error_count << " rgbcolor tests." << endl;
        cout << "---------------------------------" << endl;
        return 2;
    }

}

//(templated) class rgbcolor
