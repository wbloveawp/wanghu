USE [master]
GO

/****** Object:  Database [LHGameRecordDB]    Script Date: 2023/3/21 0:02:45 ******/
CREATE DATABASE [LHGameRecordDB]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'LHGameRecordDB', FILENAME = N'D:\数据库\LHGameRecordDB.mdf' , SIZE = 5120KB , MAXSIZE = UNLIMITED, FILEGROWTH = 10%)
 LOG ON 
( NAME = N'LHGameRecordDB_log', FILENAME = N'D:\数据库\LHGameRecordDB_log.ldf' , SIZE = 5120KB , MAXSIZE = UNLIMITED, FILEGROWTH = 10%)
 WITH CATALOG_COLLATION = DATABASE_DEFAULT
GO

IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [LHGameRecordDB].[dbo].[sp_fulltext_database] @action = 'disable'
end
GO

ALTER DATABASE [LHGameRecordDB] SET ANSI_NULL_DEFAULT OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET ANSI_NULLS OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET ANSI_PADDING OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET ANSI_WARNINGS OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET ARITHABORT OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET AUTO_CLOSE OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET AUTO_SHRINK OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET AUTO_UPDATE_STATISTICS ON 
GO

ALTER DATABASE [LHGameRecordDB] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET CURSOR_DEFAULT  GLOBAL 
GO

ALTER DATABASE [LHGameRecordDB] SET CONCAT_NULL_YIELDS_NULL OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET NUMERIC_ROUNDABORT OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET QUOTED_IDENTIFIER OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET RECURSIVE_TRIGGERS OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET  DISABLE_BROKER 
GO

ALTER DATABASE [LHGameRecordDB] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET TRUSTWORTHY OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET PARAMETERIZATION SIMPLE 
GO

ALTER DATABASE [LHGameRecordDB] SET READ_COMMITTED_SNAPSHOT OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET HONOR_BROKER_PRIORITY OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET RECOVERY SIMPLE 
GO

ALTER DATABASE [LHGameRecordDB] SET  MULTI_USER 
GO

ALTER DATABASE [LHGameRecordDB] SET PAGE_VERIFY CHECKSUM  
GO

ALTER DATABASE [LHGameRecordDB] SET DB_CHAINING OFF 
GO

ALTER DATABASE [LHGameRecordDB] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO

ALTER DATABASE [LHGameRecordDB] SET TARGET_RECOVERY_TIME = 60 SECONDS 
GO

ALTER DATABASE [LHGameRecordDB] SET DELAYED_DURABILITY = DISABLED 
GO

ALTER DATABASE [LHGameRecordDB] SET ACCELERATED_DATABASE_RECOVERY = OFF  
GO

ALTER DATABASE [LHGameRecordDB] SET QUERY_STORE = OFF
GO

ALTER DATABASE [LHGameRecordDB] SET  READ_WRITE 
GO


