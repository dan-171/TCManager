// TuitionCentre_Manager.cpp
#define NOMINMAX
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13

#include <iostream>
#include <string>
#include <iomanip>
#include <mysql.h>
#include <mysql/jdbc.h>
#include <conio.h>
#include <thread>
#include <sstream>
#include <windows.h>
#include <algorithm>
#include <ctime>

using namespace std;

//Function declarations
void AdminReg();
void StaffReg();
void SetPW(string &pw, string &cofirmpw);
void SetHP(string &HP);
void Login(string &uID, char &uType);
void ASmain(char uType, string uID);

void StudentDB();
void StudentDBView();
void StudentDBAdd();
void StudentDBUpdate();
void AddDropSubj(string &sID, string &upSubj, string &update, bool &querytrigger);

void ClassMgmt(char uType, string sID);
void ScheduleView();
void NamelistView(string sID, string classID);
void ClassUpdate();
void SubjSchedule(string subj, string subjn);
void TeacherAssignation(string subj, string subjn);
void SubjAdd();
void SubjRem(string subj, string subjn);

void StudentPerf(char uType, string uID);
void PerfView(char uType);
void PerfFilter(string filter);
void PerfUpdate(string uID);
void ScoreUpdate(string sID, string SID, string quizScore);
void Comment(string sID, string SID);

void FinMgmt(string uID);
void FinRec(string uID);
void MonetaryCheck(string &amount);
void FinInsert(string entryType, string title, string amount, string uID);
void FinDel();
void FinReport();

void IncrementID(string& IDType, string& newID, int width);
int getCursorYPosition();
void setCursorPosition(int x, int y);
void scrollmenu(vector<string>options, int &selected, int startX, int startY, string& choice);
void selectionmenu(vector<string>options, int &selected, int startX, int startY, bool highlighted);
void hideCursor();
void showCursor();

//global variables
int qState;
MYSQL* conn = mysql_init(0);
MYSQL_RES* res;
MYSQL_ROW row;
MYSQL_FIELD* fields;

class db_response
{
public:
    static void ConnectionFunction()
    {
        if (conn)
            cout << "Database connected" << endl;
        else
            cout << "Failed to connect" << endl;
        conn = mysql_real_connect(conn, "localhost", "root", "", "tuitioncentre_manager", 3306, NULL, 0);
        if (conn)
            cout << "Database connected to MySQL\n" << endl;
        else
            cout << "Failed to Connect!\n" << endl;
    }
};

int main()
{
    string UserID, UserPW, UserName, op;
    char UserType;

    system("title Tuition Centre Manager");

    //check mysql connection
    db_response::ConnectionFunction();

    while (true)
    {
        //intro msg
        cout << "[Welcome to Tuition Centre Manager!]\n\n";
        cout << "\033[1;36mSelect an operation:\033[0m\n\n";
        cout << "1: Login \n2: Registration\n3: Exit\n\n";
        cout << "\033[1;36mOperation selected:\033[0m ";

        //operation selection
        getline(cin, op);

        //proceed to module
        if (op == "1")
            Login(UserID, UserType);
        else if (op == "2")
        {
            system("cls");
            cout << "\033[1;36mRegistering as:\033[0m\n\n1: Admin\n2: Staff\n\n[Enter any other key to cancel registration]\n\n";
            cout << "\033[1;36mOperation selected:\033[0m ";
            getline(cin, op);
            if (op == "1")
                AdminReg();
            else if (op == "2")
                StaffReg();
        }
        else if (op == "3")
        {
            cout << "\nProgram closed\n";
            break;
        }
        else
        {
            cout << "\n\033[31mInvalid Operation. Press any key...\033[0m\n\n";
            _getch();
        }
        system("cls");
        continue;
    }
   
    return 0;
}

void Login(string &uID, char &uType)
{
    system("cls");
    string password;

    //get input for user ID & pwd
    cout << "[Login]\n\033[1;36mEnter your credentials\033[0m\n\n";
    cout << "User ID: ";
    cin >> uID;
    cout << "Password: ";
    char ch;
    while ((ch = _getch()) != 13) //so long as ENTER not pressed
    {
        if (ch == 8 && !password.empty()) //if backspace pressed
        {
            password.pop_back();
            cout << "\b \b";
        }
        else
        {
            password += ch;
            cout << '*'; //hide pw with asterisk
        }
    }

    //check if user-input credentials matches
    string checkCredentials;
    if (uID[0] == 'A') //if logging in as admin
    {
        uType = 'A';
        checkCredentials = "SELECT AdminID FROM adminbase WHERE AdminID = '" + uID + "' AND AdminPWD = '" + password + "'";
    }
    else if (uID[0] == 'S') //if logging in as staff
    {
        uType = 'S';
        checkCredentials = "SELECT StaffID FROM staffbase WHERE StaffID = '" + uID + "' AND StaffPWD = '" + password + "'";
    }
    qState = mysql_query(conn, checkCredentials.c_str());
    if (!qState) //if select query succeeded
    {
        res = mysql_store_result(conn);
        if (res->row_count == 1) //if credentials match
        {
            while (row = mysql_fetch_row(res))
                uID = row[0];
            cout << "\n\n\033[1;32mLogin successful. Press any key to proceed...\033[0m";
            _getch();
            mysql_free_result(res);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            ASmain(uType, uID);
            return;
        }
        else // if credentials don't match
            cout << "\n\n\033[31mInvalid credentials.\033[0m\nPress any key to return to main menu...\n\n";
    }
    else //if select query failed
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << ": Failed to match credentials!]\033[0m\nPress any key to return to main menu...\n\n";
    _getch();
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    mysql_free_result(res);
    return;
}

void AdminReg()
{
    system("cls");
    
    string adminID, adminPW = "0", adminName, adminHP, confirmPW = "0";
    cout << "[Admin Registration]\n\n\033[1;36mSet your admin ID\033[1;33m [Format: A followed by 2 integers e.g. A01]\033[0m\n\n";

    //set admin ID
    cout << "Admin ID: ";
    getline(cin, adminID);
    while (adminID.length() !=3 || adminID[0] != 'A' || !all_of(adminID.begin()+1, adminID.end(),isdigit))
    {
        cout << "\033[31mIncorrect Admin ID format detected.\033[1;33m\n[Format: A followed by 2 integers e.g. A01]\033[0m\n\n";
        cout << "Admin ID: ";
        getline(cin, adminID);
    }

    //set admin pw
    system("cls");
    SetPW(adminPW, confirmPW);

    //set admin name
    system("cls");
    cout << "Admin Name: ";
    getline(cin, adminName);
    while (adminName.empty())
    {
        cout << "\033[1;31mName cannot be left blank.\033[0m\n";
        cout << "Admin Name: ";
        getline(cin, adminName);
    }

    //set admin HP
    cout << "Admin HP (e.g. 01XXXXXXXXX): ";
    SetHP(adminHP);

    //insert entry to db
    string insert_query = "INSERT INTO adminbase (AdminID, AdminPWD, AdminName, AdminHP) VALUES ('" + adminID + "', '" + adminPW + "', '" + adminName + "', '" + adminHP + "')";
    qState = mysql_query(conn, insert_query.c_str());
    if (!qState)
        cout << endl << "\033[32mAdmin successfully registered.\033[0m\n\nPress any key to return to main menu..." ;
    else
        cout << "\033[31mAdmin registration failed! [Error:" << mysql_errno(conn) << "] Admin ID already chosen by someone else!\033[0m\n\nPress any key to return to main menu...";
    _getch();
    return;
}

void StaffReg()
{
    system("cls");

    string staffID, staffPW = "0", staffName, staffHP, confirmPW = "0", radminID;
    cout << "[Staff Registration]\n\n\033[1;36mSet your Staff ID\033[1;33m [Format: S followed by 2 integers e.g. S01]\033[0m\n\n" << endl;

    //set staff ID
    cout << "Staff ID: ";
    getline(cin, staffID);
    while (staffID.length() != 3 || staffID[0] != 'S' || !all_of(staffID.begin() + 1, staffID.end(), isdigit))
    {
        cout << "\033[31mIncorrect Staff ID format detected.\033[1;33m\n[Format: S followed by 2 integers e.g. S01]\033[0m\n\n";
        cout << "Staff ID: ";
        getline(cin, staffID);
    }

    //set staff pw
    system("cls");
    SetPW(staffPW, confirmPW);

    //set staff name
    system("cls");
    cout << "Staff Name: ";
    getline(cin, staffName);
    while (staffName.empty())
    {
        cout << "\033[1;31mName cannot be left blank.\033[0m\n";
        cout << "Staff Name: ";
        getline(cin, staffName);
    }

    //set staff HP
    cout << "Staff HP \033[1;33m[Format: 01XXXXXXXXX]\033[0m: ";
    SetHP(staffHP);

    //get recruiting admin ID
    system("cls");
    cout << "Recruiting Admin ID: ";
    getline(cin, radminID);
    
    //check validity of recruiting admin ID
    do
    {
        string recruit = "SELECT AdminID FROM adminbase WHERE AdminID = '" + radminID + "'";
        qState = mysql_query(conn, recruit.c_str());
        if (!qState)
        {
            res = mysql_store_result(conn);
            if (mysql_num_rows(res) != 0)
                break;
            else
            {
                cout << "\033[31m[Error:" << mysql_errno(conn) << " Admin ID not found in database or incorrect Admin ID format.\033[1;33m\n\n[Format:A followed by 2 integer e.g.A01]\033[0m\n\n";
                cout << "Recruiting Admin ID: ";
                getline(cin, radminID);
            }
            mysql_free_result(res);
        }
        else
        {
            cout << "\033[31m[Error:" << mysql_errno(conn) << "]\033[0m\n\nStaff registration cancelled. Press any key to return to main menu...";
            _getch();
            return;
        }
    } while (!qState);

    //insert entry to db
    string insert_query = "INSERT INTO staffbase (StaffID, StaffPWD, StaffName, StaffHP, AdminID) VALUES ('" + staffID + "', '" + staffPW + "', '" + staffName + "', '" + staffHP + "', '" + radminID + "')";
    qState = mysql_query(conn, insert_query.c_str());
    if (!qState)
        cout << endl << "\n\033[32mStaff successfully registered.\033[0m\n\nPress any key to return to main menu..." << endl;
    else
        cout << "\n\033[31mStaff registration failed! [Error:" << mysql_errno(conn) << "] Staff ID already chosen by someone else! or Invalid Admin ID!\033[0m\n\nPress any key to return to main menu..." << endl;
    _getch();
    return;
}

