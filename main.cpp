#include <iostream>
#include <string>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

const std::string api_key = ""; // Here you need to put your API key for FixerAPI

int main(int argc, char* argv[]) {
    std::string vname, date;
    int vnom;
    bool time_series;
    if (argc == 4) {
        std::string arg1 = argv[1];
        std::string arg2 = argv[2];
        std::string arg3 = argv[3];

        int pos;

        if ((pos = arg1.find("--Vname=")) != std::string::npos && pos == 0) {
            vname = arg1.substr(pos + 8);
        } else {
            std::cerr << "Invalid Vname argument" << "\n";
            exit(1);
        }

        if ((pos = arg2.find("--Vnom=")) != std::string::npos && pos == 0) {
            vnom = std::stoi(arg2.substr(pos + 7));
        } else {
            std::cerr << "Invalid Vnom argument" << "\n";
            exit(1);
        }

        if ((pos = arg3.find("--DateDate=")) != std::string::npos && pos == 0) {
            date = arg3.substr(pos + 11);
            time_series = true;
        } else if ((pos = arg3.find("--Date=")) != std::string::npos && pos == 0) {
            date = arg3.substr(pos + 7);
            time_series = false;
        } else {
            std::cerr << "Invalid date argument" << "\n";
            exit(1);
        }
    } else {
        std::cerr << "Invalid number of arguments" << "\n";
        exit(1);
    }

    std::string start_date, end_date;
    if (time_series) {
        if (date.find("-") != std::string::npos) {
            int pos = date.find("-");
            std::string start_date_raw = date.substr(0, pos);
            std::string end_date_raw = date.substr(pos + 1);

            std::replace(start_date_raw.begin(), start_date_raw.end(), '.', '-');
            std::replace(end_date_raw.begin(), end_date_raw.end(), '.', '-');

            start_date = start_date_raw.substr(6, 4) + "-" + start_date_raw.substr(3, 2) + "-" + start_date_raw.substr(0, 2);
            end_date = end_date_raw.substr(6, 4) + "-" + end_date_raw.substr(3, 2) + "-" + end_date_raw.substr(0, 2);
        }
    } else {
        std::replace(date.begin(), date.end(), '.', '-');
        date = date.substr(6, 4) + "-" + date.substr(3, 2) + "-" + date.substr(0, 2);
    }

    cpr::Response r;

    if (time_series) {
        r = cpr::Get(cpr::Url{"https://api.apilayer.com/fixer/timeseries"},
                     cpr::Header{{"apikey", api_key}},
                     cpr::Parameters{{"start_date", start_date},
                                     {"end_date", end_date},
                                     {"base", "USD"},
                                     {"symbols", "RUB"}});
    } else {
        r = cpr::Get(cpr::Url{"https://api.apilayer.com/fixer/" + date},
                     cpr::Header{{"apikey", api_key}},
                     cpr::Parameters{{"base", "USD"},
                                     {"symbols", "RUB"}});
    }

    nlohmann::json j = nlohmann::json::parse(r.text);

    if (j["success"] != true) {
        std::cerr << "API Request Failed" << "\n";
        std::cerr << "Error code: " << j["error"]["code"]  << "\n";
        std::cerr << "Error type: " << j["error"]["type"]  << "\n";
        std::cerr << "Error info: " << j["error"]["info"]  << "\n";
        exit(1);
    } else {
        std::fstream file;
        file.open("result.txt", std::ios::out);

        file << "API Request Successful" << "\n";
        file << "Currency Conversion: From " << j["base"] << " to \"RUB\"" << "\n";

        if (time_series) {
            file << "Time Period: From " << start_date << " to " << end_date << "\n";

            for (auto& elemnt : j["rates"].items()) {
                float cost = elemnt.value()["RUB"];
                cost *= vnom;
                file << "Date: " << elemnt.key() << " Amount in RUB: " << cost << "\n";
            }

        } else {
            file << "Date: " << date << "\n";

            float cost = j["rates"]["RUB"];
            cost *= vnom;
            file << "Amount in RUB: " << cost << "\n";
        }

        file.close();
    }

    std::cout << "Finished Successfully" << "\n";

    return 0;
}
