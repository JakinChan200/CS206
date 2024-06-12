#include <bits/stdc++.h>
#include "../../json.hpp"
#include <map>

using namespace std;
using json = nlohmann::json;

struct testNode {
	int testCase;
	int numLinesCovered = 0;
	int additionalLinesCovered = 0;
	int numBranchesCovered = 0;
	int additionalBranchesCovered = 0;
	vector<vector<int>> lines;
	vector<vector<int>> branches;
};

struct Compare{
    bool operator()(testNode &a, testNode &b){
        return a.numLinesCovered < b.numLinesCovered; 
    }
};

struct addCompare{
    bool operator()(testNode &a, testNode &b){
        return a.additionalLinesCovered < b.additionalLinesCovered; 
    }
};

struct branchCompare {
	bool operator()(testNode &a, testNode &b){
        return a.numBranchesCovered < b.numBranchesCovered; 
    }
};

struct branchAddCompare {
	bool operator()(testNode &a, testNode &b){
        return a.additionalBranchesCovered < b.additionalBranchesCovered; 
    }
};

// check if string is number : json will only have positives
// https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
bool isNum(const string& s) {
	for (unsigned i = 0; i < s.size(); i++) {
		if (!isdigit(s.at(i))) {
			return false;
		}
	}
	return true;
}

void printMap(map<int, testNode> &testCases){
	for(auto testCase : testCases){
		cout << "Test Case " << testCase.first << endl;
		for(int i = 0; i < testCase.second.lines.size(); i++){
			cout << "line: " << testCase.second.lines[i][0] << " " << testCase.second.lines[i][1] << endl; 
		}
		cout << endl;
	}
}

void printJson(string prog_name){
	unsigned fcount = 1;

	ifstream f("json/" + prog_name + to_string(fcount) + ".json");
	while (f.good())
	{
		json data = json::parse(f);
		cout << data.dump(4) << endl;

		f.close();
		fcount++;

		f.open("json/" + prog_name + to_string(fcount) + ".json");
	}
	f.close();
}

void printSuiteNumbered(vector<int> suite){
	for(int i = 0; i < suite.size(); i++){
		cout << i+1 << ": " << suite[i] << endl;
	}
}

void printSuite(const vector<int>& suite){
	for(int i = 0; i < suite.size(); i++){
		cout << suite[i] << endl;
	}
}

int parseJsonFiles(string prog_name, map<int, testNode> &testCases){
	unsigned fcount = 1;
	int numLines = 0;

	ifstream f("json/" + prog_name + to_string(fcount) + ".json");
	while (f.good())
	{
		json data = json::parse(f);
		numLines = data["files"][0]["lines"].size();
		// cout << data.dump(4) << endl;
		testNode temp;
		temp.testCase = fcount;

		// cout << "File: " << fcount << endl;
		for(int i = 0; i < data["files"][0]["lines"].size(); i++){
			// cout << "Line Number: " << data["files"][0]["lines"][i]["line_number"]  << " " << data["files"][0]["lines"][i]["unexecuted_block"] << endl;
			int linesExecuted = data["files"][0]["lines"][i]["unexecuted_block"] == false ? 1 : 0;
			temp.lines.push_back({data["files"][0]["lines"][i]["line_number"] , linesExecuted});
			temp.numLinesCovered += linesExecuted;

			if(data["files"][0]["lines"][i]["branches"].size() > 0){
				vector<int> branchTemp = {data["files"][0]["lines"][i]["line_number"]};

				for(int j = 0; j < data["files"][0]["lines"][i]["branches"].size(); j++){
					int branchCovered = data["files"][0]["lines"][i]["branches"][j]["count"] > 0 ? 1 : 0;
					branchTemp.push_back(branchCovered);
					temp.numBranchesCovered += branchCovered;
				}
				temp.branches.push_back(branchTemp);
			}
		}
		temp.additionalLinesCovered = temp.numLinesCovered;
		temp.additionalBranchesCovered = temp.numBranchesCovered;
		testCases[fcount] = temp;
		// cout << endl;

		f.close();
		fcount++;

		f.open("json/" + prog_name + to_string(fcount) + ".json");
	}
	f.close();
	return numLines;
}

