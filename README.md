# Tuition Centre Manager

## Overview
Tuition Centre Manager is designed to help both administrative and teaching staff efficiently manage a tuition centre with modules for **student database**, **class scheduling**, **performance tracking**, and **finance management**.

## Tech Stack
- C++
- MySQL
- Visual Studio

## Features
Admins
- CRUD on student database (student details, subject taken, etc.)
- CRUD on class management (class scheduling & teacher assignment)
- filtered view of student performance
- CRUD for finance tracking

Teaching staff
- view class details
- CRUD on student performance (with auto-generated system comment based on performance trend)

## Setup
1. Get MySQL Connector: https://dev.mysql.com/downloads/connector/cpp/
2. In project properties,
   - C/C++ -> General -> Additional Include Directories -> add 'include'
   - Linker -> General -> Additional Library Directories -> add 'lib64'
   - Linker -> Input -> Additional Dependencies -> add vs14/mysqlcppconn.lib
3. Get libmysql-cpp from NuGet Package Manager
4. Test Accounts
   - **Admin:** ID: A01, PW: 123
   - **Staff:** ID: S01, PW: 123