void SetPW(string &pw, string &confirmpw)
{
    cout << "\033[1;36mSet a password\033[1;33m [Format: max 15 alphanumerical chars]\033[0m\n";
    do
    {
        if (pw != confirmpw)
        {
            system("cls");
            cout << "\033[31mPassword doesn't match first input, please try again.\033[1;33m\n[Format: max 15 alphanumerical chars]\033[0m\n\n";
        }

        pw.clear();
        confirmpw.clear();

        char ch;
        cout << "Password: "; //enter pw
        while ((ch = _getch()) != 13) //so long as ENTER not pressed
        {
            if (ch == 8 && !pw.empty()) //if backspace pressed
            {
                pw.pop_back();
                cout << "\b \b";
            }
            else if (ch != 8 && pw.length() < 15) //if pw doesn't exceed max length
            {
                pw += ch;
                cout << '*'; //hide pw with asterisk
            }
        }

        cout << "\nReenter password: "; //confirm pw
        while ((ch = _getch()) != 13) //so long as ENTER not pressed
        {
            if (ch == 8 && !confirmpw.empty()) //backspace pressed
            {
                confirmpw.pop_back();
                cout << "\b \b";
            }
            else if (ch != 8 && confirmpw.length() < 15)
            {
                confirmpw += ch;
                cout << '*'; //hide pw with asterisk
            }
        }
    } while (pw != confirmpw);
}

void SetHP(string& HP)
{
    getline(cin, HP);
    while (true)
    {
        int breaker = 0;
        while ((HP.length() != 10 && HP.length() != 11) || !all_of(HP.begin(), HP.end(), isdigit) || (HP[0] != '0' || HP[1] != '1')) // incorrect HP format detected
        {
            if (HP.empty()) // HP entry is left blank
                cout << "\033[31mHP cannot be left blank\033[1;33m\n[Format: 0123456789]\033[0m\n\n";
            else
                cout << "\033[31mIncorrect HP format detected.\033[1;33m\n[Format: 0123456789]\033[0m\n\n";
            cout << "Enter HP: ";
            getline(cin, HP);
            breaker++;
        }
        if (breaker == 0)
            break;
    }
}

void ASmain(char uType, string uID)
{
    string op, name, userName;

    //fetch userName from db for greetings
    if (uType == 'A')
        name = "SELECT AdminName FROM adminbase WHERE AdminID = '" + uID + "'";
    else
        name = "SELECT StaffName FROM staffbase WHERE StaffID = '" + uID + "'";
    qState = mysql_query(conn, name.c_str());
    res = mysql_store_result(conn);
    if ((row = mysql_fetch_row(res)) != NULL)
        userName = row[0];
    else
        userName = "#missingno user";
    mysql_free_result(res);

    while (TRUE)
    {
        system("cls");

        //module selection
        if (uType == 'A')
        {
            cout << "Welcome \033[1;35m" << userName << "\033[0m\n\n--------------------\n";
            cout << "\033[1;34mAdmin Operation Menu\033[0m\n--------------------\n";
            cout << "\033[1;36mSelect a module:\033[0m\n\n";
            cout << "1: Student Database\n2: Class Management\n3: Student Performance\n4: Finance Management\n5: Logout\n\n";
            cout << "\033[1;36mModule selected:\033[0m ";
            getline(cin, op);
            if (op == "1")
                StudentDB();
            else if (op == "2")
                ClassMgmt(uType, uID);
            else if (op == "3")
                StudentPerf(uType, uID);
            else if (op == "4")
                FinMgmt(uID);
            else if (op == "5")
                return;
            else
            {
                cout << "\033[31mInvalid Operation. Press any key...\033[0m";
                _getch();
            }
        }
        else
        {
            cout << "Welcome \033[1;35m" << userName << "\033[0m\n\n--------------------\n";
            cout << "\033[1;34mStaff Operation Menu\033[0m\n--------------------\n";
            cout << "\033[1;36mSelect a module:\033[0m\n\n";
            cout << "1: Class Management\n2: Student Performance\n3: Logout\n\n";
            cout << "\033[1;36mModule selected:\033[0m ";
            getline(cin, op);
            if (op == "1")
                ClassMgmt(uType, uID);
            else if (op == "2")
                StudentPerf(uType, uID);
            else if (op == "3")
                break;
            else
            {
                cout << "\033[31mInvalid Operation. Press any key...\033[0m";
                _getch();
            }
        }
    }
    return;
}

//Student Database Module
void StudentDB()
{
    string op;

    while (true)
    {
        system("cls");

        cout << "--------------------\n\033[1;34mStudent Database\033[0m\n--------------------\n";
        cout << "\033[1;36mSelect an operation:\033[0m\n\n";
        cout << "1: View database\n2: Update database\n\n[Enter any other key to return to module selection]\n\n";
        cout << "\033[1;36mOperation selected:\033[0m ";
        //select operation
        getline(cin, op);
        if (op == "1")
            StudentDBView();
        else if (op == "2")
        {
            system("cls");

            cout << "\033[1;36mSelect an operation:\033[0m\n\n";
            cout << "1: Add new entry\n2: Update existing entry\n\n[Enter any other key to return to the previous menu]\n\n";
            cout << "\033[1;36mOperation selected:\033[0m ";
            //select operation
            getline(cin, op);
            if (op == "1")
                StudentDBAdd();
            else if (op == "2")
                StudentDBUpdate();
            else
                continue;
        }
        else
            break;
    }
    return;
}

void StudentDBView()
{
    system("cls");
    string view = "SELECT * FROM studentbase";
    qState = mysql_query(conn, view.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        int colNo = mysql_num_fields(res);
        int rowNo = mysql_num_rows(res);
        fields = mysql_fetch_field(res);

        if (rowNo > 0)
        {
            cout << "                                    \033[1;34mStudent Database\033[0m";
            //print column headers
            cout << endl << "\033[1;33m-------------------------------------------------------------------------------------\n|";
            for (int i = 0; i < colNo; i++)
                cout << "\033[1;36m" << setw(13) << fields[i].name << "\033[1;33m|";
            cout << endl << "-------------------------------------------------------------------------------------\033[0m" << endl;

            //print rows
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                cout << "\033[1;33m|\033[0m";
                for (int i = 0; i < colNo; i++)
                    cout << setw(13) << row[i] << "\033[1;33m|\033[0m";
                cout << endl;
            }
            mysql_free_result(res);

            //search entry?
            while (true)
            {
                string search;
                cout << "\n\nEnter S to search Student ID or any other key to exit: ";
                getline(cin, search);
                if (search == "S" || search == "s") //if searching
                {
                    string sID;
                    cout << "Enter Student ID: ";
                    getline(cin, sID);
                    string checksID = "SELECT * FROM studentbase WHERE StudentID = '" + sID + "'";
                    qState = mysql_query(conn, checksID.c_str());
                    if (!qState)
                    {
                        res = mysql_store_result(conn);
                        rowNo = mysql_num_rows(res);
                        fields = mysql_fetch_field(res);
                        if (rowNo > 0) //matching student ID found
                        {
                            cout << endl << "\033[1;33m-------------------------------------------------------------------------------------\n|";
                            
                            //print column headers
                            for (int i = 0; i < colNo; i++)
                                cout << "\033[1;36m" << setw(13) << fields[i].name << "\033[1;33m|";
                            cout << endl << "-------------------------------------------------------------------------------------" << endl << "|\033[0m";
                            
                            //print rows
                            while ((row = mysql_fetch_row(res)) != nullptr)
                            {
                                for (int i = 0; i < colNo; i++)
                                    cout << setw(13) << row[i] << "\033[1;33m|\033[0m";
                            }
                        }
                        else
                        {
                            cout << "\n\033[31mStudent ID not found. Press any key...\033[0m\n";
                            _getch();
                        }
                        mysql_free_result(res);
                    }
                    else
                        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n\n";
                }
                else // if not searching
                    return;
            }
        }
        else
            cout << "Student database is currently empty\n\nPress any key to return to previous menu...";
    }
    else
        cout << "\n\nQuery Execution Failed! \n\n\033[31m[Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key to return to main menu...\n\n";
    mysql_free_result(res);
    _getch();
    return;
}

