#include <openssl/des.h>          // Provides DES encryption functions
#include <openssl/rand.h>         // Used to generate random salt

#include <array>                 
#include <cstdint>                
#include <iomanip>                
#include <iostream>               
#include <sstream>                
#include <stdexcept>              
#include <string>                 
#include <vector>                 

using namespace std;              

struct PasswordRecord             
{
    uint16_t salt;                
    string hash;                  
};

string toHex(const unsigned char *data, size_t len) // Function to convert bytes to hex string
{
    stringstream ss;              // Create string stream for formatting
    ss << hex << setfill('0');    // Set hex format and fill with 0 if needed

    for (size_t i = 0; i < len; ++i) 
    {
        ss << setw(2) << static_cast<int>(data[i]); // Convert byte to 2-digit hex
    }

    return ss.str();              
}

string saltToHex(uint16_t salt)   // Function to convert salt to hex
{
    stringstream ss;              // Create string stream
    ss << hex << uppercase << setfill('0') << setw(4) << salt; // Format salt as 4-digit hex
    return ss.str();              
}

uint16_t randomSalt16()           // Function to generate random 16-bit salt
{
    uint16_t salt = 0;            

    if (RAND_bytes(reinterpret_cast<unsigned char *>(&salt), sizeof(salt)) != 1) // Generate random bytes
    {
        throw runtime_error("Could not generate random salt."); // Throw error if failed
    }

    return salt;                 
}

array<unsigned char, 8> buildKeyFromPassword(const string &password) // Convert password to DES key
{
    array<unsigned char, 8> key{}; // Create 8-byte key array

    for (size_t i = 0; i < 8; ++i) 
    {
        unsigned char ch = (i < password.size()) ? static_cast<unsigned char>(password[i]) : 0; 
        key[i] = static_cast<unsigned char>((ch & 0x7F) << 1); 
    }

    return key;                   
}

array<unsigned char, 8> buildSaltedBlock(uint16_t salt) // Build initial block using salt
{
    array<unsigned char, 8> block{}; // Create 8-byte block

    for (int i = 0; i < 8; i += 2) // Fill block in pairs
    {
        block[i] = static_cast<unsigned char>((salt >> 8) & 0xFF);     // High byte of salt
        block[i + 1] = static_cast<unsigned char>(salt & 0xFF);         // Low byte of salt
    }

    return block;                
} // <-- this brace was missing

string encryptPasswordMT16(const string &password, uint16_t salt) 
{
    auto keyArray = buildKeyFromPassword(password); // Convert password to key

    DES_cblock key{};           // DES key block
    for (int i = 0; i < 8; ++i) // Copy key values
    {
        key[i] = keyArray[i];   // Assign each byte
    }

    DES_set_odd_parity(&key);   
    DES_key_schedule schedule;  // Create key schedule
    if (DES_set_key_checked(&key, &schedule) != 0) // Validate key
    {
        throw runtime_error("Invalid DES key."); // Error if invalid
    }

    auto block = buildSaltedBlock(salt); // Create initial data block

    DES_cblock input{};        // Input block for encryption
    DES_cblock output{};       // Output block after encryption

    for (int round = 0; round < 25; ++round) // Repeat DES 25 times
    {
        for (int i = 0; i < 8; ++i) // Copy block into input
        {
            input[i] = block[i];    // Assign values
        }

        DES_ecb_encrypt(&input, &output, &schedule, DES_ENCRYPT); // Perform DES encryption

        for (int i = 0; i < 8; ++i) // Mix salt into result
        {
            block[i] = output[i] ^ // XOR output
                       static_cast<unsigned char>((salt >> ((i % 2) ? 0 : 8)) & 0xFF); // Mix salt bits
        }
    }

    return saltToHex(salt) + "$" + toHex(block.data(), block.size()); 
}

bool verifyPassword(const string &candidate, const PasswordRecord &record) // Check password validity
{
    return encryptPasswordMT16(candidate, record.salt) == record.hash; // Compare hashes
}

int main() 
{
    try
    {
        vector<string> samplePasswords = // List of given passwords
        {
            "ahmed1712",
            "joe0708",
            "Essam2908",
            "baz2103",
            "kordy2811",
            "sherif2409",
            "yehia0504",
            "rawan1905",
            "nour0202",
            "haya0704"
        };

        vector<PasswordRecord> records; // Vector to store encrypted passwords

        cout << "10 Encrypted Passwords" << endl; 

        for (const auto &pwd : samplePasswords) 
        {
            uint16_t salt = randomSalt16(); // Generate random salt
            string hash = encryptPasswordMT16(pwd, salt); // Encrypt password

            records.push_back({salt, hash}); // Store salt and hash

            cout << left << setw(12) << pwd << " -> " << hash << endl; // Print result
        }

        cout << endl; 
        cout << "Verification Demo" << endl; // Print section title

        cout << "Correct password test: " // Test correct password
             << (verifyPassword("ahmed1712", records[0]) ? "VALID" : "INVALID") << endl;

        cout << "Wrong password test:   " // Test wrong password
             << (verifyPassword("wrongpass", records[0]) ? "VALID" : "INVALID") << endl;
    }
    catch (const exception &e) // Catch any errors
    {
        cerr << "Error: " << e.what() << endl; 
        return 1; 
    }

    return 0; 
}
