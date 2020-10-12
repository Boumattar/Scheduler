#pragma once
#include "../Scheduler/json.hpp"
#include "structs.h"
#include <string>

typedef map<Section*, vector<pair<int, bool>>> SectionToSubjectsAndAvailability;
typedef map<Teacher*, vector<bool>> TeacherAvailability;

using namespace std;
using namespace nlohmann;

int getDaysCount(TimePlan timePlan);

Response validateInputs(TimePlan, int maxNumberOfSubjects, vector<Section*>, TeacherAssignments);
Response solve(TimePlan, int maxNumberOfSubjects, vector<Section*>, TeacherAssignments);

json solveJsonRequest(json& input);
