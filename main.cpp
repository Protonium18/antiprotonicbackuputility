#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

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

int dir_copy(const std::string* in_path, const std::string* out_path, bool overwrite) {

	int index(in_path->rfind("/"));
	std::string end = in_path->substr(index + 1);
	std::string newpath = *out_path + "/" + end;
	
	auto copyoptions = fs::copy_options::recursive;

	if (overwrite) {
		copyoptions = fs::copy_options::recursive | fs::copy_options::overwrite_existing;
	}

	if (!fs::exists(newpath)) {
		fs::create_directories(newpath);
	}
	else if(fs::exists(newpath) && overwrite == false) {
		std::cout << "Path already exists! Overwrite? [y/n]" << std::endl;
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

	fs::copy(*in_path, newpath, copyoptions);
	return 0;

}

int dir_copy(const path_pair* path, const bool overwrite) {

	std::string in_path = path->src;
	std::string out_path = path->dst;

	int index(in_path.rfind("/"));
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
		std::cout << "Path already exists! Overwrite? [y/n]" << std::endl;
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
	std::cin >> filepath;
	std::cout << "Directory to be copied to." << std::endl;
	std::cin >> filedest;

	output.src = filepath;
	output.dst = filedest;

	return(output);

}

bool copy_confirm(std::vector<path_pair>* list) {

	std::string cinput;
	int iinput = 0;

	std::cout << "These files will be copied:" << std::endl;
	for (int i = 0; i < list->size(); i++) {
		std::cout << list->at(i).src + " to " + list->at(i).dst << std::endl;
	}
	while (true) {
		std::cout << "Continue? [y/n]" << std::endl;
		std::cin >> cinput;
		if (cinput == "y") {
			return true;
		}
		else if (cinput == "n") {
			std::cout << "Alter entry? [y/n]" << std::endl;
			std::cin >> cinput;
			if (cinput == "y") {
				std::cout << "Which entry?" << std::endl;
				std::cin >> iinput;
				path_pair output = file_dialogue();
				list->at(iinput).src = output.src;
				list->at(iinput).dst = output.dst;
			}
			else if (cinput == "n") {
				return false;
			}
		}
	}
}

int loadfromtxt(std::string path) {
	std::ifstream file_open(path, std::ios_base::in);
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
		output_vector.push_back(output);

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
		for (int i = 0; i < path_pairs->size(); i++) {
			dir_copy(&path_pairs->at(i), overwrite);
		}

	}

	delete(path_pairs);

}

void single_copy() {
	path_pair path = file_dialogue();
	dir_copy(&path, false);
}

void multi_copy() {
	int input = 0;
	int count = 0;
	bool overwrite = false;

	std::vector<path_pair> path_pairs;

	std::cout << "How many directories to copy?\n";
	std::cin >> input;
	while (count < input) {
		path_pairs.push_back(file_dialogue());
		count++;
	}
	copy_confirm(&path_pairs);
	for (int i = 0; i < count; i++) {
		dir_copy(&path_pairs.at(i), overwrite);
	}
}

void start_menu() {
	int input = 0;

	std::cout << "1. Single copy\n";
	std::cout << "2. Multi-copy\n";
	std::cout << "3. Load path pairs from .txt\n";

	std::cin >> input;

	if (input == 1) {
		single_copy();
	}

	if (input == 2) {
		multi_copy();
	}

	if (input == 3) {
		loadfromtxt("textinput.txt");
	}


}

int main() {
	
	int input = 0;
	std::string cinput;
	int iinput;
	bool auto_overwrite = false;

	std::vector<std::string> list_copy;
	std::vector<std::string> list_dest;

	std::vector<path_pair> pathpair_list;


	std::cout << "AntiProtonic's Backup Utility" << std::endl;

	start_menu();

	//while (input < 2) {

	//	path_pair output = file_dialogue();
	//	pathpair_list.push_back(output);
	//	input++;
	//}

	//		}
	//		
 //		}
	//	if (cinput == "n") {
	//		return 0;
	//	}
	//}


	return 0;

}

