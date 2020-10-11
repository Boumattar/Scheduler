#include "schedule_funcs.h"
#include <memory>

#include "gtest/gtest.h"
using namespace std;

class InputValidationInvalidCurriculumTest : public ::testing::Test {

public:
    InputValidationInvalidCurriculumTest()
    {
        timePlan = TimePlan { 5, 5 };
        map<int, int> invalidCurriculumMap = { { 0, 4 }, { 4, 6 }, { 7, 8 }, { 3, 1 }, { 1, 5 } };
        map<int, int> invalidSubjectMap1 = { { 0, 4 }, { 4, 6 }, { 7, 8 }, { -2, 2 }, { 1, 5 } };
        map<int, int> invalidSubjectMap2 = { { 0, 4 }, { 4, 6 }, { 11, 8 }, { 3, 2 }, { 1, 5 } };
        map<int, int> tooFrequentSubjectMap = { { 0, 4 }, { 4, 6 }, { 9, 11 }, { 3, 2 }, { 1, 2 } };
        invalidCurriculumClass = Class { "invalidCurriculum", invalidCurriculumMap };
        invalidSubjectsClass1 = Class { "invalidSubject1", invalidSubjectMap1 };
        invalidSubjectsClass2 = Class { "invalidSubject2", invalidSubjectMap2 };
        tooFrequenctSubjectClass = Class { "tooFreqSubject", tooFrequentSubjectMap };
        emptyCurriculumClass = Class { "emptyCurriculum", map<int, int>() };
        invalidCurriculumSession = Section { &invalidCurriculumClass, "invalidCurriculum" };
        emptyCurriculumSession = Section { &emptyCurriculumClass, "emptyCurriculum" };
        invalidSubjectsSection1 = Section { &invalidSubjectsClass1, "invalidSubject1" };
        invalidSubjectsSection2 = Section { &invalidSubjectsClass2, "invalidSubject2" };
        tooFrequenctSubjectSection = Section { &tooFrequenctSubjectClass, "tooFreqSubject" };
    }
    ~InputValidationInvalidCurriculumTest() { }

public:
    Class invalidCurriculumClass;
    Class emptyCurriculumClass;
    Class invalidSubjectsClass1;
    Class invalidSubjectsClass2;
    Class tooFrequenctSubjectClass;
    TimePlan timePlan;
    Section invalidCurriculumSession;
    Section emptyCurriculumSession;
    Section invalidSubjectsSection1;
    Section invalidSubjectsSection2;
    Section tooFrequenctSubjectSection;
};

class TeacherOverloadTest : public ::testing::Test {

public:
    TeacherOverloadTest()
    {
        timePlan = TimePlan { 5, 5 };
        map<int, int> class1Curr = { { 0, 4 }, { 1, 6 }, { 2, 8 }, { 3, 2 }, { 4, 5 } };
        map<int, int> class2Curr = { { 0, 7 }, { 1, 1 }, { 2, 5 }, { 3, 6 }, { 4, 6 } };
        class1 = Class { "class1", class1Curr };
        class2 = Class { "class2", class2Curr };
        section1_1 = Section { &class1, "section1_2" };
        section1_2 = Section { &class1, "section1_2" };
        section2_1 = Section { &class2, "section2_1" };
        section2_2 = Section { &class2, "section2_2" };

        vector<bool> fullTime = vector<bool>(25, true);

        auto partTime = fullTime;
        partTime[9] = false;
        partTime[14] = false;
        partTime[17] = false;
        partTime[18] = false;
        partTime[21] = false;
        partTime[24] = false;
        teacher1 = { "teacher1", fullTime };
        teacher2 = { "teacher2", fullTime };
        teacher3 = { "teacher3", fullTime };
        teacher4 = { "teacher4", fullTime };
        teacher5 = { "teacher5", fullTime };
        overloadedTeacher = { "overloadedTeacher", partTime };

        teacherAssignments = {
            { &section1_1, { { 0, &teacher1 }, { 1, &teacher1 }, { 2, &teacher2 }, { 3, &teacher2 }, { 4, &teacher1 } } },
            { &section1_2, { { 0, &teacher3 }, { 1, &teacher3 }, { 2, &teacher4 }, { 3, &overloadedTeacher }, { 4, &overloadedTeacher } } },
            { &section2_1, { { 0, &teacher5 }, { 1, &overloadedTeacher }, { 2, &teacher1 }, { 3, &overloadedTeacher }, { 4, &overloadedTeacher } } },
            { &section2_2, { { 0, &teacher3 }, { 1, &teacher3 }, { 2, &teacher1 }, { 3, &teacher4 }, { 4, &teacher5 } } }
        };
    }
    ~TeacherOverloadTest() { }

public:
    Class class1;
    Class class2;
    TimePlan timePlan;
    Section section1_1;
    Section section1_2;
    Section section2_1;
    Section section2_2;
    Teacher teacher1;
    Teacher teacher2;
    Teacher teacher3;
    Teacher teacher4;
    Teacher teacher5;
    Teacher overloadedTeacher;
    TeacherAssignments teacherAssignments;
};
TEST(InputValidationTest, checkTimePlan)
{
    Response response = solve(TimePlan { 0, 5 }, 10, vector<Section*>(),
        TeacherAssignments());

    EXPECT_EQ(response.success, false);
    EXPECT_EQ(response.code, ErrorCode::INVALID_INPUT);
    EXPECT_EQ(response.message, "Total number of lessons is invalid: 0");
    EXPECT_EQ(response.result, Result());

    Response response2 = solve(TimePlan { 2, -5 }, 10, vector<Section*>(),
        TeacherAssignments());

    EXPECT_EQ(response2.success, false);
    EXPECT_EQ(response2.code, ErrorCode::INVALID_INPUT);
    EXPECT_EQ(response2.message, "Total number of lessons is invalid: -10");
    EXPECT_EQ(response2.result, Result());
}

