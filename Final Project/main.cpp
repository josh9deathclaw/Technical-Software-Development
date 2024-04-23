#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ios>
#include <limits>

using namespace std;

/*
 * Eamon (103984754) and Joshua (103666887)
 * Code displays a menu with the following options:
 * Enter a new patient, report results of a test, display high risk locations
 * update a patient and display a patient details.
 *
 * The patient information is stored locally in a CSV file with one line per patient.
 * High risk locations are stored locally in a TXT file with one line per location.
 * Symptoms are stored locally in a CSV file with one line per symptom level.
 */

// Clears the CIN so that there are no issues with input being skipped.
void ClearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Takes a string and removes the new line symbols from the end. Used to clean up values read from files.
void RemoveTrailingNewLine(string &s) {
    // Remove trailing new line.
    // https://stackoverflow.com/a/1488798
    if (!s.empty() && (s[s.length() - 1] == '\n' || s[s.length() - 1] == '\r')) {
        s.erase(s.length() - 1);
    }
}

// Displays "Press any key to continue..." and waits for user input.
void PressAnyKeyToContinue() {
    // Mac/Linux and Windows have different commands to wait for input.
    // https://stackoverflow.com/a/3213261
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    system("pause");
#else
    cout << "Press any key to continue..." << endl;
    system("read");
#endif
}

// Takes a user input, forcing them to retry if it is not numeric.
int GetNumericUserInput() {
    fflush(stdout);
    int input;
    while (true) {

        cin >> input;

        if (!cin.good()) {
            cout << "Unknown selection, please try again" << endl;
            ClearCin();
            continue;
        }
        return input;
    }
}

// Get a numeric input, ensuring it is within a range.
// Min and Max are inclusive values
int GetNumericUserInput(int min, int max) {
    cout << "Please enter a value between " << min << " and " << max << ":" << endl;
    int input;
    while (true) {
        input = GetNumericUserInput();

        if (input < min || input > max) {
            cout << "Unknown selection, please try again" << endl;
            continue;
        }
        return input;
    }
}

// Takes a line from a CSV file and returns the string before the next comma.
// Modifies the original string to remove the first element and comma.
string GetNextCSVValue(string *csvLine) {
    string returnValue = "";
    int commaIndex = csvLine->find(",");
    // If no more commas, send the rest of the string back.
    if (commaIndex == -1) {
        returnValue = csvLine->substr(0, csvLine->length());
        csvLine->erase(0, csvLine->length());
        // Remove any new lines that might be from the end of a CSV line.
        RemoveTrailingNewLine(returnValue);
    } else {
        returnValue = csvLine->substr(0, commaIndex);
        csvLine->erase(0, commaIndex + 1);
    }
    return returnValue;
}

class PatientDetails {
public:
    // Takes a line of CSV and applies its values to the object.
    void FromCSV(string csv) {
        patientId = stoi(GetNextCSVValue(&csv));
        name = GetNextCSVValue(&csv);
        dob = GetNextCSVValue(&csv);
        address = GetNextCSVValue(&csv);
        visitedLocation = GetNextCSVValue(&csv);
        dateTime = GetNextCSVValue(&csv);
        lastOverseasTravel = GetNextCSVValue(&csv);
        covidTest = GetNextCSVValue(&csv);
        status = GetNextCSVValue(&csv);
    }

    // Converts object values to a line of CSV.
    string AsCSVString() {
        return to_string(patientId) + "," + name + "," + dob + "," + address + "," + visitedLocation + "," +
               dateTime + "," + lastOverseasTravel + "," + covidTest + "," + status;
    }

