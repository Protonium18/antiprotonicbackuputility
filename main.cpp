#include <iostream>
#include <fstream>
#include <filesystem>

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

	std::string newpath = *out_path + "/" + *in_path;
	
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

void loadfromtxt(std::string path) {



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

int main() {
	
	int input = 0;
	std::string cinput;
	int iinput;
	bool auto_overwrite = false;

	std::vector<std::string> list_copy;
	std::vector<std::string> list_dest;


	std::cout << "AntiProtonic's Backup Utility" << std::endl;



	while (input < 2) {

		path_pair output = file_dialogue();

		list_copy.push_back(output.src);
		list_dest.push_back(output.dst);
		input++;
	}

	std::cout << "These files will be copied:" << std::endl;
	for (int i = 0; i < list_copy.size(); i++) {
		std::cout << i + " " + list_copy.at(i) + " to " + list_dest.at(i) << std::endl;
	}

	std::cout << "Continue? [y/n]" << std::endl;
	std::cin >> cinput;
	if (cinput == "y") {
		for (int i = 0; i < list_copy.size(); i++) {
			dir_copy(&list_copy.at(i), &list_dest.at(i), auto_overwrite);
		}
	}
	else if (cinput == "n") {
		std::cout << "Alter entry? [y/n]" << std::endl;
		std::cin >> cinput;
		if (cinput == "y") {
			std::cout << "Which entry?" << std::endl;
			std::cin >> iinput;
			path_pair output = file_dialogue();
			list_copy.at(iinput) = output.src;
			list_dest.at(iinput) = output.dst;
			for (int i = 0; i < list_copy.size(); i++) {
				dir_copy(&list_copy.at(i), &list_dest.at(i), auto_overwrite);
			}
			
 		}
		if (cinput == "n") {
			return 0;
		}
	}


	return 0;

}

