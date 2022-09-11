#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

#include <Eigen/Dense>
using namespace Eigen;

int main(void) {
	ifstream ifs("Input_P2.txt"); string is; int count = 0; /* Pre-image, Post-image Points */

	/* check if file is empty and ignore the first line */
	if (!getline(ifs, is)) {
		cout << "Error: File Corrupted..." << endl;
		return 1;
	}

	/* read the data and store into the 2d array */
	int mode = 0;
	vector<vector<double>> P, Q;
	while (getline(ifs, is)) {
		if (is.length() == 0) {
			mode = 0; continue;
		}
		else if (is.compare(0, 16, "pre-image points") == 0) {
			mode = 1; continue;
		}
		else if (is.compare(0, 17, "post-image points") == 0) {
			mode = 2; continue;
		}

		istringstream iss(is);
		vector<double> x(2);
		iss >> x[0] >> x[1];

		if (mode == 1) {
			P.push_back(x);
		}
		else if (mode == 2) {
			Q.push_back(x);
		}
	}

	ifs.close(); // close file handle

	/* Check if two dimensions are same */
	if (P.size() == Q.size()) {
		count = P.size();
	}
	else {
		cout << "Error: Number of points is not consistent..." << endl;
		return 2;
	}

#define SHUFFLE 0
	bool Shuffled = false;
#if SHUFFLE
	swap(P[1], P[2]);
	swap(Q[1], Q[2]);
	Shuffled = true;
#endif

	/* Y = tX ... Y^T = X^T * t^T
	-> t^T = (X^T)^-1 * Y^T */
	MatrixXd X(count, 3); MatrixXd Y(count, 2);
	for (int i = 0; i < count; i++) {
		X(i, 0) = P[i][0], X(i, 1) = P[i][1], X(i, 2) = 1;
		Y(i, 0) = Q[i][0], Y(i, 1) = Q[i][1];
	}

	/* [v] 2022. 3. 12 00:58 -> Post the question on the Piazza class: What's the display format of the affine transformation? */
	MatrixXd t = X.householderQr().solve(Y); // using QR solver is more efficient than calculating the inverse
	// X.householderQr().solve(Y); for fast LS Solver 
	// X.jacobiSvd(ComputeThinU | ComputeThinV).solve(Y); for general LS Solver

	/* Split the transposed augmented matrix
	into the transformation matrix and the translation vector */
	Matrix2d A; Vector2d b;
	A << t(0, 0), t(1, 0), t(0, 1), t(1, 1);
	b << t(2, 0), t(2, 1);

	cout << "For the affine transformation y=Ax+b," << '\n'
		<< "The approximate(least square) solution is printed below)" << '\n'
		<< "The transformation matrix A is:" << '\n' << A << '\n'
		<< "The translation vector b is:" << '\n' << b << endl;

	ofstream output;

	/* [v] 2022. 3. 13. 16:05 -> Check the Piazza Q&A answer about output ps scale */
	constexpr float SCALE = 32;
	constexpr float LINEWIDTH = 1;

	string filename;

	/* pre-image points */
	filename.assign("P2_before");
	filename += Shuffled ? "_shuffled" : "";
	filename += ".ps";
	output.open(filename);
	/* magic number */
	output << "%!PS" << '\n';
	output << "newpath" << '\n'; // newpath
	output << P[0][0] * SCALE << ' ' << P[0][1] * SCALE << ' ' << "moveto" << '\n';
	for (int i = 1; i < count; i++) {
		output << P[i][0] * SCALE << ' ' << P[i][1] * SCALE << ' ' << "lineto" << '\n';
	}
	output << "closepath" << '\n'; // closepath
	output << LINEWIDTH << ' ' << "setlinewidth" << '\n';
	output << "stroke" << '\n';
	/* showpage */
	output << "showpage" << endl;

	output.close(); //

	/* post-image points */
	filename.assign("P2_after");
	filename += Shuffled ? "_shuffled" : "";
	filename += ".ps";
	output.open(filename);
	/* magic number */
	output << "%!PS" << '\n';

	/* projected by approximated affine transformation */
#if SHUFFLE
	MatrixXd Yp = t.transpose() * X.transpose();
	output << "newpath" << '\n'; // newpath
	output << Yp(0, 0) * SCALE << ' ' << Yp(1, 0) * SCALE << ' ' << "moveto" << '\n';
	for (int i = 1; i < count; i++) {
		output << Yp(0, i) * SCALE << ' ' << Yp(1, i) * SCALE << ' ' << "lineto" << '\n';
	}
	output << "closepath" << '\n'; // closepath
	output << LINEWIDTH << ' ' << "setlinewidth" << '\n';
	output << "[3] 0 setdash" << '\n';
	output << "0.6 setgray" << '\n';
	output << "stroke" << '\n';
#endif

	/* by data */
	output << "newpath" << '\n'; // newpath
	output << Q[0][0] * SCALE << ' ' << Q[0][1] * SCALE << ' ' << "moveto" << '\n';
	for (int i = 1; i < count; i++) {
		output << Q[i][0] * SCALE << ' ' << Q[i][1] * SCALE << ' ' << "lineto" << '\n';
	}
	output << "closepath" << '\n'; // closepath
	output << LINEWIDTH << ' ' << "setlinewidth" << '\n';
	output << "[] 0 setdash" << '\n';
	output << "0.0 setgray" << '\n';
	output << "stroke" << '\n';
	/* showpage */
	output << "showpage" << endl;

	output.close(); //

	return 0;
}