TEST_F(InputValidationInvalidCurriculumTest, checkInvalidCurriculum)
{
    Response response = solve(
        timePlan, 10, { &invalidCurriculumSession }, TeacherAssignments());

    EXPECT_EQ(response.success, false);
    EXPECT_EQ(response.code, ErrorCode::CLASS_CURRICULUM_INVALID);
    EXPECT_EQ(response.message, "Class invalidCurriculum has an invalid "
                                "curriculum with total number of lessons 24");
    EXPECT_EQ(response.result, Result());

    response = solve(
        timePlan, 10, { &emptyCurriculumSession }, TeacherAssignments());

    EXPECT_EQ(response.success, false);
    EXPECT_EQ(response.code, ErrorCode::CLASS_CURRICULUM_INVALID);
    EXPECT_EQ(response.message, "Class emptyCurriculum has an invalid curriculum "
                                "with total number of lessons 0");
    EXPECT_EQ(response.result, Result());
}

TEST_F(InputValidationInvalidCurriculumTest, checkInvalidSubjects)
{
    Response response = solve(
        timePlan, 10, { &invalidSubjectsSection1 }, TeacherAssignments());

    EXPECT_EQ(response.success, false);
    EXPECT_EQ(response.code, ErrorCode::CLASS_CURRICULUM_INVALID);
    EXPECT_EQ(response.message, "Class invalidSubject1 has one or more invalid subjects");
    EXPECT_EQ(response.result, Result());

    response = solve(
        timePlan, 10, { &invalidSubjectsSection2 }, TeacherAssignments());

    EXPECT_EQ(response.success, false);
    EXPECT_EQ(response.code, ErrorCode::CLASS_CURRICULUM_INVALID);
    EXPECT_EQ(response.message, "Class invalidSubject2 has one or more invalid subjects");
    EXPECT_EQ(response.result, Result());
}

TEST_F(InputValidationInvalidCurriculumTest, checkTooFrequentSubjects)
{
    Response response = solve(
        timePlan, 10, { &tooFrequenctSubjectSection }, TeacherAssignments());

    EXPECT_EQ(response.success, false);
    EXPECT_EQ(response.code, ErrorCode::SUBJECT_FREQ_TOO_HIGH);
    EXPECT_EQ(response.message, "Subject 9 in class tooFreqSubject is too frequent");
    EXPECT_EQ(response.result, Result());
}

TEST_F(TeacherOverloadTest, checkIfTeacherIsOverloaded)
{
    Response response = solve(
        timePlan, 10, { &section1_1, &section1_2, &section2_1, &section2_2 }, teacherAssignments);

    EXPECT_EQ(response.success, false);
    EXPECT_EQ(response.code, ErrorCode::TEACHER_OVERLOADED);
    EXPECT_EQ(response.message, "Teacher overloadedTeacher is overloaded with a load of 20");
    EXPECT_EQ(response.result, Result());
}
