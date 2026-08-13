#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class User
{
private:
    string username;
    string password;

public:

    User()
    {
        username = "";
        password = "";
    }

    User(string u, string p)
    {
        username = u;
        password = p;
    }

    bool validate()
    {
        if (username.length() < 3)
        {
            cout << "\nUsername must be at least 3 characters.\n";
            return false;
        }

        if (password.length() < 6)
        {
            cout << "\nPassword must be at least 6 characters.\n";
            return false;
        }

        return true;
    }

    bool exists()
    {
        ifstream file("users.txt");

        string storedUsername;
        string storedPassword;

        while (file >> storedUsername >> storedPassword)
        {
            if (storedUsername == username)
            {
                file.close();
                return true;
            }
        }

        file.close();
        return false;
    }

    bool registerUser()
    {
        if (!validate())
        {
            return false;
        }

        if (exists())
        {
            cout << "\nUser already exists!\n";
            return false;
        }

        ofstream file("users.txt", ios::app);

        if (!file)
        {
            cout << "\nUnable to open user file.\n";
            return false;
        }

        file << username << " "
             << password << endl;

        file.close();

        cout << "\nRegistration successful!\n";

        return true;
    }

    bool loginUser()
    {
        ifstream file("users.txt");

        if (!file)
        {
            cout << "\nUser database not found.\n";
            return false;
        }

        string storedUsername;
        string storedPassword;

        while (file >> storedUsername >> storedPassword)
        {
            if (storedUsername == username &&
                storedPassword == password)
            {
                file.close();

                cout << "\nLogin successful!\n";
                cout << "Welcome to ManiMart, "
                     << username << "!\n";

                return true;
            }
        }

        file.close();

        cout << "\nInvalid username or password!\n";

        return false;
    }

    void session()
    {
        int choice;

        do
        {
            cout << "\n====================================\n";
            cout << "          ManiMART ACCOUNT\n";
            cout << "====================================\n";

            cout << "Logged in as: "
                 << username << "\n\n";

            cout << "1. View Profile\n";
            cout << "2. Logout\n";

            cout << "\nEnter choice: ";
            cin >> choice;

            if (choice == 1)
            {
                cout << "\n---------- PROFILE ----------\n";
                cout << "Username: " << username << endl;
                cout << "Status: Active\n";
                cout << "-----------------------------\n";
            }
            else if (choice == 2)
            {
                cout << "\nLogout successful!\n";
            }
            else
            {
                cout << "\nInvalid choice!\n";
            }

        } while (choice != 2);
    }
};

// Create initial seed account
void createSeedData()
{
    ifstream check("users.txt");

    if (check.good())
    {
        check.close();
        return;
    }

    check.close();

    ofstream file("users.txt");

    file << "admin admin123" << endl;

    file.close();
}

int main()
{
    createSeedData();

    int choice;

    do
    {
        cout << "\n====================================\n";
        cout << "          WELCOME TO Mani MART\n";
        cout << "====================================\n";

        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
            case 1:
            {
                string username;
                string password;

                cout << "\nEnter username: ";
                cin >> username;

                cout << "Enter password: ";
                cin >> password;

                User user(username, password);

                user.registerUser();

                break;
            }

            case 2:
            {
                string username;
                string password;

                cout << "\nEnter username: ";
                cin >> username;

                cout << "Enter password: ";
                cin >> password;

                User user(username, password);

                if (user.loginUser())
                {
                    user.session();
                }

                break;
            }

            case 3:
            {
                cout << "\nThank you for using ManiMart!\n";
                break;
            }

            default:
            {
                cout << "\nInvalid choice!\n";
            }
        }

    } while (choice != 3);

    return 0;
}
