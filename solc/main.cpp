/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @author Christian <c@ethdev.com>
 * @date 2014
 * Solidity commandline compiler.
 */

#include <solc/CommandLineInterface.h>
#include <boost/exception/all.hpp>
#include <clocale>
#include <iostream>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

using namespace std;

/*
The equivalent of setlocale(LC_ALL, "C") is called before any user code is run.
If the user has an invalid environment setting then it is possible for the call
to set locale to fail, so there are only two possible actions, the first is to
throw a runtime exception and cause the program to quit (default behaviour),
or the second is to modify the environment to something sensible (least
surprising behaviour).

The follow code produces the least surprising behaviour. It will use the user
specified default locale if it is valid, and if not then it will modify the
environment the process is running in to use a sensible default. This also means
that users do not need to install language packs for their OS.
*/
static void setDefaultOrCLocale()
{
#if __unix__
	if (!std::setlocale(LC_ALL, ""))
	{
		setenv("LC_ALL", "C", 1);
	}
#endif
}

static void initGmssl() {
    /* Load the human readable error strings for libcrypto */
    ERR_load_crypto_strings();

    /* Load all digest and cipher algorithms */
    OpenSSL_add_all_algorithms();

    /* Load config file, and other important initialisation */
    // OPENSSL_config(NULL);
}

static void cleanGmssl() {
   /* Removes all digests and ciphers */
    EVP_cleanup();

    /* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
    CRYPTO_cleanup_all_ex_data();

    /* Remove error strings */
    ERR_free_strings();
}

int main(int argc, char** argv)
{
	setDefaultOrCLocale();
	initGmssl();
	dev::solidity::CommandLineInterface cli;
	if (!cli.parseArguments(argc, argv)) {
		// cleanGmssl();
		return 1;
	}
	if (!cli.processInput()) {
		cleanGmssl();
		return 1;
	}
	bool success = false;
	try
	{
		success = cli.actOnInput();
	}
	catch (boost::exception const& _exception)
	{
		cerr << "Exception during output generation: " << boost::diagnostic_information(_exception) << endl;
		success = false;
	}

	cleanGmssl();
	return success ? 0 : 1;
}
