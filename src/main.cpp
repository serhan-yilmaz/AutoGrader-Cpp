#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <cctype>
#include <ctime> 
#include <chrono>
#include <thread>

#include "TestResult.h"
#include "XMLexporter.h"
#include "TimeReporter.h"

using namespace std;

struct Config {
	string gcc_options = "-std=c++11";
	string valgrind_options;
	string timeout_time = "5";
	string timeout_kill_time = "0.1";
	string worksheet_name = "AutoGrader Report";
	string font_size = "15";
	string font_size_headings = "18";
	string column_width = "100";
	string column_width_first = "150";
	bool cache_enabled = true;
	double total_points = 0;
};

string getMD5sum(string path) {
	string tempPath = ".md5temp" + autograder::to_string(rand());
	system(("md5sum " + path + " > " + tempPath).c_str());
	ifstream tempFile(tempPath);
	if (!tempFile.is_open()) {
		return "";
	}
	string md5sum;
	getline(tempFile, md5sum);
	md5sum = md5sum.substr(0, 32);
	tempFile.close();
	system(("rm " + tempPath).c_str());
	return md5sum;
}

std::vector<std::string> split(const std::string &text, char sep) {
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != std::string::npos) {
		if (end != start) {
			tokens.push_back(text.substr(start, end - start));
		}
		start = end + 1;
	}
	if (end != start) {
		tokens.push_back(text.substr(start));
	}
	return tokens;
}

string parseSampleName(string path) {
	vector<string> tokens;
	if (path.at(path.size() - 1) == '/') {
		path = path.substr(0, path.size() - 1);
	}
	tokens = split(path, '/');
	return tokens.back();
}

string parseTestName(string path) {
	vector<string> tokens;
	if (path.at(path.size() - 1) == '/') {
		path = path.substr(0, path.size() - 1);
	}
	tokens = split(path, '/');
	string s = tokens.back();
	tokens = split(s, '.');
	return tokens.front();
}

bool parseTestFile(string path, vector<TestInfo>& out) {
	ifstream in(path);
	if (in.is_open()) {
		string line;
		while (getline(in, line)) {
			vector<string> tokens = split(line, ',');
			TestInfo t;
			if (tokens.size() <= 2) {
				cout << "The test is invalid : " << line << endl;
				return false;
			}
			for (int i = 0; i < tokens.size(); i++) {
				string s = tokens.at(i);
				s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
				tokens.at(i) = s;
			}
			t.testPath = tokens.at(0);
			t.testName = parseTestName(t.testPath);
			t.testGrade = stod(tokens.at(1));
			t.md5sum = getMD5sum(t.testPath);
			if (tokens.size() >= 3) {
				t.valgrind_enabled = stoi(tokens.at(2));
			}
			out.push_back(t);
		}
		in.close();
		return true;
	}
	cout << "Test file cannot be opened." << endl;
	return false;
}

bool parseFile(string path, vector<string>& out) {
	ifstream in(path);
	if (in.is_open()) {
		string line;
		while (getline(in, line)) {
			out.push_back(line);
		}
		in.close();
		return true;
	}
	return false;
}

void printColoredText(string text, int colorCode) {
	string colorCodeString = autograder::to_string(colorCode);
	system(("tput setaf " + colorCodeString + ";").c_str());
	cout << text;
	cout.flush();
}

vector<TestResult> testResultList;
void initialize() {
	testResultList.push_back(COMPILE_ERROR);
	testResultList.push_back(CORRECT_LOGIC);
	testResultList.push_back(INCORRECT_LOGIC);
	testResultList.push_back(TIMEOUT_ERROR);
	testResultList.push_back(MEMORY_LEAK);
	testResultList.push_back(FATAL_ERROR);
}

