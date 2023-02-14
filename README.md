# AutoGrader-Cpp
An application to automatize the grading of coding assignments in C++. It prepares a summary
Excel file reporting compile errors, time outs, memory leaks, incorrect logic and fatal errors. 

For grading, it uses a series of unit tests specified in ```tests.txt``` and ```tests/``` folders. 

The home works to be graded need to be supplied in ```samples.txt``` and ```samples/``` folders. 

Configuration file ```config.txt``` specifies various options related to the report generation and unit test (e.g., the time out duration if the compiled code is not responding). 

### Sample Output

<img src="sample_output.png">
