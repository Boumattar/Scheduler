#include "schedule_funcs.h"
#include <memory>

#include "gtest/gtest.h"
using namespace std;

class GoldenTest : public ::testing::Test {

public:
    GoldenTest()
    {

        /// <summary>
        ///  0 : Ant
        ///  1 : Maven
        ///  2 : C
        ///  3 : Lisp
        ///  4 : Ruby
        ///  5 : J
        ///  6 : MVC
        ///  7 : Qt
        ///  8 : Unix
        ///  9 : OO
        /// </summary>

        // 5 days with 5 slots each day
        timePlan = TimePlan { 5, 5 };

        // Class curricula
        map<int, int> classACurr = { { 0, 5 }, { 1, 7 }, { 2, 2 }, { 3, 1 }, { 4, 3 }, { 9, 6 }, { 8, 1 } };
        map<int, int> classBCurr = { { 1, 2 }, { 2, 2 }, { 5, 5 }, { 6, 5 }, { 7, 5 }, { 8, 5 }, { 9, 1 } };
        map<int, int> classCCurr = { { 3, 4 }, { 5, 4 }, { 6, 4 }, { 7, 6 }, { 8, 7 } };

        // Classes
        A = Class { "A", classACurr };
        B = Class { "B", classBCurr };
        C = Class { "C", classCCurr };

        //Sections
        sectionA_1 = Section { &A, "sectionA_2" };
        sectionA_2 = Section { &A, "sectionA_2" };
        sectionB_1 = Section { &B, "sectionB_1" };
        sectionB_2 = Section { &B, "sectionB_2" };
        sectionC_1 = Section { &C, "sectionC_1" };
        sectionC_2 = Section { &C, "sectionC_2" };

        //Teacher time availability
        vector<bool> NemlTime = { 1, 1, 0, 1, 1,
            1, 1, 1, 0, 1,
            1, 1, 1, 1, 1,
            1, 0, 0, 1, 1,
            1, 1, 1, 0, 1 };

        vector<bool> SheTime = { 1, 0, 1, 0, 0,
            0, 0, 1, 1, 0,
            1, 0, 0, 1, 1,
            1, 1, 1, 0, 0,
            1, 1, 0, 1, 1 };

        vector<bool> GrbTime = { 1, 1, 1, 0, 1,
            1, 0, 0, 1, 1,
            1, 0, 1, 0, 0,
            0, 0, 0, 0, 1,
            1, 1, 0, 1, 1 };

        vector<bool> HmmTime = { 1, 1, 1, 0, 0,
            0, 0, 1, 1, 1,
            0, 1, 1, 1, 1,
            1, 1, 0, 0, 1,
            0, 0, 1, 0, 0 };

        vector<bool> KafTime = { 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1,
            1, 1, 1, 1, 1,
            1, 1, 1, 1, 0,
            1, 1, 1, 1, 1 };

        vector<bool> MehdTime = { 0, 0, 0, 1, 1,
            1, 1, 0, 0, 0,
            0, 0, 1, 0, 1,
            0, 0, 1, 1, 0,
            1, 0, 1, 0, 0 };

        vector<bool> BelTime = { 1, 1, 0, 0, 1,
            1, 0, 0, 0, 1,
            1, 1, 0, 1, 0,
            0, 1, 1, 1, 1,
            0, 0, 1, 0, 0 };

        vector<bool> LusTime = { 0, 1, 1, 1, 0,
            1, 1, 1, 1, 0,
            1, 1, 1, 1, 1,
            1, 1, 0, 1, 1,
            1, 1, 0, 1, 1 };

        vector<bool> NedTime = { 0, 0, 1, 1, 0,
            0, 1, 1, 1, 1,
            0, 1, 0, 0, 0,
            1, 0, 1, 0, 0,
            0, 0, 0, 1, 1 };

        vector<bool> FergTime = { 0, 0, 0, 1, 1,
            0, 1, 0, 0, 0,
            0, 0, 0, 0, 0,
            0, 1, 1, 1, 1,
            0, 1, 1, 1, 0 };

        //Teachers
        Neml = { "Neml", NemlTime };
        She = { "She", SheTime };
        Grb = { "Grb", GrbTime };
        Hmm = { "Hmm", HmmTime };
        Kaf = { "Kaf", KafTime };
        Mehd = { "Mehd", MehdTime };
        Bel = { "Bel", BelTime };
        Lus = { "Lus", LusTime };
        Ned = { "Ned", NedTime };
        Ferg = { "Ned", FergTime };

        //Teacher subject assignments
        teacherAssignments = {
            { &sectionA_1, { { 0, &Neml }, { 1, &Grb }, { 2, &Kaf }, { 3, &Ferg }, { 4, &Ferg }, { 9, &She }, { 8, &Ferg } } },
            { &sectionA_2, { { 0, &Neml }, { 1, &Hmm }, { 2, &Kaf }, { 3, &Ferg }, { 4, &Ferg }, { 9, &She }, { 8, &Ferg } } },
            { &sectionB_1, { { 1, &Grb }, { 2, &Kaf }, { 5, &Neml }, { 6, &Grb }, { 7, &Mehd }, { 8, &Bel }, { 9, &She } } },
            { &sectionB_2, { { 1, &Hmm }, { 2, &Kaf }, { 5, &Neml }, { 6, &Hmm }, { 7, &Mehd }, { 8, &Lus }, { 9, &She } } },
            { &sectionC_1, { { 3, &Bel }, { 5, &Ned }, { 6, &Lus }, { 7, &Kaf }, { 8, &Lus } } },
            { &sectionC_2, { { 3, &Bel }, { 5, &Kaf }, { 6, &Lus }, { 7, &Kaf }, { 8, &Ned } } }
        };
    }
    ~GoldenTest() { }

public:
    Class A;
    Class B;
    Class C;
    TimePlan timePlan;
    Section sectionA_1;
    Section sectionA_2;
    Section sectionB_1;
    Section sectionB_2;
    Section sectionC_1;
    Section sectionC_2;
    Teacher Neml;
    Teacher She;
    Teacher Grb;
    Teacher Hmm;
    Teacher Kaf;
    Teacher Mehd;
    Teacher Bel;
    Teacher Lus;
    Teacher Ned;
    Teacher Ferg;

    TeacherAssignments teacherAssignments;
};

TEST_F(GoldenTest, solveGoldenTestCase)
{
    Response response = solve(
        timePlan, 10, { &sectionA_1, &sectionA_2, &sectionB_1, &sectionB_2, &sectionC_1, &sectionC_2 }, teacherAssignments);

    EXPECT_EQ(response.success, true);
    EXPECT_EQ(response.code, ErrorCode::NO_ERROR);
    EXPECT_EQ(response.message, "");
    //EXPECT_EQ(response.result, Result());
}