void randomStatement(vector<int> &finalSuite, map<int, testNode> &testCases, int numLines){
	finalSuite.clear();
	int totalTestCases = testCases.size();
	vector<int> testSuiteChoices(totalTestCases+1, false);	//Which test cases are in the test suite (technically, finalSuite, testSuiteChoices can be placed in a set but im lazy atm)

	int totalLineCoverage = 0;
	set<int> totalVisitedTestCases;
	vector<bool> coverage(numLines, false);					//Which lines are covered

	while(totalLineCoverage < numLines && totalVisitedTestCases.size() < totalTestCases){ //Either cover all of the statements or run out of test cases
		int cur = (rand() % totalTestCases) + 1;
		totalVisitedTestCases.insert(cur);
		if(testSuiteChoices[cur] == false){	//This test case is not in the test suite yet
			int numChanges = 0;
			for(int i = 0; i < coverage.size(); i++){ //Compare line coverage already covered and in the test suite
				if(testCases[cur].lines[i][1] == 1 && coverage[i] == false){ //Only look at the lines that the test case covers, also covers cases where no additional coverage
					numChanges++;
					totalLineCoverage++;
					coverage[i] = true;
				}
			}
			testSuiteChoices[cur] = numChanges > 0? true : false;
			
		}
	}

	//Populate finalSuite with the test cases choices
	for(int i = 0; i < testSuiteChoices.size(); i++){
		if(testSuiteChoices[i] == true){
			finalSuite.push_back(i);
		}
	}
}

void totalCoverageStatement(vector<int> &finalSuite, map<int, testNode> &testCases, int numLines){
	finalSuite.clear();
	
	priority_queue<testNode, vector<testNode>, Compare> prioQue;

	for(auto testCase : testCases){
		prioQue.push(testCase.second);
	}

	// Testing out Priority Queue
	// testNode temp;
	// while(!prioQue.empty()){
	// 	temp = prioQue.top();
	// 	prioQue.pop();
	// 	cout << "Test Number: " << temp.testCase << " #: " << temp.numLinesCovered << endl;
	// }

	int numLinesCovered = 0;
	vector<bool> coverage(numLines, false);
	testNode cur;
	
	while(numLinesCovered < numLines && !prioQue.empty()){
		cur = prioQue.top();
		prioQue.pop();
		// cout << "Test Case: " << cur.testCase << " with " << cur.numLinesCovered << " lines covered" << endl;

		int numChanges = 0;
		for(int i = 0; i < numLines; i++){ //Compare line coverage already covered and in the test suite
			if(cur.lines[i][1] == 1 && coverage[i] == false){ //Only look at the lines that the test case covers, also covers cases where no additional coverage
				numChanges++;
				numLinesCovered++;
				coverage[i] = true;
			}
		}
		if(numChanges > 0) finalSuite.push_back(cur.testCase);
	}
}

void additionalCoverageStatement(vector<int> &finalSuite, map<int, testNode> &testCases, int numLines){
	finalSuite.clear();
	
	priority_queue<testNode, vector<testNode>, addCompare> prioQue;

	for(auto testCase : testCases){
		testCase.second.additionalLinesCovered = testCase.second.numLinesCovered;   //Resetting Additional lines counter
		prioQue.push(testCase.second);
	}

	int numLinesCovered = 0;
	vector<bool> coverage(numLines, false);
	testNode cur;

	while(numLinesCovered < numLines && !prioQue.empty()){
        // cout << "Lines Covered " << numLinesCovered << " numLines " << numLines << endl;
		cur = prioQue.top();
		prioQue.pop();
		// cout << "Test Case: " << cur.testCase << " with " << cur.numLinesCovered << " lines covered and " << cur.additionalLinesCovered << " additional lines covered" << endl;

		for(int i = 0; i < numLines; i++){ //Compare line coverage already covered and in the test suite
			if(cur.lines[i][1] == 1 && coverage[i] == false){ //Only look at the lines that the test case covers, also covers cases where no additional coverage
				numLinesCovered++;
				coverage[i] = true;
			}
		}
		finalSuite.push_back(cur.testCase);

		//Update Coverage for all of the test cases
		priority_queue<testNode, vector<testNode>, addCompare> tempPrioQue;
		testNode temp;
		while(!prioQue.empty()){
			temp = prioQue.top();
			prioQue.pop();
			temp.additionalLinesCovered = 0;
			
			for(int i = 0; i < numLines; i++){
				if(coverage[i] == false && temp.lines[i][1] == 1){
					temp.additionalLinesCovered++;
				}
			}
            if(temp.additionalLinesCovered != 0){
			    tempPrioQue.push(temp);
            }
		}
		prioQue = tempPrioQue;
	}
}

void resetCovTracker(vector<vector<int>> &coverage, int &totalBranches){
	for(int i = 0; i < coverage.size(); i++){
		for(int j = 0; j < coverage[i].size(); j++){
			coverage[i][j] = 0;
			totalBranches++;
		}
	}
}

