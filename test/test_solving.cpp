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

static bool testResultValidity(TimePlan tp, TeacherAssignments& assignments, Result result)
{
    for (auto& sectionResult : result) {
        Section* section = sectionResult.first;
        vector<int> schedule = sectionResult.second;

        map<int, int> subjectToFrequency;
        for (int day = 0; day < tp.days; day++) {
            map<int, int> dailyMap;
            for (int lesson = 0; lesson < tp.lessonsInDay; lesson++) {
                int lessonIndex = day * tp.lessonsInDay + lesson;
                int subject = result[section][lessonIndex];

                if (subjectToFrequency.find(subject) == subjectToFrequency.end()) {
                    subjectToFrequency[subject] = 1;
                } else {
                    subjectToFrequency[subject]++;
                }

                if (dailyMap.find(subject) == dailyMap.end()) {
                    dailyMap[subject] = 1;
                } else {
                    dailyMap[subject]++;
                }
                for (auto& subjFreqPair : dailyMap) {
                    if (subjFreqPair.second > MaxNumberOfSubjectPerDay)
                        return false;
                }
            }
        }
        if (subjectToFrequency != section->c->curriculum)
            return false;
    }

    map<Teacher*, vector<pair<Section*, int>>> teacherToAssignments;

    for (auto& sectionAssignment : assignments) {
        Section* section = sectionAssignment.first;
        for (auto& teacherAssignment : sectionAssignment.second) {
            Teacher* teacher = teacherAssignment.second;
            teacherToAssignments[teacher].push_back({ section, teacherAssignment.first });
        }
    }

    for (auto& teacherToAssignment : teacherToAssignments) {
        Teacher* teacher = teacherToAssignment.first;
        vector<pair<Section*, int>> taughtSessions = teacherToAssignment.second;
        for (int lesson = 0; lesson < getDaysCount(tp); lesson++) {
            int numberOfLessons = 0;
            for (auto taughtPair : taughtSessions) {
                if (result[taughtPair.first][lesson] == taughtPair.second)
                    numberOfLessons++;
            }
            if (numberOfLessons > 1 || (numberOfLessons == 1 && !teacher->availability[lesson]))
                return false;
        }
    }
    return true;
}

TEST_F(GoldenTest, solveGoldenTestCase)
{
    auto sections = { &sectionA_1, &sectionA_2, &sectionB_1, &sectionB_2, &sectionC_1, &sectionC_2 };
    Response response = solve(
        timePlan, 10, sections, teacherAssignments);

    EXPECT_EQ(response.success, true);
    EXPECT_EQ(response.code, ErrorCode::NO_ERROR);
    EXPECT_EQ(response.message, "");
    EXPECT_EQ(testResultValidity(timePlan, teacherAssignments, response.result), true);
}
