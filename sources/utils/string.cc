#include "utils/string.h"

#include <string>
#include <vector>
#include <utility>  // std::pair, make_pair()

std::vector<std::wstring> split(std::wstring input, std::wstring delimiter)
{
        std::vector<std::wstring> tokens;
        std::wstring::size_type delimiter_pos;
        
        while((delimiter_pos = input.find(delimiter)) != std::wstring::npos) {
                tokens.push_back(input.substr(0, delimiter_pos));
                input = input.substr(delimiter_pos + 1, input.size());   
        }

        if (input.size() > 0)
                tokens.push_back(input);
        
        return tokens;
}

std::pair<std::wstring, std::wstring> split_once(std::wstring input, std::wstring delimiter)
{
        std::wstring::size_type delimiter_pos = input.find(delimiter);

        if (delimiter_pos == std::wstring::npos)
                return std::make_pair(input, L"");
        else
                return std::make_pair(input.substr(0, delimiter_pos),
                        input.substr(delimiter_pos + 1, input.size()));
}