void randBranch(vector<int> &finalSuite, map<int, testNode> &testCases){
	finalSuite.clear();

	int totalTestCases = testCases.size();

	int totalBranchCoverage = 0;
	int totalBranches = 0;
	set<int> totalVisitedTestCases;

	vector<vector<int>> coverage = testCases[1].branches;
	resetCovTracker(coverage, totalBranches);

	int numChanges = 0;
	while(totalVisitedTestCases.size() < totalTestCases && totalBranchCoverage  < totalBranches){
		int cur = (rand() % testCases.size()) + 1;
		int tempSetSize = totalVisitedTestCases.size();
		totalVisitedTestCases.insert(cur);

		if(totalVisitedTestCases.size() != tempSetSize){
			int numChanges = 0;
			for(int i = 0; i < testCases[cur].branches.size(); i++){
				for(int j = 0; j < testCases[cur].branches[i].size(); j++){
					if(coverage[i][j] == 0 && testCases[cur].branches[i][j] == 1){
						numChanges++;
						coverage[i][j] = 1;
						totalBranchCoverage++;
					}
				}
			}

			if(numChanges > 0){
				finalSuite.push_back(cur);
			}
		}
	}
}

void totalCoverageBranch(vector<int> &finalSuite, map<int, testNode> &testCases){
	finalSuite.clear();
	
	priority_queue<testNode, vector<testNode>, Compare> prioQue;

	for(auto testCase : testCases){
		prioQue.push(testCase.second);
	}

	testNode cur;

	int totalBranchCoverage = 0;
	int totalBranches = 0;

	vector<vector<int>> coverage = testCases[1].branches;
	resetCovTracker(coverage, totalBranches);

	while(totalBranchCoverage < totalBranches && !prioQue.empty()){
		cur = prioQue.top();
		prioQue.pop();

		int numChanges = 0;
		for(int i = 0; i < cur.branches.size(); i++){
			for(int j = 0; j < cur.branches[i].size(); j++){
				if(coverage[i][j] == 0 && cur.branches[i][j] == 1){
					numChanges++;
					coverage[i][j] = 1;
					totalBranchCoverage++;
				}
			}
		}
		if(numChanges > 0) finalSuite.push_back(cur.testCase);
	}

	// cout << "start" << endl;
	// for(int i = 0; i < coverage.size(); i++){
	// 	for(int j = 0; j < coverage[i].size(); j++){
	// 		cout << coverage[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	// cout << "end" << endl;
}