void StudentDBAdd()
{
    system("cls");

    string newID, sName, sSchoolID, sForm, sParentName, sParentHP, addsubj;
    string subjjCount; //no. of subj in db
    string idtype = "X";
    int subjCount = 0; //no. of subj taken by student
    
    //increment StudentID;
    IncrementID(idtype, newID, 3);

    cout << "Adding Student with ID : \033[35m" << newID << "\033[0m\n\n";
    cout << "----------------------\n\033[1;36mEnter Student Details\033[0m\n----------------------\n"; //Insert Student Detail
    cout << "Name: ";
    getline(cin, sName);
    while (sName.empty())
    {
        cout << "\033[1;31mStudent name cannot be left blank.\033[0m\n";
        cout << "Name: ";
        getline(cin, sName);
    }
    bool correctSchoolID = false;
    while (!correctSchoolID) //get SchoolID & check if entered SchoolID exists in schoolbase
    {
        cout << "SchoolID: ";
        getline(cin, sSchoolID);
        string checkschool = "SELECT SchoolID FROM schoolbase WHERE BINARY SchoolID = '" + sSchoolID + "'";
        qState = mysql_query(conn, checkschool.c_str());

        if (!qState)
        {
            res = mysql_store_result(conn);
            if (res->row_count == 1)
                correctSchoolID = TRUE;
            else
                cout << "\033[31mInvalid input for SchoolID.\033[1;33m\n[Format: SCH99]\033[0m\n\n";
            mysql_free_result(res);
        }
        else
            cout << "\033[31m[Error:" << mysql_errno(conn) << "]\033[0m\n\nPress any key...";
    }

    //get form
    cout << "Form: ";
    getline(cin, sForm);
    while (sForm.length() != 1 || !all_of(sForm.begin(), sForm.end(), isdigit) || stoi(sForm) < 1 || stoi(sForm) > 6)
    {
        cout << "\033[31mInvalid input for Form.\033[1;33m\nPlease enter an integer between 1 - 6.\033[0m\n\n";
        cout << "Form: ";
        getline(cin, sForm);
    }

    //get parent name
    cout << "Parent Name: ";
    getline(cin, sParentName);
    while (sParentName.empty())
    {
        cout << "\033[1;31mParent name cannot be left blank.\033[0m\n";
        cout << "Parent Name: ";
        getline(cin, sParentName);
    }
    //get parent HP
    cout << "Parent HP: ";
    SetHP(sParentHP);

    vector<string> sSubj;
    //add entry to studentdb, then reg subj
    string insert_queryy = "INSERT INTO studentbase (StudentID, StudentName, SchoolID, Form, ParentName, ParentHP) VALUES ('" + newID + "', '" + sName + "', '" + sSchoolID + "', '" + sForm + "', '" + sParentName + "', '" + sParentHP + "')";
    qState = mysql_query(conn, insert_queryy.c_str());
    if (!qState)
    {
        int i = 0;
        subjCount++;

        subjjCount = "SELECT COUNT(*) FROM subjectbase";
        qState = mysql_query(conn, subjjCount.c_str());
        if (!qState)
        {
            res = mysql_store_result(conn);
            if ((row = mysql_fetch_row(res)) != NULL)
            {
                int subjno = stoi(row[0]);
                sSubj.resize(subjno);
                mysql_free_result(res);

                while (i < subjno) //assign subj to student
                {
                    cout << "Subject taken " << i + 1 << ": ";
                    getline(cin, sSubj[i]);

                    //check if sSubj matches SubjID in subjectbase table
                    string checksubj = "SELECT sb.SubjID FROM subjectbase sb LEFT JOIN subjenrollment se ON sb.SubjID = se.SubjID AND se.StudentID = '" + newID + "' WHERE BINARY sb.SubjID = '" + sSubj[i] + "' AND se.SubjID is NULL";
                    qState = mysql_query(conn, checksubj.c_str());

                    if (!qState)
                    {
                        res = mysql_store_result(conn);

                        //if SubjID matches, add subjenrollment entry
                        if ((row = mysql_fetch_row(res)) != NULL)
                        {
                            string newEnNo, IDType = "SU";

                            //increment EnNo
                            IncrementID(IDType, newEnNo, 0);

                            string insert_query2 = "INSERT INTO subjenrollment (EnNo, StudentID, SubjID) VALUES ('" + newEnNo + "', '" + newID + "', '" + sSubj[i] + "')";
                            qState = mysql_query(conn, insert_query2.c_str());
                            if (!qState)
                            {
                                if (i < subjno - 1)
                                {
                                    cout << "Add another subject? \033[1;33m[Enter Y to continue adding or any other key to stop]\033[0m: ";
                                    getline(cin, addsubj);

                                    if (addsubj == "Y" || addsubj == "y")
                                    {
                                        i++;
                                        subjCount++;
                                    }
                                    else
                                        break;
                                }
                                else
                                    break;
                            }
                            else
                            {
                                mysql_free_result(res);
                                cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\nPress any key to return to the previous menu...\n\n";
                                _getch();
                                return;
                            }
                        }
                        else
                        {
                            mysql_free_result(res);
                            cout << "\n\n\033[31m[Error: Invalid/Duplicate Subject ID]\n\033[1;33m[Format: SU99]\033[0m\nPress any key...\n";
                            _getch();
                            continue;
                        }
                    }
                    else
                    {
                        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\nPress any key to return to the previous menu...\n\n";
                        _getch();
                        return;
                    }
                }

            }

        }
        
        //confirm studentbase entry
        string confirmEntry;
        system("cls");
        cout << "--------------------\n\033[1;34mEntry confirmation\033[0m\n--------------------\n";
        cout << "\033[1;33mStudent ID:\033[0m " << newID << endl;
        cout << "\033[1;33mName:\033[0m " << sName << endl;
        cout << "\033[1;33mSchool:\033[0m " << sSchoolID << endl;
        cout << "\033[1;33mForm:\033[0m " << sForm << endl;
        cout << "\033[1;33mParent Name:\033[0m " << sParentName << endl;
        cout << "\033[1;33mParent HP:\033[0m " << sParentHP << endl;
        for (int i = 0; i < subjCount; i++)
            cout << "\033[1;33mSubject taken " << i+1 << ":\033[0m " << sSubj[i] << endl;
        cout << endl << "Confirm entry? \033[1;33m[Enter Y to confirm or any other key to cancel registration]:\033[0m ";
        getline(cin, confirmEntry);
        if (confirmEntry == "Y" || confirmEntry == "y")
            cout << endl << endl << "\033[32mStudent successfully registered.\033[0m\n\nPress any key to return to the previous menu...";
        else
        {
            string delEntry = "DELETE FROM studentbase WHERE StudentID = '" + newID + "'";
            qState = mysql_query(conn, delEntry.c_str());
            string delEntry2 = "DELETE FROM subjenrollment WHERE StudentID = '" + newID + "'";
            qState = mysql_query(conn, delEntry2.c_str());
            cout << endl << endl << "\033[1;31mStudent registration cancelled.\033[0m\n\nPress any key to return to the previous menu...";
        }
    }
    else
        cout << "\033[31mStudent registration failed! [Error:" << mysql_errno(conn) << "]\033[0m\n\nPress any key to return to the previous menu...\n\n";
    _getch();
    return;
}

void StudentDBUpdate()
{
    system("cls");
    
    string sID;
    cout << "-------------------------------\n\033[1;34mStudent Database Update\033[0m\n-------------------------------\n";
    cout << "Enter ID of student to update [or X to return to the previous menu]: ";
    getline(cin, sID);
    if (sID == "X" || sID == "x")
        return;
    else
    {
        string newData, update, upSubj;
        bool queryTrigger = TRUE;
        string checksID = "SELECT StudentID FROM studentbase WHERE BINARY StudentID = '" + sID + "'";
        qState = mysql_query(conn, checksID.c_str());
        if (!qState)
        {
            res = mysql_store_result(conn);
            if (res->row_count == 1) //studentID valid?
            {
                mysql_free_result(res);
                string op;

                cout << "\n\n\033[1;36mSelect an operation\033[0m\n";
                cout << "-------------------------------\nUpdate\n-------------------------------\n";
                cout << "1: Name\n2: School\n3: Form\n4: Parent Name\n5: Parent HP\n6: Subjects Taken\n\n";
                cout << "-------------------------------\n7: Remove student from database\n-------------------------------\n\n";
                cout << "[Enter any other key to cancel update]\n\n";
                cout << "\033[1;36mOperation selected:\033[0m ";
                getline(cin, op);
                if (op == "1") //update name
                {
                    cout << "Enter Name: ";
                    getline(cin, newData);
                    update = "UPDATE studentbase SET StudentName = '" + newData + "' WHERE StudentID = '" + sID + "'";
                }
                else if (op == "2") //update school
                {
                    cout << "Enter School ID: ";
                    getline(cin, newData);
                    update = "UPDATE studentbase SET SchoolID = '" + newData + "' WHERE StudentID = '" + sID + "'";
                }
                else if (op == "3") //update form
                {
                    cout << "Enter Form: ";
                    getline(cin, newData);
                    while (newData.length() != 1 || !all_of(newData.begin(), newData.end(), isdigit) || stoi(newData) < 1 || stoi(newData) > 6)
                    {
                        cout << "\033[31mInvalid input for Form.\033[1;33m Please enter an integer between 1 - 6.\033[0m\n\n";
                        cout << "Enter Form: ";
                        getline(cin, newData);
                    }
                    update = "UPDATE studentbase SET Form = '" + newData + "' WHERE StudentID = '" + sID + "'";
                }
                else if (op == "4") //update parent name
                {
                    cout << "Enter Parent Name: ";
                    getline(cin, newData);
                    update = "UPDATE studentbase SET ParentName = '" + newData + "' WHERE StudentID = '" + sID + "'";
                }
                else if (op == "5") //update parent hp
                {
                    cout << "Enter Parent HP: ";
                    getline(cin, newData);
                    while ((newData.length() != 10 && newData.length() != 11) || !all_of(newData.begin(), newData.end(), isdigit) || (newData[0] != '0' || newData[1] != '1'))
                    {
                        cout << "\n\033[31mIncorrect HP format detected.\n\033[1;33m[Format: 0123456789]\033[0m\n\n";
                        cout << "Enter Parent HP: ";
                        getline(cin, newData);
                    }
                    update = "UPDATE studentbase SET ParentHP = '" + newData + "' WHERE StudentID = '" + sID + "'";
                }
                else if (op == "6") //update taken subj
                    AddDropSubj(sID, upSubj, update, queryTrigger);
                else if (op == "7") //delete student
                {
                    string del;
                    cout << "\nDo you really wish to remove Student " << sID << " from the database? \033[1;33m[Enter Y to confirm or any other key to cancel]:\033[0m ";
                    getline(cin, del);
                    if (del == "Y" || del == "y")
                        update = "DELETE FROM studentbase WHERE StudentID = '" + sID + "'";
                    else
                    {
                        cout << "\n\033[1;31mOperation cancelled.\033[0m\n\nPress any key to return to the previous menu...";
                        _getch();
                        return;
                    }
                }
                else
                    return;
            }
            else
            {
                cout << "\n\033[31m[Error " << mysql_errno(conn) << ": StudentID " << sID << " not found in database]\033[0m\n\n";
                cout << "Press any key to return to the previous menu...";
                _getch();
                return;
            }
        }
        else
        {
            cout << "\n\033[31m[Error " << mysql_errno(conn) << "]\033[0m\n\n";
            cout << "Press any key to return to the previous menu...";
            _getch();
            return;
        }

        if (queryTrigger)
        {
            qState = mysql_query(conn, update.c_str());
            if (!qState)
                cout << "\n\033[32mUpdate succeeded.\033[0m\n\nPress any key to return to the previous menu...";
            else
                cout << "\033[31mUpdate failed! [Error:" << mysql_errno(conn) << "]\n\033[0mPress any key to return to the previous menu...";
            _getch();
            return;
        }
    }
}

