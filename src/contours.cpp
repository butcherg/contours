#include <opencv2/opencv.hpp>
#include <iostream>


using namespace cv;
using namespace std;

std::vector<std::string> split(std::string s, std::string delim)
{
	std::vector<std::string> v;
	if (s.find(delim) == std::string::npos) {
		v.push_back(s);
		return v;
	}
	size_t pos=0;
	size_t start;
	while (pos < s.length()) {
		start = pos;
		pos = s.find(delim,pos);
		if (pos == std::string::npos) {
			v.push_back(s.substr(start,s.length()-start));
			return v;
		}
		v.push_back(s.substr(start, pos-start));
		pos += delim.length();
	}
	return v;
}

void err(string msg)
{
	cout << msg << endl;
	exit(EXIT_FAILURE);
}

string val(string nameval)
{
	vector<string> nv = split(nameval, "=");
	if (nv.size() >= 2) return nv[1];
	return "";
}

int main(int argc, char **argv) {
	if (argc < 2) {	
		cout << "Usage:\n";
		cout << "\twhitepatch <inputimage> [threshold=<int>|epsilon=<float>|border|minarea=<int>|destimage=<outputimage>]...\n\n";
		cout << "\tWhere:\n";
		cout << "\t\tthreshold: The value between determining if a pixel goes white or black in the gray->binary translation. Default: 128\n";
		cout << "\t\tepsilon: The value used to specify the degree of simplification of contours, larger is simpler.\n";
		cout << "\t\t\tSet to 0.0 to disable.  Default: 3.0\n";
		cout << "\t\tborder: If present, a 2px white border is added to the image to isolate the contours.\n";
		cout << "\t\tminarea: Contours with area less than this number are culled from the list.\n";
		cout << "\t\tdestimage: Filename of the image upon which to display the contours drawn on the input image.\n";
		cout << "\t\t\tIf not present, the OpenSCAD array of contour arrays is printed for redirection to a text file.\n";
		cout << endl;
		exit(EXIT_SUCCESS);
		
	} //err("No input image specified");
    Mat image = imread(argv[1]);

    if (image.empty()) err("Could not open or find the image."); 
	
	unsigned thresh = 128;
	float epsilon = 3.0;
	bool border = false;
	int bw = 1;  // border width, default = 1
	unsigned minarea = 0;
	
	string destimage = "";
	
	for (unsigned i=1; i<argc; i++) {
		if(string(argv[i]).find("threshold") != string::npos) {
			string t = val(argv[i]);
			if (t.size() > 0) thresh = atoi(t.c_str());
		}
		else if(string(argv[i]).find("epsilon") != string::npos) {
			string e = val(argv[i]);
			if (e.size() > 0) epsilon = atoi(e.c_str());
		}
		else if(string(argv[i]).find("border") != string::npos) {
			border = true;
			string b = val(argv[i]);
			if (b.size() > 0) bw = atoi(b.c_str());
		}
		else if(string(argv[i]).find("minarea") != string::npos) {
			string m = val(argv[i]);
			if (m.size() > 0) minarea = atoi(m.c_str());
		}
		else if(string(argv[i]).find("destimage") != string::npos) {
			destimage = val(argv[i]);
		}
	}
	fprintf(stderr, "threshold: %d  epsilon: %0.2f ", thresh, epsilon);
	if (destimage.size() > 0) fprintf(stderr, "destimage: %s\n", destimage.c_str()); else fprintf(stderr, "\n");
	fflush(stderr);
	
	if (border) {
		copyMakeBorder( image, image, bw, bw, bw, bw, BORDER_CONSTANT, Scalar(255,255,255) );
	}

    // Convert to grayscale
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // Threshold to create a binary image
    Mat binary;
    threshold(gray, binary, thresh, 255, THRESH_BINARY);

    // Find contours
    vector<vector<Point>> contours, culledcontours;
    findContours(binary, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	
	//cull contours and points
	for (const auto& contour : contours) {
		if (contour.size() <= 4) continue;  //invalid contour
		if (contourArea(contour) < minarea) continue;  //too small
		vector<Point> pts;
		if (epsilon > 0.0) {  //
			vector<Point> pts;
			approxPolyDP(contour, pts, epsilon, true);
			culledcontours.push_back(pts);
		}
		else {
			culledcontours.push_back(contour);
		}
	}
	
	fprintf(stderr, "poly count: %ld\n", culledcontours.size()); fflush(stderr);

	if (destimage.size() == 0) { // spit out OpenSCAD polygon points
		cout << "p = [" << endl;
		for (const auto& contour : culledcontours) {
			cout << "  [" << endl;
			for (const auto& point : contour) {
				cout << "    [" << point.x << "," << point.y << "]"; // << endl;
				if (point != contour[contour.size()-1]) cout << "," << endl; else cout << endl;
				
			}
			cout << "  ]" ; //<< endl;
			if (contour != culledcontours[culledcontours.size()-1]) cout << "," << endl; else cout << endl;
		}
		cout << "];" << endl;
	}
	else {
		for (const auto& contour : culledcontours) 
			drawContours(image, vector<vector<Point>>{contour}, 0, Scalar(0, 0, 255), 2);
		cv::imwrite(destimage, image);
	}

    return 0;
}