void additionalCoverageBranch(vector<int> &finalSuite, map<int, testNode> &testCases){
	finalSuite.clear();

	priority_queue<testNode, vector<testNode>, branchAddCompare> prioQue;

	for(auto testCase : testCases){
		testCase.second.additionalBranchesCovered = testCase.second.numBranchesCovered;   //Resetting Additional lines counter
		prioQue.push(testCase.second);
	}

	testNode cur;

	int totalBranchCoverage = 0;
	int totalBranches = 0;

	vector<vector<int>> coverage = testCases[1].branches;
	resetCovTracker(coverage, totalBranches);

	while(totalBranchCoverage < totalBranches && !prioQue.empty()){
        // cout << "Lines Covered " << numLinesCovered << " numLines " << numLines << endl;
		cur = prioQue.top();
		prioQue.pop();
		// cout << "Test Case: " << cur.testCase << " with " << cur.numLinesCovered << " lines covered and " << cur.additionalLinesCovered << " additional lines covered" << endl;

		for(int i = 0; i < coverage.size(); i++){
			for(int j = 0; j < coverage[i].size(); j++){
				if(coverage[i][j] == 0 && cur.branches[i][j] == 1){
					coverage[i][j] = 1;
					totalBranchCoverage++;
				}
			}
		}
		finalSuite.push_back(cur.testCase);

		//Update Coverage for all of the test cases
		priority_queue<testNode, vector<testNode>, branchAddCompare> tempPrioQue;
		testNode temp;
		while(!prioQue.empty()){
			// cout << "breh" << endl;
			temp = prioQue.top();
			prioQue.pop();
			temp.additionalBranchesCovered = 0;
			
			for(int i = 0; i < temp.branches.size(); i++){
				for(int j = 0; j < temp.branches[i].size(); j++){
					// cout << coverage[i][j] << " " << cur.branches[i][j] << endl;
					if(coverage[i][j] == 0 && temp.branches[i][j] == 1){
						temp.additionalBranchesCovered += 1;
					}
				}
			}
			// cout << temp.additionalBranchesCovered << endl;
            if(temp.additionalBranchesCovered != 0){
			    tempPrioQue.push(temp);
            }
		}
		prioQue = tempPrioQue;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cout << "Error: Usage: parse prog_name universe.txt" << endl;
		return -1;
	}
	const string prog_name = argv[1], prog_test = argv[2];	// needed prog_test to get test cases for output
	string temp;
	unsigned fcount = 1;
	// std::map<string, string> sum;

	map<int, testNode> testCases;
	int numLines = parseJsonFiles(prog_name, testCases);

	// put the test into each node
	vector<string> tests;
	ifstream inFS(prog_test.c_str());
	while (getline(inFS, temp)) {
		tests.push_back(temp);
	}

	// printMap(testCases);
	ofstream outFS;
	vector<int> randStatementSuite;
	randomStatement(randStatementSuite, testCases, numLines);

	cout << "Test Cases Chosen for Random Statement Suite" <<  endl;
	printSuite(randStatementSuite);
	outFS.open("test_suites/S-Rand_test.txt");
	for(int i = 0; i < randStatementSuite.size(); i++){
		outFS << tests.at(randStatementSuite[i]-1) << endl;
	}
	outFS.close();

	//printJson(prog_name);

	// //printMap(testCases);

	vector<int> totalCoverageStateSuite;
	totalCoverageStatement(totalCoverageStateSuite, testCases, numLines);

	cout << "Test Cases Chosen for Total Coverage Statement Suite" <<  endl;
	printSuite(totalCoverageStateSuite);
	outFS.open("test_suites/S-Total_test.txt");
	for(int i = 0; i < totalCoverageStateSuite.size(); i++){
		outFS << tests.at(totalCoverageStateSuite[i]-1) << endl;
	}
	outFS.close();

	vector<int> addCoverageStateSuite;
	additionalCoverageStatement(addCoverageStateSuite, testCases, numLines);

	cout << "Test Cases for Additional Coverage Statement Suite" << endl;
	printSuite(addCoverageStateSuite);
	outFS.open("test_suites/S-Add_test.txt");
	for(int i = 0; i < addCoverageStateSuite.size(); i++){
		outFS << tests.at(addCoverageStateSuite[i]-1) << endl;
	}
	outFS.close();

	vector<int> randomBranchSuite;
	randBranch(randomBranchSuite, testCases);

	cout << "Test Cases for Random Branch Suite" << endl;
	printSuite(randomBranchSuite);
	outFS.open("test_suites/B-Rand_test.txt");
	for(int i = 0; i < randomBranchSuite.size(); i++){
		outFS << tests.at(randomBranchSuite[i]-1) << endl;
	}
	outFS.close();

	vector<int> totalCoverageBranchSuite;
	totalCoverageBranch(totalCoverageBranchSuite, testCases);

	cout << "Test Cases for Total Coverage Branch Suite" << endl;
	printSuite(totalCoverageBranchSuite);
	outFS.open("test_suites/B-Total_test.txt");
	for(int i = 0; i < totalCoverageBranchSuite.size(); i++){
		outFS << tests.at(totalCoverageBranchSuite[i]-1) << endl;
	}
	outFS.close();

	vector<int> additionalCoverageBranchSuite;
	additionalCoverageBranch(additionalCoverageBranchSuite, testCases);

	cout << "Test Cases for Additional Coverage Branch Suite" << endl;
	printSuite(additionalCoverageBranchSuite);
	outFS.open("test_suites/B-Add_test.txt");
	for(int i = 0; i < additionalCoverageBranchSuite.size(); i++){
		outFS << tests.at(additionalCoverageBranchSuite[i]-1) << endl;
	}
	outFS.close();
	return 0;
}

// int main(int argc, char* argv[]) {
// 	if (argc < 2) {
// 		cout << "Error: Usage: parse prog_name" << endl;
// 		return -1;
// 	}
// 	const string prog_name = argv[1];
// 	string temp;
// 	unsigned fcount = 1;

// 	map<int, testNode> testCases;
// 	int numLines = parseJsonFiles(prog_name, testCases);

// 	vector<int> finalSuite;
// 	randomStatementSuite(finalSuite, testCases, numLines);

// 	cout << "Test Cases Chosen for random Statement Suite" <<  endl;
// 	printSuite(finalSuite);

// 	vector<int> totalCoverageStateSuite;
// 	totalCoverageStatementSuite(totalCoverageStateSuite, testCases, numLines);

// 	cout << "Test Cases Chosen for total Coverage Statement Suite" <<  endl;
// 	printSuite(totalCoverageStateSuite);

// 	vector<int> addCoverageStateSuite;
// 	additionalCoverageStatementSuite(addCoverageStateSuite, testCases, numLines);

// 	cout << "Test Cases for Additional Coverage Statement Suite" << endl;
// 	printSuite(addCoverageStateSuite);

// 	vector<int> randomBranchSuite;
// 	randBranchSuite(randomBranchSuite, testCases);

// 	cout << "Test Cases for Random Branch Suite" << endl;
// 	printSuite(randomBranchSuite);

// 	vector<int> totalCovBranchSuite;
// 	totalCoverageBranchSuite(totalCovBranchSuite, testCases);

// 	cout << "Test Cases for Total Coverage Branch Suite" << endl;
// 	printSuite(totalCovBranchSuite);
// 	return 0;
// }