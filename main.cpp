#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

struct path_pair {
	std::string src;
	std::string dst;

};


void file_open(const std::string* in_path, const std::string* dest_path) {
	std::ifstream src;
	std::ofstream dst;

	src.open(*in_path, std::ios::binary);
	dst.open(*dest_path, std::ios::binary);


	dst << src.rdbuf();

	src.close();
	dst.close();

}

std::string pathfix(const std::string* input) {

	std::string output;

	for (const char i : *input) {
		switch (i) {
		case '\a':  output += "\\a";        break;
		case '\b':  output += "\\b";        break;
		case '\f':  output += "\\f";        break;
		case '\n':  output += "\\n";        break;
		case '\r':  output += "\\r";        break;
		case '\t':  output += "\\t";        break;
		case '\v':  output += "\\v";        break;
		case '/':  output += "\\";        break;
		default:    output += i;            break;

		}
	}

	return(output);

}

int dir_copy(const path_pair* path, const bool overwrite) {

	std::string in_path = pathfix(&path->src);
	std::string out_path = pathfix(&path->dst);

	int index(in_path.rfind("\\"));

	if (index == std::string::npos) {
		index = in_path.rfind("/");
	}

	std::string end = in_path.substr(index + 1);
	std::string newpath = out_path + "/" + end;

	auto copyoptions = fs::copy_options::recursive;

	if (overwrite) {
		copyoptions = fs::copy_options::recursive | fs::copy_options::overwrite_existing;
	}

	if (!fs::exists(newpath)) {
		fs::create_directories(newpath);
	}
	else if (fs::exists(newpath) && overwrite == false) {
		std::cout << newpath + " already exists! Overwrite? [y/n]" << std::endl;
		char input;
		std::cin >> input;
		if (input == 'y') {
			copyoptions = fs::copy_options::recursive | fs::copy_options::overwrite_existing;
		}
		else if (input == 'n') {
			std::cout << "Skipped!" << std::endl;
			return 1;
		}
		else {
			std::cout << "Bad input" << std::endl;
			return 1;
		}
	}



	fs::copy(in_path, newpath, copyoptions);
	return 0;

}

path_pair file_dialogue()
{
	std::string filepath;
	std::string filedest;
	path_pair output;

	std::cout << "Directory to copy." << std::endl;

	while (true) {
		std::cin >> filepath;
		if (fs::exists(filepath)) {
			break;
		}
		else {
			std::cout << "Directory does not exist.\n";
		}
	}

	std::cout << "Directory to be copied to." << std::endl;
	std::cin >> filedest;

	output.src = filepath;
	output.dst = filedest;

	return(output);

}

bool copy_confirm(std::vector<path_pair>* list, bool write = false) {

	std::string cinput;
	unsigned int iinput = 0;
	while (true) {
		if(!write){
			std::cout << "\nThese files will be copied:" << std::endl;
		}
		else if (write) {
			std::cout << "\nThese paths will be written to txt:" << std::endl;
		}

		for (int i = 0; i < list->size(); i++) {
			std::cout << list->at(i).src + " to " + list->at(i).dst << std::endl;
		}
	
		std::cout << "Continue? [y/n]" << std::endl;
		
		while (true) {
			std::cin >> cinput;
			if (cinput == "y") {
				return true;
			}
			else if (cinput == "n") {
				std::cout << "Alter entry? [y/n]" << std::endl;
				while (true) {
					std::cin >> cinput;
					if (cinput == "y") {
						std::cout << "Which entry?" << std::endl;
						while (true) {
							std::cin >> iinput;
							if (std::cin && iinput < list->size()) {
								path_pair output = file_dialogue();
								list->at(iinput).src = output.src;
								list->at(iinput).dst = output.dst;
								break;
							}
							else {
								std::cout << "Invalid entry.\n";
								std::cin.clear();
								std::cin.ignore(10000, '\n');
							}
						}
						break;
					}
					else if (cinput == "n") {
						return false;
					}
					else {
						std::cout << "Invalid entry.\n";
					}
				}
				break;
			}
			else {
				std::cout << "Invalid entry.\n";
			}
		}
	}
}

void overwrite_dialogue(bool* overwrite_ptr) {

	std::string sinput;

	std::cout << "Force overwrite on all files? [y/n]\n";
	std::cin >> sinput;
	if (sinput == "y") {
		*overwrite_ptr = true;
	}
	else {
		return;
	}
}