void AddDropSubj(string &sID, string &upSubj, string &update, bool &querytrigger)
{
    system("cls");

    int cuSCount;
    cout << "\033[1;36mCurrently taken subject:\033[0m\n\n"; //print currently taken subj
    string takenSubj = "SELECT COUNT(*) FROM subjenrollment WHERE StudentID = '" + sID + "'";
    qState = mysql_query(conn, takenSubj.c_str());
    if (!qState)
    {
        bool hasSubj = true;
        res = mysql_store_result(conn);
        if ((row = mysql_fetch_row(res)) != nullptr && stoi(row[0]) == 0) //no subj taken
        {
            cuSCount = 0;
            cout << "*This student is not enrolled in any subject currently*\n";
            hasSubj = false;
            mysql_free_result(res);
        }
        else //display subject(s) taken
        {
            mysql_free_result(res);

            cuSCount = stoi(row[0]);
            string view = "SELECT SubjID FROM subjenrollment WHERE StudentID = '" + sID + "' ORDER BY SubjID";
            qState = mysql_query(conn, view.c_str());
            if (!qState)
            {
                res = mysql_store_result(conn);
                while ((row = mysql_fetch_row(res)) != nullptr)
                    cout << row[0] << "\n";
                mysql_free_result(res);
            }
            else
            {
                querytrigger = false;
                cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\nPress any key...\n\n";
                _getch();
                return;
            }
        }

        cout << "\n\033[1;36mSelect an operation:\033[0m\n\n";
        string op, subjcheck;

        //if student currently has subject(s) taken
        if (hasSubj)
        {
            cout << "1: Unenroll from subject\n";

            string subjjCount = "SELECT COUNT(*) FROM subjectbase"; 
            qState = mysql_query(conn, subjjCount.c_str());
            if (!qState)
            {
                res = mysql_store_result(conn);
                if ((row = mysql_fetch_row(res)) != NULL)
                {
                    int subjno = stoi(row[0]);
                    if (cuSCount < subjno) //check if student can still enroll in more subj, if yes, display option to do so
                        cout << "2: Enroll in subject";
                }
                mysql_free_result(res);
            }
            else
            {
                querytrigger = false;
                cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\nPress any key...\n\n";
                _getch();
                return;
            }
            
            cout << "\n\n[Enter any other key to return to the previous menu]\n\n";
            cout << "\033[1;36mOperation selected: \033[0m";
            getline(cin, op);
            if (op == "1") //unenroll from subj
            {
                cout << "\nEnter Subject ID: ";
                getline(cin, upSubj);

                //check if subj matches sID in subjenrollment
                subjcheck = "SELECT SubjID from subjenrollment WHERE StudentID = '" + sID + "' AND SubjID = '" + upSubj + "'";
                qState = mysql_query(conn, subjcheck.c_str());
                if (!qState)
                {
                    res = mysql_store_result(conn);
                    if (res->row_count == 1)
                    {
                        mysql_free_result(res);
                        //unenroll student from subj
                        update = "DELETE FROM subjenrollment WHERE StudentID = '" + sID + "' AND SubjID = '" + upSubj + "'"; //remove entry
                        return;
                    }
                    else
                        cout << "\033[31mStudent " << sID << " is not enrolled in " << upSubj << ".\033[0m\nPress any key to return to the previous menu...\n\n";
                    mysql_free_result(res);
                }
                else
                {
                    querytrigger = false;
                    cout << "\n\n\033[31m[Error " << mysql_errno(conn) << "]\033[0m\nPress any key...\n\n";
                }
            }
            else if (op != "2")
                StudentDB();
        }

        //if student currently has no subj taken, or if student alr has subj assigned and is going to be assigned more
        if (!hasSubj || (hasSubj && op == "2"))
        {
            if (!hasSubj) //if no subj taken
            {
                cout << "1: Enroll in subject";
                cout << "\n\n[Enter any other key to return to the previous menu]\n\n";
                cout << "\033[1;36mOperation selected: \033[0m";
                getline(cin, op);
            }
            if ((!hasSubj && op == "1") || (hasSubj && op == "2")) //if student is to be assigned subj
            {
                cout << "\nEnter Subject ID: ";
                getline(cin, upSubj);

                //check if subj exists in subjbase and is not already enrolled
                subjcheck = "SELECT sb.SubjID FROM subjectbase sb LEFT JOIN subjenrollment se ON sb.SubjID = se.SubjID AND se.StudentID = '" + sID + "' WHERE sb.SubjID = '" + upSubj + "' AND se.SubjID is NULL";
                qState = mysql_query(conn, subjcheck.c_str());
                if (!qState)
                {
                    res = mysql_store_result(conn);
                    if ((row = mysql_fetch_row(res)) != NULL) // if not already enrolled
                    {
                        mysql_free_result(res);
                        string IDType = "SU", newEnNo;
                        //increment EnNo
                        IncrementID(IDType, newEnNo, 0);

                        //add enrollment
                        update = "INSERT INTO subjenrollment (EnNo, StudentID, SubjID) VALUES ('" + newEnNo + "', '" + sID + "', '" + upSubj + "')";
                        return;
                    }
                    else
                    {
                        querytrigger = false;
                        cout << "\n\033[31m[Error: Student " << sID << " is already enrolled in " << upSubj << " or Invalid Subject ID.]\033[0m\nPress any key...\n\n";
                        mysql_free_result(res);
                    }
                }
                else
                    cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\nPress any key...\n\n";
            }
            else
                StudentDBUpdate();
        }
    }
    else
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\nPress any key...\n\n";
    querytrigger = false;
    _getch();
    StudentDBUpdate();
}

//Class Management Module
void ClassMgmt(char uType, string sID)
{
    string op;

    while (true)
    {
        system("cls");

        cout << "--------------------\n\033[1;34mClass Management\033[0m\n--------------------\n";
        cout << "\033[1;36mSelect an operation:\033[0m\n\n";
        cout << "1: View class schedule";

        if (uType == 'A') //display extra option depending on user type
            cout << "\n2: Class setup";
        else if (uType == 'S')
            cout << "\n2: View class namelist";

        cout << "\n\n[Enter any other key to return to module selection] \n\n";
        cout << "\033[1;36mOperation selected:\033[0m ";

        getline(cin, op);

        if (op == "1")
            ScheduleView();
        else if (uType == 'A' && op == "2")
        {
            while (true)
            {
                system("cls");

                cout << "\033[1;36mSelect an operation:\033[0m\n\n";
                cout << "1: Update existing class\n2: Add new subject\n\n[Enter any other key to return to the previous menu]\n\n";
                cout << "\033[1;36mOperation selected:\033[0m ";
                getline(cin, op);
                if (op == "1")
                    ClassUpdate();
                else if (op == "2")
                    SubjAdd();
                else
                    break;
            }
        }
        else if (uType == 'S' && op == "2")
        {
            string classID;
            cout << "\nEnter subject ID: ";
            getline(cin, classID);
            string query = "SELECT SubjID FROM subjectbase WHERE StaffID = '" + sID + "' AND SubjID = '" + classID + "'";
            mysql_query(conn, query.c_str());
            if (!qState)
            {
                res = mysql_store_result(conn);
                if ((row = mysql_fetch_row(res)) != nullptr)
                {
                    mysql_free_result(res);
                    NamelistView(sID, classID);
                }
                else
                {
                    mysql_free_result(res);
                    cout << "\033[1;31mSubject ID invalid or You aren't assigned to this class!\033[0m\nPress any key...";
                    _getch();
                    classID.clear();
                    continue;
                }
            }
            else
                cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n\n";
        }
        else
            return;
    }
}

void ScheduleView()
{
    system("cls");
    string day[7] = { "MON", "TUES", "WED", "THURS", "FRI", "SAT", "SUN" };
   
    bool headerprinted = false;

    string isScheduleEmpty = "SELECT COUNT(*) FROM subjectbase WHERE ScheduledDay IS NOT NULL AND ScheduledTime IS NOT NULL AND StaffID IS NOT NULL"; //check if any class(subj) had been established
    qState = mysql_query(conn, isScheduleEmpty.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        int rowNo = mysql_num_rows(res);

        if (rowNo > 0) //if subj(s) exist
        {
            mysql_free_result(res);
            for (int i = 0; i < 7; i++) //display schedule for each day of a week, where subj(s) exist
            {
                string fetchSche = "SELECT SubjID, SubjName, ScheduledTime, subjectbase.StaffID, staffbase.StaffName FROM subjectbase LEFT JOIN staffbase ON subjectbase.StaffID = staffbase.StaffID WHERE ScheduledDay = '" + day[i] + "' AND ScheduledTime IS NOT NULL AND subjectbase.StaffID IS NOT NULL ORDER BY ScheduledTime";
                qState = mysql_query(conn, fetchSche.c_str());
                if (!qState)
                {
                    bool hasDay = false;
                    res = mysql_store_result(conn);
                    int colno = mysql_num_fields(res);
                    
                    while ((row = mysql_fetch_row(res)) != nullptr)
                    {
                        
                        if (!headerprinted) //print header & column header
                        {
                            system("cls");
                            cout << "                           \033[1;34mClass Schedule\033[0m";
                            cout << endl << "\033[1;33m---------------------------------------------------------------\033[0m\n";
                            cout << "\033[1;33m|\033[1;36m" << setw(8) << "Subj ID" << "\033[1;33m|\033[1;36m" << setw(13) << "Subj Name" << "\033[1;33m|\033[1;36m" << setw(13) << "Time" << "\033[1;33m|\033[1;36m" << setw(10) << "Taught by" << "\033[1;33m|\033[1;36m" << setw(13) << "Teacher Name" << "\033[1;33m|\033[1;36m" << "\033[0m";
                            cout << endl << "\033[1;33m---------------------------------------------------------------\033[0m\n";
                            headerprinted = true;
                        }

                        if (!hasDay) //print day
                        {
                            cout << "\033[36m" << day[i] << "\033[0m\n";
                            hasDay = true;
                        }
                        cout << "\033[1;33m|\033[0m";

                        for (int j = 0; j < colno; j++) //print timetable
                        {
                            if (j == 0)
                                cout << setw(8) << row[j] << "\033[1;33m|\033[0m";
                            else if (j == 3)
                                cout << setw(10) << row[j] << "\033[1;33m|\033[0m";
                            else
                                cout << setw(13) << row[j] << "\033[1;33m|\033[0m";
                        }
                        cout << "\n";
                    }
                    mysql_free_result(res);
                }
                else
                    cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
                cout << "\n";
            }
        }
        else //no class established
            cout << "No class has been established.\n";
        cout << endl << "Press any key to return to previous menu...";
    }
    else
    {
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
        mysql_free_result(res);
    }
    _getch();
    return;
}