    // Displays dialogue to update the details of this user.
    void Update() {
        int userSelection;

        cout << "Please select The field you wish to update:\n"
             << "1- Patient Id\n"
             << "2- Name\n"
             << "3- Date of Birth\n"
             << "4- Address\n"
             << "5- Visited Location\n"
             << "6- Time of Visit\n"
             << "7- Last Overseas Travel\n"
             << "8- Status\n"
             << "9- Quit\n";

        userSelection = GetNumericUserInput(1, 9);

        cout << "Please enter the new value: " << endl;

        string newValue;
        // Making sure to include spaces.
        ClearCin();
        getline(cin, newValue);

        switch (userSelection) {
            case 1:
                break;
            case 2:
                name = newValue;
                break;
            case 3:
                break;
            case 4:
                address = newValue;
                break;
            case 5:
                visitedLocation = newValue;
                break;
            case 6:
                break;
            case 7:
                lastOverseasTravel = newValue;
                break;
            case 8:
                status = newValue;
                break;
            case 9:
                return;
        }

        cout << "Updated." << endl;
    }

    // Prints out this user's details
    void Print() {
        cout << "Patient Details:" << endl;
        cout << "Id: " << patientId << endl;
        cout << "Name: " << name << endl;
        cout << "Date of Birth: " << dob << endl;
        cout << "Address: " << address << endl;
        if (visitedLocation != "" && dateTime != "")
            cout << "Visited Location: " << visitedLocation << " at "
                 << dateTime << "." << endl;
        cout << "Recent Overseas Travel: " << lastOverseasTravel << endl;
        if (covidTest != "")
            cout << "Covid Test Result: " << covidTest << endl;
        if (status != "")
            cout << "Status: " << status << endl;
    }

    int patientId;
    string name;
    string dob;
    string address;
    string visitedLocation;
    string dateTime;
    string lastOverseasTravel;
    string covidTest;
    string status;
};

