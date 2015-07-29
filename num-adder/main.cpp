#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <iostream>
#include <string>
#include <vector>

namespace client
{
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  // Use global variable since we are single-threaded
  double sum;

  template <typename Iterator>
  bool parse_numbers(Iterator first, Iterator last, std::vector<double>& v)
  {
    using qi::double_;
    using qi::phrase_parse;
    using qi::_1;
    using ascii::space;
    using boost::phoenix::ref;

    auto init = [](const double& n) { sum = n; };  // no need to capture sum since it's global
    auto add = [](const double& n) { sum += n; };
    auto store = [&v](const double& n) { v.push_back(n); };

    auto init_store = [&](const double& n) { init(n); store(n); };
    auto add_store = [&](const double& n) { add(n); store(n); };

    /*
    bool r = phrase_parse(first, last,
			  // Begin grammar
			  double_[init_store] >> *(',' >> double_[add_store]),
			  // End grammar
			  space);
    */

    // Equivalent to the above code (using attribute)
    bool r = phrase_parse(first, last,
			  double_[init] >> *(',' >> double_[add]),
			  space,
			  v);

    if (!r || first != last) // fail if we did not get a full match
      return false;

    return r;
  }
}

int main()
{
  std::cout << "Give me a comma separated list of numbers. Hit Enter to quit.\n";

  std::string str;
  while (getline(std::cin, str))
    {
      if (str.empty())
	break;

      std::vector<double> v;
      if (client::parse_numbers(str.begin(), str.end(), v))
	{
	  std::cout << "Parsing succeeded";
	  std::cout << " Sum: " << client::sum << std::endl;
	  std::cout << "Stored numbers: {";
	  for (double n : v) {
	    std::cout << n << ", ";
	  }
	  std::cout << "}" << std::endl;
	  std::cout << "-------------------------\n";
	}
      else
	{
	  std::cout << "Parsing failed\n";
	  std::cout << "-------------------------\n";
	}
    }

  std::cout << "Bye... :-) \n\n";
  return 0;
}
