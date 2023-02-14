#include "XMLexporter.h"

XMLexporter::XMLexporter(XMLoptions options) : options_(options) {}

bool XMLexporter::export_to_file(std::string path, GradeBook gradeBook)
{
	int nTest = gradeBook.tests.size();
	int nSample = gradeBook.gradeReports.size();
	if (nTest <= 0 || nSample <= 0) {
		return false;
	}
	std::ofstream xmlFile(path);
	if (xmlFile.is_open()) {
		xmlFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
		xmlFile << "<?mso-application progid=\"Excel.Sheet\"?>" << std::endl;
		xmlFile << "<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"" << std::endl
			<< "xmlns:o=\"urn:schemas-microsoft-com:office:office\"" << std::endl
			<< "xmlns:x=\"urn:schemas-microsoft-com:office:excel\"" << std::endl
			<< "xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"" << std::endl
			<< "xmlns:html=\"http://www.w3.org/TR/REC-html40\">" << std::endl;
		//xmlFile << "<Workbook>" << endl;
		std::vector<Style> styles;
		terminal::Color heading_color(0, "#ddd9c3", "");
		terminal::Color samples_color(0, "#fde9d9", "");
		terminal::Color total_points_color(0, "#DBEEF3", "");
		Style STYLE_HEADINGS(heading_color, "STYLE_HEADINGS", options_.font_size_headings);
		Style STYLE_REGULAR(terminal::WHITE, "STYLE_REGULAR", options_.font_size);
		Style STYLE_SAMPLES(samples_color, "STYLE_SAMPLES", options_.font_size);
		Style STYLE_GRADE(total_points_color, "STYLE_POINTS", options_.font_size);
		STYLE_HEADINGS.setBold(true);
		styles.push_back(STYLE_HEADINGS);
		styles.push_back(STYLE_REGULAR);
		styles.push_back(STYLE_SAMPLES);
		styles.push_back(STYLE_GRADE);
		styles.push_back(Style(COMPILE_ERROR.color, "STYLE_" + COMPILE_ERROR.id, options_.font_size));
		styles.push_back(Style(CORRECT_LOGIC.color, "STYLE_" + CORRECT_LOGIC.id, options_.font_size));
		styles.push_back(Style(INCORRECT_LOGIC.color, "STYLE_" + INCORRECT_LOGIC.id, options_.font_size));
		styles.push_back(Style(TIMEOUT_ERROR.color, "STYLE_" + TIMEOUT_ERROR.id, options_.font_size));
		styles.push_back(Style(MEMORY_LEAK.color, "STYLE_" + MEMORY_LEAK.id, options_.font_size));
		styles.push_back(Style(FATAL_ERROR.color, "STYLE_" + FATAL_ERROR.id, options_.font_size));
		styles.push_back(Style(NOT_FOUND.color, "STYLE_" + NOT_FOUND.id, options_.font_size));
		xmlFile << "<Styles>" << std::endl;
		for (int i = 0; i < styles.size(); i++) {
			styles.at(i).print(xmlFile);
		}
		xmlFile << "</Styles>" << std::endl;
		xmlFile << "<Worksheet ss:Name=\"" << options_.worksheet_name << "\" >" << std::endl;
		xmlFile << "<Table>" << std::endl;
		for (int i = 0; i <= nTest + 1; i++) {
			std::string columnWidth = options_.column_width;
			if (i == 0) {
				columnWidth = options_.column_width_first;
			}
			xmlFile << "<Column ss:AutoFitWidth=\"0\" ss:Width=\"" << columnWidth << "\"/>" << std::endl;
		}

		xmlFile << "<Row>" << std::endl;
		print_cell(xmlFile, STYLE_HEADINGS.getId(), "Name");
		print_cell(xmlFile, STYLE_HEADINGS.getId(), "Total");
		for (int i = 0; i < nTest; i++) {
			std::string value = gradeBook.tests.at(i).testName;
			print_cell(xmlFile, STYLE_HEADINGS.getId(), value);
		}
		xmlFile << "</Row>" << std::endl;
		xmlFile << "<Row>" << std::endl;
		print_cell(xmlFile, STYLE_SAMPLES.getId(), "Correct_Solution");
		print_formula_cell(xmlFile, STYLE_GRADE.getId(), nTest);
		for (int i = 0; i < nTest; i++) {
			double value = gradeBook.tests.at(i).testGrade;
			print_cell(xmlFile, "STYLE_" + CORRECT_LOGIC.id, value);
		}
		xmlFile << "</Row>" << std::endl;

		for (int iSample = 0; iSample < nSample; iSample++) {
			GradeReport gradeReport = gradeBook.gradeReports.at(iSample);
			xmlFile << "<Row>" << std::endl;
			print_cell(xmlFile, STYLE_SAMPLES.getId(), gradeReport.sampleName);
			print_formula_cell(xmlFile, STYLE_GRADE.getId(), nTest);
			for (int iTest = 0; iTest < gradeReport.testResults.size(); iTest++) {
				TestResult testResult = gradeReport.testResults.at(iTest);
				std::string styleId = "STYLE_" + testResult.id;
				std::string gradeText = testResult.successful ? autograder::to_string(testResult.grade) : testResult.text;
				std::string dataType = testResult.successful ? "Number" : "String";
				xmlFile << "<Cell ss:StyleID=\"" << styleId << "\"><Data ss:Type=\"" + dataType + "\">"
					<< gradeText  << "</Data></Cell>" << std::endl;
			}
			xmlFile << "</Row>" << std::endl;
		}
		xmlFile << "</Table>" << std::endl;
		xmlFile << "</Worksheet>" << std::endl;
		xmlFile << "<Worksheet ss:Name=\"" << "Color Legend " << "\" >" << std::endl;
		xmlFile << "<Table>" << std::endl;
		xmlFile << "<Column ss:AutoFitWidth=\"0\" ss:Width=\"" << options_.column_width_first << "\"/>" << std::endl;
		xmlFile << "<Column ss:AutoFitWidth=\"0\" ss:Width=\"" << options_.column_width_first << "\"/>" << std::endl;
		xmlFile << "<Row>" << std::endl;
		xmlFile << "<Cell ss:StyleID=\"" << STYLE_HEADINGS.getId() << "\"><Data ss:Type=\"String\">" << "Color" << "</Data></Cell>" << std::endl;
		xmlFile << "<Cell ss:StyleID=\"" << STYLE_HEADINGS.getId() << "\"><Data ss:Type=\"String\">" << "Description" << "</Data></Cell>" << std::endl;
		xmlFile << "</Row>" << std::endl;
		for (int i = 1; i <= 6; i++) {
			std::string colorName;
			std::string description;
			switch (i) {
			case 1:
				colorName = "Red";
				description = "Compile Error";
				break;
			case 2:
				colorName = "Green";
				description = "Correct Solution";
				break;
			case 3:
				colorName = "Orange";
				description = "Logical Error";
				break;
			case 4:
				colorName = "Light-Blue";
				description = "Timeout";
				break;
			case 5:
				colorName = "Purple";
				description = "Memory Leak";
				break;
			case 6:
				colorName = "Dark-Blue";
				description = "Fatal Error";
				break;
			case 7:
				description = "Not Found";
				break;
			}
			colorName = styles.at(i + 3).getColor().getName();
			xmlFile << "<Row>" << std::endl;
			xmlFile << "<Cell ss:StyleID=\"" << styles.at(i + 3).getId() << "\"><Data ss:Type=\"String\">" << colorName << "</Data></Cell>" << std::endl;
			xmlFile << "<Cell ss:StyleID=\"" << STYLE_REGULAR.getId() << "\"><Data ss:Type=\"String\">" << description << "</Data></Cell>" << std::endl;
			xmlFile << "</Row>" << std::endl;
		}
		xmlFile << "</Table>" << std::endl;
		xmlFile << "</Worksheet>" << std::endl;
		xmlFile << "</Workbook>" << std::endl;
		xmlFile.close();
		return true;
	}
	else {
		return false;
	}
}

