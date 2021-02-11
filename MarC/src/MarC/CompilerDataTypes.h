#pragma once

#include <string>
#include <vector>

namespace MarC
{
	enum class TokenName
	{
		Unknown,
		Separator,
		Identifier,
		Operator,
		Comment,
		LiteralNumber,
		LiteralChar,
		LiteralString,
	};
	struct Token
	{
		TokenName name = TokenName::Unknown;
		std::string value = "";
	};
	typedef std::vector<Token> TokenList;
}