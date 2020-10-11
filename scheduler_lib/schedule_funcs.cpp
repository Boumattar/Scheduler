#include <algorithm>
#include <cassert>
#include <cmath>
#include <map>
#include <naxos.h>
#include <numeric>
#include <schedule_funcs.h>
#include <set>
#include <sstream>
#include <utility>

using namespace std;
using namespace naxos;

int getDaysCount(TimePlan timePlan) { return timePlan.days * timePlan.lessonsInDay; }

static int getTeacherTotalAvailability(Teacher& teacher)
{
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
    if (getDaysCount(timePlan) <= 0) {
        message << "Total number of lessons is invalid: "
                << getDaysCount(timePlan);
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
        if (numberOfLessonsInClass != getDaysCount(timePlan)) {
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
        for (auto& teacherSubjectPair : teacherAssignment.second) {
            int additionalLoad = teacherAssignment.first->c->curriculum.find(teacherSubjectPair.first) != teacherAssignment.first->c->curriculum.end() ? teacherAssignment.first->c->curriculum.find(teacherSubjectPair.first)->second : 0;
            if (teacherLoads.find(teacherSubjectPair.second) != teacherLoads.end()) {
                teacherLoads[teacherSubjectPair.second] += additionalLoad;
            } else {
                teacherLoads[teacherSubjectPair.second] = additionalLoad;
            }
        }
    }
    for (const auto& teacherLoad : teacherLoads) {
        if (teacherLoad.second > getTeacherTotalAvailability(*teacherLoad.first)) {
            message << "Teacher " << teacherLoad.first->name << " is overloaded with a load of " << teacherLoad.second;
            return Response {
                false, ErrorCode::TEACHER_OVERLOADED, message.str(), Result()
            };
        }
    }

    return Response { true, ErrorCode::NO_ERROR, "", Result() };
}

Response solve(TimePlan timePlan, int maxNumberOfSubjects, vector<Section*> sections, TeacherAssignments teacherAssignments)
{
    Response validationResponse = validateInputs(timePlan, maxNumberOfSubjects, sections, teacherAssignments);
    if (!validationResponse.success) {
        return validationResponse;
    }

    Result result;
    for (auto section : sections) {
        result[section] = vector<int>(getDaysCount(timePlan), -1);
    }

    SectionToSubjectsAndAvailability sectionToSubjectAndAvailability;
    TeacherAvailability teacherAvailability;

    //Fill map of section to available subjects
    for (Section* section : sections) {
        sectionToSubjectAndAvailability.insert(pair<Section*, vector<pair<int, bool>>>(section, vector<pair<int, bool>>()));
        for (auto& subjectAndFrequency : section->c->curriculum) {
            auto newSubjects = vector<pair<int, bool>>(subjectAndFrequency.second, pair<int, bool>(subjectAndFrequency.first, true));
            sectionToSubjectAndAvailability[section].insert(end(sectionToSubjectAndAvailability[section]), begin(newSubjects), end(newSubjects));
        }
        const bool isRightSize = sectionToSubjectAndAvailability[section].size() == getDaysCount(timePlan);
        assert(isRightSize);
    }

    //Fill map of teacher availability
    for (auto& assignment : teacherAssignments) {
        for (auto& teacherSubjectPair : assignment.second) {
            if (teacherAvailability.find(teacherSubjectPair.second) == teacherAvailability.end())
                teacherAvailability[teacherSubjectPair.second] = teacherSubjectPair.second->availability;
        }
    }

    bool solved = backtrack(timePlan, sections, teacherAssignments, sectionToSubjectAndAvailability, teacherAvailability, result, 0);

    if (solved)
        return Response { true, ErrorCode::NO_ERROR, "", result };

    return Response { false, ErrorCode::NO_SOLUTION, "", result };
}

bool checkIfSubjectFits(TimePlan timePlan, int subject, Section* section, TeacherAssignments& assignments, TeacherAvailability& teacherSlots, Result& result, int slotInWeek)
{
    Teacher* assignedTeacher = assignments[section][subject];

    if (!teacherSlots[assignedTeacher][slotInWeek])
        return false;

    vector<int>& schedule = result[section];
    int day = slotInWeek / timePlan.lessonsInDay;
    int subjectInDayCount = 0;
    for (int i = day * timePlan.lessonsInDay; i < (day + 1) * timePlan.lessonsInDay; i++) {
        if (schedule[i] == subject)
            subjectInDayCount++;
    }
    if (subjectInDayCount >= MaxNumberOfSubjectPerDay)
        return false;
    return true;
}

bool backtrack(TimePlan timePlan, vector<Section*> sections, TeacherAssignments& assignments, SectionToSubjectsAndAvailability& subjects, TeacherAvailability& teacherSlots, Result& result, int index)
{

    if (index < 0)
        return false;

    if (index >= sections.size() * getDaysCount(timePlan))
        return true;

    int indexOfSection = index / getDaysCount(timePlan);
    int indexInWeek = index - indexOfSection * getDaysCount(timePlan);

    Section* section = sections.at(indexOfSection);
    auto& subjectsAndAvailabilities = subjects[section];

    for (auto& subject : subjectsAndAvailabilities) {
        if (subject.second) {
            bool fits = checkIfSubjectFits(timePlan, subject.first, section, assignments, teacherSlots, result, indexInWeek);
            if (fits) {
                result[section][indexInWeek] = subject.first;
                subject.second = false;
                Teacher* assignedTeacher = assignments.at(section).at(subject.first);
                teacherSlots[assignedTeacher][indexInWeek] = false;

                if (backtrack(timePlan, sections, assignments, subjects, teacherSlots, result, index + 1))
                    return true;

                //otherwise backtrack
                result[section][indexInWeek] = -1;
                subject.second = true;
                teacherSlots[assignedTeacher][indexInWeek] = true;
            }
        }
    }
    return false;
}

string
getMessage()
{
    return "CMake working ...";
}
