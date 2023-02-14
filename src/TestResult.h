#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace terminal{
	class Color {
	public:
		Color(int colorId, std::string rgb, std::string name) 
			: colorId_(colorId), rgb_(rgb), name_(name) {}
		void apply() const {
			system(("tput setaf " + std::to_string(colorId_) + ";").c_str());
		}
		void print(std::string text) const {
			apply();
			std::cout << text;
			std::cout.flush();
		}
		void println(std::string text) const {
			print(text);
			std::cout << std::endl;
		}
		std::string getRGB() const {
			return rgb_;
		}
		std::string getName() const {
			return name_;
		}
	private:
		int colorId_;
		std::string rgb_;
		std::string name_;
	};
	const Color BLACK       (0 , "#000000", "BLACK"       );
	const Color DARK_RED    (1 , "#800000", "DARK_RED"    );
	const Color DARK_GREEN  (2 , "#008000", "DARK_GREEN"  );
	const Color DARK_YELLOW (3 , "#808000", "DARK_YELLOW" );
	const Color DARK_BLUE   (4 , "#000080", "DARK_BLUE"   );
	const Color DARK_MAGENTA(5 , "#800080", "DARK_MAGENTA");
	const Color DARK_CYAN   (6 , "#008080", "DARK_CYAN"   );
	const Color GRAY        (7 , "#c0c0c0", "GRAY"        );
	const Color RED         (9 , "#ff0000", "RED"         );
	const Color GREEN       (10, "#00ff00", "GREEN"       );
	const Color YELLOW      (11, "#ffff00", "YELLOW"      );
	const Color BLUE        (12, "#0000ff", "BLUE"        );
	const Color MAGENTA     (13, "#ff00ff", "MAGENTA"     );
	const Color CYAN        (14, "#00ffff", "CYAN"        );
	const Color WHITE       (15, "#ffffff", "WHITE"       );
	const Color LIGHT_BLUE  (45, "#0087ff", "LIGHT_BLUE"  );
}

struct TestResult{
	bool successful;
	std::string id;
	std::string text;
	terminal::Color color;
	double grade = 0;
	TestResult(bool _successful, std::string _id, std::string _text, terminal::Color _color) 
		: successful(_successful), id(_id), text(_text), color(_color) {}
	std::string getRGB() {
		return color.getRGB();
	}
};
const TestResult UNDEFINED_ERROR(false, "UNDEFINED"      , "Undefined Error", terminal::DARK_RED  );
const TestResult COMPILE_ERROR  (false, "COMPILE_ERROR"  , "Compile Error"  , terminal::RED       );
const TestResult CORRECT_LOGIC  (true , "CORRECT_LOGIC"  , "Correct Logic"  , terminal::GREEN     );
const TestResult INCORRECT_LOGIC(false, "INCORRECT_LOGIC", "Incorrect Logic", terminal::YELLOW    );
const TestResult TIMEOUT_ERROR  (false, "TIMEOUT_ERROR"  , "Timeout Error"  , terminal::CYAN      );
const TestResult MEMORY_LEAK    (false, "MEMORY_LEAK"    , "Memory Leak"    , terminal::MAGENTA   );
const TestResult FATAL_ERROR    (false, "FATAL_ERROR"    , "Fatal Error"    , terminal::LIGHT_BLUE);
const TestResult NOT_FOUND      (false, "NOT_FOUND"      , "Not Found"      , terminal::WHITE     );

struct TestInfo {
	std::string testName;
	std::string testPath;
	std::string md5sum = "noMD5sum";
	double testGrade;
	bool valgrind_enabled = false;
	TestInfo() {}
	TestInfo(std::string testName_, std::string testPath_, double testGrade_) : testName(testName_), testPath(testPath_), testGrade(testGrade_) {}
	bool operator==(TestInfo& rhs) {
		return testName == rhs.testName && testPath == rhs.testPath 
			&& testGrade == rhs.testGrade && md5sum == rhs.md5sum 
			&& valgrind_enabled == rhs.valgrind_enabled;
	}
};

struct GradeReport {
	std::vector<TestResult> testResults;
	double grade;
	std::string sampleName;
	std::string samplePath;
	GradeReport(std::string sampleName_, std::string samplePath_)
		: sampleName(sampleName_), samplePath(samplePath_) {}
};

struct GradeBook {
	std::vector<GradeReport> gradeReports;
	std::vector<TestInfo> tests;
	double maxGrade;
};
