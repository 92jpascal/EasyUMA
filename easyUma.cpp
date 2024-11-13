// Last Updated: 11-12-24

// Include depedencies
#include <iostream>
#include <string>
#include <curl/curl.h>	// cURL to access server data
#include <json/json.h>	// jSON for text formatting and data representation
#include <boost/multiprecision/cpp_int.hpp>	// boost for larger data types

using namespace boost::multiprecision;

// Callback function for libcurl to capture response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to check Ethereum wallet balance
void checkBalance(const std::string& infura_url, const std::string& eth_address) {
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();

    if (curl) {
        // Prepare JSON-RPC request
        Json::Value json_request;
        json_request["jsonrpc"] = "2.0";
        json_request["method"] = "eth_getBalance";
        Json::Value params;
        params.append(eth_address);
        params.append("latest");
        json_request["params"] = params;
        json_request["id"] = 1;

        Json::StreamWriterBuilder writer;
        std::string request_data = Json::writeString(writer, json_request);

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, infura_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // Set headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Capture response
        std::string response_string;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            // Print the full response for debugging
            std::cout << "Full JSON Response: " << response_string << std::endl;

            // Parse the JSON response
            Json::CharReaderBuilder reader;
            Json::Value json_response;
            std::string errs;
            std::istringstream ss(response_string);

            if (Json::parseFromStream(reader, ss, &json_response, &errs)) {
                if (json_response.isMember("error")) {
                    std::cerr << "Error from server: " << json_response["error"]["message"].asString() << std::endl;
                    return;
                }

                if (!json_response.isMember("result")) {
                    std::cerr << "Error: 'result' field is missing in the response." << std::endl;
                    return;
                }

                // Get the balance in hexadecimal format
                std::string balance_hex = json_response["result"].asString();

                // Ensure the balance has the "0x" prefix and remove it
                if (balance_hex.rfind("0x", 0) == 0) { // check if balance_hex starts with "0x"
                    balance_hex = balance_hex.substr(2);
                }

                try {
                    // Convert hex balance to boost::multiprecision::uint256_t
                    uint256_t balance("0x" + balance_hex);
                    std::cout << "Balance: " << balance << " wei" << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "Error: Balance value is too large to convert. " << e.what() << std::endl;
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error: Invalid balance format. " << e.what() << std::endl;
                }
            } else {
                std::cerr << "Failed to parse JSON response: " << errs << std::endl;
            }
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

int main() {
    std::string infura_url = "https://mainnet.infura.io/v3/API_KEY"; // Infura API Key
    std::string eth_address = "0xde0B295669a9FD93d5F28D9Ec85E40f4cb697BAe"; // Example Ethereum address

    std::cout << "Checking Ethereum balance for address: " << eth_address << std::endl;
    checkBalance(infura_url, eth_address);

    return 0;
}