// Reads patient details from given file path.
vector<PatientDetails *> DetailsFromFile(string path) {
    ifstream file;
    file.open(path);

    // Get the line count by counting the occurrences of the newline char.
    int lineCount = count(istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');

    // Go to start of the file again.
    file.clear();
    file.seekg(0);

    // Prefill a vector with null pointers for the amount of expected patients.
    vector<PatientDetails *> details(lineCount, nullptr);

    string line;
    int i = 0;

    while (getline(file, line)) {
        PatientDetails *patient = new PatientDetails();
        patient->FromCSV(line);
        details[i] = patient;
        i++;
    }

    file.close();

    return details;
}

// Writes given patients to the file. Will overwrite existing patients within file.
void AddPatientsToFile(vector<PatientDetails *> patientDetails, string path) {
    ofstream file;
    file.open(path);

    for (int i = 0; i < patientDetails.size(); i++)
        file << patientDetails[i]->AsCSVString() << endl;

    file.close();
}

// Searches the patients details until a patient with the given id is found.
// Returns -1 if patient not found.
int GetPatientIndexFromId(vector<PatientDetails *> patientDetails, int id) {
    for (int i = 0; i < patientDetails.size(); i++) {
        if (patientDetails[i]->patientId == id) {
            return i;
        }
    }

    return -1;
}

// Reads symptoms from file, returns 2d vector.
vector<vector<string> > GetSymptoms(string path) {
    ifstream file;
    file.open(path);

    string line;

    // Create first dimension vector and prefill with 3 empty vectors for the 3 levels of symptoms.
    vector<vector<string>> symptoms{
            {},
            {},
            {}
    };

    int i = 0;
    while (getline(file, line)) {
        // Only 3 symptom levels expected.
        if (i > 2) break;
        while (!line.empty()) {
            string x = GetNextCSVValue(&line);
            symptoms[i].push_back(x);
        }
        i++;
    }

    file.close();

    return symptoms;
};

// Gets locations from file.
vector<string> GetLocations(string path) {
    ifstream file;
    file.open(path);

    vector<string> locations{};

    string line;

    while (getline(file, line)) {
        // Clean up end of line
        RemoveTrailingNewLine(line);
        locations.push_back(line);
    }

    file.close();

    return locations;
}

// Writes all given locations to the path.
void WriteLocation(vector<string> locations, string path) {
    ofstream file;
    file.open(path);
    for (int i = 0; i < locations.size(); i++) {
        file << locations[i] << endl;
    }
    file.close();
}

// Prints out the locations from the file.
void PrintCovidLocations() {
    vector<string> locations = GetLocations("locations.txt");

    cout << "The current High Risk Locations for COVID are:" << endl;

    for (int i = 0; i < locations.size(); i++) {
        cout << locations[i] << endl;
    }
    PressAnyKeyToContinue();
}

// Finds the id of the intended patient then prints the given
// patient using the Print() method within the class.
void PrintPatientDetails() {
    ClearCin();

    vector<PatientDetails *> patientDetails = DetailsFromFile("patientDetails.csv");

    for (int i = 0; i < patientDetails.size(); i++) {
        patientDetails[i]->Print();
        cout << endl;
    }
    PressAnyKeyToContinue();
}

// Finds the id of the intended patient then updates the given
// patient using the Update() method within the class which
// takes user input to update.
void UpdatePatientDetails() {
    ClearCin();
    cout << "Enter the Id of the patient you wish to update: " << endl;

    int patientId = GetNumericUserInput();

    vector<PatientDetails *> patientDetails = DetailsFromFile("patientDetails.csv");

    int patientIndex = GetPatientIndexFromId(patientDetails, patientId);

    if (patientIndex != -1) {
        patientDetails[patientIndex]->Update();
        AddPatientsToFile(patientDetails, "patientDetails.csv");
        PressAnyKeyToContinue();
    } else {
        cout << "Could not find patient with id: " << patientId << endl;
        PressAnyKeyToContinue();
    }
}

// Takes new patient information and makes a
// recommendation based on their input.
void EnterDetailsAndDisplayRecommendation() {
    ClearCin();
    PatientDetails *patientDetails = new PatientDetails();

    // Get basic user info.
    cout << "Please enter your name:" << endl;
    getline(cin, patientDetails->name);
    cout << "Please enter your date of birth:" << endl;
    getline(cin, patientDetails->dob);
    cout << "Please enter your address:" << endl;
    getline(cin, patientDetails->address);
    cout << "Have you traveled overseas in the past 2 weeks:" << endl;
    getline(cin, patientDetails->lastOverseasTravel);

    // Check for locations

    vector<string> locations = GetLocations("locations.txt");

    if (locations.size() == 0) {
        cout << "No locations found" << endl;
    } else {
        // Display all locations and allow picking a location with numbers.

        cout << endl << "Please enter the number of the location you have visited recently:" << endl;

        for (int i = 0; i < locations.size(); i++) {
            cout << (i + 1) << "- " << locations[i] << endl;
        }
        cout << (locations.size() + 1) << "- None of the above" << endl;

        int locationSelection = GetNumericUserInput(1, locations.size() + 1);
        // If not "none of the above".
        if (locationSelection != locations.size() + 1) {
            ClearCin();
            cout << "Please enter when you visited:" << endl;
            getline(cin, patientDetails->dateTime);
            patientDetails->visitedLocation = locations[locationSelection - 1];
        }
    }

    // Find a first patient id that is not taken by looping through all
    // patients, setting the patient id to one more than the maximum found.
    patientDetails->patientId = 0;

    vector<PatientDetails *> allPatientDetails = DetailsFromFile("patientDetails.csv");

    for (int i = 0; i < allPatientDetails.size(); i++) {
        if (allPatientDetails[i]->patientId >= patientDetails->patientId) {
            patientDetails->patientId = allPatientDetails[i]->patientId + 1;
        }
    }

    // Save new patient.
    allPatientDetails.push_back(patientDetails);
    AddPatientsToFile(allPatientDetails, "patientDetails.csv");

    // Display patient id to user.
    cout << "Your patient id is: " << patientDetails->patientId << endl;

    vector<vector<string>> symptoms = GetSymptoms("symptoms.csv");

    // If no symptoms in any of the 3 vectors, no symptoms exist.
    if (symptoms[0].size() == 0 && symptoms[1].size() == 0 && symptoms[2].size() == 0) {
        cout << "Symptom database not found." << endl;
        PressAnyKeyToContinue();
        return;
    }

    cout << "What are your current symptoms? Enter 'Done' to finish entering symptoms:" << endl;

    int maxSymptomLevel = -1;

    string input;

    // Get worst symptom level.
    // All symptoms that are not at the maximum level are irrelevant.
    while (true) {
        getline(cin, input);

        if (input == "Done")
            break;

        // Don't check unneeded symptoms, but ensure always starting at, at least 0;
        for (int i = max(maxSymptomLevel, 0); i < symptoms.size(); i++) {
            for (int j = 0; j < symptoms[i].size(); j++) {
                if (symptoms[i][j] == input) {
                    maxSymptomLevel = i;
                }
            }
        }
    }

    // If not visited high risk location a different set of outcomes will be printed.
    if (patientDetails->visitedLocation != "") {
        switch (maxSymptomLevel) {
            case -1:
                cout << "Please isolate at home, if you have any symptoms, please immediately test for COVID." << endl;
                break;
            default:
                cout << "Please immediately isolate and test for COVID as soon as possible." << endl;
                break;
        }
    } else {
        switch (maxSymptomLevel) {
            case -1:
                cout << "We do not recommend you isolate at the moment." << endl;
                break;
            case 1:
            case 2:
                cout
                        << "We do not recommend you get tested, however please "
                        << "isolate at home and get tested if your symptoms worsen."
                        << endl;
                break;
            case 3:
                cout << "Please immediately isolate and test for COVID as soon as possible." << endl;
                break;
        }
    }

    PressAnyKeyToContinue();
}

// Submit test results and update locations and patient if positive.
void SubmitTestResults() {
    ClearCin();
    cout << "Enter the Id of the patient you wish to submit a test for: " << endl;

    int patientId = GetNumericUserInput();

    vector<PatientDetails *> patientDetails = DetailsFromFile("patientDetails.csv");

    int patientIndex = GetPatientIndexFromId(patientDetails, patientId);

    if (patientIndex != -1) {
        // Patient found
        cout << "Please select the test result for " << patientDetails[patientIndex]->name << ":" << endl;
        cout << "1- Positive" << endl;
        cout << "2- Negative" << endl;

        int result = GetNumericUserInput(1, 2);

        patientDetails[patientIndex]->covidTest = result == 1 ? "Positive" : "Negative";

        if (result == 1) {
            // Positive test
            vector<string> locations = GetLocations("locations.txt");
            ClearCin();
            cout << "Please enter the last location " << patientDetails[patientIndex]->name << " visited: " << endl;
            string location;
            getline(cin, location);


            // Search if location exists.
            bool found = false;
            for (int i = 0; i < locations.size(); i++) {
                if (locations[i] == location) {
                    found = true;
                    break;
                }
            }
            // If not, save it.
            if (!found) {
                locations.push_back(location);
                WriteLocation(locations, "locations.txt");
            }

            // Write the visited location to the patient.
            patientDetails[patientIndex]->visitedLocation = location;

            // Get time data.
            cout << "Please enter when you visited:" << endl;
            getline(cin, patientDetails[patientIndex]->dateTime);
        }

        // Save patient.
        AddPatientsToFile(patientDetails, "patientDetails.csv");
        PressAnyKeyToContinue();
    } else {
        cout << "Could not find patient with id: " << patientId << endl;
        PressAnyKeyToContinue();
    }
}

int main() {
    int userSelection;
    while (true) {
        cout << "1- Enter your detail for COVID-Test Recommendation\n" <<
             "2- Submit Your Covid test status & Update the Location database\n" <<
             "3- Display the Updated Location (High Risk for COVID)\n" <<
             "4- Update COVID Patient Details\n" <<
             "5- Display the COVID Positive Patient Detail\n" <<
             "6- Quit\n";
        userSelection = GetNumericUserInput(1, 6);

        switch (userSelection) {
            case 1:
                EnterDetailsAndDisplayRecommendation();
                break;
            case 2:
                SubmitTestResults();
                break;
            case 3:
                PrintCovidLocations();
                break;
            case 4:
                UpdatePatientDetails();
                break;
            case 5:
                PrintPatientDetails();
                break;
            case 6:
                cout << "Goodbye";
                return 0;
        }
    }
}