#pragma once
#include "structs.h"
#include <string>

typedef map<Section*, vector<pair<int, bool>>> SectionToSubjectsAndAvailability;
typedef map<Teacher*, vector<bool>> TeacherAvailability;
using namespace std;

int getDaysCount(TimePlan timePlan);

Response validateInputs(TimePlan, int maxNumberOfSubjects, vector<Section*>, TeacherAssignments);
Response solve(TimePlan, int maxNumberOfSubjects, vector<Section*>, TeacherAssignments);
string getMessage();
bool backtrack(TimePlan, vector<Section*>, TeacherAssignments&, SectionToSubjectsAndAvailability&, TeacherAvailability&, Result&, int index);
