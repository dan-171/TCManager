-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: 127.0.0.1
-- Generation Time: Jan 25, 2025 at 12:35 PM
-- Server version: 10.4.32-MariaDB
-- PHP Version: 8.2.12

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `tuitioncentre_manager`
--
DROP DATABASE IF EXISTS tuitioncentre_manager;
CREATE DATABASE IF NOT EXISTS tuitioncentre_manager DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE tuitioncentre_manager;
-- --------------------------------------------------------

--
-- Table structure for table `adminbase`
--

DROP TABLE IF EXISTS `adminbase`;
CREATE TABLE `adminbase` (
  `AdminID` varchar(3) NOT NULL,
  `AdminPWD` varchar(15) NOT NULL,
  `AdminName` varchar(50) NOT NULL,
  `AdminHP` varchar(12) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `adminbase`
--

INSERT INTO `adminbase` (`AdminID`, `AdminPWD`, `AdminName`, `AdminHP`) VALUES
('A01', '123', 'AdminName', '0111234567'),

-- --------------------------------------------------------

--
-- Table structure for table `financebase`
--

DROP TABLE IF EXISTS `financebase`;
CREATE TABLE `financebase` (
  `FEntryNO` varchar(5) NOT NULL,
  `FType` varchar(7) NOT NULL,
  `Title` varchar(20) NOT NULL,
  `Amount(RM)` decimal(7,2) NOT NULL,
  `DateRecorded` date NOT NULL,
  `AdminID` varchar(3) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `performancebase`
--

DROP TABLE IF EXISTS `performancebase`;
CREATE TABLE `performancebase` (
  `PEntryNO` int(4) NOT NULL,
  `SubjID` varchar(4) NOT NULL,
  `StudentID` varchar(4) NOT NULL,
  `QuizScore_1` int(3) DEFAULT NULL,
  `QuizScore_2` int(3) DEFAULT NULL,
  `QuizScore_3` int(3) DEFAULT NULL,
  `Comment` varchar(25) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `schoolbase`
--

DROP TABLE IF EXISTS `schoolbase`;
CREATE TABLE `schoolbase` (
  `SchoolID` varchar(5) NOT NULL,
  `SchoolName` varchar(30) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `schoolbase`
--

INSERT INTO `schoolbase` (`SchoolID`, `SchoolName`) VALUES
('SCH01', 'SchoolName1'),
('SCH02', 'SchoolName2');

-- --------------------------------------------------------

--
-- Table structure for table `staffbase`
--

DROP TABLE IF EXISTS `staffbase`;
CREATE TABLE `staffbase` (
  `StaffID` varchar(3) NOT NULL,
  `StaffPWD` varchar(15) NOT NULL,
  `StaffName` varchar(50) NOT NULL,
  `StaffHP` varchar(12) NOT NULL,
  `AdminID` varchar(3) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `staffbase`
--

INSERT INTO `staffbase` (`StaffID`, `StaffPWD`, `StaffName`, `StaffHP`, `AdminID`) VALUES
('S01', '123', 'StaffName1', '0122234556', 'A01'),
('S02', '123', 'StaffName2', '0167785443', 'A01');

-- --------------------------------------------------------

--
-- Table structure for table `studentbase`
--

DROP TABLE IF EXISTS `studentbase`;
CREATE TABLE `studentbase` (
  `StudentID` varchar(4) NOT NULL,
  `StudentName` varchar(15) NOT NULL,
  `SchoolID` varchar(5) NOT NULL,
  `Form` int(1) NOT NULL,
  `ParentName` varchar(15) NOT NULL,
  `ParentHP` varchar(12) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Dumping data for table `studentbase`
--

INSERT INTO `studentbase` (`StudentID`, `StudentName`, `SchoolID`, `Form`, `ParentName`, `ParentHP`) VALUES
('X001', 'StudentName1', 'SCH02', 2, 'ParentName1', '0123456778'),
('X002', 'StudentName2', 'SCH01', 4, 'ParentName2', '0156643567');

-- --------------------------------------------------------

--
-- Table structure for table `subjectbase`
--

DROP TABLE IF EXISTS `subjectbase`;
CREATE TABLE `subjectbase` (
  `SubjID` varchar(4) NOT NULL,
  `SubjName` varchar(10) NOT NULL,
  `ScheduledDay` varchar(5) DEFAULT NULL,
  `ScheduledTime` varchar(11) DEFAULT NULL,
  `StaffID` varchar(3) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- --------------------------------------------------------

--
-- Table structure for table `subjenrollment`
--

DROP TABLE IF EXISTS `subjenrollment`;
CREATE TABLE `subjenrollment` (
  `EnNo` int(4) NOT NULL,
  `StudentID` varchar(4) NOT NULL,
  `SubjID` varchar(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `adminbase`
--
ALTER TABLE `adminbase`
  ADD PRIMARY KEY (`AdminID`);

--
-- Indexes for table `financebase`
--
ALTER TABLE `financebase`
  ADD PRIMARY KEY (`FEntryNO`),
  ADD KEY `AdminID_FK` (`AdminID`);

--
-- Indexes for table `performancebase`
--
ALTER TABLE `performancebase`
  ADD PRIMARY KEY (`PEntryNO`),
  ADD KEY `studentID_FK` (`StudentID`),
  ADD KEY `subjID_FKK` (`SubjID`);

--
-- Indexes for table `schoolbase`
--
ALTER TABLE `schoolbase`
  ADD PRIMARY KEY (`SchoolID`);

--
-- Indexes for table `staffbase`
--
ALTER TABLE `staffbase`
  ADD PRIMARY KEY (`StaffID`),
  ADD KEY `AdminID_FK2` (`AdminID`);

--
-- Indexes for table `studentbase`
--
ALTER TABLE `studentbase`
  ADD PRIMARY KEY (`StudentID`),
  ADD KEY `SchoolID_FK` (`SchoolID`);

--
-- Indexes for table `subjectbase`
--
ALTER TABLE `subjectbase`
  ADD PRIMARY KEY (`SubjID`),
  ADD KEY `StaffID` (`StaffID`);

--
-- Indexes for table `subjenrollment`
--
ALTER TABLE `subjenrollment`
  ADD PRIMARY KEY (`EnNo`),
  ADD KEY `SubjID_FK2` (`SubjID`),
  ADD KEY `StudentID_FK2` (`StudentID`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `performancebase`
--
ALTER TABLE `performancebase`
  MODIFY `PEntryNO` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=16;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `financebase`
--
ALTER TABLE `financebase`
  ADD CONSTRAINT `AdminID_FK` FOREIGN KEY (`AdminID`) REFERENCES `adminbase` (`AdminID`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Constraints for table `performancebase`
--
ALTER TABLE `performancebase`
  ADD CONSTRAINT `studentID_FK` FOREIGN KEY (`StudentID`) REFERENCES `studentbase` (`StudentID`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `subjID_FKK` FOREIGN KEY (`SubjID`) REFERENCES `subjectbase` (`SubjID`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `staffbase`
--
ALTER TABLE `staffbase`
  ADD CONSTRAINT `AdminID_FK2` FOREIGN KEY (`AdminID`) REFERENCES `adminbase` (`AdminID`) ON DELETE SET NULL ON UPDATE CASCADE;

--
-- Constraints for table `studentbase`
--
ALTER TABLE `studentbase`
  ADD CONSTRAINT `SchoolID_FK` FOREIGN KEY (`SchoolID`) REFERENCES `schoolbase` (`SchoolID`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `subjectbase`
--
ALTER TABLE `subjectbase`
  ADD CONSTRAINT `StaffID` FOREIGN KEY (`StaffID`) REFERENCES `staffbase` (`StaffID`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `subjenrollment`
--
ALTER TABLE `subjenrollment`
  ADD CONSTRAINT `StudentID_FK2` FOREIGN KEY (`StudentID`) REFERENCES `studentbase` (`StudentID`) ON DELETE CASCADE ON UPDATE CASCADE,
  ADD CONSTRAINT `SubjID_FK2` FOREIGN KEY (`SubjID`) REFERENCES `subjectbase` (`SubjID`) ON DELETE CASCADE ON UPDATE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
