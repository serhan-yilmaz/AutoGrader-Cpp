# AutoGrader-Cpp
An application to automatize the grading of coding assignments in C++. It prepares a summary
Excel file reporting compile errors, time outs, memory leaks, incorrect logic and fatal errors. 

### Requred inputs

For grading, the application uses a series of unit tests specified in ```tests.txt``` and ```tests/``` folders. 
- Each test should be on a separate line in the following format: ```[Test File Path], [Grade], [Binary 1/0 Flag for Memory Leak Testing]```. 
- For example, ```tests/test1.cpp, 40, 1```

The home works to be graded need to be supplied in ```samples.txt``` and ```samples/``` folders. 
- Each home work should be on a separate file in the following format: ```[Homework File Path]```. For example, ```samples/sample1.cpp```. 

Configuration file ```config.txt``` specifies various options related to the report generation and unit test (e.g., the time out duration if the compiled code is not responding). 

### Sample Output

<img src="sample_output.png">
