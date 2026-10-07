#include <filesystem>
#include <iostream>
#include <regex>

using namespace std;
namespace fs = std::filesystem;

void fix_name(const fs::directory_entry entry)
{
	if (!entry.is_regular_file()) {
		return;
	}
	const string prev_filename = entry.path().filename().stem().string();

	// If no pluses or spaces in the name, skip the file.
	if (prev_filename.find_first_of("+", 0) == string::npos || prev_filename.find_first_of(" ", 0) != string::npos) {
		return;
	}

	// Replace plusses with spaces.
	regex needle("([+])");
	string output = regex_replace(prev_filename.c_str(), needle, " ");

	// Replace double spaces with ' +' as we assume in these cases that the plus was intended to be there.
	regex double_space("  ");
	output = regex_replace(output.c_str(), double_space, " +");

	// Trim whitespace from beginning and end of name
	output.erase(0, output.find_first_not_of(' '));
	output = output.substr(0, output.find_last_not_of(' ') + 1);

	// Re-form full path
	const string ext = entry.path().filename().extension().string();
	const fs::path after = entry.path().parent_path().append(output + ext);

	fs::rename(entry.path().c_str(), after.c_str());
	cout << entry.path() << "  ->  " << after << endl;
}

int main()
{
	for (fs::directory_entry entry : fs::directory_iterator(".")) {
		fix_name(entry);
	}
}