bool saveTestResult(string path, TestInfo testInfo, TestResult testResult) {
	ofstream testResultFile(path);
	if (testResultFile.is_open()) {
		testResultFile << testResult.id << ",";
		testResultFile << testInfo.testName << ",";
		testResultFile << testInfo.testPath << ",";
		testResultFile << testInfo.testGrade << ",";
		testResultFile << testInfo.valgrind_enabled << ",";
		testResultFile << testInfo.md5sum << endl;
		return true;
	}
	return false;
}

bool saveAllTestResults(string path, vector<TestInfo> tests, GradeReport gradeReport) {
	if (tests.size() != gradeReport.testResults.size())
		return false;
	ofstream allTestResultsFile(path);
	if (allTestResultsFile.is_open()) {
		allTestResultsFile << gradeReport.grade << ",";
		allTestResultsFile << gradeReport.testResults.size() << endl;
		for (int i = 0; i < gradeReport.testResults.size(); i++) {
			TestInfo testInfo = tests.at(i);
			allTestResultsFile << gradeReport.testResults.at(i).id << ",";
			allTestResultsFile << gradeReport.testResults.at(i).grade << ",";
			allTestResultsFile << testInfo.testName << ",";
			allTestResultsFile << testInfo.testPath << ",";
			allTestResultsFile << testInfo.testGrade << ",";
			allTestResultsFile << testInfo.valgrind_enabled << ",";
			allTestResultsFile << testInfo.md5sum << endl;
		}
		return true;
	}
	return false;
}

bool parseTestResult(string path, TestInfo testInfo, TestResult& out) {
	ifstream testResultFile(path);
	string line;
	if (testResultFile.is_open() && getline(testResultFile, line)) {
		vector<string> tokens = split(line, ',');
		if (tokens.size() != 6) {
			return false;
		}
		string testResultString = tokens.at(0);
		TestInfo cachedTestInfo;
		cachedTestInfo.testName = tokens.at(1);
		cachedTestInfo.testPath = tokens.at(2);
		//cachedTestInfo.testGrade = stod(tokens.at(3));
		cachedTestInfo.testGrade = testInfo.testGrade;
		cachedTestInfo.valgrind_enabled = stoi(tokens.at(4));
		cachedTestInfo.md5sum = tokens.at(5);
		if (!(testInfo == cachedTestInfo)) {
			return false;
		}
		for (int i = 0; i < testResultList.size(); i++) {
			if (testResultString == testResultList.at(i).id) {
				out = testResultList.at(i);
				return true;
			}
		}
	}
	return false;
}

bool parseAllTestResults(string path, vector<TestInfo> tests, GradeReport& out) {
	ifstream allTestResultsFile(path);
	string line;
	if (allTestResultsFile.is_open() && getline(allTestResultsFile, line)) {
		vector<string> tokens = split(line, ',');
		if (tokens.size() != 2) {
			return false;
		}
		GradeReport gradeReport(out.sampleName, out.samplePath);
		gradeReport.grade = stod(tokens.at(0));
		int nTest = stoi(tokens.at(1));
		if (nTest != tests.size()) {
			return false;
		}
		for (int iTest = 0; iTest < nTest; iTest++) {
			if (!getline(allTestResultsFile, line)) {
				return false;
			}
			vector<string> tokens = split(line, ',');
			if (tokens.size() != 7) {
				return false;
			}
			string testResultString = tokens.at(0);
			double testResultGrade = stod(tokens.at(1));
			TestInfo cachedTestInfo;
			cachedTestInfo.testName = tokens.at(2);
			cachedTestInfo.testPath = tokens.at(3);
			cachedTestInfo.testGrade = stod(tokens.at(4));
			cachedTestInfo.valgrind_enabled = stoi(tokens.at(5));
			cachedTestInfo.md5sum = tokens.at(6);
			if (!(tests.at(iTest) == cachedTestInfo)) {
				return false;
			}
			bool testResultFound = false;
			for (int i = 0; i < testResultList.size(); i++) {
				if (testResultString == testResultList.at(i).id) {
					TestResult tr = testResultList.at(i);
					tr.grade = testResultGrade;
					gradeReport.testResults.push_back(tr);
					testResultFound = true;
					break;
				}
			}
			if (!testResultFound) {
				return false;
			}
		}
		out = gradeReport;
		return true;
	}
	return false;
}