int loadfromtxt(std::string path = "") {
	std::string path_out;
	while (true) {
		if (path.empty() == true) {
			std::cout << "\nPath to load from?\n";
			std::cin >> path;
		}
		else {
			std::cout << "Loaded from dragged .txt\n";
		}

		path_out = pathfix(&path);

		if (path_out.rfind(".txt") == std::string::npos) {
			std::cout << "Invalid filetype!\n";
			path_out.erase(path_out.begin(), path_out.end());
			path.erase(path.begin(), path.end());
			std::cin.clear();
			std::cin.ignore(10000, '\n');
		}

		else if (!fs::exists(path_out)) {
			std::cout << "Path does not exist!\n";
			path_out.erase(path_out.begin(), path_out.end());
			path.erase(path.begin(), path.end());
			std::cin.clear();
			std::cin.ignore(10000, '\n');
		}

		else {
			break;
		}

	}

	std::ifstream file_open(path_out, std::ios_base::in);
	std::string output;
	std::vector<std::string> output_vector;
	std::vector<path_pair>* path_pairs = new std::vector<path_pair>;
	bool overwrite = false;

	path_pair tmppath;

	if (!file_open.is_open()) {
		std::cout << "File failed to open.\n";
		return 0;
	}

	while (std::getline(file_open, output)) {
		if (output != "") {
			output_vector.push_back(output);
		}

	}

	int pos = 0;


	for (int i = 0; 0 < output_vector.size()/2; i++) {
		path_pair tmppath;

		if (pos == output_vector.size()) {
			break;
		}

		tmppath.src = output_vector.at(pos);
		tmppath.dst = output_vector.at(pos + 1);
		
		path_pairs->push_back(tmppath);

		pos+=2;

	}

	file_open.close();

	if (copy_confirm(path_pairs)) {
		overwrite_dialogue(&overwrite);
		for (int i = 0; i < path_pairs->size(); i++) {
			dir_copy(&path_pairs->at(i), overwrite);
		}

	}

	delete(path_pairs);

}

void savetotxt() {
	std::string txtpath;
	unsigned int iinput = 0;

	std::vector<path_pair> paths_to_save;


	std::cout << "Path of .txt to save?\n";
	std::cin >> txtpath;

	txtpath = pathfix(&txtpath);

	int last_slash = txtpath.rfind("\\");

	std::string dir_path = txtpath.substr(0, last_slash);

	if (!fs::exists(dir_path)) {
		fs::create_directories(dir_path);
		std::cout << "Created directories...\n";
	}

	std::ofstream txt_out(txtpath, std::ios::binary);

	std::cout << "How many paths to save?\n";
	while (!(std::cin >> iinput)) {
		std::cout << "Bad input \n";
		std::cin.clear();
		std::cin.ignore(10000, '\n');
	}
	

	for (int i = 0; i < iinput; i++) {
		paths_to_save.push_back(file_dialogue());
		
	}

	if (copy_confirm(&paths_to_save, true)) {

		for (int i = 0; i < paths_to_save.size(); i++) {
			txt_out.write(paths_to_save.at(i).src.c_str(), paths_to_save.at(i).src.length());
			txt_out.write("\n", 1);
			txt_out.write(paths_to_save.at(i).dst.c_str(), paths_to_save.at(i).dst.length());
			txt_out.write("\n", 1);
		}
		txt_out.close();
	}




}

void single_copy() {
	path_pair path = file_dialogue();
	dir_copy(&path, false);
}

void multi_copy() {
	unsigned int input = 0;
	int count = 0;
	bool overwrite = false;

	std::vector<path_pair> path_pairs;

	std::cout << "How many directories to copy?\n";
	while (!(std::cin >> input)) {
		std::cout << "Invalid entry.\n";
		std::cin.clear();
		std::cin.ignore(10000, '\n');
	}
	while (count < input) {
		path_pairs.push_back(file_dialogue());
		count++;
	}
	copy_confirm(&path_pairs);
	overwrite_dialogue(&overwrite);
	for (int i = 0; i < count; i++) {
		dir_copy(&path_pairs.at(i), overwrite);
	}
}

bool start_menu() {
	unsigned int input = 0;
	std::cout << " \n";
	std::cout << "1. Single copy\n";
	std::cout << "2. Multi-copy\n";
	std::cout << "3. Load path-pairs from .txt\n";
	std::cout << "4. Write path-pairs to .txt\n";
	std::cout << "5. Quit\n";

	while (true) {
		std::cin >> input;
		if (input == 1) {
			single_copy();
			break;
		}

		else if (input == 2) {
			multi_copy();
			break;
		}

		else if (input == 3) {
			loadfromtxt();
			break;
		}
		else if (input == 4) {
			savetotxt();
			break;
		}
		else if (input == 5) {
			return false;
		}
		else {
			std::cout << "Invalid entry.\n";
			std::cin.clear();
			std::cin.ignore(10000, '\n');
		}
	}



}

int main(int argc, char* argv[]) {

	std::cout << "AntiProtonic's Backup Utility" << std::endl;

	if (argc > 1) {
		std::string string = argv[1];
		if (string.rfind(".txt") != std::string::npos) {
			loadfromtxt(argv[1]);
		}
		else {
			std::cout << "Invalid filetype!\n";
			while (start_menu()) {

			}
		}


	}
	
	else {
		while (start_menu()) {

		}
	}
	return 0;

}

