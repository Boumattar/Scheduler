#pragma once 
#include<string>
#include "structs.h"


using namespace std;

Response validateInputs(TimePlan, int maxNumberOfSubjects, vector<Section*>, TeacherAssignments);
string getMessage();

