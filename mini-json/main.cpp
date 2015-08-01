#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/variant/recursive_variant.hpp>

#include <vector>
#include <iostream>

namespace json {
  struct object_t;
  struct array_t;

  typedef boost::variant<boost::recursive_wrapper<object_t>,
			 boost::recursive_wrapper<array_t>,
			 std::string, std::nullptr_t, double, bool> value_t;

  typedef std::pair<std::string, value_t> pair_t;
  
  struct object_t
  {
    std::vector<pair_t> pairs;
  };

  struct array_t
  {
    std::vector<value_t> entries;
  };
}

BOOST_FUSION_ADAPT_STRUCT(json::object_t, (std::vector<json::pair_t>, pairs))
BOOST_FUSION_ADAPT_STRUCT(json::array_t, (std::vector<json::value_t>, entries))

namespace json {
  class formatter : public boost::static_visitor<>
  {
  public:
    formatter(int indent) : indent(indent) {}
    
    void operator()(const object_t& obj) const {
      tab(indent);
      std::cout << '{';
      for (const auto& pair : obj.pairs) {
	formatter(indent + 2)(pair);
	std::cout << ',';
      }
      tab(indent);
      std::cout << '}';
    }

    void operator()(const array_t& arr) const {
      tab(indent);
      std::cout << '[';
      for (const auto& entry : arr.entries) {
	boost::apply_visitor(formatter(indent + 2), entry);
	std::cout << ',';
      }
      tab(indent);
      std::cout << ']';
    }

    void operator()(const pair_t& pair) const {
      tab(indent);
      std::cout << pair.first << " : ";
      boost::apply_visitor(formatter(indent + 2), pair.second);
    }

    void operator()(const std::string& str) const {
      tab(indent);
      std::cout << '"' << str << '"';
    }

    void operator()(const std::nullptr_t& ptr) const {
      tab(indent);
      std::cout << "null";
    }

    void operator()(const double& num) const {
      tab(indent);
      std::cout << num;
    }

    void operator()(const bool& b) const {
      tab(indent);
      if (b)
	std::cout << "true";
      else
	std::cout << "false";
    }

  private:
    void tab(int spaces) const {
      std::cout << std::endl;
      for (int i = 0; i < spaces; ++i)
	std::cout << ' ';
    }

    int indent;
  };
}

namespace json {
  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  struct null_symbol : qi::symbols<char, std::nullptr_t>
  {
    null_symbol() {
      add("null", nullptr);
    }
  } null_;

  template <typename Iterator>
  struct grammar_ : qi::grammar<Iterator, value_t(), ascii::space_type>
  {
    grammar_(): grammar_::base_type(start) {
      using qi::lexeme;
      using qi::double_;
      using qi::bool_;
      using ascii::char_;

      start = value_.alias();
      object_ = '{' >> pair_ % ',' >> '}';
      pair_ = string_ >> ':' >> value_;
      value_ = object_ | array_ | string_ | null_ | double_ | bool_;
      array_ = '[' >> value_ % ',' >> ']';
      string_ = lexeme['"' >> *( char_ - '"' ) >> '"'];
    }

    qi::rule<Iterator, value_t(), ascii::space_type> start;
    qi::rule<Iterator, object_t(), ascii::space_type> object_;
    qi::rule<Iterator, pair_t(), ascii::space_type> pair_;
    qi::rule<Iterator, value_t(), ascii::space_type> value_;
    qi::rule<Iterator, array_t(), ascii::space_type> array_;
    qi::rule<Iterator, std::string(), ascii::space_type> string_;
  };

}

int main() {
  const std::string source = "[ null, 3.14159, false, {\"key\" : \"value\", \"one\" : 1} ]";

  json::grammar_<std::string::const_iterator> g;
  json::value_t v;

  auto first = source.begin();
  auto last = source.end();
  bool r = boost::spirit::qi::phrase_parse(first, last, g, boost::spirit::ascii::space, v);
  if (r) {
    std::cout << "Parsed JSON tree:" << std::endl;
    boost::apply_visitor(json::formatter(0), v);
    std::cout << std::endl;
  }
  return 0;
}