void runTests(vector<string> samples, vector<TestInfo> tests, Config config) {
	TimeReporter tr;
	tr.reset();
	GradeBook gb;
	gb.tests = tests;
	gb.maxGrade = config.total_points;
	ofstream logFile("log.txt");
	for (int iSample = 0; iSample < samples.size(); iSample++) {
		string samplePath = samples.at(iSample);
		string sampleName = parseSampleName(samplePath);
		gb.gradeReports.push_back(GradeReport(sampleName, samplePath));
		if (system(("[ -d """ + samplePath + """ ]").c_str()) != 0) { // Sample not found
			if (logFile.is_open()) {
				logFile << sampleName << " is not found." << endl;
			}
			for (int iTest = 0; iTest < tests.size(); iTest++) {
				TestResult testResult = NOT_FOUND;
				testResult.grade = 0;
				gb.gradeReports.at(iSample).testResults.push_back(testResult);
			}
			gb.gradeReports.at(iSample).grade = 0;
			terminal::DARK_GREEN.print(sampleName);
			terminal::GRAY.print(" - ");
			terminal::DARK_RED.println(" not found!");
			continue;
		}
		string cachePath = samplePath + ".cache/";
		if (system(("[ -d """ + cachePath + """ ]").c_str()) != 0) {
			system(("mkdir " + cachePath).c_str());
		}
		terminal::GRAY.print("Running tests for ");
		terminal::DARK_GREEN.print(sampleName);
		terminal::GRAY.println(".");
		string allTestsCachePath = cachePath + ".allTests";
		bool allTestsCached = config.cache_enabled && parseAllTestResults(allTestsCachePath, tests, gb.gradeReports.at(iSample));
		if (!allTestsCached) {
			string summaryFilePath = samplePath + "summary.txt";
			ofstream summaryFile(summaryFilePath);
			if (summaryFile.is_open()) {
				summaryFile << "Name : " << sampleName << endl;
			}
			for (int iTest = 0; iTest < tests.size(); iTest++) {
				TestResult testResult = UNDEFINED_ERROR;
				TestInfo t = tests.at(iTest);
				string testCachePath = cachePath + t.testName;
				int b = system(("[ -f """ + testCachePath + """ ]").c_str());
				bool testCacheExists = b == 0;
				bool testCached = false;
				if (config.cache_enabled && testCacheExists) {
					if (parseTestResult(testCachePath, t, testResult)) {
						testCached = true;
					}
				}
				if (!testCached) {
					system(("cp " + t.testPath + " " + samplePath + ";").c_str());
					string compileFilePath = samplePath + "CompileFiles/" +
						"compile_file_" + t.testName + ".txt";
					system(("mkdir -p " + samplePath + "CompileFiles/;").c_str());
					system(("echo "" "" 1> " + compileFilePath).c_str());
					string outFilePath = samplePath + "out";
					string inputFilePath = samplePath + "*.cpp";
					string compileCommand = "g++ " + config.gcc_options + " "
						+ inputFilePath + " -o " + outFilePath
						+ " 2>> " + compileFilePath + " 1>/dev/null;";
					int b1 = system(compileCommand.c_str());
					system(("rm " + samplePath + t.testName + ".cpp;").c_str());
					if (b1 == 0) {
						system(("echo ""Compilation has been successfully completed."" 1>> " + compileFilePath).c_str());
						string runFileFolder = samplePath + "RunFiles/";
						system(("mkdir -p " + runFileFolder + ";").c_str());
						string runFilePath = runFileFolder + "run_file_" + t.testName + ".txt";
						system(("echo "" "" 1> " + runFilePath).c_str());
						string command = outFilePath;
						if (t.valgrind_enabled) {
							command = "valgrind --leak-check=yes --error-exitcode=101 "
								+ config.valgrind_options + " " + outFilePath;
						}
						string timeoutCommand = "{ timeout -k " + config.timeout_kill_time + "s "
							+ config.timeout_time + "s " + command + " 1>>" + runFilePath
							+ " ; } 2>>" + runFilePath + "";
						//1 >> " + runFilePath + "; ";
						int b2 = system(timeoutCommand.c_str());
						if (b2 >= 256) {
							b2 = b2 / 256;
						}
						if (b2 == 0) {
							testResult = INCORRECT_LOGIC;
						} else if (b2 == 1) {
							testResult = CORRECT_LOGIC;
						} else if (b2 == 124 || b2 == 137) {
							testResult = TIMEOUT_ERROR;
						} else if (b2 == 101) {
							testResult = MEMORY_LEAK;
						} else {
							testResult = FATAL_ERROR;
						}
						while (system(("rm " + outFilePath).c_str()) != 0) {
							terminal::RED.println("Cannot remove output file. Trying again...");
							std::this_thread::sleep_for(std::chrono::milliseconds(1000));
						}
					} else {
						if (b1 == 2) {
							cout << "The autograder application is aborted. Signal : " << b1 << endl;
							if (summaryFile.is_open()) {
								summaryFile.close();
							}
							if (logFile.is_open()) {
								logFile.close();
							}
							return;
						}
						testResult = COMPILE_ERROR;
					}
					saveTestResult(testCachePath, t, testResult);
				}
				testResult.grade = testResult.successful ? t.testGrade : 0;
				gb.gradeReports.at(iSample).testResults.push_back(testResult);
				terminal::DARK_GREEN.print(sampleName);
				terminal::GRAY.print(", ");
				terminal::DARK_CYAN.print(t.testName);
				terminal::GRAY.print(" - ");
				string testResultText = autograder::to_string(testResult.grade) + " (" + testResult.text + ")";
				testResultText = testCached ? (testResultText + " (Cached).") : (testResultText + ".");
				testResult.color.println(testResultText);
				gb.gradeReports.at(iSample).grade += testResult.grade;
				if (gb.gradeReports.at(iSample).grade <= 1e-10) {
					gb.gradeReports.at(iSample).grade = 0;
				}
				if (summaryFile.is_open()) {
					summaryFile << t.testName << " - " << testResult.text << " - " << testResult.grade << " points" << endl;
				}
			}
			if (summaryFile.is_open()) {
				summaryFile << "Grade is " << gb.gradeReports.at(iSample).grade << " out of " << config.total_points << "." << endl;
				summaryFile.close();
			}
			saveAllTestResults(allTestsCachePath, tests, gb.gradeReports.at(iSample));
		}
		terminal::GRAY.print("Grade is ");
		terminal::DARK_YELLOW.print(autograder::to_string(gb.gradeReports.at(iSample).grade));
		terminal::GRAY.print(" out of ");
		terminal::DARK_YELLOW.print(autograder::to_string(config.total_points));
		if (allTestsCached) {
			terminal::WHITE.print(" (Cached)");
		}
		terminal::GRAY.println(".");
		int progress = (int)(100 * (1 + iSample) / samples.size());
		terminal::YELLOW.println(("Progress : " + autograder::to_string(progress) + "%").c_str());
	}
	if (logFile.is_open()) {
		logFile.close();
	}
	string xmlPath = "report.xml";
	XMLoptions xmlOptions;
	xmlOptions.column_width = config.column_width;
	xmlOptions.column_width_first = config.column_width_first;
	xmlOptions.worksheet_name = config.worksheet_name;
	xmlOptions.font_size = config.font_size;
	xmlOptions.font_size_headings = config.font_size_headings;
	XMLexporter xmle(xmlOptions);
	terminal::WHITE.apply();
	if (xmle.export_to_file(xmlPath, gb)) {
		cout << "Successfully exported to XML file """ << xmlPath << """." << endl;
	} else {
		cout << "Cannot be exported to XML file """ << xmlPath << """." << endl;
	}
	int time_passed = (int) tr.report();
	cout << "Time Passed : " << time_passed << " s" << endl;
}

Config parseConfigFile(string path) {
	ifstream in(path);
	Config config;
	if (in.is_open()) {
		string line;
		while (getline(in, line)) {
			vector<string> tokens = split(line, '=');
			if (tokens.size() <= 0) {
				continue;
			}
			string argument = tokens.at(0);
			argument.erase(std::remove_if(argument.begin(), argument.end(), ::isspace), argument.end());
			if (argument.size() <= 0)
				continue;
			if (argument.at(0) == '%')
				continue;
			if (tokens.size() <= 1) {
				cout << "Invalid config argument : " << argument << ". Argument is ignored." << endl;
			}
			string value;
			for (int i = 1; i < tokens.size(); i++) {
				if (i != 1) {
					value = value + "=";
				}
				value = value + tokens.at(i);
			}
			bool bo = true;
			if (argument.compare("timeout_time") == 0) {
				config.timeout_time = value;
				bo = false;
			}
			if (argument.compare("timeout_kill_time") == 0) {
				config.timeout_kill_time = value;
				bo = false;
			}
			if (argument.compare("gcc_options") == 0) {
				config.gcc_options = value;
				bo = false;
			}
			if (argument.compare("valgrind_options") == 0) {
				config.valgrind_options = value;
				bo = false;
			}
			if (argument.compare("font_size_headings") == 0) {
				config.font_size_headings = value;
				bo = false;
			}
			if (argument.compare("font_size") == 0) {
				config.font_size = value;
				bo = false;
			}
			if (argument.compare("worksheet_name") == 0) {
				config.worksheet_name = value;
				bo = false;
			}
			if (argument.compare("column_width") == 0) {
				config.column_width = value;
				bo = false;
			}
			if (argument.compare("column_width_first") == 0) {
				config.column_width_first = value;
				bo = false;
			}
			if (argument.compare("cache_enabled") == 0) {
				config.cache_enabled = stoi(value);
				bo = false;
			}
			if (bo) {
				cout << "Invalid config argument : " << argument << ". Argument is ignored." << endl;
			}
		}
		in.close();
		cout << "Succesfully loaded """ << path << """ file." << endl;
	}
	return config;
}

int main(int argc, char *argv[]) {
	srand((unsigned)time(0));
	string path1 = "tests.txt";
	string path2 = "samples.txt";
	string path3 = "config.txt";
	if (argc >= 2) {
		path1 = argv[1];
	}
	if (argc >= 3) {
		path2 = argv[2];
	}
	if (argc >= 3) {
		path3 = argv[3];
	}
	vector<TestInfo> tests;
	vector<string> samples;
	initialize();
	if (!parseTestFile(path1, tests)) {
		return -2;
	} else {
		cout << "Succesfully loaded """ << path1 << """ file." << endl;
	}
	if (!parseFile(path2, samples)) {
		cout << "Sample file cannot be opened." << endl;
		return -3;
	} else {
		cout << "Succesfully loaded """ << path2 << """ file." << endl;
	}
	Config config = parseConfigFile(path3);
	for (int i = 0; i < samples.size(); i++) {
		string samplePath = samples.at(i);
		if (samplePath.at(samplePath.size() - 1) != '/')
			samplePath = samplePath + '/';
		samples.at(i) = samplePath;
		//cout << samples.at(i) << endl;
	}
	config.total_points = 0;
	for (int i = 0; i < tests.size(); i++) {
		TestInfo t = tests.at(i);
		config.total_points += t.testGrade;
	}
	cout << "Number of tests : " << tests.size() << endl;
	cout << "Number of samples : " << samples.size() << endl;
	cout << "Total points : " << config.total_points << endl;
	cout << "Starting tests." << endl;
	runTests(samples, tests, config);
}