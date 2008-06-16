#include "general/string_tokenizer.hpp"

#include "test_framework/testinclude.hpp"

#include <iostream>
#include <ostream>

#define ERROR_TEXT(text) ((++error_count),text)

int main(int argc, const char** argv)
{
	using std::cout;
	using std::endl;
	int error_count = 0;
	const char* passed = "Passed";
	const char* failed = "FAILED!";

	using namespace amethyst;
	using std::string;

	try
	{
		quick_vector<string> empty_vector;
		string empty_string;

		TEST_RESULT_NAMED(
			"if the size of an empty string returns an empty vector ",
			tokenize(empty_string) == empty_vector,
			passed,
			ERROR_TEXT(failed));

		TEST_RESULT_NAMED(
			"if null delimiters return the original string",
			tokenize("A B C D E F G H I J", NULL) == quick_vector<string>(1, "A B C D E F G H I J"),
			passed,
			ERROR_TEXT(failed));

		const string junk1 = "A B	C D\nE F G";
		const string foo1[] = { "A", "B", "C", "D", "E", "F", "G" };
		quick_vector<string> foo1_vec(foo1, foo1 + sizeof(foo1) / sizeof(*foo1));
		quick_vector<string> result1_vec = tokenize(junk1);
		TEST_RESULT_NAMED(
			"if simple tokenization works",
			result1_vec == foo1_vec,
			passed,
			ERROR_TEXT(failed));

		const string junk2 = "A B C D";
		const string foo2[] = { "A", " ", "B", " ", "C", " ", "D" };
		quick_vector<string> foo2_vec(foo2, foo2 + sizeof(foo2) / sizeof(*foo2));
		quick_vector<string> result2_vec = tokenize(junk2, " ", tokenizer::RETURN_DELIMITERS);
		TEST_RESULT_NAMED(
			"if tokenization successfully returns delimiters",
			result2_vec == foo2_vec,
			passed,
			ERROR_TEXT(failed));

		const string junk3 = "A  B  C  D";
		const string foo3[] = { "A", " ", "", " ", "B", " ", "", " ", "C", " ", "", " ", "D" };
		quick_vector<string> foo3_vec(foo3, foo3 + sizeof(foo3) / sizeof(*foo3));
		quick_vector<string> result3_vec = tokenize(junk3, " ", tokenizer::RETURN_DELIMITERS, tokenizer::RETURN_EMPTY_TOKENS);
		TEST_RESULT_NAMED(
			"if tokenization successfully returns empty tokens and delimiters",
			result3_vec == foo3_vec,
			passed,
			ERROR_TEXT(failed));

		const string junk4 = "A  B  C  D";
		const string foo4[] = { "A", "", "B", "", "C", "", "D" };
		quick_vector<string> foo4_vec(foo4, foo4 + sizeof(foo4) / sizeof(*foo4));
		quick_vector<string> result4_vec = tokenize(junk4, " ", tokenizer::IGNORE_DELIMITERS, tokenizer::RETURN_EMPTY_TOKENS);
		TEST_RESULT_NAMED(
			"if tokenization successfully returns empty tokens (without delimiters)",
			result4_vec == foo4_vec,
			passed,
			ERROR_TEXT(failed));

		const string junk5 = "A  B  C  D  ";
		const string foo5[] = { "A", "", "B", "", "C", "", "D", "" };
		quick_vector<string> foo5_vec(foo5, foo5 + sizeof(foo5) / sizeof(*foo5));
		quick_vector<string> result5_vec = tokenize(junk5, " ", tokenizer::IGNORE_DELIMITERS, tokenizer::RETURN_EMPTY_TOKENS);
		TEST_RESULT_NAMED(
			"if tokenization successfully returns empty tokens (without delimiters) at end of string",
			result5_vec == foo5_vec,
			passed,
			ERROR_TEXT(failed));

		if( !error_count )
		{
			cout << "----------------------------------" << endl;
			cout << "*** All string_tokenizer tests passed. ***" << endl;
			cout << "----------------------------------" << endl;
			return 0;
		}
		else
		{
			cout << "---------------------------------" << endl;
			cout << "ERROR: Failed " << error_count << " string_tokenizer tests." << endl;
			cout << "---------------------------------" << endl;
			return(2);
		}
	}
	catch(std::exception& e)
	{
		std::cerr << "TEST ERROR: An exception leaked out: " << e.what() << endl;
		return(1);
	}
}

// function string_tokenizer