void NamelistView(string sID, string classID)
{
    system("cls");
    transform(classID.begin(), classID.end(), classID.begin(), ::toupper);
    cout << "---------------\n\033[1;34m" << classID << " Namelist\033[0m\n---------------\n\n";
    string query = "SELECT subjenrollment.StudentID, studentbase.StudentName FROM subjectbase JOIN subjenrollment ON subjectbase.SubjID = subjenrollment.SubjID JOIN studentbase ON subjenrollment.StudentID = studentbase.StudentID WHERE subjectbase.SubjID = '" + classID + "' AND subjectbase.StaffID = '" + sID + "'";
    mysql_query(conn, query.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);

        int rowNo = mysql_num_rows(res);
        if (rowNo > 0) //class has been enrolled
        {
            int colNo = mysql_num_fields(res);
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                for (int i = 0; i < colNo; i++)
                    cout << row[i] << " ";
                cout << endl;
            }
        }
        else //class hasn't been enrolled
            cout << "No student has enrolled in this class yet";
        mysql_free_result(res);
        cout << "\n\nPress any key...";
    }
    else
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n\n";
    _getch();
    return;
}

void ClassUpdate() 
{
    string op;

    while (true)
    {
        system("cls");
        cout << "\033[1;36mSelect an operation\033[0m\n\n";
        cout << "1: Schedule class\n2: Assign teacher\n3: Remove subject\n\n[Enter any other key to return to the previous menu]\n\n";
        cout << "\033[1;36mOperation selected: \033[0m";

        getline(cin, op);
        string subj, upClass;
        if (op == "1")
        {
            system("cls");
            cout << "--------------------\n\033[1;34mScheduling Class\033[0m\n--------------------\n";
        }
        else if (op == "2")
        {
            system("cls");
            cout << "--------------------\n\033[1;34mTeacher Assignation\033[0m\n--------------------\n";
        }
        else if (op == "3")
        {
            system("cls");
            cout << "--------------------\n\033[1;34mSubject Removal\033[0m\n--------------------\n";
        }
        else
            return;

        cout << "Enter subject ID or X to cancel: ";
        getline(cin, subj);
        if (subj == "X" || subj == "x") //return to previous menu
            continue;
        else
        {
            upClass = "SELECT SubjID, SubjName FROM subjectbase WHERE BINARY SubjID = '" + subj + "'"; //check if subj is valid SubjID
            qState = mysql_query(conn, upClass.c_str());
            if (!qState)
            {
                res = mysql_store_result(conn);
                if ((row = mysql_fetch_row(res)) != nullptr) //subj is valid subjid
                {
                    string subjn = row[1];
                    mysql_free_result(res);
                    if (op == "1")
                        SubjSchedule(subj, subjn);
                    else if (op == "2")
                        TeacherAssignation(subj, subjn);
                    else if (op == "3")
                        SubjRem(subj, subjn);
                }
                else //subj is not valid usbjid
                {
                    cout << "\033[31mInvalid subject ID.\033[0m\nPress any key...";
                    _getch();
                }
            }
            else
            {
                cout << "\033[31mError: " << mysql_error(conn) << "\033[0m\n\nPress any key to return to the previous menu...";
                _getch();
            }
        }
    }
    
}

void SubjSchedule(string subj, string subjn)
{
    cout << "\n\nSubject: " << subj << " " << subjn << "\n";
    cout << "\033[1;36mCurrent schedule:\033[0m \n";
    string daytime = "SELECT ScheduledDay, ScheduledTime FROM subjectbase WHERE SubjID = '" + subj + "'";
    qState = mysql_query(conn, daytime.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);

        //print current scheduled time for subj
        for (int i = 0; i < mysql_num_fields(res); i++)
        {
            if (i == 0)
                cout << "Day: ";
            else
                cout << "Time: ";
            if (row[i] == nullptr)
                cout << "\033[1;33m" << setw(13) << "NOT SET" << "\033[0m";
            else
                cout << "\033[1;33m" << setw(13) << row[i] << "\033[0m";
            if (i < mysql_num_fields(res) - 1)
                cout << "\033[1;36m | \033[0m";
        }

        hideCursor();

        bool breakwhile = false;

        //set day
        vector<string>days = { "MON","TUES","WED","THURS","FRI","SAT","SUN" };
        string day;

        cout << "\n\n";
        int dayStartY = getCursorYPosition() + 1;
        cout << "\033[1;36mSelect day:\033[0m \n";
        int selected = 0; //index of currently selected menu option

        scrollmenu(days, selected, 0, dayStartY, day);

        //set time
        vector<string>times = { "0900 - 1030","1100 - 1230","1300 - 1430","1500 - 1630","1700 - 1830","1900 - 2030","2100 - 2230" };
        string time;

        int timeStartX = 20;
        setCursorPosition(timeStartX, dayStartY - 1);
        cout << "\033[1;36mSelect time slot:\033[0m";
        selected = 0; //index of currently selected menu option
        int timeStartY = dayStartY;

        scrollmenu(times, selected, timeStartX, timeStartY, time);

        showCursor();

        //confirmation
        cout << "\nSchedule " << subj << " on  " << day << " " << time << "? \033[1;33m[Enter Y to confirm or any other key to cancel]:\033[0m ";
        string op;
        getline(cin, op);
        if (op == "Y" || op == "y")
        {
            string schedule = "UPDATE subjectbase SET ScheduledDay = '" + day + "', ScheduledTime = '" + time + "' WHERE SubjID = '" + subj + "'";
            qState = mysql_query(conn, schedule.c_str());

            if (!qState)
                cout << endl << "\n\033[32mClass successfully scheduled.\033[0m\nPress any key to return to the previous menu...";
            else
                cout << "\n\033[31mClass scheduling failed! [Error:" << mysql_errno(conn) << "]\033[0m\nPress any key to return to the previous menu...";
        }
        else
            return;
    }
    _getch();
}

void TeacherAssignation(string subj, string subjn)
{
    cout << "\n\nSubject: " << subj << " " << subjn << "\n\n";
    cout << "\033[1;36mCurrently taught by:\033[0m ";
    string daytime = "SELECT StaffID FROM subjectbase WHERE SubjID = '" + subj + "'";
    qState = mysql_query(conn, daytime.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        row = mysql_fetch_row(res);

        //print current teaching staff for subj
        if (row[0] == nullptr)
            cout << "\033[1;33mNOT SET\033[0m\n\n";
        else
            cout << "\033[1;33m" << row[0] << "\033[0m";

        //assign teacher
        string op;

        while (true)
        {
            cout << "\n\n[Re]Assign a teacher? \033[1;33m[Y/N]\033[0m: ";
            getline(cin, op);
            if (op == "Y" || op == "y") //if assign teacher
            {
                string staffID;

                cout << "\nEnter Staff ID: ";
                getline(cin, staffID);

                //check if valid StaffID
                string sIDcheck = "SELECT StaffName FROM staffbase WHERE StaffID = '" + staffID + "'";
                qState = mysql_query(conn, sIDcheck.c_str());
                if (!qState)
                {
                    res = mysql_store_result(conn);
                    row = mysql_fetch_row(res);
                    if (row != nullptr)
                    {
                        //confirmation
                        cout << "Assign \033[35mTeacher " << row[0] << "\033[0m to subject " << subj << "? \033[1;33m[Enter Y to confirm or any other key to cancel]\033[0m: ";
                        getline(cin, op);
                        if (op == "Y" || op == "y")
                        {
                            string addTeacher = "UPDATE subjectbase SET StaffID = '" + staffID + "' WHERE SubjID = '" + subj + "'";
                            qState = mysql_query(conn, addTeacher.c_str());
                            if (!qState)
                                cout << "\033[32mTeacher assignation succeeded.\033[0m\n\nPress any key to return to the previous menu...";
                            else
                                cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\nPress any key to return to main menu...\n\n";
                        }
                        else
                            return;
                    }
                    else
                        cout << "\033[31mInvalid Staff ID.\033[0m\nPress any key...";
                }
                else
                    cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\nPress any key to return to main menu...\n\n";
                break;
            }
            else if (op == "N" || op == "n") //if x assign teacher
                return;
            else
            {
                cout << "\033[31mInvalid input.\033[0m\nPress any key...";
                _getch();
            }
        }
    }
    else
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << ":]\033[0m\nPress any key to return to main menu...\n\n";
    _getch();
    return;
}

void SubjRem(string subj, string subjn)
{
    string op;

    //confirmation
    cout << "\nDo you really wish to remove Subject " << subj << " " << subjn << " from the database? \033[1;33m[Enter Y to confirm or any other key to cancel]:\033[0m ";
    getline(cin, op);
    if (op == "Y" || op == "y")
    {
        string remSubj = "DELETE FROM subjectbase WHERE SubjID = '" + subj + "'";
        qState = mysql_query(conn, remSubj.c_str());
        if (!qState)
            cout << "\n\033[32mUpdate succeeded.\033[0m\n\nPress any key to return to the previous menu...";
        else
            cout << "\n\033[31mUpdate failed! [Error:" << mysql_errno(conn) << "]\n\033[0mPress any key to return to the previous menu...";
    }
    else
        cout << "\n\033[1;31mOperation cancelled.\033[0m\n\nPress any key to return to the previous menu...";
    _getch();
    return;
}

void SubjAdd()
{
    string subjID, subjName;
    system("cls");

    cout << "--------------------\n\033[1;34mSubject Registration\033[0m\n--------------------\n";
    //get SubjID & SubjName
    cout << "\033[1;36mEnter the following details:\033[0m\n\n";
    cout << "Subject ID \033[1;33m[Format: SU99]:\033[0m ";
    getline(cin, subjID);
    while (subjID.length() != 4 || subjID[0] != 'S' || subjID[1] != 'U' || !all_of(subjID.begin() + 2, subjID.end(), isdigit))
    {
        cout << "\033[31mInvalid input for Subject ID.\n\033[1;33m[Format: SU99]\033[0m\n\n";
        cout << "Subject ID: ";
        getline(cin, subjID);
    }
    cout << "Subject Name: ";
    getline(cin, subjName);
    while (subjName.empty())
    {
        cout << "\033[1;31mSubject name cannot be left blank.\033[0m\n";
        cout << "Subject Name: ";
        getline(cin, subjName);
    }

    //insert subj into DB
    string insert_query = "INSERT INTO subjectbase (SubjID, SubjName) VALUES ('" + subjID + "', '" + subjName + "')";
    qState = mysql_query(conn, insert_query.c_str());

    if (!qState)
        cout << endl << "\n\033[32mSubject successfully registered.\033[0m\nPress any key to return to the previous menu...";
    else
        cout << "\n\033[31mSubject registration failed! [Error:" << mysql_errno(conn) << " Subject ID already taken!]\033[0m\nPress any key to return to the previous menu...";
    _getch();
    return;
}

