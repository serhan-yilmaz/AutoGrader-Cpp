#pragma once

#include "TestResult.h"
#include "Util.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

struct XMLoptions {
	std::string column_width;
	std::string column_width_first;
	std::string worksheet_name;
	std::string font_size;
	std::string font_size_headings;
};

class Style{
public:
	Style(terminal::Color color, std::string id, std::string fontSize);

	void setBold(bool b);
	void print(std::ofstream& stream) const;
	std::string getId() const;
	terminal::Color getColor() const;

private:
	terminal::Color color_;
	std::string id_;
	std::string fontSize_;
	std::string isBold_ = "0";

};

class XMLexporter {
public:
	XMLexporter(XMLoptions options);
	bool export_to_file(std::string path, GradeBook gradeBook);  

private:
	XMLoptions options_;

	void print_cell(std::ofstream& xmlFile, std::string styleId, std::string value);
	void print_cell(std::ofstream& xmlFile, std::string styleId, double value);
	void print_formula_cell(std::ofstream& xmlFile, std::string styleId, unsigned int nTest);

};




















