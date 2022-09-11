/* basic i/o */
#include <iostream>
#include <fstream>
#include <sstream>
/* data stack */
#include <vector>
using namespace std;

int main(void) {
	ifstream ifs("Input_P3.txt");
	string is;
	int count = 0; /* Number of circles */

	/* check if file is empty and ignore the first line */
	if (!getline(ifs, is)) {
		cout << "Error: File Corrupted..." << endl;
		return 1;
	}

	/* read the data and store into the 2d array */
	int mode = 0;
	vector<vector<double>> c;
	while (getline(ifs, is)) {
		if (is.length() == 0) {
			mode = 0; continue;
		}
		else if (is.compare(0, 19, "circle image points") == 0) {
			mode = 1; continue;
		}

		istringstream iss(is);
		vector<double> x(3);
		iss >> x[0] >> x[1] >> x[2];

		if (mode == 1) {
			c.push_back(x);
		}
	}

	ifs.close(); // close file handle

	/* [v] 2022. 3. 13. 16:05 -> Check the Piazza Q&A answer about output ps scale */
	constexpr float SCALE = 72;
	constexpr float LINEWIDTH = 1;

	ofstream output;

	output.open("P3.ps");
	/* magic number */
	output << "%!PS" << '\n';
	for (int i = 0; i < c.size(); i++) {
		output << "newpath" << '\n'; // newpath
		output 
			<< c[0][0] * SCALE << ' ' 
			<< c[0][1] * SCALE << ' ' 
			<< c[0][2] * SCALE << ' ' 
			<< 0.0 << ' ' << 360.0 << ' ' << "arc" << '\n';
		output << "closepath" << '\n'; // closepath
		output << "gsave" << '\n'; // as the command 'fill' consumes current path, save current state for the subsequent command
		output << 0.9 << ' ' << "setgray" << '\n';
		output << "fill" << '\n';
		output << "grestore" << '\n'; // continue to work on the path
		output << 0.4 << ' ' << "setgray" << '\n';
		output << LINEWIDTH << ' ' << "setlinewidth" << '\n';
		output << "stroke" << '\n';
	}
	
	/* showpage */
	output << "showpage" << endl;

	output.close();

	return 0;
}