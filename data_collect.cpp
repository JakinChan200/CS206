#include <bits/stdc++.h>
using namespace std;

// Comparison of two files: Copied from stack overflow
// https://stackoverflow.com/questions/6163611/compare-two-files
bool compareFiles(const std::string& p1, const std::string& p2) {
  std::ifstream f1(p1, std::ifstream::binary|std::ifstream::ate);
  std::ifstream f2(p2, std::ifstream::binary|std::ifstream::ate);

  if (f1.fail() || f2.fail()) {
    return false; //file problem
  }

	if (f1.tellg() == -1) {
		cout << p1 << " is empty." << endl;
	}
	if (f2.tellg() == -1) {
		cout << p2 << " is empty." << endl;
	}

  if (f1.tellg() != f2.tellg()) {
    return false; //size mismatch
  }

  //seek back to beginning and use std::equal to compare contents
  f1.seekg(0, std::ifstream::beg);
  f2.seekg(0, std::ifstream::beg);
  return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}

// get file name for numbered output file in subdirectory
string generate_fname(unsigned i, unsigned j) {
	return "v" + to_string(i) + "/" + to_string(j) + ".txt";
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		cout << "Error: Usage: ./data_collect test_cases.txt" << endl;
		return -1;
	}
	string test_file = argv[1];
	ifstream t(test_file);
	if (!t.good()) {
		cout << "Error: " << test_file << " not found." << endl;
		return -1;
	}
	string temp;
	vector<int> v;
	vector<pair<string, vector<int>>> test_cases;		// 0-indexed
	while (getline(t, temp)) {
		test_cases.push_back(make_pair(temp, v));
	}

	unsigned fcount = 1, tcount = 1;
	// get directory
	// read test file
	string curr = generate_fname(fcount, tcount), fname;
	ifstream i(curr);
	if (!i.good()) {
		cout << "Error: " << curr << " not found." << endl;
		return -1;
	}
	while (i.good())
	{
		// differing test results
		fname = to_string(tcount)+".txt";
		if (!compareFiles(fname, curr)) {
			// if (test_cases.at(tcount-1).second.empty() || test_cases.at(tcount-1).second.at(test_cases.at(tcount-1).second.size()-1) != fcount)
			v.push_back(fcount);
			test_cases.at(tcount-1).second.push_back(fcount);
		}

		i.close();

		// cleanup
		// {
		// 	curr = "rm " + curr;
		// 	system(curr.c_str());
		// }
		tcount++;

		// if we use all of the test_cases, go to the next directory
		if (tcount > test_cases.size()) {
			cout << "Compared with v" << fcount << endl;
			fcount++;
			tcount = 1;
		}
		curr = generate_fname(fcount, tcount);
		i.open(curr);
	}
	i.close();

	// cleanup
	// for (unsigned j = 1; j <= test_cases.size(); j++) {
	// 	curr = "rm " + to_string(j) + ".txt";
	// 	system(curr.c_str());
	// }

	if (test_file.find("test_suites/") != string::npos) {		// remove directory name if needed
		test_file = test_file.substr(12);
	}
	ofstream outFS("results/by_case/" + test_file.substr(0,test_file.size()-9) + "_bycase_results.txt");	// put output files in results
	for (unsigned j = 0; j < test_cases.size(); j++) {
		outFS << j+1 << ":";
		for (unsigned k = 0; k < test_cases.at(j).second.size(); k++) {
			outFS << " " << test_cases.at(j).second.at(k);
		}
		outFS << endl;
	}
	outFS.close();

	std::sort(v.begin(), v.end());
	unsigned count = 0;
	outFS.open("results/" + test_file.substr(0,test_file.size()-9) + "_results.txt");
	for (unsigned j = 0; j < v.size(); j++) {
		if (j == 0) {
			outFS << v.at(j);
			count++;
		}
		else {
			if (v.at(j) != v.at(j-1)) {
				outFS << "," << v.at(j);
				count++;
			}
		}
	} outFS << endl << "Faults: " << count << endl;;
	outFS.close();

	return 0;
}