#include <opencv2/opencv.hpp>
#include <iostream>

//extract parameter statements with:   grep "//parm" ../src/contours.cpp | awk -F'//parm' '{printf "cout << \"" $2 "\" << endl << endl;\n" }'


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
		cout << "Usage: twhitepatch <inputimage> [threshold=<int>|epsilon=<float>|border|minarea=<int>|destimage=<outputimage>]...\n\n";
		cout << "Where:\n\n";
		
cout << " threshold: rubicon between black and white for the gray->binary translation, betwee 0 and 255.  Default: 128" << endl << endl;
cout << " resize: if defined, resizes the input image using WxH, e.g., 'resize:640x480'.  If either width or height is not specified, the resize of that dimension is calculated with the proportion of the defined dimension to the original image dimension, e.g., 'resize:640', or 'resize:x480'" << endl << endl;
cout << " epsilon: The value used to specify the degree of simplification of contours, larger is simpler.  Set to 0.0 to disable.  Default: 3.0" << endl << endl;
cout << " border: if defined, draws a white border around the image, useful for isolating contours that butt up against the image edge.  Default width: 1" << endl << endl;
cout << " minarea: minimum area of a valid contour.  Default: 0" << endl << endl;
cout << " minpoints: culls polygons with number of points less than this number.  Default: 4" << endl << endl;
cout << " destimage: if defined, outputs the original image wtih the contours drawn in red.  If not defined, program dumps an OpenSCAD array called 'p', contains the contours defined as point lists." << endl << endl;


		cout << endl;
		exit(EXIT_SUCCESS);
		
	} //err("No input image specified");
    Mat image = imread(argv[1]);

    if (image.empty()) err("Could not open or find the image."); 
	
	unsigned thresh = 128;
	float epsilon = 3.0;
	bool border = false, resize = false;
	int bw = 1;  // border width, default = 1
	unsigned minarea = 0, minpoints=4;
	unsigned rw, rh;
	
	string destimage = "";
	
	for (unsigned i=1; i<argc; i++) {
		if(string(argv[i]).find("threshold") != string::npos) {  //parm threshold: rubicon between black and white for the gray->binary translation, betwee 0 and 255.  Default: 128
			string t = val(argv[i]);
			if (t.size() > 0) thresh = atoi(t.c_str());
		}
		if(string(argv[i]).find("resize") != string::npos) {  //parm resize: if defined, resizes the input image using WxH, e.g., 'resize:640x480'.  If either width or height is not specified, the resize of that dimension is calculated with the proportion of the defined dimension to the original image dimension, e.g., 'resize:640', or 'resize:x480'
			string r = val(argv[i]);
			vector<string> dim = split(r, "x");
			if (dim[0].size() == 0 & dim[1].size() == 0) err("invalid dimension");
			if (dim.size() == 1) {
				rw = atoi(r.c_str());
				rh = int((float) image.rows * ((float) rw / (float) image.cols));
				resize = true;
			}
			else if (dim.size() == 2) {
				if (dim[0].size() == 0) 
					rw = int((float) image.cols * ((float) rh / (float) image.rows));
				else 
					rw = atoi(dim[0].c_str());
				rh = atoi(dim[1].c_str());
				resize = true;
			}
		}
		else if(string(argv[i]).find("epsilon") != string::npos) {  //parm epsilon: The value used to specify the degree of simplification of contours, larger is simpler.  Set to 0.0 to disable.  Default: 3.0
			string e = val(argv[i]);
			if (e.size() > 0) epsilon = atoi(e.c_str());
		}
		else if(string(argv[i]).find("border") != string::npos) { //parm border: if defined, draws a white border around the image, useful for isolating contours that butt up against the image edge.  Default width: 1
			border = true;
			string b = val(argv[i]);
			if (b.size() > 0) bw = atoi(b.c_str());
		}
		else if(string(argv[i]).find("minarea") != string::npos) { //parm minarea: minimum area of a valid contour.  Default: 0
			string m = val(argv[i]);
			if (m.size() > 0) minarea = atoi(m.c_str());
		}
		else if(string(argv[i]).find("minpoints") != string::npos) {  //parm minpoints: culls polygons with number of points less than this number.  Default: 4
			string m = val(argv[i]);
			if (m.size() > 0) minpoints = atoi(m.c_str());
		}
		else if(string(argv[i]).find("destimage") != string::npos) {  //parm destimage: if defined, outputs the original image wtih the contours drawn in red.  If not defined, program dumps an OpenSCAD array called 'p', contains the contours defined as point lists.
			destimage = val(argv[i]);
		}
	}
	fprintf(stderr, "image dimensions: %dx%d\n", image.rows, image.cols);
	fprintf(stderr, "threshold: %d  epsilon: %0.2f\n", thresh, epsilon);
	if (resize) fprintf(stderr, "resize: %dx%d\n", rw, rh);
	if (destimage.size() > 0) fprintf(stderr, "destimage: %s\n", destimage.c_str());
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
		if (contour.size() <= minpoints) continue;  //invalid contour
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
