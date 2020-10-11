#pragma once
#include <vector>
#include <map>
#include <string>

using namespace std;

const int MaxNumberOfSubjectPerDay  = 2;

struct TimePlan
{
    int days;
    int lessonsInDay;
};


struct Class
{
    string name;
    map<int, int> curriculum; // frequency of subjects (sum should be d * l/d)
};

struct Section
{
    Class* c;
    string name;
};

struct Teacher
{
    string name;
    vector<bool> availability;
};

typedef map<Section*, map<int, Teacher*> > TeacherAssignments;
struct MaxHoursConstraint
{
    int subject;
    int maxHoursPerDay;
};

enum class ErrorCode
{
    NO_ERROR,
    NO_SOLUTION,
    CLASS_CURRICULUM_INVALID,
    INVALID_SUBJECT,
    TEACHER_OVERLOADED,
    INVALID_INPUT,
    SUBJECT_FREQ_TOO_HIGH
};

typedef map<Section*, vector<int> > Result;
struct Response
{
    bool success;
    ErrorCode code;
    string message;
    Result result; //map of section to vector of subjects of length d * l/d
};
