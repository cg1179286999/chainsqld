//------------------------------------------------------------------------------
/*
This file is part of rippled: https://github.com/ripple/rippled
Copyright (c) 2012, 2013 Ripple Labs Inc.

Permission to use, copy, modify, and/or distribute this software for any
purpose  with  or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#include <string>

#include <ripple/beast/unit_test.h>
#include <ripple/basics/StringUtilities.h>

#include "FakeExtVM.h"

/*
* usage:
	--unittest="VM" --unittest-arg="code=0x... input=0x..."
*/

namespace ripple {

size_t fromHex(const std::string& hex, std::string& binary)
{
	std::string pure_hex;
	if (hex[0] == '0' && hex[1] == 'x') {
		pure_hex = hex.substr(2);
	}
	else {
		pure_hex = hex;
	}
	std::pair<Blob, bool> ret = strUnHex(pure_hex);
	if (ret.second == false)
		return 0;
	binary.assign(ret.first.begin(), ret.first.end());
	return binary.size();
}

class VM_test : public beast::unit_test::suite {
public:
	VM_test() {

	}

    void run() {
		init_env();
		call();
		createAndCall();
		pass();
	}

private:
	void init_env() {
		std::string args = arg();
		size_t code_npos = args.find("code=");
		size_t input_npos = args.find("input=");

		std::string code;
		if (code_npos != std::string::npos) {
			if (input_npos != std::string::npos) {
				code = args.substr(code_npos + 5, input_npos - 6);
			}
			else {
				code = args.substr(code_npos + 5);
			}
			fromHex(code, code_);
		}

		if (input_npos != std::string::npos) {
			std::string data;
			data = args.substr(input_npos + 6);

			if (data.size())
				fromHex(data, data_);
		}
	}

	void call() {
		bytes code;
		code.assign(code_.begin(), code_.end());
		bytesConstRef data((uint8_t*)data_.c_str(), data_.size());
		FakeExecutive execute(data, code);
		evmc_address contractAddress = { { 1,2,3,4 } };
		execute.call(contractAddress);
	}

	void createAndCall() {
		bytes code;
		code.assign(code_.begin(), code_.end());
		evmc_address contractAddress = { {1,2,3,4} };
		{
			FakeExecutive execute(code);
			execute.create(contractAddress);
		}
		{
			bytesConstRef data((uint8_t*)data_.c_str(), data_.size());
			FakeExecutive execute(data, contractAddress);
			execute.call(contractAddress);
		}
	}

	std::string code_;
	std::string data_;
};
BEAST_DEFINE_TESTSUITE_MANUAL(VM, evm, ripple);
}
