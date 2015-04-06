#include "gtest/gtest.h"
#include "Parser.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace fs = ::boost::filesystem;
using namespace std;

class EndToEndTest : public ::testing::Test
{
public:
    std::map<uint32_t, vector<ErrorCode>> TestNumberToMiniumErrors
    {
        {9,  {ErrorCode::Err14}},
        {10, {ErrorCode::Err66}},
        {11, {ErrorCode::Err16, ErrorCode::Err67}},
        {13, {ErrorCode::Err9}},
        {14, {ErrorCode::Err9}},
        {15, {ErrorCode::Err68}},
        {16, {ErrorCode::Err73}},
        {17, {ErrorCode::Err24}}, // TODO should we warn about interuption?
        {18, {ErrorCode::Err5}}, // TODO should we warn about interuption?
        {19, {ErrorCode::Err0}},
        {20, {ErrorCode::Err0}},
        {21, {ErrorCode::Err70}}, // does this need another error?
        {22, {ErrorCode::Err1}},
        {23, {ErrorCode::Err12}},
        {24, {ErrorCode::Err2}},
        {25, {ErrorCode::Err1}},
        {26, {ErrorCode::Err9}},
        {28, {ErrorCode::Err22}},
        {29, {ErrorCode::Err22}},
        {31, {ErrorCode::Err22}},
        {32, {ErrorCode::Err20}},
        {33, {ErrorCode::Err11}},
        {34, {ErrorCode::Err8}},
        {35, {ErrorCode::Err7}},
        {36, {ErrorCode::Err7}},
        {38, {ErrorCode::Err72}},
        {39, {ErrorCode::Err72}},
        {40, {ErrorCode::Err70}},
        {43, {ErrorCode::Err9}},
        {45, {ErrorCode::Err23}},
        {47, {ErrorCode::Err72}},
        {48, {ErrorCode::Err72}},
        {49, {ErrorCode::Err41}}, // TODO a better error for this one
        {53, {ErrorCode::Err73}},
        {54, {ErrorCode::Err74}},
        {55, {ErrorCode::Err75}}
    };

    uint32_t GetTestNumberFromName(const string& test)
    {
        auto firstDigit = test.find_first_of("0123456789");
        auto lastDigit = test.find_last_of("0123456789");
        stringstream number { test.substr(firstDigit, lastDigit) };
        int testNumber;
        number >> testNumber;
        return testNumber;
    }

    bool TestHasExpectedErrors(const string& test, const CompileTimeErrorReporter* errors)
    {
        if (boost::algorithm::ends_with(test, "merge.tig") || boost::algorithm::ends_with(test, "queens.tig"))
        {
            if (errors->HasAnyErrors())
            {
                return false;
            }
            return true;
        }

        auto testNumber = GetTestNumberFromName(test);
        auto expectedErrors = TestNumberToMiniumErrors.find(testNumber);

        if (expectedErrors == end(TestNumberToMiniumErrors))
        {
            if (errors->HasAnyErrors())
            {
                return false;
            }
            return true;
        }
        else if (!errors->HasAnyErrors())
        {
            return false;
        }
        
        // extra errors are ok for now
        for (const auto& err : expectedErrors->second)
        {
            if (!errors->ContainsErrorCode(err))
            {
                return false;
            }
        }
        return true;
    }

	void RunAllTigFilesInTestDirectory()
	{
        bool allPassed = true;
		fs::path path(fs::system_complete("../../tests/TestPrograms"));
		fs::directory_iterator iter(path), eod;
		BOOST_FOREACH(fs::path const &p, std::make_pair(iter, eod))
		{
			if (is_regular_file(p) && boost::algorithm::ends_with(p.filename().string(), ".tig"))
			{
                std::shared_ptr<CompileTimeErrorReporter> errorReporter = std::make_shared<CompileTimeErrorReporter>();
                std::shared_ptr<WarningReporter> warningReporter = std::make_shared<WarningReporter>();
				Parser parser = Parser::CreateParserForFile(p.string(), errorReporter, warningReporter);
				try
				{
					auto prog = parser.Parse();
                    prog->TypeCheck();
				}
				catch (const std::exception& t)
				{
					std::cout << t.what() << " thrown in " << p.filename().string() << "\n";
                    allPassed = false;
				}
                
                if (!TestHasExpectedErrors(p.filename().string(), errorReporter.get()))
                {
                    std::cerr << "++++++++++++++ Failure in " << p.filename().string() << " +++++++++++++\n";
                    errorReporter->ReportAllGivenErrors();
                    warningReporter->ReportAllWarningsAtOrBelowLevel(WarningLevel::High);
                    allPassed = false;
                }
			}
		}

        ASSERT_TRUE(allPassed);
	}
};

TEST_F(EndToEndTest, RunAll)
{
	RunAllTigFilesInTestDirectory();
}