//Student Performance Module
void StudentPerf(char uType, string uID)
{
    string op;

    while (true)
    {
        system("cls");

        cout << "--------------------\n\033[1;34mStudent Performance\033[0m\n--------------------\n";
        cout << "\033[1;36mSelect an operation:\033[0m\n\n";
        cout << "1: View student performance tracksheet";

        if (uType == 'S') //display extra option if user is admin
            cout << "\n2: Update student performance";

        cout << "\n\n[Enter any other key to return to module selection] \n\n";
        cout << "\033[1;36mOperation selected:\033[0m ";

        getline(cin, op);

        if (op == "1")
            PerfView(uType);
        else if (uType == 'S' && op == "2")
            PerfUpdate(uID);
        else
            return;
    }
}

void PerfView(char uType)
{
    system("cls");

    string viewPerf = "SELECT * FROM performancebase ORDER BY SubjID, StudentID";
    qState = mysql_query(conn, viewPerf.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        int rowNo = mysql_num_rows(res);

        if (rowNo > 0) //display performance entries if exist
        {
            cout << "                                           \033[1;34mStudent Performance\033[0m";
            cout << endl << "\033[1;33m------------------------------------------------------------------------------------------------\033[0m\n";
            cout << "\033[1;33m|\033[1;36m" << setw(10) << "Subj ID" << "\033[1;33m|\033[1;36m" << setw(13) << "Student ID" << "\033[1;33m|\033[1;36m"  << setw(15) << "Quiz Score 1" << "\033[1;33m|\033[1;36m"  << setw(15) << "Quiz Score 2" << "\033[1;33m|\033[1;36m"  << setw(15) << "Quiz Score 3" << "\033[1;33m|\033[1;36m"  << setw(21) << "Comment" << "\033[1;33m|\033[0m";
            cout << endl << "\033[1;33m------------------------------------------------------------------------------------------------\033[0m\n";
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                if (row[4] == nullptr) //if only quizscore1 is available
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << "-" << "\033[1;33m|\033[0m" << setw(15) << "-" << "\033[1;33m|\033[0m" << setw(21) << "-" << "\033[1;33m|\033[0m\n";
                else if (row[5] == nullptr) //if only quizscore1 & 2 is available
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << row[4] << "\033[1;33m|\033[0m" << setw(15) << "-" << "\033[1;33m|\033[0m" << setw(21) << "-" << "\033[1;33m|\033[0m\n";
                else if (!strcmp(row[6], "Attention required")) //if comment1
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << row[4] << "\033[1;33m|\033[0m" << setw(15) << row[5] << "\033[1;33m|\033[1;31m" << setw(21) << row[6] << "\033[1;33m|\033[0m\n";
                else if (!strcmp(row[6], "Doing good")) //if comment2
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << row[4] << "\033[1;33m|\033[0m" << setw(15) << row[5] << "\033[1;33m|\033[1;32m" << setw(21) << row[6] << "\033[1;33m|\033[0m\n";
                else //if no comment
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << row[4] << "\033[1;33m|\033[0m" << setw(15) << row[5] << "\033[1;33m|\033[0m" << setw(21) << row[6] << "\033[1;33m|\033[0m\n";
            }

            string op, ID, filter;

            while (true)
            {
                cout << "\n\n\033[1;36mEnter S to filter or any other key to exit: \033[0m";  //filter entry?
                getline(cin, op);
                if (op == "S" || op == "s")
                {
                    system("cls");
                    cout << "\n\033[1;36mFilter by\033[0m\n\n1) Subject ID\n2) Filter by Student ID\n\n[Enter any other key to exit]\n\n\033[1;36mOperation selected:\033[0m ";
                    getline(cin, op);
                    if (op != "1" && op != "2")
                        return;
                    else
                    {
                        if (op == "1") //if filter by subj
                            cout << "\nEnter subject ID: ";
                        else //if filter by student
                            cout << "\nEnter student ID: ";
                        getline(cin, ID);
                        if (op == "1")
                            filter = "SELECT * FROM performancebase WHERE SubjID = '" + ID + "' ORDER BY StudentID";
                        else
                            filter = "SELECT * FROM performancebase WHERE StudentID = '" + ID + "' ORDER BY SubjID";
                        PerfFilter(filter);
                    }
                }
                else
                    return;
            }
        }
        else
            cout << "No student performance has been recorded\n\nPress any key to return to main menu...";
        mysql_free_result(res);
    }
    else
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
    _getch();
}

void PerfFilter(string filter)
{
    qState = mysql_query(conn, filter.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        int noRow = mysql_num_rows(res);
        if (noRow > 0)
        {
            cout << endl << "\033[1;33m------------------------------------------------------------------------------------------------\033[0m\n";
            cout << "\033[1;33m|\033[1;36m" << setw(10) << "Subj ID" << "\033[1;33m|\033[1;36m" << setw(13) << "Student ID" << "\033[1;33m|\033[1;36m" << setw(15) << "Quiz Score 1" << "\033[1;33m|\033[1;36m" << setw(15) << "Quiz Score 2" << "\033[1;33m|\033[1;36m" << setw(15) << "Quiz Score 3" << "\033[1;33m|\033[1;36m" << setw(21) << "Comment" << "\033[1;33m|\033[0m";
            cout << endl << "\033[1;33m------------------------------------------------------------------------------------------------\033[0m\n";
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                if (row[4] == nullptr)
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << "-" << "\033[1;33m|\033[0m" << setw(15) << "-" << "\033[1;33m|\033[0m" << setw(21) << "-" << "\033[1;33m|\033[0m\n";
                else if (row[5] == nullptr)
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << row[4] << "\033[1;33m|\033[0m" << setw(15) << "-" << "\033[1;33m|\033[0m" << setw(21) << "-" << "\033[1;33m|\033[0m\n";
                else if (!strcmp(row[6], "Attention required"))
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << row[4] << "\033[1;33m|\033[0m" << setw(15) << row[5] << "\033[1;33m|\033[1;31m" << setw(21) << row[6] << "\033[1;33m|\033[0m\n";
                else if (!strcmp(row[6], "Doing good"))
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << row[4] << "\033[1;33m|\033[0m" << setw(15) << row[5] << "\033[1;33m|\033[1;32m" << setw(21) << row[6] << "\033[1;33m|\033[0m\n";
                else
                    cout << "\033[1;33m|\033[0m" << setw(10) << row[1] << "\033[1;33m|\033[0m" << setw(13) << row[2] << "\033[1;33m|\033[0m" << setw(15) << row[3] << "\033[1;33m|\033[0m" << setw(15) << row[4] << "\033[1;33m|\033[0m" << setw(15) << row[5] << "\033[1;33m|\033[0m" << setw(21) << row[6] << "\033[1;33m|\033[0m\n";
            }
            cout << "\nEnd of result. Press any key...";
        }
        else
            cout << "\033[1;31mNo result for filtered subject/student!\033[0m\n\nPress any key...";
    }
    else
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
    _getch();
    return;
}

void PerfUpdate(string uID)
{
    system("cls");
    string SID, sID, quizScore;

    cout << "--------------------------\n\033[1;34mUpdate Student Performance\033[0m\n--------------------------\n";

    cout << "Enter ID of subject to be updated or X to cancel: ";
    getline(cin, sID);
    if (sID == "X" || sID == "x") //cancel update
        return;
    else
    {
        while (true)
        {
            cout << "Enter Student ID or X to cancel: ";
            getline(cin, SID);
            if (SID == "X" || SID == "x") //cancel update
                return;
            else 
            {
                //check if student SID is enrolled in subj sID, and if the staff updating their perf is actually the correct teaching staff
                string checksID = "SELECT StudentID FROM subjenrollment INNER JOIN subjectbase ON subjenrollment.SubjID = subjectbase.SubjID WHERE subjenrollment.StudentID = '" + SID + "' AND subjectbase.SubjID = '" + sID + "' AND subjectbase.StaffID = '" + uID + "'";
                qState = mysql_query(conn, checksID.c_str());
                if (!qState)
                {
                    res = mysql_store_result(conn);
                    if ((row = mysql_fetch_row(res)) != nullptr) //if matching SID, sID, staffID
                    {
                        //update
                        cout << "Enter latest quiz score: ";
                        getline(cin, quizScore);
                        while (quizScore.empty() || stoi(quizScore) < 0 || stoi(quizScore) > 100 || !all_of(quizScore.begin(), quizScore.end(), isdigit)) //valid quiz score?
                        {
                            cout << "\033[31mInvalid quiz score.\033[1;33m\n[Enter a score from 0 - 100]\033[0m\n\n";
                            cout << "Enter latest quiz score: ";
                            getline(cin, quizScore);
                        }
                        ScoreUpdate(sID, SID, quizScore);
                        Comment(sID, SID);
                        continue;
                    }
                    else //if x matching SID, sID, staffID
                        cout << "\n\n\033[31m[Error: Invalid Subject ID or Student ID]\033[0m\n\nPress any key...\n";
                    mysql_free_result(res);
                }
                else
                    cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
            }
            _getch();
            break;
        }
    }
}

void ScoreUpdate(string sID, string SID, string quizScore)
{
    string checkQuiz = "SELECT QuizScore_1, QuizScore_2, QuizScore_3 FROM performancebase WHERE StudentID = '" + SID + "' AND SubjID = '" + sID + "'";
    qState = mysql_query(conn, checkQuiz.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        int colno = mysql_num_fields(res);

        bool anycolumnempty = false, newEntry = false;
        string addScore;

        if (!(row = mysql_fetch_row(res))) //new entry
            addScore = "INSERT INTO performancebase(StudentID, SubjID, QuizScore_1) VALUES('" + SID + "', '" + sID + "', '" + quizScore + "')";
        else
        {
            for (int i = 0; i < colno; i++)
            {
                if (row[i] == nullptr)
                {
                    if (i == 1) //if quizscore 2 empty
                        addScore = "UPDATE performancebase SET QuizScore_2 = '" + quizScore + "' WHERE StudentID = '" + SID + "' AND SubjID = '" + sID + "'";
                    else if (i == 2) //if quizscore 3 empty
                        addScore = "UPDATE performancebase SET QuizScore_3 = '" + quizScore + "' WHERE StudentID = '" + SID + "' AND SubjID = '" + sID + "'";
                    anycolumnempty = true;
                    break;
                }
            }
            if (!anycolumnempty) //student already has 3 previous quizscore -> move quizscore 2 to 1, 3 to 2 and update 3 with new score
                addScore = "UPDATE performancebase SET QuizScore_1 = QuizScore_2, QuizScore_2 = QuizScore_3, QuizScore_3 = '" + quizScore + "' WHERE StudentID = '" + SID + "' AND SubjID = '" + sID + "'";
        }
        mysql_free_result(res);
        qState = mysql_query(conn, addScore.c_str());
        if (!qState)
            cout << "\033[32mUpdate succeeded.\033[0m\nPress any key to continue...\n\n";
        else
            cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
    }
    else
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
    _getch();
    return;
}

