add_library(amethyst_general
	base_logger.cpp
	fd_stream.cpp
	inspect.cpp
	log_formatter.cpp
	string_tokenizer.cpp
	stream_reference.cpp
	string_dumpable.cpp
)

unit_test(test_tokenizer LIBS amethyst_general)
