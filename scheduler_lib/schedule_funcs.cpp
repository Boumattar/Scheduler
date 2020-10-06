#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <schedule_funcs.h>
#include <sstream>
#include <utility>

using namespace std;

static int getTeacherTotalAvailability(Teacher& teacher) {
    return count(teacher.availability.begin(), teacher.availability.end(), true);
}

Response
validateInputs(TimePlan timePlan,
    int maxNumberOfSubjects,
    vector<Section*> sections,
    TeacherAssignments teacherAssignments)
{
    stringstream message;

    // Check if the total number of lessons is positive
    if (timePlan.days * timePlan.lessonsInDay <= 0) {
        message << "Total number of lessons is invalid: "
                << timePlan.days * timePlan.lessonsInDay;
        return Response { false, ErrorCode::INVALID_INPUT, message.str(), Result() };
    }

    for each (Section* section in sections) {
        // Check if the total number of lessons for each class is equal to the
        // number of lessons in time plan
        const int numberOfLessonsInClass = accumulate(section->c->curriculum.begin(),
            section->c->curriculum.end(),
            0,
            [](const int result, const auto& element) {
                return result + element.second;
            });
        if (numberOfLessonsInClass != timePlan.days * timePlan.lessonsInDay) {
            message << "Class " << section->c->name
                    << " has an invalid curriculum with total number of lessons "
                    << numberOfLessonsInClass;
            return Response {
                false, ErrorCode::CLASS_CURRICULUM_INVALID, message.str(), Result()
            };
        }

        // Check if the subjects in the class curriculum are valid (less than the
        // max number of subjects
        if (section->c->curriculum.begin()->first < 0 || section->c->curriculum.rbegin()->first >= maxNumberOfSubjects) {
            message << "Class " << section->c->name
                    << " has one or more invalid subjects";
            return Response {
                false, ErrorCode::CLASS_CURRICULUM_INVALID, message.str(), Result()
            };
        }

        // Check if the frequency of subjects does not exceed the max possible
        // frequency (taking the max number of sujects per days into account)
        auto& mostFrequentSubject = max_element(section->c->curriculum.begin(),
            section->c->curriculum.end(),
            [](const auto& element1, const auto& element2) {
                return element1.second < element2.second;
            });
        if (mostFrequentSubject->second > timePlan.days * MaxNumberOfSubjectPerDay) {
            message << "Subject " << mostFrequentSubject->first << " in class "
                    << section->c->name << " is too frequent";
            return Response {
                false, ErrorCode::SUBJECT_FREQ_TOO_HIGH, message.str(), Result()
            };
        }
    }

    //Check if any of the teachers are overloaded
    map<Teacher*, int> teacherLoads;
    for (const auto& teacherAssignment : teacherAssignments) {
        for (int i = 0; i < teacherAssignment.second.size(); i++) {
            int additionalLoad = teacherAssignment.first->c->curriculum.find(i) != teacherAssignment.first->c->curriculum.end() ?
                teacherAssignment.first->c->curriculum.find(i)->second : 0;
            if (teacherLoads.find(teacherAssignment.second.at(i)) != teacherLoads.end()) {
                teacherLoads[teacherAssignment.second.at(i)] += additionalLoad;
            }
            else {
                teacherLoads[teacherAssignment.second.at(i)] = additionalLoad;
            }
        }
    }
    for (const auto& teacherLoad : teacherLoads) {
        if (teacherLoad.second > getTeacherTotalAvailability(*teacherLoad.first)) {
            message << "Teacher " << teacherLoad.first->name << " is overloaded with a load of " << teacherLoad.second;
            return Response{
                false, ErrorCode::TEACHER_OVERLOADED, message.str(), Result()
            };
        }
    }


    return Response { true, ErrorCode::NO_ERROR, "", Result() };
}

string
getMessage()
{
    return "CMake working ...";
}