void Comment(string sID, string SID)
{
    string comment;

    string getscore = "SELECT QuizScore_1, QuizScore_2, QuizScore_3 FROM performancebase WHERE StudentID = '" + SID + "' AND SubjID = '" + sID + "'";
    qState = mysql_query(conn, getscore.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        if ((row = mysql_fetch_row(res)) && row[0] != nullptr && row[1] != nullptr && row[2] != nullptr)
        { //insert comment
            if (stoi(row[2]) < stoi(row[1]) && stoi(row[1]) < stoi(row[0]) && (stoi(row[0]) - stoi(row[2])) > 15) //if student's doing bad
                comment = "UPDATE performancebase SET Comment = 'Attention required' WHERE StudentID = '" + SID + "' AND SubjID = '" + sID + "'";
            else if (stoi(row[0]) < stoi(row[1]) && stoi(row[1]) < stoi(row[2])) //if student's doing good
                comment = "UPDATE performancebase SET Comment = 'Doing good' WHERE StudentID = '" + SID + "' AND SubjID = '" + sID + "'";
            else //if student's doing as usual / somewhat fluctuating
                comment = "UPDATE performancebase SET Comment = '-' WHERE StudentID = '" + SID + "' AND SubjID = '" + sID + "'";
            qState = mysql_query(conn, comment.c_str());
            if (qState)
            {
                cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
                _getch();
            }
        }
        mysql_free_result(res);
    }
    else
    {
        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n";
        _getch();
    }
    return;
}

//Finance Management Module
void FinMgmt(string uID)
{
    string op1, op2;

    while (true)
    {
        system("cls");
        cout << "--------------------\n\033[1;34mFinance Management\033[0m\n--------------------\n";
        cout << "\033[1;36mSelect an operation:\033[0m\n\n";
        cout << "1: Update finance\n2: Print financial report\n\n[Enter any other key to return to module selection]\n\n";
        cout << "\033[1;36mOperation selected:\033[0m ";
        getline(cin, op1);

        if (op1 == "1")
        {
            while (true)
            {
                system("cls");
                cout << "--------------------\n\033[1;34mFinance Update\033[0m\n--------------------\n";
                cout << "\033[1;36mSelect an operation:\033[0m\n\n";
                cout << "1: Record entry\n2: Delete entry\n\n[Enter any other key to return to module selection]\n\n";
                cout << "\033[1;36mOperation selected:\033[0m ";
                getline(cin, op2);
                if (op2 == "1")
                    FinRec(uID);
                else if (op2 == "2")
                    FinDel();
                else
                    break;
            }
        }
        else if (op1 == "2")
            FinReport();
        else
            return;
    }
}

void FinRec(string uID)
{
    string entryType, title, amount, cont;

    while (true)
    {
        system("cls");
        cout << "--------------------\n\033[1;34mUpdate Finance Entry\033[0m\n--------------------\n";
        cout << "\033[1;36mSelect type of entry to be recorded:\033[0m\n\n1: Income\n2: Expense\n\n[Enter any other key to return to previous menu...]\n\n\033[1;36mOption selected:\033[0m ";
        getline(cin, entryType);
        if (entryType != "1" && entryType != "2")
            return;
        cout << "\n\n\033[1;36mPlease provide the following details\033[0m\nTitle: ";
        getline(cin, title); //get finance entry title
        while (title.empty() || title.length() > 20)
        {
            if (title.empty())
                cout << "\033[1;31mTitle cannot be left blank.\033[0m\n";
            else if (title.length() > 20)
                cout << "\033[1;31mPlease limit the entry title to 20 characters or less.\033[0m\n";
            cout << "Title: ";
            getline(cin, title);
        }
        cout << "Amount: RM";
        getline(cin, amount); //get transaction amount
        MonetaryCheck(amount); //check if amount abides by monetary value format
        FinInsert(entryType, title, amount, uID); //insert entry to db

        cout << "\nRecord another entry? \033[1;33m[Enter Y to continue recording or any other key to stop]\033[0m: "; //repeat process?
        getline(cin, cont);
        if (cont != "Y")
            break;
    }
    return;
}

void MonetaryCheck(string &amount)
{
    while (true)
    {
        bool regetAmount = false;

        //if input is blank
        if (amount.empty())
            regetAmount = true;

        //if no decimal pt, check if invalid char in string for monetary value / amount is too large
        else if (amount.find('.') == string::npos)
        {
            if (!all_of(amount.begin(), amount.end(), isdigit) || amount.length() > 5)
                regetAmount = true;
            else
                amount = amount + ".00";
        }
            
        //if has decimal pt, check if invalid char in string for monetary value / too many decimal points / no. of digits before decimal > 5 / no. of digits after decimal > 2
        else if (amount.find('.') != string::npos)
        {
            if (!all_of(amount.begin(), amount.begin() + amount.find('.'), isdigit) || !all_of(amount.begin() + amount.find('.') + 1, amount.end(), isdigit) || count(amount.begin(), amount.end(), '.') != 1 || amount.find('.') > 5 || (amount.size() - amount.find('.') - 1) != 2)
                regetAmount = true;
        }
        
        //get re-input for amount
        if (regetAmount)
        {
            cout << "\n\033[1;31mInvalid monetary value.\n\033[33m[Note: Amount must be less than RM99999.99]\033[0m\n";
            cout << "Amount: RM";
            getline(cin, amount);
        }
        else
            break;
    }
}

void FinInsert(string entryType, string title, string amount, string uID)
{
    char currtime[11];
    //get sysTime
    time_t currentTime = std::time(nullptr);
    tm localTime;
    localtime_s(&localTime, &currentTime);
    //convert time to string
    strftime(currtime, sizeof(currtime), "%Y-%m-%d", &localTime);
    //initialize date
    string date(currtime);

    string FID = "F", newID, fType;
    IncrementID(FID, newID, 4);
    if (entryType == "1")
        fType = "I"; //income
    else
        fType = "E"; //expense

    //insert finance entry
    string query = "INSERT INTO financebase (FEntryNO, FType, Title, `Amount(RM)`, DateRecorded, AdminID) VALUES ('" + newID + "', '" + fType + "', '" + title + "', '" + amount + "', '" + date + "', '" + uID + "')";
    qState = mysql_query(conn, query.c_str());
    if (!qState)
        cout << "\033[1;32mUpdate succeeded!\033[0m\n";
    else
        cout << "\n\033[31mEntry insertion failed! [Error:" << mysql_errno(conn) << "\033[0m";
    return;
}

void FinDel() 
{
    string entryNo;
    system("cls");
    cout << "--------------------\n\033[1;34mDelete Finance Entry\033[0m\n--------------------\n";

    while (true)
    {
        cout << "Enter Entry No or X to cancel: ";
        getline(cin, entryNo);

        if (entryNo == "X" || entryNo == "x")
            break;
        else
        {
            string getEntry = "SELECT * FROM financebase WHERE FEntryNO = '" + entryNo + "'";
            qState = mysql_query(conn, getEntry.c_str());
            if (!qState)
            {
                res = mysql_store_result(conn);
                if ((row = mysql_fetch_row(res)) != nullptr) //if entryNo valid
                {
                    int colno = mysql_num_fields(res);
                    fields = mysql_fetch_field(res);

                    string confirmDel;
                    //display entry
                    cout << endl << "\033[1;33m---------------------------------------------------------------------------------------------\n|";
                    for (int i = 0; i < colno; i++)
                    {
                        if (i == 2)
                            cout << "\033[1;36m" << setw(21) << fields[i].name << "\033[1;33m|";
                        else
                            cout << "\033[1;36m" << setw(13) << fields[i].name << "\033[1;33m|";
                    }
                    cout << endl << "---------------------------------------------------------------------------------------------\033[0m" << endl;
                    cout << "\033[1;33m|\033[0m";
                    for (int i = 0; i < colno; i++)
                    {
                        if (i == 2)
                            cout << setw(21) << row[i] << "\033[1;33m|\033[0m";
                        else
                            cout << setw(13) << row[i] << "\033[1;33m|\033[0m";
                    }

                    //confirmation before deleting entry
                    cout << "\n\nConfirm delete? \033[1;33m[Enter Y to confirm or any other key to cancel]\033[0m: ";
                    getline(cin, confirmDel);
                    if (confirmDel == "Y" || confirmDel == "y")
                    {
                        string delEntry = "DELETE FROM financebase WHERE FEntryNO = '" + entryNo + "'";
                        qState = mysql_query(conn, delEntry.c_str());
                        if (!qState)
                            cout << "\033[1;32mEntry deleted!\033[0m\nPress any key...\n\n";
                        else
                            cout << "\033[31mError: " << mysql_error(conn) << "\033[0m\n\nPress any key to return to the previous menu...";
                        _getch();
                    }
                    else
                        break;
                }
                else
                    cout << "\033[1;31mInvalid entry.\033[0m\n\n";
                mysql_free_result(res);
            }
            else
            {
                cout << "\033[31mError: " << mysql_error(conn) << "\033[0m\n\nPress any key to return to the previous menu...";
                _getch();
            }
                
        }
    }
}

