#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BPI");
    BOOST_CHECK(GetBoolArg("-BPI"));
    BOOST_CHECK(GetBoolArg("-BPI", false));
    BOOST_CHECK(GetBoolArg("-BPI", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BELLYo"));
    BOOST_CHECK(!GetBoolArg("-BELLYo", false));
    BOOST_CHECK(GetBoolArg("-BELLYo", true));

    ResetArgs("-BPI=0");
    BOOST_CHECK(!GetBoolArg("-BPI"));
    BOOST_CHECK(!GetBoolArg("-BPI", false));
    BOOST_CHECK(!GetBoolArg("-BPI", true));

    ResetArgs("-BPI=1");
    BOOST_CHECK(GetBoolArg("-BPI"));
    BOOST_CHECK(GetBoolArg("-BPI", false));
    BOOST_CHECK(GetBoolArg("-BPI", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBELLY");
    BOOST_CHECK(!GetBoolArg("-BPI"));
    BOOST_CHECK(!GetBoolArg("-BPI", false));
    BOOST_CHECK(!GetBoolArg("-BPI", true));

    ResetArgs("-noBELLY=1");
    BOOST_CHECK(!GetBoolArg("-BPI"));
    BOOST_CHECK(!GetBoolArg("-BPI", false));
    BOOST_CHECK(!GetBoolArg("-BPI", true));

    ResetArgs("-BPI -noBELLY");  // -BPI should win
    BOOST_CHECK(GetBoolArg("-BPI"));
    BOOST_CHECK(GetBoolArg("-BPI", false));
    BOOST_CHECK(GetBoolArg("-BPI", true));

    ResetArgs("-BPI=1 -noBELLY=1");  // -BPI should win
    BOOST_CHECK(GetBoolArg("-BPI"));
    BOOST_CHECK(GetBoolArg("-BPI", false));
    BOOST_CHECK(GetBoolArg("-BPI", true));

    ResetArgs("-BPI=0 -noBELLY=0");  // -BPI should win
    BOOST_CHECK(!GetBoolArg("-BPI"));
    BOOST_CHECK(!GetBoolArg("-BPI", false));
    BOOST_CHECK(!GetBoolArg("-BPI", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BPI=1");
    BOOST_CHECK(GetBoolArg("-BPI"));
    BOOST_CHECK(GetBoolArg("-BPI", false));
    BOOST_CHECK(GetBoolArg("-BPI", true));

    ResetArgs("--noBELLY=1");
    BOOST_CHECK(!GetBoolArg("-BPI"));
    BOOST_CHECK(!GetBoolArg("-BPI", false));
    BOOST_CHECK(!GetBoolArg("-BPI", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BPI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BPI", "eleven"), "eleven");

    ResetArgs("-BPI -bar");
    BOOST_CHECK_EQUAL(GetArg("-BPI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BPI", "eleven"), "");

    ResetArgs("-BPI=");
    BOOST_CHECK_EQUAL(GetArg("-BPI", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BPI", "eleven"), "");

    ResetArgs("-BPI=11");
    BOOST_CHECK_EQUAL(GetArg("-BPI", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BPI", "eleven"), "11");

    ResetArgs("-BPI=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BPI", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BPI", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BPI", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BPI", 0), 0);

    ResetArgs("-BPI -bar");
    BOOST_CHECK_EQUAL(GetArg("-BPI", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BPI=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BPI", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BPI=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BPI", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BPI");
    BOOST_CHECK_EQUAL(GetBoolArg("-BPI"), true);

    ResetArgs("--BPI=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BPI", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBELLY");
    BOOST_CHECK(!GetBoolArg("-BPI"));
    BOOST_CHECK(!GetBoolArg("-BPI", true));
    BOOST_CHECK(!GetBoolArg("-BPI", false));

    ResetArgs("-noBELLY=1");
    BOOST_CHECK(!GetBoolArg("-BPI"));
    BOOST_CHECK(!GetBoolArg("-BPI", true));
    BOOST_CHECK(!GetBoolArg("-BPI", false));

    ResetArgs("-noBELLY=0");
    BOOST_CHECK(GetBoolArg("-BPI"));
    BOOST_CHECK(GetBoolArg("-BPI", true));
    BOOST_CHECK(GetBoolArg("-BPI", false));

    ResetArgs("-BPI --noBELLY");
    BOOST_CHECK(GetBoolArg("-BPI"));

    ResetArgs("-noBELLY -BPI"); // BPI always wins:
    BOOST_CHECK(GetBoolArg("-BPI"));
}

BOOST_AUTO_TEST_SUITE_END()
