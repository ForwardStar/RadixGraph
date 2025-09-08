#include "headers.h"
#include "optimized_trie.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <workload_file> <sort_setting_file>" << std::endl;
        return 1;
    }
    // Initialize SORT with settings from file
    std::ifstream setting_file(argv[2]);
    if (!setting_file.is_open()) {
        std::cerr << "Error opening setting file: " << argv[2] << std::endl;
        return 1;
    }
    int d;
    setting_file >> d;
    std::vector<int> num_bits(d);
    for (int i = 0; i < d; ++i) {
        setting_file >> num_bits[i];
    }
    setting_file.close();
    SORT sort(d, num_bits);
    std::ifstream infile(argv[1]);
    if (!infile.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }
    std::string line;
    std::vector<int> IDs;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        NodeID id;
        if (!(iss >> id)) { break; } // error
        IDs.push_back(id);
    }
    infile.close();
    // Insert IDs to SORT in original order
    // Print memory usage after 10% insertions
    size_t total = IDs.size();
    std::cout << "Inserting in random order..." << std::endl;
    for (size_t i = 0; i < total; ++i) {
        sort.RetrieveVertex(IDs[i], true);
        if ((i + 1) % (total / 10) == 0)
            std::cout << "Memory after " << (i + 1) << " insertions: " << sort.size() << " bytes" << std::endl;
    }
    // Insert IDs to SORT in increasing order
    SORT sort2(d, num_bits);
    std::cout << "Inserting in increasing order..." << std::endl;
    std::sort(IDs.begin(), IDs.end());
    for (size_t i = 0; i < total; ++i) {
        sort2.RetrieveVertex(IDs[i], true);
        if ((i + 1) % (total / 10) == 0)
            std::cout << "Memory after " << (i + 1) << " insertions: " << sort2.size() << " bytes" << std::endl;
    }
    return 0;
}