void FinReport()
{
    system("cls");
    string view = "SELECT * FROM financebase ORDER BY DateRecorded, FEntryNo"; //check if any finance entry has been recorded
    qState = mysql_query(conn, view.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        int colNo = mysql_num_fields(res);
        int rowNo = mysql_num_rows(res);
        fields = mysql_fetch_field(res);

        if (rowNo > 0) //>= 1 finance entry found
        {
            cout << "                                    \033[1;34mFinance Overview\033[0m";
            //print column headers
            cout << endl << "\033[1;33m---------------------------------------------------------------------------------------------\n|";
            for (int i = 0; i < colNo; i++)
            {
                if (i == 2)
                    cout << "\033[1;36m" << setw(21) << fields[i].name << "\033[1;33m|";
                else
                    cout << "\033[1;36m" << setw(13) << fields[i].name << "\033[1;33m|";
            }
            cout << endl << "---------------------------------------------------------------------------------------------\033[0m" << endl;

            //print rows
            while ((row = mysql_fetch_row(res)) !=nullptr)
            {
                cout << "\033[1;33m|\033[0m";
                for (int i = 0; i < colNo; i++)
                {
                    if (i == 2)
                        cout << setw(21) << row[i] << "\033[1;33m|\033[0m";
                    else
                        cout << setw(13) << row[i] << "\033[1;33m|\033[0m";
                }
                cout << endl;
            }
            mysql_free_result(res);
            cout << "\n\n";

            //search entry?
            while (true)
            {
                showCursor();
                string search;
                cout << "Enter S to generate monthly financial report or any other key to exit: ";
                getline(cin, search);
                if (search == "S" || search == "s")
                {
                    hideCursor();
                    bool breakwhile = false;
                    vector<string>monthsindigit;
                    string year, month; //chosen year, month to filter

                    //select year
                    int yearStartY;

                    string query = "SELECT DISTINCT YEAR(DateRecorded) AS year FROM financebase ORDER BY DateRecorded";
                    qState = mysql_query(conn, query.c_str());
                    if (!qState)
                    {
                        vector<string>years;
                        res = mysql_store_result(conn);
                        while ((row = mysql_fetch_row(res)) != nullptr)
                            years.push_back(row[0]);

                        cout << "\n\n";
                        cout << "\033[1;36mSelect year:\033[0m \n";
                        int selected = 0; //index of currently selected menu option
                        
                        yearStartY = getCursorYPosition() + 1;

                        scrollmenu(years, selected, 0, yearStartY, year);
                        mysql_free_result(res);
                    }
                    else
                        cout << "\n\nQuery Execution Failed! \n\n\033[31m[Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key to return to previous menu...\n\n";

                    //select month
                    string query2 = "SELECT DISTINCT MONTH(DateRecorded) AS month FROM financebase ORDER BY MONTH(DateRecorded)";
                    int selectedm;
                    qState = mysql_query(conn, query2.c_str());
                    if (!qState)
                    {
                        string monthname[12] = { "Jan", "Feb", "Mac", "April", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
                        vector<string>months;
                        res = mysql_store_result(conn);
                        int i = 0;
                        while ((row = mysql_fetch_row(res)) != nullptr)
                        {
                            monthsindigit.push_back(row[0]);
                            int monthNo = stoi(monthsindigit[i]);
                            months.push_back(monthname[monthNo - 1]);
                            i++;
                        }

                        cout << "\n\n";
                        int monthStartX = 20;
                        setCursorPosition(monthStartX, yearStartY - 2);
                        cout << "\033[1;36mSelect month:\033[0m \n";
                        selectedm = 0; //index of currently selected menu option
                        int monthStartY = yearStartY;

                        scrollmenu(months, selectedm, monthStartX, monthStartY, month);
                        mysql_free_result(res);
                    }
                    else
                        cout << "\n\nQuery Execution Failed! \n\n\033[31m[Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key to return to previous menu...\n\n";

                    //print filtered result
                    string filter = "SELECT * FROM financebase WHERE YEAR(DateRecorded) = '" + year + "' AND MONTH(DateRecorded) = '" + monthsindigit[selectedm] + "'";
                    qState = mysql_query(conn, filter.c_str());
                    if (!qState)
                    {
                        res = mysql_store_result(conn);
                        fields = mysql_fetch_field(res);
                        rowNo = mysql_num_rows(res);
                        if (rowNo > 0)
                        {
                            //print header
                            cout << endl << "\033[1;33m---------------------------------------------------------------------------------------------\n|";
                            for (int i = 0; i < colNo; i++)
                            {
                                if (i == 2)
                                    cout << "\033[1;36m" << setw(21) << fields[i].name << "\033[1;33m|";
                                else
                                    cout << "\033[1;36m" << setw(13) << fields[i].name << "\033[1;33m|";
                            }
                            cout << endl << "---------------------------------------------------------------------------------------------" << endl;
                            //print rows
                            while ((row = mysql_fetch_row(res)) != nullptr)
                            {
                                cout << "\033[1;33m|\033[0m";
                                for (int i = 0; i < colNo; i++)
                                {
                                    if (i == 2)
                                        cout << setw(21) << row[i] << "\033[1;33m|\033[0m";
                                    else
                                        cout << setw(13) << row[i] << "\033[1;33m|\033[0m";
                                }
                                cout << endl;
                            }
                            mysql_free_result(res);
                        }
                        else
                        {
                            cout << "\n\n\033[31mFinance entry not found for set period. Press any key...\033[0m\n\n";
                            mysql_free_result(res);
                            _getch();
                            system("cls");
                            continue;
                        }
                    }
                    else
                        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key...\n\n";

                    //analysis: profit/loss
                    double income, expenditure, bal;
                    string sumIncome = "SELECT SUM(`Amount(RM)`) FROM financebase WHERE YEAR(DateRecorded) = '" + year + "' AND MONTH(DateRecorded) = '" + monthsindigit[selectedm] + "' AND FType = 'I'";
                    string sumExpenditure = "SELECT SUM(`Amount(RM)`) FROM financebase WHERE YEAR(DateRecorded) = '" + year + "' AND MONTH(DateRecorded) = '" + monthsindigit[selectedm] + "' AND FType = 'E'";
                    
                    //fetch income
                    qState = mysql_query(conn, sumIncome.c_str());
                    if (!qState)
                    {
                        res = mysql_store_result(conn);
                        row = mysql_fetch_row(res);
                        if (row[0] == nullptr)
                            income = 0;
                        else
                            income = stod(row[0]);
                    }
                    else
                        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "] Financial analysis went wrong!\033[0m\n\nPress any key...\n\n";
                    
                    //fetch expenditure
                    qState = mysql_query(conn, sumExpenditure.c_str());
                    if (!qState)
                    {
                        res = mysql_store_result(conn);
                        row = mysql_fetch_row(res);
                        if (row[0]  == nullptr)
                            expenditure = 0;
                        else
                            expenditure = stod(row[0]);
                        mysql_free_result;
                    }
                    else
                        cout << "\n\n\033[31mQuery Execution Failed! [Error " << mysql_errno(conn) << "] Financial analysis went wrong!\033[0m\n\nPress any key...\n\n";

                    //calc monthly balance
                    bal = income - expenditure;
                    cout << "\n\nTotal income : RM" << fixed << setprecision(2) << income;
                    cout << "\nTotal expenditure : RM" << fixed << setprecision(2) << expenditure;
                    if (bal > 0)
                        cout << "\nMonthly balance [RM]: \033[1;32m" << fixed << setprecision(2) << bal << "\033[0m\n\n";
                    else if (bal < 0)
                        cout << "\nMonthly balance [RM]: \033[1;31m" << fixed << setprecision(2) << bal << "\033[0m\n\n";
                    else
                        cout << "\nMonthly balance [RM]: " << fixed << setprecision(2) << bal <<"\n\n";
                    cout << "Press any key...";
                    _getch();
                    system("cls");
                }
                else
                    return;
            }
        }
        else
            cout << "No finance entry has been recorded.\n\nPress any key to return to previous menu...";
    }
    else
        cout << "\n\nQuery Execution Failed! \n\n\033[31m[Error " << mysql_errno(conn) << "]\033[0m\n\nPress any key to return to previous menu...\n\n";
    _getch();
    return;
}

//Misc
void IncrementID(string &IDType, string& newID, int width)
{
    string query;

    if (IDType == "X")
        query = "SELECT MAX(CAST(SUBSTRING(StudentID, 2) AS UNSIGNED)) FROM studentbase";
    else if (IDType == "F")
        query = "SELECT MAX(CAST(SUBSTRING(FEntryNO, 2) AS UNSIGNED)) FROM financebase";
    else if (IDType == "SU")
        query = "SELECT MAX(EnNo) FROM subjenrollment";

    qState = mysql_query(conn, query.c_str());
    if (!qState)
    {
        res = mysql_store_result(conn);
        if ((row = mysql_fetch_row(res)) != NULL && row[0] != NULL)
        {
            if (IDType == "X" || IDType == "F")
            {
                // Extract numeric part, increment, and format
                int numPart = stoi(row[0]) + 1;
                ostringstream oss;
                oss << IDType << setw(width) << setfill('0') << numPart;
                newID = oss.str();
            }
            else if (IDType == "SU")
                newID = to_string(stoi(row[0]) + 1);
        }
        else // Default for the first row
        {
            if (IDType == "X")
                newID = "X001";
            else if (IDType == "F")
                newID = "F0001";
            else if (IDType == "SU")
                newID = "1";
        }
        mysql_free_result(res);
    }
    else
    {
        cout << "\033[31mError: " << mysql_error(conn) << "\033[0m\n\nPress any key to return to the previous menu...";
        _getch();
        return;
    }
}

int getCursorYPosition() 
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    if (GetConsoleScreenBufferInfo(console, &csbi)) {
        return csbi.dwCursorPosition.Y; //startY value
    }
    return 0;  // if unable to get position
}

void setCursorPosition(int x, int y)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { x,y };
    SetConsoleCursorPosition(console, position);
}

void scrollmenu(vector<string>options, int &selected, int startX, int startY, string &choice)
{
    while (true)
    {
        selectionmenu(options, selected, startX, startY, false);
        int key = _getch();
        if (key == KEY_UP)
            selected = (selected - 1 + options.size()) % options.size();
        else if (key == KEY_DOWN)
            selected = (selected + 1) % options.size();
        else if (key == KEY_ENTER)
        {
            selectionmenu(options, selected, startX, startY, true);
            choice = options[selected];
            break;
        }
    }
    return;
}

void selectionmenu(vector<string>options, int &selected, int startX, int startY, bool highlighted)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int i = 0; i < options.size(); i++)
    {
        setCursorPosition(startX, startY + i);

        if (i == selected)
        {
            if (int color = highlighted)
                SetConsoleTextAttribute(console, 14); //highlight decision
            else
                SetConsoleTextAttribute(console, 240);//normal highlight
        }
        else
            SetConsoleTextAttribute(console, 15); //default
        cout << options[i] << "\n";
    }
    SetConsoleTextAttribute(console, 15);
}

void hideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);  // Get current cursor state
    cursorInfo.bVisible = FALSE;                       // Set cursor visibility to false
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);  // Apply changes
}

void showCursor() 
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);  // Get current cursor state
    cursorInfo.bVisible = TRUE;                        // Set cursor visibility to true
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);  // Apply changes
}

















// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
