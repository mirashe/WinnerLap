After publishing the project I thought it might be better to add a read me file:

a) To build the projest you can use these commands in the command line:
	1. cd <WinnerLap_source_directory>
	2. mkdir build
	3. cd build
	4. cmake ..
	5. depended on the platform you use you can run one of these options:
		aa) make
		bb) cmake --build .
	6. you can find the executable file inside the created directories like Debug, etc
	
b) To run the executable file you can run it in a command line like this:
	<executable_file_path> <karttimes_file_path>
	
c) There is room for more work, for example:
	1. I assumed kart numbers are starting from 1 and continue without large gaps.
		Otherwise we can use a map instead of vector to store the temporary best laps.
	2. It was possible to read the time records part by part and parse it manually.
		But instead of recreating the wheel I took the beneficial of standard methods
		which makes it possible to parse different formats of time and date
		easily with a little change for possible future changes in input time format
	3. Currently all classes are small and we don't use them in other projects but in other similar projects we can create separated libraries, one for each class. 
	4. It is possible to create a script to run the build or even run commands, clean, etc. But for a small project like this that is not very usual.

regards