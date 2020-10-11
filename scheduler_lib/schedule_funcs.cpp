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

static set<int> getAvailableSubjects(Section* section)
{
    set<int> result;
    for (auto subject : section->c->curriculum) {
        result.insert(subject.first);
    }
    return result;
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

    NsProblemManager pm;

    map<Section*, NsIntVarArray> sectionSchedules;
    map<Teacher*, vector<NsInt>> teacherAvailability;
    map<Teacher*, vector<NsIntVarArray>> teacherLoad;
    map<Section*, vector<NsIntVarArray>> sectionDaySchedules;

    for (Section* section : sections) {
        auto availableSubjetcs = getAvailableSubjects(section);
        for (int day = 0; day < timePlan.days; day++) {
            NsIntVarArray daySchedule;
            for (int lesson = 0; lesson < timePlan.lessonsInDay; lesson++) {
                int lessonIndex = day * timePlan.lessonsInDay + lesson;
                sectionSchedules[section].push_back(NsIntVar(pm, 0, maxNumberOfSubjects - 1));

                //Remove subjects not compatible with section curriculum from domain
                for (int subject = 0; subject < maxNumberOfSubjects; subject++) {
                    if (availableSubjetcs.find(subject) == availableSubjetcs.end()) {
                        sectionSchedules[section][lessonIndex].remove(subject);
                    }
                }

                daySchedule.push_back(sectionSchedules[section][lessonIndex] + 0);
            }
            sectionDaySchedules[section].push_back(daySchedule);
        }
    }

    map<Teacher*, vector<pair<Section*, int>>> teacherToAssignments;

    //Fill map
    for (auto& sectionAssignment : teacherAssignments) {
        Section* section = sectionAssignment.first;
        for (auto& teacherAssignment : sectionAssignment.second) {
            Teacher* teacher = teacherAssignment.second;
            teacherToAssignments[teacher].push_back({ section, teacherAssignment.first });
        }
    }

    for (auto& teacherToAssignment : teacherToAssignments) {
        Teacher* teacher = teacherToAssignment.first;
        vector<pair<Section*, int>> taughtSessions = teacherToAssignment.second;
        for (int day = 0; day < timePlan.days; day++) {
            NsIntVarArray daySchedule;
            for (int lesson = 0; lesson < timePlan.lessonsInDay; lesson++) {
                int lessonIndex = day * timePlan.lessonsInDay + lesson;
                teacherAvailability[teacher].push_back(teacher->availability[lessonIndex]);
                NsIntVarArray teacherLoadForSlot;
                for (auto& session : taughtSessions) {
                    Section* section = session.first;
                    int subject = session.second;
                    teacherLoadForSlot.push_back(sectionSchedules[section][lessonIndex] == (NsInt)subject);
                }
                teacherLoad[teacher].push_back(teacherLoadForSlot);
            }
        }
    }

    //Ensure max of MaxNumberOfSubjectPerDay similar sessions per day
    for (auto& sectionPair : sectionDaySchedules) {
        for (auto& daySchedule : sectionPair.second) {
            pm.add(NsAllDiff(daySchedule, MaxNumberOfSubjectPerDay));
        }
    }

    for (auto& teacherPair : teacherLoad) {
        Teacher* teacher = teacherPair.first;
        for (int lessonIndex = 0; lessonIndex < getDaysCount(timePlan); lessonIndex++) {
            pm.add(NsSum(teacherPair.second[lessonIndex]) - teacherAvailability[teacher][lessonIndex] == 0);
        }
    }

    for (auto& sectionSchedule : sectionSchedules) {

        map<int, int> sectionCurriculum = sectionSchedule.first->c->curriculum;
        NsDeque<NsInt> subjArr;
        NsDeque<NsInt> freqArr;
        int index = 0;
        for (auto subjPair : sectionCurriculum) {
            subjArr.push_back(subjPair.first);
            freqArr.push_back(subjPair.second);
        }

        pm.add(NsCount(sectionSchedule.second, subjArr, freqArr));

        pm.addGoal(new NsgLabeling(sectionSchedule.second));
    }

    if (pm.nextSolution() != false) {
        for (auto& sectionSchedule : sectionSchedules) {
            Section* section = sectionSchedule.first;
            for (auto& subject : sectionSchedule.second) {
                result[section].push_back(subject.value());
            }
        }

        return Response { true, ErrorCode::NO_ERROR, "", result };
    }

    return Response { false, ErrorCode::NO_SOLUTION, "", result };
}

string
getMessage()
{
    return "CMake working ...";
}