void XMLexporter::print_cell(std::ofstream & xmlFile, std::string styleId, std::string value)
{
	xmlFile << "<Cell ss:StyleID=\"" + styleId + "\"><Data ss:Type=\"String\">" << value << "</Data></Cell>" << std::endl;
}

void XMLexporter::print_cell(std::ofstream & xmlFile, std::string styleId, double value)
{
	xmlFile << "<Cell ss:StyleID=\"" + styleId + "\"><Data ss:Type=\"Number\">" << value << "</Data></Cell>" << std::endl;
}

void XMLexporter::print_formula_cell(std::ofstream & xmlFile, std::string styleId, unsigned int nTest)
{
	xmlFile << "<Cell ss:StyleID=\"" + styleId + "\" ss:Formula=\"=SUM(RC[1]:RC[" << nTest << "])\"><Data ss:Type=\"Number\">" << "0" << "</Data></Cell>" << std::endl;
}

Style::Style(terminal::Color color, std::string id, std::string fontSize)
	: color_(color), id_(id), fontSize_(fontSize) {};

void Style::setBold(bool b)
{
	if (b)
		isBold_ = "1";
	else
		isBold_ = "0";
}

void Style::print(std::ofstream & stream) const
{
	stream << "<Style ss:ID=\"" << id_ << "\">" << std::endl;
	stream << "<Borders>" << std::endl;
	stream << "<Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>" << std::endl
		<< "<Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>" << std::endl
		<< "<Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>" << std::endl
		<< "<Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>" << std::endl;
	stream << "</Borders>" << std::endl;
	stream << "<Font ss:Size=\"" << fontSize_ << "\" ss:Bold=\"" + isBold_ + "\" ss:FontName=\"Calibri\" />" << std::endl;
	stream << "<Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Top\" />" << std::endl;
	stream << "<Interior ss:Color=\"" << color_.getRGB() << "\" ss:Pattern=\"Solid\" />" << std::endl;
	stream << "</Style>" << std::endl;
}

std::string Style::getId() const
{
	return id_;
}

terminal::Color Style::getColor() const
{
	return color_;
}
