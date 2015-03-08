#include "gtest/gtest.h"
#include "Parser.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace fs = ::boost::filesystem;

class EndToEndTest : public ::testing::Test
{
public:
    bool FileShouldFailToTypeCheck(const char* error, const std::string& filename)
    {
        if (true /* short circut all of these for now, let's see dem errors*/ || !boost::algorithm::ends_with(filename, "32.tig")
            && !boost::algorithm::ends_with(filename, "33.tig")
            && !boost::algorithm::ends_with(filename, "20.tig")
            )
        {
            std::cout << error << " thrown in " << filename << "\n";
            return false;
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
				catch (const ParseException& t)
				{
					if (!boost::algorithm::ends_with(p.filename().string(), "49.tig"))
					{
						std::cout << t.what() << " thrown in " << p.filename().string() << "\n";
                        allPassed = false;
					}
				}
                catch (const SemanticAnalysisException& t)
				{
                    allPassed = FileShouldFailToTypeCheck(t.what(), p.filename().string()) && allPassed;
				}
				catch (const std::exception& t)
				{
					std::cout << t.what() << " thrown in " << p.filename().string() << "\n";
                    allPassed = false;
				}

                std::cerr << "++++++++++++++ " << p.filename().string() << " +++++++++++++\n";
                errorReporter->ReportAllGivenErrors();
                warningReporter->ReportAllWarningsAtOrBelowLevel(WarningLevel::High);
			}
		}

        ASSERT_TRUE(allPassed);
	}
};

TEST_F(EndToEndTest, RunAll)
{
	RunAllTigFilesInTestDirectory();
}
