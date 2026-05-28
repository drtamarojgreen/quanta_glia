#include <fstream>
#include <iostream> // For error reporting

class SorrelTest_CreateFile
{
public:
    static int run(int argc, char** argv)
    {
        std::ofstream outfile("sorrel_created_file.tmp");
        if (outfile.is_open())
        {
            outfile.close();
            // We don't  "delete" it here, as cleanup is a separate question/sip.
            // The fact we could open and close it is recorded as numeric evidence.
            std::cout << "created_file_count = 1" << std::endl;
            std::cout << "create_file_errno = 0" << std::endl;
            return 0;
        }
        else
        {
            std::cerr << "created_file_count = 0" << std::endl;
            std::cerr << "create_file_errno = 1" << std::endl;
            return 1;
        }
    }
};

int main(int argc, char** argv)
{
    return SorrelTest_CreateFile::run(argc, argv);
}
